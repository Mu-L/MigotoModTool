#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MigotoParseUtil.h"
#include "ModFormatGIMI.h"
#include "GlobalFunctions.h"
#include "IndexBufferBufFile.h"
#include "FmtData.h"

void ReverseOutfitCompilerCompressed() {
    //��ʼ��
    LOG.Info("Start to reverse outfit_compiler.py format merged mod.");
    json reverseJsonObject = MMTJson_ReadJsonFromFile(G.Path_RunInputJson);
    std::wstring GameName = MMTString_ToWideString(reverseJsonObject["GameName"]);
    std::wstring reverseFilePath = MMTString_ToWideString(reverseJsonObject["ReverseFilePath"]);
    LOG.Info(L"ReverseFilePath: " + reverseFilePath);
    LOG.Info(L"GameName: " + GameName);
    LOG.NewLine();

    //������������ļ���
    std::wstring reverseFolderPath = MMTString_GetFolderPathFromFilePath(reverseFilePath) + L"Reverse\\";
    std::filesystem::create_directories(reverseFolderPath);

    //��ʼ��ͨ��ini��������
    ModFormat_GIMI_INI modFormatGIMI(reverseFilePath);
    LOG.NewLine();

    //����ֻ����ֻ��һ��hash��TextureOverride�����������������Resource�������hash��Ӧ��Resource
    std::vector<M_Resource> ResourceVBList;
    for (const auto& pair: modFormatGIMI.VertexNumberMResourceMap) {
        std::vector<M_Resource> tmpList = pair.second;
        ResourceVBList = tmpList;
        break;
    }
    LOG.Info("ResourceVBList Size: " + std::to_string(ResourceVBList.size()));
    LOG.Info("modFormatGIMI.VertexNumberMResourceMap Size: " + std::to_string(modFormatGIMI.VertexNumberMResourceMap.size()));

    LOG.Info("Hash_TextureOverrideIBList_Map Size: " + std::to_string(modFormatGIMI.Hash_TextureOverrideIBList_Map.size()));
    std::vector<M_TextureOverride> TextureOverrideIBList;
    for (const auto& pair: modFormatGIMI.Hash_TextureOverrideIBList_Map) {
        std::vector<M_TextureOverride> tmpList = pair.second;
        TextureOverrideIBList = tmpList;
    }
    LOG.Info("TextureOverrideIBList Size: " + std::to_string(TextureOverrideIBList.size()));


    //������Ҫ��TextureOverrideIBƥ���Ӧ��IB��Դ��
    std::vector<M_TextureOverride> matchedTextureOverrideIBList;
    for (M_TextureOverride textureOverride : TextureOverrideIBList) {
        for (M_ResourceReplace resourceReplace : textureOverride.ResourceReplaceList) {
            if (resourceReplace.ReplaceTarget == L"ib") {

                //��Resource�б�����Ҷ�Ӧ��Resource
                for (M_Resource m_resource:modFormatGIMI.Global_M_ResourceList) {
                    if (m_resource.ResourceName == resourceReplace.ReplaceResource) {
                        textureOverride.IBFileName = m_resource.FileName;
                        textureOverride.IBFormat = m_resource.Format;
                        LOG.Info(m_resource.ResourceName + L"  " + resourceReplace.ReplaceResource);
                        break;
                    }
                }
                matchedTextureOverrideIBList.push_back(textureOverride);
                break;
            }
        }
    }

    for (M_TextureOverride textureOverride : matchedTextureOverrideIBList) {
        LOG.Info(textureOverride.IBFileName);
    }
    LOG.NewLine();

    //ֱ�ӿ�ʼ����ElementList,�������е�D3D11�������������������������
    int possibleModelNumber = 0;

    std::vector<SingleMod> matchReversedObjectList;

    for (D3D11GameType d3d11GameType: G.CurrentD3d11GameTypeList) {
        
        SingleMod singleMod(d3d11GameType, ResourceVBList, TextureOverrideIBList);

        if (!singleMod.ValidMod) {
            LOG.Info("Not a valid mod for this gametype: " + d3d11GameType.GameType + " so skip this.");
            LOG.NewLine();
            continue;
        }
        else {
            possibleModelNumber++;
        }

        std::wstring OutputGameTypeFolderPath = reverseFolderPath + MMTString_ToWideString(d3d11GameType.GameType) + L"\\";
        std::filesystem::create_directories(OutputGameTypeFolderPath);

        //Ȼ��ֱ�������ÿ��IB��Ҫ����DrawIBList���зָ�VB�ļ������
        for (M_TextureOverride textureOverrideIB: matchedTextureOverrideIBList) {
            for (M_DrawIndexed drawIndexed : textureOverrideIB.DrawIndexedList) {
                
                std::wstring fileNamePrefix = drawIndexed.DrawNumber + L"_" + drawIndexed.DrawOffsetIndex;
                

                std::wstring readIBFilePath = textureOverrideIB.NameSpace + L"\\" + textureOverrideIB.IBFileName;
                LOG.Info(readIBFilePath);
                std::wstring outputIBFilePath = OutputGameTypeFolderPath + fileNamePrefix + L".ib";
                IndexBufferBufFile ibBufFile(readIBFilePath, textureOverrideIB.IBFormat);

                int drawNumber = std::stoi(drawIndexed.DrawNumber);
                int drawOffset = std::stoi(drawIndexed.DrawOffsetIndex);

                std::vector<uint32_t> SomeNumberList = MMTFormat_GetRange_UINT32T(ibBufFile.NumberList, drawOffset, drawOffset + drawNumber );
                ibBufFile.NumberList = SomeNumberList;
                ibBufFile.SaveToFile_UINT32(outputIBFilePath, ibBufFile.MinNumber * -1);

                std::wstring outputFmtFilePath = OutputGameTypeFolderPath + fileNamePrefix + L".fmt";
                FmtFileData fmtFile;
                fmtFile.ElementNameList = singleMod.ElementList;
                fmtFile.d3d11GameType = d3d11GameType;
                fmtFile.Format = L"DXGI_FORMAT_R32_UINT";
                fmtFile.OutputFmtFile(outputFmtFilePath);


                std::wstring outputVBFilePath = OutputGameTypeFolderPath + fileNamePrefix + L".vb";
                LOG.Info(L"output vbFileName: " + outputVBFilePath);

                if (singleMod.finalVB0Bytes.size() == 0) {
                    LOG.Error(L"Can't output because size is 0, so Skip this.");
                }
                else {

                    int readNumberOffset = ibBufFile.MinNumber + 1;

                    //����ÿ��VB�ļ����ǵø��ݵ�ǰIB��Buffer�������Сֵ��ȡһ���֣�û�취
                    std::vector<byte> outputBytes;
                    for (readNumberOffset; readNumberOffset <= ibBufFile.MaxNumber + 1; readNumberOffset++) {
                        std::vector<byte> indexBytes = singleMod.VB0IndexNumberBytes[readNumberOffset];
                        outputBytes.insert(outputBytes.end(), indexBytes.begin(), indexBytes.end());
                    }


                    std::ofstream outputVBFile(outputVBFilePath, std::ios::binary);
                    LOG.Info(L"Start to output to folder.");
                    outputVBFile.write(reinterpret_cast<const char*>(outputBytes.data()),outputBytes.size());
                    outputVBFile.close();
                }


            }
        }


    }
    

    if (possibleModelNumber == 0) {
        LOG.Error(L"Can't match any D3D11Element type,please check if it's a valid mod,if it's valid then contact NicoMico for help.");
    }
}