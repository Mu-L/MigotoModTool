#pragma once
#include "FmtData.h"
#include "VertexBufferTxtFile.h"


class SplitUtil {
public:
	GlobalConfigs wheelConfig;
	ExtractConfig basicConfig;
	D3D11GameType d3d11GameType;

	std::wstring splitReadFolder;
	std::wstring splitOutputFolder;

	//��ȡ��Ȼ���滻COLOR��TANGENT��Ȼ�����ͷָ�ŵ����map��
	std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap;

	// TODO Ϊ��KDTree�㷨��������Ҫ��ȡʱ����partName����ʱ�ֿ���ȡ���ڼ������nearest_original֮��
	// ��������Ϸŵ�finalVBCategoryDataMap��
	std::unordered_map<std::string, std::unordered_map<std::wstring, std::vector<std::byte>>> partName_VBCategoryDaytaMap;
	
	//��Ҫһ��partName,offset��map������ib��ȡʹ��
	std::unordered_map<std::wstring, uint64_t> partNameOffsetMap;

	std::vector<std::string> CategoryList;
	std::unordered_map<std::string, int> CategoryStrideMap;

	int blendElementByteWidth = 0;
	uint64_t drawNumber = 0;

	//һ��partName��UUID��map������ib�ļ���ʹ��
	std::unordered_map<std::string, std::string> partNameUUIDMap;
	//һ��Category��UUID��map������buf�ļ���ʹ��
	std::unordered_map<std::string, std::string> categoryUUIDMap;

	void readSplitRecalculate();

	void outputModFiles();

	SplitUtil();
	SplitUtil(GlobalConfigs wheelConfig, ExtractConfig basicConfig, D3D11GameType d3d11GameType);
};