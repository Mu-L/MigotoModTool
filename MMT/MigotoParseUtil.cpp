#include "MigotoParseUtil.h"
#include "MMTLogUtils.h"
#include "MMTFormatUtils.h"
#include "MMTFileUtils.h"
#include <filesystem>


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

std::vector<std::vector<std::wstring>> cartesianProduct2(const std::vector<std::vector<std::wstring>>& data) {
    std::vector<std::vector<std::wstring>> result;
    std::vector<std::wstring> current;
    cartesianProductHelper2(data, current, result, 0);
    return result;
}

std::vector<std::unordered_map<std::wstring, std::wstring>> MigotoParseUtil_Get_M_Key_Combination(std::vector<M_Key> cycleKeyList) {
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



std::vector<M_DrawIndexed> MigotoParseUtil_GetActiveDrawIndexedListByKeyCombination(std::unordered_map<std::wstring, std::wstring> KeyCombinationMap, std::vector<M_DrawIndexed> DrawIndexedList) {
    std::vector<M_DrawIndexed> activitedDrawIndexedList;
    //����Ҫ��DrawIndex��ÿ��Condition������,�����Ǳ�������

    for (M_DrawIndexed drawIndexed : DrawIndexedList) {

        if (drawIndexed.AutoDraw) {
            //���û��������˵��ֱ�Ӿ����Ѽ����
            activitedDrawIndexedList.push_back(drawIndexed);
            //LOG.Info(L"Detect [auto draw drawindexed]");
        }
        else {
            //LOG.Info(L"ActiveConditionList size: " + std::to_wstring(drawIndexed.ActiveConditionList.size()));
            int activateNumber = 0;
            for (M_Condition condition : drawIndexed.ActiveConditionList) {
                //LOG.Info(condition.ConditionVarName);
                //�����condition��Ҫ��ȡһ��NameSapced KeyName
                std::wstring conditionNameSpacedKeyName = condition.NameSpace + L"\\" + condition.ConditionVarName.substr(1);
                if (condition.ConditionVarValue == KeyCombinationMap[conditionNameSpacedKeyName]) {
                    activateNumber++;
                }
                else {
                    break;
                }

            }
            if (activateNumber == drawIndexed.ActiveConditionList.size()) {
                activitedDrawIndexedList.push_back(drawIndexed);
                //LOG.Info(L"Activated");
            }
        }
       
    }
    //LOG.Error(L"Stop");
    return activitedDrawIndexedList;
}


void MigotoParseUtil_OutputIBFileByDrawIndexedList(
    std::wstring OriginalIBPath,std::wstring TargetIBPath,
    std::wstring IBReadFormat,std::vector<M_DrawIndexed> ActiveDrawIndexedList,
    int MinNumber) 
{

    std::wstring IBReadDxgiFormat = IBReadFormat;
    boost::algorithm::to_lower(IBReadDxgiFormat);
    std::wstring IBFilePath = OriginalIBPath;
    //TODO ��Ҫһ����ȡIB�ļ���Сֵ�����ֵ�ķ������������ã���������ֻ��Ҫ�õ���Сֵ
    int minNumber = MinNumber;


    int readLength = 2;
    if (IBReadDxgiFormat == L"dxgi_format_r16_uint") {
        readLength = 2;
    }
    if (IBReadDxgiFormat == L"dxgi_format_r32_uint") {
        readLength = 4;
    }
    std::ifstream ReadIBFile(IBFilePath, std::ios::binary);

    std::vector<uint16_t> IBR16DataList = {};
    std::vector<uint32_t> IBR32DataList = {};

    char* data = new char[readLength];

    while (ReadIBFile.read(data, readLength)) {
        if (IBReadDxgiFormat == L"dxgi_format_r16_uint") {
            std::uint16_t value = MMTFormat_CharArrayToUINT16_T(data);
            value = value - static_cast<std::uint16_t>(minNumber);
            IBR16DataList.push_back(value);

        }
        if (IBReadDxgiFormat == L"dxgi_format_r32_uint") {
            std::uint32_t value = MMTFormat_CharArrayToUINT32_T(data);
            value = value - static_cast<std::uint32_t>(minNumber);
            IBR32DataList.push_back(value);
        }
    }

    ReadIBFile.close();
    std::wstring outputIBFileName = TargetIBPath;

    if (IBReadDxgiFormat == L"dxgi_format_r16_uint") {
        IBR32DataList = std::vector<uint32_t>(IBR16DataList.size());
        std::transform(IBR16DataList.begin(), IBR16DataList.end(), IBR32DataList.begin(),
            [](uint16_t value) { return static_cast<uint32_t>(value); });
    }

    //����Ҫ����ActiveDrawIndexed����ȡ����Ҫд���Ĳ��ֶ��������еĲ���
    std::vector<uint32_t> Output_UINT32T_DataList;

    for (M_DrawIndexed drawIndexed : ActiveDrawIndexedList) {
        //LOG.Info(drawIndexed.DrawNumber);
        //LOG.Info(drawIndexed.DrawOffsetIndex);
        boost::algorithm::trim(drawIndexed.DrawNumber);
        boost::algorithm::trim(drawIndexed.DrawOffsetIndex);

        int drawNumber = std::stoi(drawIndexed.DrawNumber);
        int offsetNumber = std::stoi(drawIndexed.DrawOffsetIndex);
        LOG.Info(L"drawNumber" + std::to_wstring(drawNumber));
        LOG.Info(L"offsetNumber" + std::to_wstring(offsetNumber));

        std::vector<uint32_t> tmpList = MMTFormat_GetRange_UINT32T(IBR32DataList, offsetNumber, offsetNumber + drawNumber);
        Output_UINT32T_DataList.insert(Output_UINT32T_DataList.end(), tmpList.begin(), tmpList.end());
    }

    LOG.Info(L"Output_UINT32T_DataList Size: " + std::to_wstring(Output_UINT32T_DataList.size()));

    LOG.Info(L"IB file format: " + IBReadDxgiFormat);
    LOG.Info(L"IB file length: " + std::to_wstring(Output_UINT32T_DataList.size()));
    std::ofstream file(outputIBFileName, std::ios::binary);
    for (const auto& data : Output_UINT32T_DataList) {
        uint32_t paddedData = data;
        file.write(reinterpret_cast<const char*>(&paddedData), sizeof(uint32_t));
    }
    file.close();

}


std::wstring MigotoParseUtil_Get_M_Key_Combination_String(std::unordered_map<std::wstring, std::wstring> KeyCombinationMap) {
    std::wstring combinationStr;
    int count = 1;
    for (const auto& pair: KeyCombinationMap) {
        if (!MMTFile_IsValidFilename(MMTString_ToByteString(pair.first))) {
            //������ļ����޷�ʹ�õ��ַ����Կ������������ʹ����ֵ����
            combinationStr = combinationStr + L"$key" + std::to_wstring(count) + L"_";
        }
        else {
            combinationStr = combinationStr + L"$" + pair.first + L"_";
        }
        combinationStr = combinationStr + pair.second + L"_";
        count++;
    }


    //��Щ�˻����ļ����в��ܳ��ֵ��ַ�����\ /����Ϊ�������Կ��Զ�������ʱ�����UUID�����ļ�������(������)��
    //if (!MMTFile_IsValidFilename(MMTString_ToByteString(combinationStr))) {
    //    combinationStr = MMTString_GenerateUUIDW();
    //}
    return combinationStr;

}


std::vector<std::wstring> MigotoParseUtil_GetRecursiveActivedIniFilePathList(std::wstring IncludePath) {
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


std::vector<M_SectionLine> MigotoParseUtil_ParseMigotoSectionLineList(std::wstring iniFilePath) {
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