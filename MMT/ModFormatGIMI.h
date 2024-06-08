#pragma once
#include "MigotoFormat.h"
#include "MigotoParseUtil.h"
#include "D3d11GameType.h"

class ModFormat_GIMI_INI {
public:
	//��������
	std::vector<M_SectionLine> MigotoSectionLineList;

	//��������
	std::vector<M_Variable> Global_M_VariableList;
	std::vector<M_Key> Global_M_KeyList;
	std::vector<M_Resource> Global_M_ResourceList;
	std::vector<M_TextureOverride> Global_M_TextureOverrideList;

	//��һ������
	std::unordered_map<std::wstring, M_Resource> Global_ResourceName_Resource_Map;
	std::unordered_map<int, std::vector<M_Resource>>  VertexNumberMResourceMap;
	std::unordered_map<std::wstring, std::vector<M_TextureOverride>> Hash_TextureOverrideIBList_Map;

	ModFormat_GIMI_INI(std::wstring IniFilePath);
};


class SingleMod {
public:
	std::vector<M_Resource> OrderedResourceVBList;
	std::vector<M_TextureOverride> TextureOverrideIBList;
	D3D11GameType d3d11GameType;

	std::wstring OutputFolderGameTypePath;
	std::vector<std::string> ElementList;

	std::unordered_map<int, std::vector<byte>> VB0IndexNumberBytes;
	std::vector<byte> finalVB0Bytes;
	bool ValidMod = false;

	SingleMod(D3D11GameType d3d11GameTypeInput, std::vector<M_Resource> ResourceVBListInput, std::vector<M_TextureOverride> TextureOverrideIBListInput);
};