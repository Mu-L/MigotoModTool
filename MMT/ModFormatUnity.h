#pragma once
#include "MigotoFormat.h"
#include "MigotoParseUtil.h"
#include "D3d11GameType.h"


class SingleModDetect {
public:
	std::wstring HashValue;
	std::vector<M_TextureOverride> TextureOverrideIBList;
	std::vector<M_Resource> ResourceVBList;

	SingleModDetect() {}
};


class ModFormat_Unity_INI {
public:
	//��������ini�����в�����ΪSection
	std::vector<M_SectionLine> MigotoSectionLineList;

	//�����������е�Sectionת��Ϊ�����Ļ�������
	std::vector<M_Variable> Global_M_VariableList;
	std::vector<M_Key> Global_M_KeyList;
	std::vector<M_Resource> Global_M_ResourceList;
	std::vector<M_TextureOverride> Global_M_TextureOverrideList;
	std::unordered_map<std::wstring, M_Resource> Global_ResourceName_Resource_Map;

	//��һ������,�м�������������ÿ�����趼�õ���
	std::unordered_map<uint32_t, std::vector<M_Resource>> VertexNumberMResourceVBMap;
	std::unordered_map<std::wstring, std::vector<M_TextureOverride>> Hash_TextureOverrideIBList_Map;

	//���ݶ�������ƥ���һ��Mod������װ��Map�������ʹ�ã��ܳ���
	std::unordered_map <std::wstring, SingleModDetect> Hash_SingleModDetect_Map;

	void Parse_VertexNumberMResourceVBMap();
	void Parse_Hash_TextureOverrideIBList_Map();
	void Parse_Hash_SingleModDetect_Map();

	//����ini·�����������������еĻ�������
	ModFormat_Unity_INI(std::wstring IniFilePath);
};




//�������Mod
class ValidModDetect {
public:
	std::vector<M_Resource> OrderedResourceVBList;
	std::vector<M_TextureOverride> TextureOverrideIBList;
	D3D11GameType d3d11GameType;

	std::wstring OutputFolderGameTypePath;
	std::vector<std::string> ElementList;

	//����һ��Ҫע�⣬�����Ǵ�1��ʼ�ģ������Ǵ�0��ʼ��
	std::unordered_map<int, std::vector<byte>> VB0IndexNumberBytes;
	std::vector<byte> finalVB0Bytes;
	bool ValidMod = false;


	//������װһ�����������Mod�������װ�ɹ���ValidMod����Ϊtrue
	ValidModDetect(D3D11GameType d3d11GameTypeInput, std::vector<M_Resource> ResourceVBListInput, std::vector<M_TextureOverride> TextureOverrideIBListInput);
};