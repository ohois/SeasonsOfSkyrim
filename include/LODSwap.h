#pragma once

#include <Seasons.h>

namespace LODSwap
{
	struct detail
	{
		template <class T>
		static std::string get_lod_filename()
		{
			const auto [canSwap, season] = SeasonManager::GetSingleton()->CanSwapLOD(T::type);
			return canSwap ? fmt::format(T::seasonalPath, season) : T::defaultPath;
		}
	};

	namespace Terrain
	{
		struct BuildMeshFileName
		{
			static void func(char* a_buffer, std::uint32_t a_sizeOfBuffer, const char* a_worldSpace, std::int16_t a_x, std::int16_t a_y, std::uint32_t a_scale)
			{
				const auto path = detail::get_lod_filename<BuildMeshFileName>();
				sprintf_s(a_buffer, a_sizeOfBuffer, path.c_str(), a_worldSpace, a_worldSpace, a_scale, a_x, a_y);
			}

			static inline size_t size = 0x39;
			static inline std::uint64_t id = 31140;

			static inline const char* seasonalPath{ R"(Data\Meshes\Terrain\%s\%s.%i.%i.%i.{}.BTR)" };
			static inline const char* defaultPath{ R"(Data\Meshes\Terrain\%s\%s.%i.%i.%i.BTR)" };

			static inline auto type = LOD_TYPE::kTerrain;
		};

		struct BuildDiffuseTextureFileName
		{
			static void func(char* a_buffer, std::uint32_t a_sizeOfBuffer, const char* a_worldSpace, std::int16_t a_x, std::int16_t a_y, std::uint32_t a_scale)
			{
				const auto path = detail::get_lod_filename<BuildDiffuseTextureFileName>();
				sprintf_s(a_buffer, a_sizeOfBuffer, path.c_str(), a_worldSpace, a_worldSpace, a_scale, a_x, a_y);
			}

			static inline size_t size = 0x39;
			static inline std::uint64_t id = 31141;

			static inline const char* seasonalPath{ R"(Data\Textures\Terrain\%s\%s.%i.%i.%i.{}.DDS)" };
			static inline const char* defaultPath{ R"(Data\Textures\Terrain\%s\%s.%i.%i.%i.DDS)" };

			static inline auto type = LOD_TYPE::kTerrain;
		};

		struct BuildNormalTextureFileName
		{
			static void func(char* a_buffer, std::uint32_t a_sizeOfBuffer, const char* a_worldSpace, std::int16_t a_x, std::int16_t a_y, std::uint32_t a_scale)
			{
				const auto path = detail::get_lod_filename<BuildNormalTextureFileName>();
				sprintf_s(a_buffer, a_sizeOfBuffer, path.c_str(), a_worldSpace, a_worldSpace, a_scale, a_x, a_y);
			}

			static inline size_t size = 0x39;
			static inline std::uint64_t id = 31142;

			static inline const char* seasonalPath{ R"(Data\Textures\Terrain\%s\%s.%i.%i.%i.{}_n.DDS)" };
			static inline const char* defaultPath{ R"(Data\Textures\Terrain\%s\%s.%i.%i.%i_n.DDS)" };

			static inline auto type = LOD_TYPE::kTerrain;
		};

		inline void Install()
		{
			stl::asm_replace<BuildMeshFileName>();
			stl::asm_replace<BuildDiffuseTextureFileName>();
			stl::asm_replace<BuildNormalTextureFileName>();
		}
	}

	namespace Object
	{
		struct BuildMeshFileName
		{
			static void func(char* a_buffer, std::uint32_t a_sizeOfBuffer, const char* a_worldSpace, std::int16_t a_x, std::int16_t a_y, std::uint32_t a_scale)
			{
				const auto path = detail::get_lod_filename<BuildMeshFileName>();
				sprintf_s(a_buffer, a_sizeOfBuffer, path.c_str(), a_worldSpace, a_worldSpace, a_scale, a_x, a_y);
			}

			static inline size_t size = 0x39;
			static inline std::uint64_t id = 31147;

			static inline const char* seasonalPath{ R"(Data\Meshes\Terrain\%s\Objects\%s.%i.%i.%i.{}.BTO)" };
			static inline const char* defaultPath{ R"(Data\Meshes\Terrain\%s\Objects\%s.%i.%i.%i.BTO)" };

			static inline auto type = LOD_TYPE::kObject;
		};

