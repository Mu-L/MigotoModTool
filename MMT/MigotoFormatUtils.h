#pragma once
#include "GlobalConfigs.h"

//COLOR�ؼ��㣬ʹ�û�������ƽ����AverageNormal�ؼ��㷽��
std::unordered_map<std::wstring, std::vector<std::byte>> RecalculateColor(std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig, std::unordered_map<std::string, int> categoryStrideConfig);

//COLORֱֵ�Ӹ���
std::unordered_map<std::wstring, std::vector<std::byte>> ResetColor(std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig, std::unordered_map<std::string, int> categoryStrideConfig);

//TANGENTʹ�û���Vector��AverageNormal��һ���ؼ��㷽��
std::unordered_map<std::wstring, std::vector<std::byte>> TANGENT_averageNormal(std::unordered_map<std::wstring, std::vector<std::byte>> inputVBCategoryDataMap, D3D11GameType d3d11GameType);

//��תNORMAL��TANGENT��ֵ
std::unordered_map<std::wstring, std::vector<std::byte>> ReverseNormalTangentValues(std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig);
