#include "ModFormatExtra.h"
#include "MMTFileUtils.h"
#include <filesystem>

//�Ŷ��ϴ��룬GPTд��
void cartesianProductHelper2(const std::vector<std::vector<std::wstring>>& data, std::vector<std::wstring>& current, std::vector<std::vector<std::wstring>>& result, size_t index) {
    if (index >= data.size()) {
        result.push_back(current);
        return;
    }

    for (const auto& str : data[index]) {
        current.push_back(str);
        cartesianProductHelper2(data, current, result, index + 1);
        current.pop_back();
    }
}


//�Ŷ��ϴ��룬GPTд��
std::vector<std::vector<std::wstring>> cartesianProduct2(const std::vector<std::vector<std::wstring>>& data) {
    std::vector<std::vector<std::wstring>> result;
    std::vector<std::wstring> current;
    cartesianProductHelper2(data, current, result, 0);
    return result;
}


std::vector<std::unordered_map<std::wstring, std::wstring>> ModFormat_INI::Parse_Util_Get_M_Key_Combination(std::vector<M_Key> cycleKeyList) {
    LOG.Info(L"Start to calculateKeyCombination");
    std::vector<std::unordered_map<std::wstring, std::wstring>> keyCombinationDictList;

    std::vector<std::vector<std::wstring>> varValuesList;
    std::vector<std::wstring> varNameList;

    for (M_Key cycleKey : cycleKeyList) {

        if (cycleKey.CycleVariableName_PossibleValueList_Map.size() != 0) {

            for (const auto& pair : cycleKey.CycleVariableName_PossibleValueList_Map) {
                LOG.Info(L"Key: " + pair.first);

                std::vector<std::wstring> trueVarValueList;
                for (std::wstring varValue : pair.second) {
                    LOG.Info(L"Value: " + varValue);
                    trueVarValueList.push_back(varValue);
                }
                varValuesList.push_back(trueVarValueList);
                varNameList.push_back(pair.first);
            }
        }
    }

    std::vector<std::vector<std::wstring>> cartesianProductList = cartesianProduct2(varValuesList);
    for (std::vector<std::wstring> keyCombinationValueList : cartesianProductList) {
        std::unordered_map<std::wstring, std::wstring> tmpDict;
        for (int i = 0; i < keyCombinationValueList.size(); i++) {
            std::wstring cycleVarValue = keyCombinationValueList[i];
            std::wstring cycleVarName = varNameList[i];
            //LOG.LogOutput(cycleVarName + L" " + cycleVarValue);
            tmpDict[cycleVarName] = cycleVarValue;
        }
        keyCombinationDictList.push_back(tmpDict);
        //LOG.LogOutputSplitStr();
    }
    //LOG.LogOutputSplitStr();
    return keyCombinationDictList;
}


std::wstring ModFormat_INI::Parse_Util_Get_M_Key_Combination_String(std::unordered_map<std::wstring, std::wstring> KeyCombinationMap) {
    std::wstring combinationStr;
    int count = 1;
    for (const auto& pair : KeyCombinationMap) {
        if (!MMTFile_IsValidFilename(MMTString_ToByteString(pair.first))) {
            //��Щ�˻����ļ����в��ܳ��ֵ��ַ�����\ /����Ϊ������ֵ���Կ��Զ�����
            //������ļ����޷�ʹ�õ��ַ����Կ������������ʹ����ֵ����
            //TODO δ���ٲ���
            combinationStr = combinationStr + L"$key" + std::to_wstring(count) + L"_";
        }
        else {
            combinationStr = combinationStr + L"$" + pair.first + L"_";
        }
        combinationStr = combinationStr + pair.second + L"_";
        count++;
    }



    return combinationStr;

}


std::vector<std::wstring> ModFormat_INI::Parse_Util_GetRecursiveActivedIniFilePathList(std::wstring IncludePath) {
    std::vector<std::wstring> includeFilePathList = MMTFile_GetFilePathListRecursive(IncludePath);
    std::vector<std::wstring> parseIniFilePathList;
    for (const auto& filePath : includeFilePathList)
    {
        std::filesystem::path filePathObject(filePath);
        std::wstring fileName = filePathObject.filename().wstring();

        std::wstring lowerFileName = MMTString_ToLowerCase(fileName);
        if (lowerFileName.starts_with(L"disabled")) {
            continue;
        }
        if (!lowerFileName.ends_with(L".ini")) {
            continue;
        }
        parseIniFilePathList.push_back(filePath);
        //LOG.Info(filePath);
    }
    //LOG.NewLine();
    return parseIniFilePathList;
}


