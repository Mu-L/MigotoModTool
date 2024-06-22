#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "ModFormatExtra.h"
#include "GlobalFunctions.h"
#include "IndexBufferBufFile.h"
#include "FmtData.h"

// This function is designed for any kinds of 3Dmigoto mod
// it will simulate 3Dmigoto's ini parsing logic and resource loading logic
// to fully simulate how a mod is loaded and running in 3Dmigoto
// then it try to extract every single mod it can extract from all files in target folder

// Notice:This method may take a lot of time if you use it in large mod folder.
//TODO Lv3�Ѿ���ʱ��,������֮ǰ��Ҫ��Toggle�Ľ���֧��
void Reverse_Lv4_3Dmigoto_Simulator() {
    LOG.Info("Start to batch reverse by 3Dmigoto Simulator.");

    //��Release�У�Ҫ��json�ļ��ж�ȡ����ini��·��
    json reverseJsonObject = MMTJson_ReadJsonFromFile(G.Path_RunInputJson);
    std::wstring reverseFilePath = MMTString_ToWideString(reverseJsonObject["ReverseFilePath"]);

    //�����������·��
    std::wstring reverseFolderPath = MMTString_GetFolderPathFromFilePath(reverseFilePath) + L"-Reverse\\";
    std::filesystem::create_directories(reverseFolderPath);
    LOG.Info(L"ReverseFilePath: " + reverseFilePath);
    LOG.NewLine();

    //TODO �ݹ��ȡĿ¼����.ini��β���ļ�·���б�

    //TODO 

}