#include "MMTLogUtils.h"
#include "MMTStringUtils.h"
#include "MMTJsonUtils.h"


MMTLogger::MMTLogger() {

}


MMTLogger::MMTLogger(std::wstring ApplicationLocation) {
    this->RunResultJsonPath = ApplicationLocation + L"\\Configs\\RunResult.json";
}


void MMTLogger::Info(std::wstring str) {
    //std::wcout << "[DebugInfo]:" << str << "\n";
    LOG(INFO) << MMTString_ToByteString(str);
}
void MMTLogger::Info(std::string str) {
    //std::wcout << "[DebugInfo]:" << str << "\n";
    LOG(INFO) << str;
}

void MMTLogger::Warning(std::wstring str) {
    std::wcout << L"[Warning]:" << str << "\n";
    LOG(INFO) << MMTString_ToByteString(L"[Warning]:" + str);
}
void MMTLogger::Warning(std::string str) {
    std::cout << "[Warning]:" << str << "\n";
    LOG(INFO) << "[Warning]:" + str;
}

void MMTLogger::SaveResultJson(std::wstring str) {
    nlohmann::json runResultJson;
    runResultJson["result"] = MMTString_ToByteString(str);
    MMTJson_SaveToJsonFile(this->RunResultJsonPath, runResultJson);
}
void MMTLogger::SaveResultJson(std::string str) {
    nlohmann::json runResultJson;
    runResultJson["result"] = str;
    MMTJson_SaveToJsonFile(this->RunResultJsonPath, runResultJson);
}

void MMTLogger::Error(std::wstring str) {
    //�ѽ��д��json�ļ�����
    SaveResultJson(str);
    std::wcout << L"[Error]:" << str << "\n";
    LOG(INFO) << MMTString_ToByteString(L"[Error]:" + str);
    exit(1);
}
void MMTLogger::Error(std::string str) {
    //�ѽ��д��json�ļ�����
    SaveResultJson(str);
    std::cout << "[Error]:" << str << "\n";
    LOG(INFO) << "[Error]:" + str;
    exit(1);
}

void MMTLogger::Success() {
    //�ѽ��д��json�ļ�����
    SaveResultJson(L"success");
    LOG(INFO) << MMTString_ToByteString(L"Run complete! Success!");
}


void MMTLogger::NewLine() {
    Info(L"--------------------------------------------------------------------");
}
