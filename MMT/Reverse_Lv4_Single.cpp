#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "ModFormatExtra.h"
#include "GlobalFunctions.h"
#include "IndexBufferBufFile.h"
#include "FmtData.h"


void Reverse_Lv4_Single() {
    //��ʼ��
    LOG.Info("Start to reverse single mod.");
    json reverseJsonObject = MMTJson_ReadJsonFromFile(G.Path_RunInputJson);
    std::wstring reverseFilePath = MMTString_ToWideString(reverseJsonObject["ReverseFilePath"]);
    std::wstring reverseFolderPath = MMTString_GetFolderPathFromFilePath(reverseFilePath) + L"-Reverse\\";
    std::filesystem::create_directories(reverseFolderPath);
    LOG.Info(L"ReverseFilePath: " + reverseFilePath);
    LOG.NewLine();

    //��ʼ��ͨ��ini��������
    ModFormat_INI modFormatUnity(reverseFilePath);
    modFormatUnity.Parse_Self_Hash_SingleModDetect_Map();
    LOG.NewLine();

    for (const auto& singleModPair : modFormatUnity.Hash_SingleModDetect_Map) {
        std::wstring DrawIBHashValue = singleModPair.first;
        SingleModDetect singleModDetect = singleModPair.second;

        std::vector<M_Resource> ResourceVBList = singleModDetect.ResourceVBList;
        std::vector<M_TextureOverride> TextureOverrideIBList = singleModDetect.TextureOverrideIBList;
        LOG.Info(L"Mod Hash:" + DrawIBHashValue);
        LOG.Info("ResourceVBList Size: " + std::to_string(ResourceVBList.size()));
        LOG.Info("TextureOverrideIBList Size: " + std::to_string(TextureOverrideIBList.size()));
        LOG.NewLine();

        //ֱ�ӿ�ʼ����ElementList,�������е�D3D11�������������������������
        int possibleModelNumber = 0;
        for (D3D11GameType d3d11GameType : G.CurrentD3d11GameTypeList) {
            //�����SingleMod��Ҫ��������ͼʶ��ǰ���͵�ElementList��ƴ�ӳ�һ�������Mod��ƴ�Ӳ������Ļ�ValidModֵ�ͻ���false
            ValidModDetect singleMod(d3d11GameType, ResourceVBList, TextureOverrideIBList);

            //������ǺϷ���Modֱ�����������ٴ���ռ�ã��������ɳ���Ҳûɶ�ã��������Ϳ϶����Ǵ��
            if (!singleMod.ValidMod) {
                LOG.Info("Not a valid mod for this gametype: " + d3d11GameType.GameType + " so skip this.");
                LOG.NewLine();
                continue;
            }
            else {
                possibleModelNumber++;
            }

            //ƴ�Ӳ���������õĶ�ӦGameTypeĿ¼
            std::wstring OutputGameTypeFolderPath = reverseFolderPath + DrawIBHashValue + L"_" + MMTString_ToWideString(d3d11GameType.GameType) + L"\\";
            std::filesystem::create_directories(OutputGameTypeFolderPath);

            //Ȼ��ֱ�������ÿ��IB��Ҫ����DrawIBList���зָ�VB�ļ������
            int numberCount = 1;
            for (M_TextureOverride textureOverrideIB : TextureOverrideIBList) {
                //ƴ��������ļ�·��
                std::wstring outputFileNamePrefix = std::to_wstring(numberCount);
                std::wstring outputIBFilePath = OutputGameTypeFolderPath + outputFileNamePrefix + L".ib";
                std::wstring outputFmtFilePath = OutputGameTypeFolderPath + outputFileNamePrefix + L".fmt";
                std::wstring outputVBFilePath = OutputGameTypeFolderPath + outputFileNamePrefix + L".vb";
                LOG.Info(L"Output IBFileName: " + outputIBFilePath);
                LOG.Info(L"Output VBFileName: " + outputVBFilePath);
                LOG.Info(L"Output FMTFileName: " + outputFmtFilePath);
                LOG.NewLine();

                //���ݵ�ǰDrawIndexed�Ļ��ƶ������ͻ���ƫ�ƣ���IB�ļ��н�ȡ��DrawIndexed���Ƶ���ЩIB���ݣ�����Ϊ��ǰibBufFile��IB���ݣ����������
                IndexBufferBufFile ibBufFile = textureOverrideIB.IBBufFile;
                ibBufFile.SaveToFile_UINT32(outputIBFilePath, ibBufFile.MinNumber * -1);


                //���ò����FMT�ļ�
                FmtFileData fmtFile;
                fmtFile.ElementNameList = singleMod.ElementList;
                fmtFile.d3d11GameType = d3d11GameType;
                fmtFile.Format = L"DXGI_FORMAT_R32_UINT";
                fmtFile.OutputFmtFile(outputFmtFilePath);

                //���SizeΪ0�򲻽����������ֱ�ӵ��������˹�����������������ǲ����ܴ���Ϊ0��
                if (singleMod.finalVB0Bytes.size() == 0) {
                    LOG.Error(L"Can't output because finalVB0Bytes size is 0,pleae check your mod ini.");
                }

                //����Ҫע�⣬MinNumber��MaxNumber��+1��ԭ����VB0IndexNumberBytes�������ڶ�ȡ����ʱ�Ǵ�1��ʼ��
                //���Ե�MinNumber��СֵΪ0ʱ����Ӧ��һ�����ݣ���Ӧ��VB0IndexNumberBytes�����������1����������Ҫ+1
                uint32_t readNumberOffset = ibBufFile.MinNumber + 1;
                //����ÿ��VB�ļ����ǵø��ݵ�ǰIB��Buffer�������Сֵ��ȡһ���֣���ȷ����������IB�ļ��ж���������ͬ
                std::vector<byte> outputBytes;
                for (readNumberOffset; readNumberOffset <= ibBufFile.MaxNumber + 1; readNumberOffset++) {
                    std::vector<byte> indexBytes = singleMod.VB0IndexNumberBytes[readNumberOffset];
                    outputBytes.insert(outputBytes.end(), indexBytes.begin(), indexBytes.end());
                }

                //������ս�ȡ���vb0�ļ�
                std::ofstream outputVBFile(outputVBFilePath, std::ios::binary);
                outputVBFile.write(reinterpret_cast<const char*>(outputBytes.data()), outputBytes.size());
                outputVBFile.close();

                //�����Զ�+1
                numberCount++;
            }
        }

        //�������Ͷ�ƥ�䲻���Ļ��͵���һ����ʾ
        if (possibleModelNumber == 0) {
            LOG.Error(L"Can't match any D3D11Element type,please check if it's a valid mod,if it's valid then contact NicoMico for help or open a issue on github with your mod file in attachment.");
        }
    }
}