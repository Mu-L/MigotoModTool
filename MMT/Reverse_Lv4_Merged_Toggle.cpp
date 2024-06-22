#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "ModFormatExtra.h"
#include "GlobalFunctions.h"
#include "IndexBufferBufFile.h"
#include "FmtData.h"


//TODO Lv3�Ѿ���ʱ��,������֮ǰ��Ҫ������&& �� || �Ľ���֧��
//TODO �޸���ͼ���ɵ�λ����.ib��.vb�ļ��Աߡ�
//TODO ��Щ��ͼ�ǲ����ݱ����仯�ģ�ҲҪһ���ƣ�����ʹ�á�
//TODO �ǵ����CommandList�Ľ�������Ϊ��Щ��ʹ��CommandList�����Ҫִ�е����
void Reverse_Lv4_Merged_Toggle() {
    LOG.Info("Start to reverse merged toggle mod.");
    
    //��Release�У�Ҫ��json�ļ��ж�ȡ����ini��·��
    json reverseJsonObject = MMTJson_ReadJsonFromFile(G.Path_RunInputJson);
    std::wstring reverseFilePath = MMTString_ToWideString(reverseJsonObject["ReverseFilePath"]);

    //�����������·��
    std::wstring reverseFolderPath = MMTString_GetFolderPathFromFilePath(reverseFilePath) + L"-Reverse\\";
    std::filesystem::create_directories(reverseFolderPath);
    LOG.Info(L"ReverseFilePath: " + reverseFilePath);
    LOG.NewLine();

    //��ʼ��ͨ��ini��������
    ModFormat_INI modFormatUnity(reverseFilePath);


}