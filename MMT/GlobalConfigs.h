#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <Windows.h>

#include <clocale>
#include <codecvt> 
#include <chrono>
#include <cmath>
#include <filesystem>
#include <map>
#include <unordered_map>

// algorithm

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/string.hpp>
// date_time
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/local_time/local_time_io.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

// property_tree
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "json.hpp"
#include "D3d11GameType.h"

#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MMTFormatUtils.h"
#include "MMTJsonUtils.h"
#include "MMTLogUtils.h"



using json = nlohmann::json;



class ExtractConfig {
public:

	//2.ͨ����Ŀ
	std::wstring DrawIB;
	std::wstring GameType;

	//bool ReverseNormal;
	bool UseDefaultColor = false;

	//4.Split����
	//��Щ�Ӷ�Ӧ��Ϸ���Ƶ������ļ�����ȡ
	std::unordered_map <std::string, std::string> TextureMap;
	std::unordered_map <std::string, std::string> ColorMap;
	std::wstring TangentAlgorithm;
	std::wstring ColorAlgorithm;

	//Mod��������
	std::wstring SwitchKey;

	bool NormalReverseX = false;
	bool NormalReverseY = false;
	bool NormalReverseZ = false;

	bool TangentReverseX = false;
	bool TangentReverseY = false;
	bool TangentReverseZ = false;
	bool TangentReverseW = false;

	//������Merge��ʱ�򱣴浽tmp.json��Ȼ��Split��ʱ���hash.json����ȡ
	std::unordered_map <std::string, std::string> CategoryHashMap;
	std::vector<std::string> MatchFirstIndexList;
	std::vector<std::string> PartNameList;
	std::vector<std::string> TmpElementList;
	std::string VertexLimitVB;
	std::string WorkGameType;

	void saveTmpConfigs(std::wstring outputPath);
};



//��������ʱ�����е�����Ӧ��������ȫ����ʼ�����
class GlobalConfigs {
public:
	//�����õ�������·�����������ȷźã�����ֱ����
	std::wstring Path_MainJson;
	std::wstring Path_Game_Folder;
	std::wstring Path_Game_3DmigotoFolder;
	std::wstring Path_Game_TypesFolder;
	std::wstring Path_RunInputJson;
	std::wstring Path_Game_VSCheckJson;
	std::wstring Path_Game_ConfigJson;
	std::wstring Path_Game_SettingJson;

	//��ǰ����λ��
	std::wstring ApplicationRunningLocation = L"";

	//1.�ȶ�ȡ��Ϸ����
	std::wstring GameName;

	std::wstring FrameAnalyseFolder;
	std::wstring LoaderFolder;
	std::wstring OutputFolder;
	std::wstring ShaderCheckConfigLocation;

	std::wstring RunCommand;

	//�Ƿ���ҪVertexShaderCheck
	bool VertexShaderCheck = false;
	//��Ҫcheck��VertexShader��λ��һ��Ϊvb1,ib
	//std::vector<std::string> VertexShaderCheckList = { "vb1","ib" };
	std::vector<std::wstring> VertexShaderCheckList;
	std::wstring VertexShaderCheckListString;


	std::unordered_map<std::wstring, std::wstring> GIMIPartNameAliasMap;


	//����ʱ���ú���Ҫ�õ����ļ���
	std::wstring WorkFolder;

	//�������Merge��Split��Ԥ������
	std::unordered_map<std::wstring, ExtractConfig> DrawIB_ExtractConfig_Map;

	//ApplicationLocation,
	GlobalConfigs();

	GlobalConfigs(std::wstring);

	//��ǰ��Ϸ���͵�ElementAttribute�б�����ƥ���Զ�ʶ��GameType
	std::vector<D3D11GameType> CurrentD3d11GameTypeList;
	std::unordered_map<std::string, D3D11GameType> GameTypeName_D3d11GameType_Map;

};

//ȫ������
extern GlobalConfigs G;





