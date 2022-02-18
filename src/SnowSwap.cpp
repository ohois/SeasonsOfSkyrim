#include "SnowSwap.h"
#include "SeasonManager.h"

namespace SnowSwap
{
	void Manager::LoadSnowShaderBlacklist()
	{
		std::vector<std::string> configs;

		for (constexpr auto folder = R"(Data\Seasons)"; const auto& entry : std::filesystem::directory_iterator(folder)) {
			if (entry.exists() && !entry.path().empty() && entry.path().extension() == ".ini"sv) {
				if (const auto path = entry.path().string(); path.contains("_NOSNOW")) {
					configs.push_back(path);
				}
			}
		}

		if (configs.empty()) {
			logger::warn("No .ini files with _{} suffix were found in the Data/Seasons folder. Snow Shader blacklist is not loaded");
			return;
		}

		logger::info("{} matching inis found", configs.size());

		for (auto& path : configs) {
			logger::info("	INI : {}", path);

			CSimpleIniA ini;
			ini.SetUnicode();
			ini.SetMultiKey();
			ini.SetAllowEmptyValues();

			if (const auto rc = ini.LoadFile(path.c_str()); rc < 0) {
				logger::error("	couldn't read INI");
				continue;
			}

			if (const auto values = ini.GetSection("Blacklist"); values && !values->empty()) {
				for (const auto& key : *values | std::views::keys) {
					if (auto formID = INI::parse_form(key.pItem); formID != 0) {
						_snowShaderBlacklist.insert(formID);
					}
				}
			}
		}
	}

	bool Manager::GetInSnowShaderBlacklist(const RE::TESForm* a_form) const
	{
		if (_snowShaderBlacklist.contains(a_form->GetFormID())) {
			return true;
		}

		std::string model = a_form->As<RE::TESModel>()->GetModel();
		return model.empty() || std::ranges::any_of(_snowShaderModelBlackList, [&](const auto& str) { return string::icontains(model, str); });
	}

	bool Manager::CanApplySnowShader(RE::TESObjectREFR* a_ref) const
	{
		if (!SeasonManager::GetSingleton()->CanApplySnowShader() || !a_ref || a_ref->IsDeleted()) {
			return false;
		}

		const auto base = util::get_original_base(a_ref);

	    if (base != a_ref->GetBaseObject() || base->IsNot(RE::FormType::MovableStatic, RE::FormType::Container) || base->IsMarker() || base->IsHeadingMarker() || GetInSnowShaderBlacklist(base)) {
			return false;
		}

		if (a_ref->IsInWater()) {
			return false;
		}

		return true;
	}

	bool Manager::CanApplySnowShader(RE::TESObjectSTAT* a_static, RE::TESObjectREFR* a_ref) const
	{
		if (!SeasonManager::GetSingleton()->CanApplySnowShader() || !a_ref || a_ref->IsDeleted()) {
			return false;
		}

		const auto base = util::get_original_base(a_ref); 

		if (base != a_static || a_static->IsMarker() || a_static->IsHeadingMarker() || GetInSnowShaderBlacklist(a_static)) {
			return false;
		}

	    if (const auto matObject = a_static->data.materialObj; matObject && (util::is_snow_shader(matObject) || util::get_editorID(matObject).contains("Ice"sv))) {
			return false;
		}

		if (a_static->IsSnowObject() || a_static->IsSkyObject() || a_static->HasTreeLOD()) {
			return false;
		}

		if (a_ref->IsInWater()) {
			return false;
		}

		return true;
	}