std::vector<M_SectionLine> ModFormat_INI::Parse_Util_ParseMigotoSectionLineList(std::wstring iniFilePath) {
    std::vector<M_SectionLine> migotoSectionLineList;

    //���Ȱ����ini�ļ���ÿһ�ж�ȡ���б���
    std::vector<std::wstring> readLineList = MMTFile_ReadIniFileLineList(iniFilePath);
    //��ʼ��Ĭ��NameSpaceΪ��ǰ�ļ�����Ŀ¼
    std::wstring defaultNameSpace = MMTString_GetFolderPathFromFilePath(iniFilePath);
    //�����ȡ����ǿ�����õ�NameSpace���ͷŵ����﹩����ʹ��
    std::wstring specifiedNameSpace = L"";

    //֮ǰ�������ÿ����һ������������һ���������ͣ���������ֱ�Ӱ�Section����
    //��ȡÿһ�У��ж��Ƿ�Ϊ[��ͷ���Ǿͽ���Section��ȡ���򣬷���Ͷ�ȡ�Ƿ�Ϊnamespace
    //Ȼ���ȡ��Section֮���أ��Ȳ������ȰѴ�section���е��зŵ�һ��section�����Ȼ������section����ŵ��б��ȴ���������
    std::vector<std::wstring> tmpSectionLineList;
    M_SectionLine lastMigotoSectionLine;
    bool inSection = false;
    for (std::wstring readLine : readLineList) {
        //LOG.LogOutput(L"Parsing: " + readLine);
        std::wstring lowerReadLine = boost::algorithm::to_lower_copy(readLine);
        boost::algorithm::trim(lowerReadLine);
        //����ע��
        if (lowerReadLine.starts_with(L";")) {
            continue;
        }

        //�ڶ�ȡ����ini�ļ�ʱ�������е�Section��ȡ֮ǰ�����ȡ����namespace�����ini�ļ����е�namespace������Ϊָ����namespace
        if (!inSection && lowerReadLine.starts_with(L"namespace")) {
            std::vector<std::wstring> readLineSplitList = MMTString_SplitString(readLine, L"=");
            if (readLineSplitList.size() < 2) {
                LOG.Error(L"Invalid namespace assign.");
            }
            //namespace = xxx������ֻ���ǳ���һ���Ⱥŵ�������������������̶�Ϊ1
            std::wstring RightStr = readLineSplitList[1];
            //ȥ�����߿ո�
            boost::algorithm::trim(RightStr);
            specifiedNameSpace = RightStr;
        }
        else if (lowerReadLine.starts_with(L"[")) {
            //ÿ����һ��[��˵���������µ�section
            inSection = true;

            //�����µ�SectionҪ�Ѿɵ�Section�ӵ��ܵ�Section�б���
            if (tmpSectionLineList.size() != 0) {
                lastMigotoSectionLine.SectionLineList = tmpSectionLineList;
                //LOG.LogOutput(L"Add Size: " + std::to_wstring(lastMigotoSectionLine.SectionLineList.size()));
                migotoSectionLineList.push_back(lastMigotoSectionLine);
                //Ȼ����յ�ǰ���б�׼����ȡ�µġ�
                tmpSectionLineList.clear();
            }

            //Ȼ������µ�
            lastMigotoSectionLine = M_SectionLine();
            lastMigotoSectionLine.NameSpace = defaultNameSpace;
            if (specifiedNameSpace != L"") {
                lastMigotoSectionLine.NameSpace = specifiedNameSpace;
            }
            std::wstring sectionName = lowerReadLine.substr(1, lowerReadLine.length() - 2);
            LOG.Info(L"SectionName: " + sectionName + L" NameSpace: " + lastMigotoSectionLine.NameSpace);
            lastMigotoSectionLine.SectionName = sectionName;
            //�����˰ѵ�ǰ��SectionName����Ҳ�����ȥ
            //�����֮ǰ�����жϲ�Ϊ��
            if (lowerReadLine != L"") {
                tmpSectionLineList.push_back(readLine);
            }
        }
        else if (inSection) {
            if (lowerReadLine != L"") {
                tmpSectionLineList.push_back(readLine);
            }
        }

    }

    // ����βʱ�����һ��Section��lineҲ��ӽ�ȥ
    //�����µ�SectionҪ�Ѿɵ�Section�ӵ��ܵ�Section�б���
    if (lastMigotoSectionLine.SectionLineList.size() != 0) {
        lastMigotoSectionLine.SectionLineList = tmpSectionLineList;
        //LOG.LogOutput(L"Add Size: " + std::to_wstring(lastMigotoSectionLine.SectionLineList.size()));
        migotoSectionLineList.push_back(lastMigotoSectionLine);
        //Ȼ����յ�ǰ���б�׼����ȡ�µġ�
        tmpSectionLineList.clear();
    }

    LOG.Info(L"MigotoSectionLineList Size: " + std::to_wstring(migotoSectionLineList.size()));
    LOG.NewLine();
    return migotoSectionLineList;
}

//--

