#pragma once
#include "GlobalConfigs.h"
//�������ֻ����ų�����ڵ��õķ�������Ҫ������ɾ������ÿ���޸����ﶼ��Ӱ�쵽���е�cpp�ļ����޸ĺ�ı���ʱ�䳤��1����

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

//ԭ��,����:�������,������,������(Unityϵ����Ϸ)		�ű�ʽ������ȡModģ��
void Reverse_Lv4_Merged_OutfitCompiler();
void Reverse_Lv4_Single();

//TODO Deprecated Reverse Method
void ReverseMerged();


