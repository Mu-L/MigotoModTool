#pragma once
#include "GlobalConfigs.h"
//�������ֻ����ų�����ڵ��õķ���

//����
void ExtractFromWW();
void ExtractFromBuffer_CS_WW_Body(std::wstring DrawIB, std::wstring GameType);
void ExtractFromBuffer_VS_WW_Object(std::wstring DrawIB, std::wstring GameType);
void Generate_CS_WW_Body();

//���׽��� SnowBreak
void ExtractFromBuffer_VS_UE4(std::wstring DrawIB, std::wstring GameType);
void Extract_VS_UE4();
void Generate_VS_UE4();

//ԭ��,����:�������,������,������
void ExtractFromBuffer_VS();
void UnityGenerate();

//ԭ��,����:�������,������,������		Dumpʽ������ȡModģ��
void ExtractFromBuffer_VS_Reverse();

//ԭ��,����:�������,������,������		�ű�ʽ������ȡModģ��
void ReverseOutfitCompilerCompressed();
void ReverseSingle();
void ReverseMerged();


