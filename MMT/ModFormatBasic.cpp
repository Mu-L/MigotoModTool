#include "ModFormatBasic.h"
#include <iostream>


//--------------------------------------------------------------------------------------------------------------------
M_Condition::M_Condition() {

}


//����if����else if����������жϲ����ַ������������������漰�ı������߼�
M_Condition::M_Condition(std::wstring ConditionStr) {
	LOG.NewLine();
	LOG.Info(L"Start to parse condition expression for: " + ConditionStr);
	//���Ȳ����Ƿ���&& ���� || �������ͷ��ţ�����ҵ����ǾͰ�������д������򰴵������д���
	bool singleExpression = true;
	if (ConditionStr.find(L"&&") != std::wstring::npos) {
		singleExpression = false;
	}
	else if (ConditionStr.find(L"||") != std::wstring::npos) {
		singleExpression = false;
	}

	if (singleExpression) {
		LOG.Info("Can't find && or || in ConditionStr, take it as a single expression condition.");
		//����͵����ʹ��= �� ==�ָ����ָ������ConditionVarValue��û���ٳ���== !=���߱���$����Ķ�����˵����������
		IniLineObject conditionLine(ConditionStr);
		this->Condition_VarName_VarValue_Map[conditionLine.LeftStrTrim] = conditionLine.RightStrTrim;
	}
	else {
		LOG.Info("find && or || in ConditionStr, take it as a multiple expression condition.");

	}
    

    //���ConditionVarValue���Ƿ��б��������߼����ŵȣ����и����ӵĽ����жϡ�
	LOG.Info(L"Parse condition expression over.");
	LOG.NewLine();
}

void M_Condition::show() {

}


//--------------------------------------------------------------------------------------------------------------------
IniLineObject::IniLineObject() {

}

IniLineObject::IniLineObject(std::wstring readLine) {
	int firstDoubleEqualIndex = (int)readLine.find(L"==");
	int firstEqualIndex = (int)readLine.find(L"=");
	//LOG.Info(L"firstDoubleEqualIndex: " + std::to_wstring(firstDoubleEqualIndex));
	//LOG.Info(L"firstEqualIndex: " + std::to_wstring(firstEqualIndex));
	//Ĭ��ʹ��==�����==�Ҳ������Ǿͻ���=
	std::wstring delimiter = L"==";
	if (firstDoubleEqualIndex == std::wstring::npos) {
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
IniLineObject::IniLineObject(std::wstring readLine, std::wstring delimiter) {
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


//--------------------------------------------------------------------------------------------------------------------
M_SectionLine::M_SectionLine() {

}


//--------------------------------------------------------------------------------------------------------------------
M_Variable::M_Variable() {

}


M_Variable::M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InType) {
	this->NameSpace = InNameSpace;
	this->VariableName = InVariableName;
	this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
	this->Type = InType;
}


M_Variable::M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InInitializeValue, std::wstring InType) {
	this->NameSpace = InNameSpace;
	this->VariableName = InVariableName;
	this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
	this->InitializeValue = InInitializeValue;
	this->Type = InType;
}


//--------------------------------------------------------------------------------------------------------------------
M_DrawIndexed::M_DrawIndexed() {

}

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------