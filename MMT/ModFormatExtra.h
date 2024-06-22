#pragma once
//-----------------------------------------------------------------------------------------------------------------------------------
// �����ŵ��Ǹ��߼��ĳ����������ͣ���Ҫ���ڽ���3Dmigoto��Mod��ʽ
//-----------------------------------------------------------------------------------------------------------------------------------
#include "D3d11GameType.h"
#include <iostream>
#include "MMTStringUtils.h"
#include "MMTLogUtils.h"
#include <boost/algorithm/string.hpp>
#include "IndexBufferBufFile.h"
#include "ModFormatBasic.h"


class SingleModDetect {
public:
	std::wstring HashValue;
	std::vector<M_TextureOverride> TextureOverrideIBList;
	std::vector<M_Resource> ResourceVBList;

	SingleModDetect() {}
};


class ModFormat_INI {
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

	//���ڽ����������ݵķ���
	std::vector<M_Variable> Parse_Basic_ConstantsSection(M_SectionLine m_sectionLine);
	M_Key Parse_Basic_KeySection(M_SectionLine m_sectionLine);
	M_Resource Parse_Basic_ResourceSection(M_SectionLine m_sectionLine);
	M_TextureOverride Parse_Basic_TextureOverrideSection(M_SectionLine m_sectionLine);

	//�����Խ�������һ��Ҫʹ��
	std::vector<std::unordered_map<std::wstring, std::wstring>> Parse_Util_Get_M_Key_Combination(std::vector<M_Key> cycleKeyList);
	std::wstring Parse_Util_Get_M_Key_Combination_String(std::unordered_map<std::wstring, std::wstring> KeyCombinationMap);
	std::vector<std::wstring> Parse_Util_GetRecursiveActivedIniFilePathList(std::wstring IncludePath);
	std::vector<M_SectionLine> Parse_Util_ParseMigotoSectionLineList(std::wstring iniFilePath);

	//���Ҹ�ֵ�����������õ�
	void Parse_Self_VertexNumberMResourceVBMap();
	void Parse_Self_Hash_TextureOverrideIBList_Map();
	void Parse_Self_Hash_SingleModDetect_Map();

	//����ini·�����������������еĻ�������
	ModFormat_INI(std::wstring IniFilePath);
};




//�������Mod
//TODO 
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