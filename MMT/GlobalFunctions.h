#pragma once
#include "GlobalConfigs.h"
//�������ֻ����ų�����ڵ��õķ���


//����
void ExtractFromWW();
void ExtractFromBuffer_CS_WW_Body(std::wstring DrawIB);
void ExtractFromBuffer_VS_WW_Object(std::wstring DrawIB);
void ExtractFromBuffer_VS_WW_ObjectType2(std::wstring DrawIB);
void Generate_CS_WW_Body();

//���׽��� SnowBreak
void ExtractFromBuffer_VS_SnB_SnB_Body(std::wstring DrawIB);
void ExtractFromSnB();
void Generate_VS_SnB();

//ԭ��,����:�������,������,������
void ExtractFromBuffer_VS();
void UnityGenerate();

//ԭ��,����:�������,������,������		Dumpʽ������ȡModģ��
void ExtractFromBuffer_VS_Reverse();

//ԭ��,����:�������,������,������		�ű�ʽ������ȡModģ��
void ReverseOutfitCompilerCompressed();
void ReverseSingle();
void ReverseMerged();


