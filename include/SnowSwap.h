#pragma once

namespace SnowSwap
{
	enum class SNOW_TYPE
	{
		kSinglePass = 0,
		kMultiPass
	};

	enum class SWAP_RESULT
	{
		kSeasonFail,
		kRefFail,
		kBaseFail,
		kSuccess
	};

	class Manager
	{
	public:
		struct SnowInfo
		{
			RE::FormID origShader;
			SNOW_TYPE snowType;
		};

		struct ProjectedUV
		{
			bool init{ false };
			RE::NiColorA projectedParams{};
			RE::NiColor projectedColor{};
		};

		static Manager* GetSingleton()
		{
			static Manager singleton;
			return std::addressof(singleton);
		}

		void LoadSnowShaderBlacklist();

		[[nodiscard]] bool CanApplySnowShader(RE::TESObjectREFR* a_ref) const;
		[[nodiscard]] SWAP_RESULT CanApplySnowShader(RE::TESObjectSTAT* a_static, RE::TESObjectREFR* a_ref) const;

		[[nodiscard]] static SNOW_TYPE GetSnowType(RE::NiAVObject* a_node);

		void ApplySinglePassSnow(RE::NiAVObject* a_node);
		void ApplySnowMaterialPatch(RE::NiAVObject* a_node);

		[[nodiscard]] std::optional<SnowInfo> GetSnowInfo(const RE::TESObjectSTAT* a_static);
		void SetSnowInfo(const RE::TESObjectSTAT* a_static, RE::BGSMaterialObject* a_originalMat, SNOW_TYPE a_snowType);

		[[nodiscard]] RE::BGSMaterialObject* GetMultiPassSnowShader();
		[[nodiscard]] RE::BGSMaterialObject* GetSinglePassSnowShader();

	protected:
		Manager() = default;
		Manager(const Manager&) = delete;
		Manager(Manager&&) = delete;
		~Manager() = default;

		Manager& operator=(const Manager&) = delete;
		Manager& operator=(Manager&&) = delete;

	private:
		using Lock = std::mutex;
		using Locker = std::scoped_lock<Lock>;
		using SnowInfoMap = Map<RE::FormID, SnowInfo>;

		bool GetInSnowShaderBlacklist(const RE::TESForm* a_form) const;

	    bool IsBaseBlacklisted(const RE::TESForm* a_form) const;

		Set<RE::FormID> _snowShaderBlacklist{};

		mutable Lock _snowInfoLock;
		SnowInfoMap _snowInfoMap{};

		ProjectedUV _defaultObj{};

		RE::BGSMaterialObject* _multiPassSnowShader{ nullptr };
		RE::BGSMaterialObject* _singlePassSnowShader{ nullptr };

		Set<std::string> _snowShaderModelBlackList{ R"(Effects\)", R"(Sky\)", R"(lod\)", "WetRocks", "DynDOLOD", "Marker" };
	};

	namespace Statics
	{
		struct Clone3D
		{
			static RE::NiAVObject* thunk(RE::TESObjectSTAT* a_static, RE::TESObjectREFR* a_ref, bool a_arg3)
			{
				const auto manager = Manager::GetSingleton();

				auto snowInfo = manager->GetSnowInfo(a_static);
				const auto result = manager->CanApplySnowShader(a_static, a_ref);

				if (result == SWAP_RESULT::kSuccess) {
					if (snowInfo) {
						auto& [origShader, snowType] = *snowInfo;
						if (snowType == SNOW_TYPE::kSinglePass) {
							a_static->data.materialObj = manager->GetSinglePassSnowShader();
						} else {
							a_static->data.materialObj = manager->GetMultiPassSnowShader();
						}
					} else {
						if (auto tempNode = func(a_static, a_ref, a_arg3); tempNode) {
							const auto snowType = Manager::GetSnowType(tempNode);
							manager->SetSnowInfo(a_static, a_static->data.materialObj, snowType);

							if (snowType == SNOW_TYPE::kSinglePass) {
								a_static->data.materialObj = manager->GetSinglePassSnowShader();
							} else {
								a_static->data.materialObj = manager->GetMultiPassSnowShader();
							}
							tempNode->DeleteThis();  //refCount is zero, nothing else should touch this.
							tempNode = nullptr;
						}
					}
				} else if ((result == SWAP_RESULT::kSeasonFail || result == SWAP_RESULT::kRefFail) && snowInfo) {
					auto& [origShaderID, snowType] = *snowInfo;
					a_static->data.materialObj = origShaderID != 0 ?
					                                 RE::TESForm::LookupByID<RE::BGSMaterialObject>(origShaderID) :
                                                     nullptr;
				}

				return func(a_static, a_ref, a_arg3);
			}
			static inline REL::Relocation<decltype(thunk)> func;

			static inline constexpr std::size_t size = 0x40;
		};

		inline void Install()
		{
			stl::write_vfunc<RE::TESObjectSTAT, Clone3D>();
		}
	}

	//Movable Statics/Containers
	namespace OtherForms
	{
		struct Clone3D
		{
			static RE::NiAVObject* thunk(RE::TESBoundObject* a_base, RE::TESObjectREFR* a_ref, bool a_arg3)
			{
				const auto node = func(a_base, a_ref, a_arg3);

				if (const auto manager = Manager::GetSingleton(); manager->CanApplySnowShader(a_ref)) {
					manager->ApplySinglePassSnow(node);
				}

				return node;
			}
			static inline REL::Relocation<decltype(thunk)> func;

			static inline constexpr std::size_t size = 0x40;
		};

		inline void Install()
		{
			stl::write_vfunc<RE::BGSMovableStatic, 2, Clone3D>();
			stl::write_vfunc<RE::TESObjectCONT, Clone3D>();
		}
	}

	inline void Install()
	{
		Statics::Install();
		OtherForms::Install();

		logger::info("Installed dynamic snow manager"sv);
	}
}