	SNOW_TYPE Manager::GetSnowType(RE::NiAVObject* a_node)
	{
		bool hasShape = false; //has trishapes (crash)
		bool hasInvalidShape = false; //has non-zero vertices (crash)

	    bool hasLightingShaderProp = true;  //has lighting prop/not skinned (crash)
		bool hasAlphaProp = false;          //no alpha prop (broken)

		RE::BSVisit::TraverseScenegraphGeometries(a_node, [&](RE::BSGeometry* a_geometry) -> RE::BSVisit::BSVisitControl {
			hasShape = true;

		    if (const auto shape = a_geometry->AsTriShape(); shape && shape->vertexCount == 0) {
				hasInvalidShape = true;
				return RE::BSVisit::BSVisitControl::kStop;
		    }

		    const auto effect = a_geometry->properties[RE::BSGeometry::States::kEffect];
			const auto lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(effect.get());
			if (!lightingShader || lightingShader->flags.any(RE::BSShaderProperty::EShaderPropertyFlag::kSkinned)) {
				hasLightingShaderProp = false;
				return RE::BSVisit::BSVisitControl::kStop;
			}

			const auto property = a_geometry->properties[RE::BSGeometry::States::kProperty];
			const auto alphaProperty = netimmerse_cast<RE::NiAlphaProperty*>(property.get());
			if (alphaProperty && (alphaProperty->GetAlphaBlending() || alphaProperty->GetAlphaTesting())) {
				hasAlphaProp = true;
				return RE::BSVisit::BSVisitControl::kStop;
			} 

			return RE::BSVisit::BSVisitControl::kContinue;
		});

		if (hasShape && !hasInvalidShape && hasLightingShaderProp && !hasAlphaProp) {
			return SNOW_TYPE::kMultiPass;
		}

		return SNOW_TYPE::kSinglePass;
	}

	void Manager::ApplySinglePassSnow(RE::NiAVObject* a_node)
	{
		if (!a_node) {
			return;
		}

		auto& [init, projectedParams, projectedColor] = _defaultObj;
		if (!init) {
			const auto snowMat = RE::TESForm::LookupByEditorID<RE::BGSMaterialObject>("SnowMaterialObject1P");

			projectedColor = snowMat->directionalData.singlePassColor;
			if (projectedColor.red == 0.0f && projectedColor.green == 0.0f && projectedColor.blue == 0.0f) {  //if using MP shader with black single pass color, use vanilla default values
				projectedColor = { 0.4196f, 0.4549f, 0.4941f };
			}

			projectedParams = RE::NiColorA{
				snowMat->directionalData.falloffScale,
				snowMat->directionalData.falloffBias,
				1.0f / snowMat->directionalData.noiseUVScale,
				std::cosf(RE::deg_to_rad(90.0f))
			};

			init = true;
		}

	    if (a_node->SetProjectedUVData(projectedParams, projectedColor, true)) {
			ApplySnowMaterialPatch(a_node);
		}
	}

    void Manager::ApplySnowMaterialPatch(RE::NiAVObject* a_node)
	{
		if (const auto snowShaderData = RE::NiBooleanExtraData::Create("SOS_SNOW_SHADER", true)) {
			a_node->AddExtraData(snowShaderData);
		}
	}

    std::optional<Manager::SnowInfo> Manager::GetSnowInfo(const RE::TESObjectSTAT* a_static)
	{
		Locker locker(_snowInfoLock);

		if (const auto it = _snowInfoMap.find(a_static->GetFormID()); it != _snowInfoMap.end()) {
			return it->second;
		}
		return std::nullopt;
	}

	void Manager::SetSnowInfo(const RE::TESObjectSTAT* a_static, RE::BGSMaterialObject* a_originalMat, SNOW_TYPE a_snowType)
	{
		Locker locker(_snowInfoLock);

		SnowInfo snowInfo{ a_originalMat ? a_originalMat->GetFormID() : 0, a_snowType };
		_snowInfoMap.emplace(a_static->GetFormID(), snowInfo);
	}

	RE::BGSMaterialObject* Manager::GetMultiPassSnowShader()
	{
		if (!_multiPassSnowShader) {
			_multiPassSnowShader = RE::TESForm::LookupByEditorID<RE::BGSMaterialObject>("SOS_WIN_SnowMaterialObjectMP");
		}
		return _multiPassSnowShader;
	}
}