		struct BuildDiffuseTextureAtlasFileName
		{
			static void func(char* a_buffer, std::uint32_t a_sizeOfBuffer, const char* a_worldSpace)
			{
				const auto path = detail::get_lod_filename<BuildDiffuseTextureAtlasFileName>();
				sprintf_s(a_buffer, a_sizeOfBuffer, path.c_str(), a_worldSpace, a_worldSpace);
			}

			static inline size_t size = 0x1F;
			static inline std::uint64_t id = 31148;

			static inline const char* seasonalPath{ R"(Data\Textures\Terrain\%s\Objects\%s.Objects.{}.DDS)" };
			static inline const char* defaultPath{ R"(Data\Textures\Terrain\%s\Objects\%s.Objects.DDS)" };

			static inline auto type = LOD_TYPE::kObject;
		};

		struct BuildNormalTextureAtlasFileName
		{
			static void func(char* a_buffer, std::uint32_t a_sizeOfBuffer, const char* a_worldSpace)
			{
				const auto path = detail::get_lod_filename<BuildNormalTextureAtlasFileName>();
				sprintf_s(a_buffer, a_sizeOfBuffer, path.c_str(), a_worldSpace, a_worldSpace);
			}

			static inline size_t size = 0x1F;
			static inline std::uint64_t id = 31149;

			static inline const char* seasonalPath{ R"(Data\Textures\Terrain\%s\Objects\%s.Objects.{}_n.DDS)" };
			static inline const char* defaultPath{ R"(Data\Textures\Terrain\%s\Objects\%s.Objects_n.DDS)" };

			static inline auto type = LOD_TYPE::kObject;
		};

		inline void Install()
		{
			stl::asm_replace<BuildMeshFileName>();
			stl::asm_replace<BuildDiffuseTextureAtlasFileName>();
			stl::asm_replace<BuildNormalTextureAtlasFileName>();
		}
	}

	namespace Tree
	{
		struct BuildMeshFileName
		{
			static void func(char* a_buffer, std::uint32_t a_sizeOfBuffer, const char* a_worldSpace, std::int16_t a_x, std::int16_t a_y, std::uint32_t a_scale)
			{
				const auto path = detail::get_lod_filename<BuildMeshFileName>();
				sprintf_s(a_buffer, a_sizeOfBuffer, path.c_str(), a_worldSpace, a_worldSpace, a_scale, a_x, a_y);
			}

			static inline size_t size = 0x39;
			static inline std::uint64_t id = 31150;

			static inline const char* seasonalPath{ R"(Data\Meshes\Terrain\%s\Trees\%s.%i.%i.%i.{}.BTT)" };
			static inline const char* defaultPath{ R"(Data\Meshes\Terrain\%s\Trees\%s.%i.%i.%i.BTT)" };

			static inline auto type = LOD_TYPE::kTree;
		};

		struct BuildTextureFileName
		{
			static void func(char* a_buffer, std::uint32_t a_sizeOfBuffer, const char* a_worldSpace)
			{
				const auto path = detail::get_lod_filename<BuildTextureFileName>();
				sprintf_s(a_buffer, a_sizeOfBuffer, path.c_str(), a_worldSpace, a_worldSpace);
			}

			static inline size_t size = 0x1F;
			static inline std::uint64_t id = 31151;

			static inline const char* seasonalPath{ R"(Data\Textures\Terrain\%s\Trees\%sTreeLOD.{}.DDS)" };
			static inline const char* defaultPath{ R"(Data\Textures\Terrain\%s\Trees\%sTreeLOD.DDS)" };

			static inline auto type = LOD_TYPE::kTree;
		};

		struct BuildTypeListFileName
		{
			static void func(char* a_buffer, std::uint32_t a_sizeOfBuffer, const char* a_worldSpace)
			{
				const auto path = detail::get_lod_filename<BuildTypeListFileName>();
				sprintf_s(a_buffer, a_sizeOfBuffer, path.c_str(), a_worldSpace, a_worldSpace);
			}

			static inline size_t size = 0x1F;
			static inline std::uint64_t id = 31152;

			static inline const char* seasonalPath{ R"(Data\Meshes\Terrain\%s\Trees\%s.{}.LST)" };
			static inline const char* defaultPath{ R"(Data\Meshes\Terrain\%s\Trees\%s.LST)" };

			static inline auto type = LOD_TYPE::kTree;
		};

		inline void Install()
		{
			stl::asm_replace<BuildMeshFileName>();
			stl::asm_replace<BuildTextureFileName>();
			stl::asm_replace<BuildTypeListFileName>();
		}
	}

	inline void Install()
	{
		Terrain::Install();
		Object::Install();
		Tree::Install();
	}
}
