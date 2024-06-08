#pragma once
#include <iostream>
#include "MMTStringUtils.h"
#include "MMTLogUtils.h"
#include <boost/algorithm/string.hpp>


class IniLineObject {
public:
	std::wstring LeftStr;
	std::wstring LeftStrTrim;
	std::wstring RightStr;
	std::wstring RightStrTrim;
	bool valid = false;

	IniLineObject() {}

	IniLineObject(std::wstring readLine) {
		int firstDoubleEqualIndex = (int)readLine.find(L"==");
		int firstEqualIndex = (int)readLine.find(L"=");
		//LOG.Info(L"firstDoubleEqualIndex: " + std::to_wstring(firstDoubleEqualIndex));
		//LOG.Info(L"firstEqualIndex: " + std::to_wstring(firstEqualIndex));
		//Ĭ��ʹ��==�����==�Ҳ������Ǿͻ���=
		std::wstring delimiter = L"==";
		if ( firstDoubleEqualIndex == std::wstring::npos) {
			delimiter = L"=";
		}

		//�ҵ���==�����ҵ���=�����Խ���
		if (firstEqualIndex != std::wstring::npos || firstDoubleEqualIndex != std::wstring::npos) {
			std::vector<std::wstring> lowerReadLineSplitList = MMTString_SplitString(readLine, delimiter);
			if (lowerReadLineSplitList.size() < 2) {
				LOG.Error(L"lowerReadLineSplitList size is " + std::to_wstring(lowerReadLineSplitList.size()) + L",please check!");
			}
			std::wstring leftStr = lowerReadLineSplitList[0];
			std::wstring rightStr = lowerReadLineSplitList[1];
			//LOG.Info(L"leftStr:" + leftStr);
			//LOG.Info(L"rightStr:" + rightStr);
			LeftStr = leftStr;
			RightStr = rightStr;
			boost::algorithm::trim(leftStr);
			boost::algorithm::trim(rightStr);
			LeftStrTrim = leftStr;
			RightStrTrim = rightStr;
			valid = true;
		}
		else {
			LeftStr = L"";
			RightStr = L"";
			LeftStrTrim = L"";
			RightStrTrim = L"";
			valid = false;
		}
	}

	//�����������ָ���ָ����������Ǹ������жϱ�����ص�ʱ��
	IniLineObject(std::wstring readLine, std::wstring delimiter) {
		int firstEqualIndex = (int)readLine.find_first_of(delimiter);
		if (firstEqualIndex != std::wstring::npos) {
			std::vector<std::wstring> lowerReadLineSplitList = MMTString_SplitString(readLine, delimiter);
			std::wstring leftStr = lowerReadLineSplitList[0];
			std::wstring rightStr = lowerReadLineSplitList[1];
			LeftStr = leftStr;
			RightStr = rightStr;
			boost::algorithm::trim(leftStr);
			boost::algorithm::trim(rightStr);
			LeftStrTrim = leftStr;
			RightStrTrim = rightStr;
			valid = true;
		}
		else {
			LeftStr = L"";
			RightStr = L"";
			LeftStrTrim = L"";
			RightStrTrim = L"";
			valid = false;
		}
	}
};


class M_SectionLine {
public:
	std::wstring NameSpace;
	std::wstring SectionName;
	std::vector<std::wstring> SectionLineList;

	M_SectionLine() {}
};


//ͬʱ����constants��present
class M_Variable {
public:
	std::wstring NameSpace;
	std::wstring VariableName;
	std::wstring InitializeValue = L"";
	std::wstring NamespacedVarName;
	std::wstring Type; //global local normal

	M_Variable() {

	}

	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InType) {
		this->NameSpace = InNameSpace;
		this->VariableName = InVariableName;
		this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
		this->Type = InType;
	}

	M_Variable(std::wstring InNameSpace, std::wstring InVariableName,std::wstring InInitializeValue, std::wstring InType) {
		this->NameSpace = InNameSpace;
		this->VariableName = InVariableName;
		this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
		this->InitializeValue = InInitializeValue;
		this->Type = InType;
	}
};


class M_Condition {
public:
	std::wstring NameSpace;
	std::wstring ConditionVarName;
	std::wstring ConditionVarValue;

	M_Condition() {}

	M_Condition(std::wstring InNameSpace,std::wstring InConditionVarName,std::wstring InConditionVarValue){
		this->NameSpace = InNameSpace;
		this->ConditionVarName = InConditionVarName;
		this->ConditionVarValue = InConditionVarValue;
	}
};


class M_Key {
public:
	std::wstring NameSpace;
	M_Condition Condition;
	std::wstring KeyName;
	std::wstring BackName;
	std::wstring Type;

	//���� $variable5 = 0,1 �����������֮�����$variable5��ֵ��0,1֮��ѭ���л�����һ����������ѭ������
	std::unordered_map<std::wstring,std::vector<std::wstring>> CycleVariableName_PossibleValueList_Map;

	//����$creditinfo = 0 �����������֮������̼����$creditinfo��Ϊ0�������Ǽ������
	std::unordered_map<std::wstring,std::wstring> ActiveVariableName_ActiveValue_Map;
};

class M_DrawIndexed {
public:
	//����DrawIndexed = 6,12,0 ��ʾDrawStartIndexΪ0��DrawOffsetIndexΪ12��DrawNumberΪ6
	//��������0��ʼ���ƣ���Offset��ʼ��ȡ����ȡDrawNumber������������
	bool AutoDraw = false;
	std::wstring DrawNumber;
	std::wstring DrawOffsetIndex;
	std::wstring DrawStartIndex;

	std::vector<M_Condition> ActiveConditionList;

	M_DrawIndexed(){}
};


class M_ResourceReplace {
public:
	std::wstring ReplaceTarget;
	std::wstring ReplaceResource;
	std::vector<M_Condition> ActiveConditionList;

};

class M_TextureOverride {
public:
	std::wstring NameSpace;
	std::wstring IndexBufferHash;
	std::vector<M_ResourceReplace> ResourceReplaceList;
	std::wstring MatchPriority;
	std::wstring Handling; //skip
	std::wstring MatchFirstIndex;
	std::vector<M_DrawIndexed> DrawIndexedList;

	//ƴ�ӵ�
	std::wstring IBFileName;
	std::wstring IBFormat;
};


class M_Resource {
public:
	std::wstring NameSpace = L"";
	std::wstring ResourceName = L"";
	std::wstring Type = L"";
	std::wstring Stride = L"";
	std::wstring Format = L"";
	std::wstring FileName = L"";

	//����ƴ�ӳ��������ԣ�����NameSpace��FileName
	std::wstring FilePath = L"";
};


class MigotoActiveMod {
public:
	
	//���ȵ�����Դ�б�ɣ�ResourceList��������Դ���ƶ�ȡ��Ӧ�����ϵ���Դ
	std::vector<M_Resource> ResourceList;

	//��ε���Key�б��
	std::vector<M_Key> KeyList;

};


std::vector<M_Variable> parseConstantsSection(M_SectionLine m_sectionLine);

M_Key parseKeySection(M_SectionLine m_sectionLine);

M_Resource parseResourceSection(M_SectionLine m_sectionLine);

M_TextureOverride parseTextureOverrideSection(M_SectionLine m_sectionLine);


