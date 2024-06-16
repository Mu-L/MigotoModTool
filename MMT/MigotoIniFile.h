#pragma once

#include "GlobalConfigs.h"


class MigotoIniFile {
public:
    GlobalConfigs wheelConfig;
    ExtractConfig basicConfig;
    D3D11GameType d3d11GameType;
    std::wstring splitOutputFolder;
    std::vector<std::string> CategoryList;
    std::unordered_map<std::string, int> CategoryStrideMap;
    int blendElementByteWidth = 0;
    uint64_t drawNumber = 0;

    //һ��partName��UUID��map������ib�ļ���ʹ��
    std::unordered_map<std::string, std::string> partNameUUIDMap;

    //һ��Category��UUID��map������buf�ļ���ʹ��
    std::unordered_map<std::string, std::string> categoryUUIDMap;




    //ģ��GIMI����ini��ʽ
    void generateINIFileGIMI();


    MigotoIniFile();
    MigotoIniFile(GlobalConfigs wheelConfig, ExtractConfig basicConfig, D3D11GameType d3d11GameType);
};

