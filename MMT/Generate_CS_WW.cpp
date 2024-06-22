#include "GlobalFunctions.h"
#include "IndexBufferBufFile.h"
#include "VertexBufferBufFile.h"
#include "MMTFormatUtils.h"
#include "WWUtil.h"


void Generate_CS_WW_Body() {

	for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        ExtractConfig extractConfig = pair.second;
       
        D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[extractConfig.WorkGameType];

        std::wstring timeStr = MMTString_GetFormattedDateTimeForFilename().substr(0, 10);
        std::wstring splitReadFolder = G.OutputFolder + DrawIB + L"\\";
        std::wstring splitOutputFolder = G.OutputFolder + timeStr + L"\\" + DrawIB + L"\\";
        std::filesystem::create_directories(splitOutputFolder);

        bool findValidFile = false;
        for (std::string partName : extractConfig.PartNameList) {
            std::wstring VBFileName = MMTString_ToWideString(partName) + L".vb";
            if (std::filesystem::exists(splitReadFolder + VBFileName)) {
                findValidFile = true;
                break;
            }
        }
        if (!findValidFile) {
            LOG.Info(L"Detect didn't export vb file for DrawIB: " + DrawIB + L" , so skip this drawIB generate.");
            continue;
        }


        std::unordered_map<std::string, int> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(extractConfig.TmpElementList);
        std::vector<std::string> CategoryList = d3d11GameType.getCategoryList(extractConfig.TmpElementList);
        //����鿴ÿ��Category�Ĳ���
        for (const auto& pair : CategoryStrideMap) {
            const std::string& key = pair.first;
            int value = pair.second;
            LOG.Info("Category: " + key + ", CategoryStride: " + std::to_string(value));
        }
        LOG.NewLine();

        //�鿴CategoryList
        LOG.Info(L"CategoryList:");
        for (std::string categoryName : CategoryList) {
            LOG.Info("Ordered CategoryName: " + categoryName);
        }
        LOG.NewLine();

        

        //(1) ���BUF�ļ�
        //��ȡvb�ļ���ÿ��vb�ļ�������category�ֿ�װ�ز�ͬcategory������
        int SplitStride = d3d11GameType.getElementListStride(extractConfig.TmpElementList);
        LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));
        int drawNumber = 0;
        std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::vector<std::byte>>> partName_VBCategoryDaytaMap;
        std::unordered_map<std::string, int> partNameOffsetMap;
        std::unordered_map<std::string, int> partNameVertexCountMap;
        for (std::string partName : extractConfig.PartNameList) {
            std::wstring VBFileName = MMTString_ToWideString(partName) + L".vb";
            uint64_t VBFileSize = MMTFile_GetFileSize(splitReadFolder + VBFileName);
            uint64_t vbFileVertexNumber = VBFileSize / SplitStride;
         
            partNameVertexCountMap[partName] = (int)vbFileVertexNumber;
            LOG.Info(L"Processing VB file: " + VBFileName + L" size is: " + std::to_wstring(VBFileSize) + L" byte." + L" vertex number is: " + std::to_wstring(vbFileVertexNumber));
            VertexBufferBufFile vbBufFile(splitReadFolder + VBFileName, d3d11GameType, extractConfig.TmpElementList);
            partName_VBCategoryDaytaMap[MMTString_ToWideString(partName)] = vbBufFile.CategoryVBDataMap;


            //����offset
            partNameOffsetMap[partName] = drawNumber;
            //��ӵ�drawNumber
            drawNumber = drawNumber + (int)vbFileVertexNumber;
        }



        //(2) ת�������ÿ��IB�ļ�,����ע�������IBҪ����Offset����Ϊ����Blender������IB���Ǵ�0��ʼ��
        //�����ö�ȡib�ļ���ʹ�õ�Format,��1.fmt�ļ����Զ���ȡ
        std::wstring IBReadDxgiFormat = MMTFile_FindMigotoIniAttributeInFile(splitReadFolder + L"1.fmt", L"format");
        for (std::string partName : extractConfig.PartNameList) {
            std::wstring IBFileName = MMTString_ToWideString(partName) + L".ib";
            std::wstring readIBFileName = splitReadFolder + IBFileName;
            std::wstring writeIBFileName = splitOutputFolder + IBFileName;
            LOG.Info(L"Converting IB file: " + IBFileName);
            IndexBufferBufFile ibBufFile(readIBFileName, IBReadDxgiFormat);
            ibBufFile.SaveToFile_UINT32(writeIBFileName, partNameOffsetMap[partName]);
        }
        LOG.Info(L"Output ib file over");
        LOG.NewLine();


        LOG.Info(L"Combine and put partName_VBCategoryDaytaMap's content back to finalVBCategoryDataMap");
        //��partName_VBCategoryDaytaMap������ݣ�����finalVBCategoryDataMap����ϳ�һ����������ʹ��
        std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap;
        for (std::string partName : extractConfig.PartNameList) {
            std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = partName_VBCategoryDaytaMap[MMTString_ToWideString(partName)];
            for (size_t i = 0; i < CategoryList.size(); ++i) {
                const std::string& category = CategoryList[i];
                std::vector<std::byte> tmpCategoryData = tmpVBCategoryDataMap[MMTString_ToWideString(category)];
                if (category == "Normal") {
                    for (int index = 0; index < tmpCategoryData.size(); index = index + 8) {
                        //1.��ȡNORMAL��TANGENTֵ
                        std::byte NormalValueX = tmpCategoryData[index + 0];
                        std::byte NormalValueY = tmpCategoryData[index + 1];
                        std::byte NormalValueZ = tmpCategoryData[index + 2];
                        std::byte TangentValueX = tmpCategoryData[index + 4];
                        std::byte TangentValueY = tmpCategoryData[index + 5];
                        std::byte TangentValueZ = tmpCategoryData[index + 6];

                        //2.�����۲�NORMAL��ֵΪTANGENTǰ��λֱ�ӷŹ��������һλ��Ϊ0x7F
                        tmpCategoryData[index + 0] = TangentValueX;
                        tmpCategoryData[index + 1] = TangentValueY;
                        tmpCategoryData[index + 2] = TangentValueZ;
                        tmpCategoryData[index + 3] = std::byte(0x7F);

                        //3.��תNORMAL��ǰ��λ���ŵ�TANGENT��ǰ��λ��NORMAL��W��Ϊ0x7F
                        tmpCategoryData[index + 4] = MMTFormat_ReverseSNORMValueSingle(NormalValueX);
                        tmpCategoryData[index + 5] = MMTFormat_ReverseSNORMValueSingle(NormalValueY);
                        tmpCategoryData[index + 6] = MMTFormat_ReverseSNORMValueSingle(NormalValueZ);
                    }
                }
                std::vector<std::byte>& finalCategoryData = finalVBCategoryDataMap[MMTString_ToWideString(category)];
                finalCategoryData.insert(finalCategoryData.end(), tmpCategoryData.begin(), tmpCategoryData.end());

            }
        }
        LOG.NewLine();

        //ֱ�����
        for (const auto& pair : finalVBCategoryDataMap) {
            const std::wstring& categoryName = pair.first;
            const std::vector<std::byte>& categoryData = pair.second;
            LOG.Info(L"Output buf file, current category: " + categoryName + L" Length:" + std::to_wstring(categoryData.size() / drawNumber));
            //���û���ǾͲ����
            if (categoryData.size() == 0) {
                LOG.Info(L"Current category's size is 0, can't output, skip this.");
                continue;
            }
            std::wstring categoryGeneratedName = DrawIB + categoryName;
            // ��������ļ�·��
            std::wstring outputDatFilePath = splitOutputFolder + categoryGeneratedName + L".buf";
            // ������ļ� ��std::vecto������д���ļ�
            std::ofstream outputFile(MMTString_ToByteString(outputDatFilePath), std::ios::binary);
            outputFile.write(reinterpret_cast<const char*>(categoryData.data()), categoryData.size());
            outputFile.close();
            LOG.Info(L"Write " + categoryName + L" data into file: " + outputDatFilePath);
        }
        LOG.NewLine();


        //(3) ����ini�ļ�
        std::wstring outputIniFileName = splitOutputFolder + extractConfig.DrawIB  + L".ini";
        std::wofstream outputIniFile(outputIniFileName);

        //�����ʹ��CS���㣬��ͻ�ƶ�����������
        //TODO �п��о�һ�����������������Ӧ�����������Dispatch

        bool debug = false;
        if (d3d11GameType.GPUPreSkinning && debug) {
            outputIniFile << std::endl;
            outputIniFile << L"; -------------- Break Vertex Count Limit -----------------" << std::endl << std::endl;
            outputIniFile << L"[TextureOverride_CSReplace_VertexLimitBreak]" << std::endl;
            //TODO ��Ҫ�ռ�cs-cb0��hashֵ
            outputIniFile << L"hash = " << MMTString_ToWideString(extractConfig.VertexLimitVB) << std::endl;

            //�ȶ�ȡcsinfo.json�������
            WuwaCSInfoJsonObject wwcsinfoObject(splitReadFolder);

            //����ÿ��partName
            int currentOffset = 0;
            for (std::string partName :extractConfig.PartNameList) {
                int currentVertexCount = partNameVertexCountMap[partName];
                WuwaCSInfo wwcsInfo = wwcsinfoObject.PartNameWuwaCSInfoMap[partName];
                int originalVertexCount = wwcsInfo.CalculateTime;
                int originalOffset = wwcsInfo.Offset;
                LOG.Info("CurrentVertexCount: " + std::to_string(currentVertexCount) + " OriginalVertexCount: " + std::to_string(originalVertexCount));
                outputIniFile << L";------------" + MMTString_ToWideString(partName) + L"-----------" << std::endl;

                if (originalVertexCount == currentVertexCount) {
                    //��������ͬʱ������ı䶥�������������ƫ�Ʋ�ͬ������Ҫ��ƫ�Ƶġ�
                    if (currentOffset != originalOffset) {
                        if (wwcsInfo.ComputeShaderHash == "4d0760c2c7406824") {
                            //�޸�3��ƫ����
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                        }
                        else if (wwcsInfo.ComputeShaderHash == "1ff924db9d4048d1") {
                            //�޸�2��ƫ����
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                        }
                        
                    }
                }
                else {
                    if (currentOffset != originalOffset) {
                        //��������ͬ��ƫ��Ҳ��ͬʱȫ����Ҫ�޸�
                        if (wwcsInfo.ComputeShaderHash == "4d0760c2c7406824") {
                            //�޸�3��ƫ����
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl << std::endl;
                            //�޸�1��������
                            outputIniFile << L"csreplace = cs-cb0, 4, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << std::endl;

                        }
                        else if (wwcsInfo.ComputeShaderHash == "1ff924db9d4048d1") {
                            //�޸�2��ƫ����
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl << std::endl;
                            //�޸�1��������
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << std::endl;

                        }
                    }
                    else {
                        //��������ͬ��ƫ����ͬʱ��ֻ��Ҫ�Ķ�����
                        if (wwcsInfo.ComputeShaderHash == "4d0760c2c7406824") {
                            //�޸�1��������
                            outputIniFile << L"csreplace = cs-cb0, 4, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << std::endl;

                        }
                        else if (wwcsInfo.ComputeShaderHash == "1ff924db9d4048d1") {
                            //�޸�1��������
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << std::endl;

                        }
                    }
                }

                currentOffset = currentOffset + currentVertexCount;
            }

        }

        outputIniFile << std::endl;
        outputIniFile << L"; -------------- TextureOverride VB -----------------" << std::endl << std::endl;
        //1.д��VBResource����
        for (std::string categoryName : CategoryList) {
            std::wstring fileName = extractConfig.DrawIB + MMTString_ToWideString(categoryName) + L".buf";
            std::wstring filePath = splitOutputFolder + fileName;
            int fileSize = MMTFile_GetFileSize(filePath);
            std::string categoryHash = extractConfig.CategoryHashMap[categoryName];
            std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
            outputIniFile << L"[TextureOverride_" + MMTString_ToWideString(categoryName) + L"_Replace]" << std::endl;
            outputIniFile << L"hash = " << MMTString_ToWideString(categoryHash) << "" << std::endl;
            outputIniFile << "this = " << L"Resource_VB_" + MMTString_ToWideString(categoryName) + L"" << std::endl << std::endl;
        }

        outputIniFile << std::endl;
        outputIniFile << L"; -------------- Resource VB -----------------" << std::endl << std::endl;
        //ResourceVB
        for (std::string categoryName : CategoryList) {
            std::wstring fileName = extractConfig.DrawIB + MMTString_ToWideString(categoryName) + L".buf";
            std::wstring filePath = splitOutputFolder + fileName;
            int fileSize = MMTFile_GetFileSize(filePath);
            std::string categoryHash = extractConfig.CategoryHashMap[categoryName];
            std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];


            outputIniFile << L"[Resource_VB_" + MMTString_ToWideString(categoryName) + L"]" << std::endl;
            outputIniFile << L"byte_width = " << std::to_wstring(fileSize) << std::endl;
            if (categoryName == "Texcoord") {
                outputIniFile << L"type = Buffer" << std::endl;
                outputIniFile << L"FORMAT = R16G16_FLOAT" << std::endl;
            }
            else if (categoryName == "Normal") {
                outputIniFile << L"type = Buffer" << std::endl;
                outputIniFile << L"FORMAT = R8G8B8A8_SNORM" << std::endl;
            }
            else if (categoryName == "Position" && categorySlot== "vb0") {
                outputIniFile << L"type = Buffer" << std::endl;
                outputIniFile << L"FORMAT = R32G32B32_FLOAT" << std::endl;
            }
            else {
                outputIniFile << L"type = ByteAddressBuffer" << std::endl;
                outputIniFile << "stride = " << CategoryStrideMap[categoryName] << std::endl;
            }
            outputIniFile << "filename = " << fileName << std::endl << std::endl;

            
        }

        outputIniFile << std::endl;
        outputIniFile << L"; -------------- IB Skip -----------------" << std::endl << std::endl;

        //IB SKIP����
        outputIniFile << L"[Resource_BakIB]" << std::endl;
        outputIniFile << L"[TextureOverride_" + extractConfig.DrawIB + L"_IB_SKIP]" << std::endl;
        outputIniFile << L"hash = " + extractConfig.DrawIB << std::endl;
        outputIniFile << "handling = skip" << std::endl;
        outputIniFile << std::endl;

        outputIniFile << std::endl;
        outputIniFile << L"; -------------- TextureOverride IB & Resource IB-----------------" << std::endl << std::endl;

        //TextureOverride IB����
        for (int i = 0; i < extractConfig.PartNameList.size(); ++i) {
            std::string partName = extractConfig.PartNameList[i];
            LOG.Info(L"Start to output UE4 ini file.");

            //��������֧��
            bool generateSwitchKey = false;
            std::wstring activateFlagName = L"ActiveFlag_" + extractConfig.DrawIB;
            std::wstring switchVarName = L"SwitchVar_" + extractConfig.DrawIB;
            std::wstring replace_prefix = L"";
            if (extractConfig.SwitchKey != L"") {
                generateSwitchKey = true;
                replace_prefix = L"  ";
                //��Ӷ�ӦConstants��KeySwitch����
                outputIniFile << "[Constants]" << std::endl;
                outputIniFile << "global persist $" << switchVarName << " = 1" << std::endl;
                outputIniFile << "global $" << activateFlagName << " = 0" << std::endl;
                outputIniFile << std::endl;

                outputIniFile << "[Key" << switchVarName << "]" << std::endl;
                outputIniFile << "condition = $" << activateFlagName << " == 1" << std::endl;
                outputIniFile << "key = " << extractConfig.SwitchKey << std::endl;
                outputIniFile << "type = cycle" << std::endl;
                outputIniFile << "$" << switchVarName << " = 0,1" << std::endl;
                outputIniFile << std::endl;

                outputIniFile << "[Present]" << std::endl;
                outputIniFile << "post $" << activateFlagName << " = 0" << std::endl;
                outputIniFile << std::endl;
            }
            LOG.Info(L"Generate Switch Key ini :" + std::to_wstring(generateSwitchKey));

            //4.IBOverride����
            std::string IBFirstIndex = extractConfig.MatchFirstIndexList[i];
            
            outputIniFile << L"[TextureOverride_IB_" + extractConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L"]" << std::endl;
            outputIniFile << L"hash = " + extractConfig.DrawIB << std::endl;
            outputIniFile << L"Resource_BakIB = ref ib" << std::endl;
            outputIniFile << L"match_first_index = " + MMTString_ToWideString(IBFirstIndex) << std::endl;
            if (generateSwitchKey) {
                outputIniFile << L"if $" + switchVarName + L" == 1" << std::endl;
            }
            outputIniFile << replace_prefix << L"ib = Resource_IB_" + extractConfig.DrawIB + L"_" + MMTString_ToWideString(partName) << std::endl;
            outputIniFile << replace_prefix << "drawindexed = auto" << std::endl;
            outputIniFile << L"ib = Resource_BakIB" << std::endl;

            if (generateSwitchKey) {
                outputIniFile << "endif" << std::endl;
            }
            outputIniFile << std::endl;
        }

        outputIniFile << std::endl;
        outputIniFile << L"; -------------- IB Resource -----------------" << std::endl << std::endl;

        //2.д��IBResource����
        for (int i = 0; i < extractConfig.PartNameList.size(); ++i) {
            std::string partName = extractConfig.PartNameList[i];
            outputIniFile << L"[Resource_IB_" + extractConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L"]" << std::endl;
            outputIniFile << "type = Buffer" << std::endl;
            outputIniFile << "format = DXGI_FORMAT_R32_UINT" << std::endl;
            outputIniFile << "filename = " << MMTString_ToWideString(partName) + L".ib" << std::endl << std::endl;
        }

        outputIniFile << L"; Mod Generated by MMT-Community." << std::endl ;
        outputIniFile << L"; Github: https://github.com/StarBobis/MigotoModTool"  << std::endl;
        outputIniFile << L"; Discord: https://discord.gg/Cz577BcRf5" << std::endl;

        outputIniFile.close();

        LOG.NewLine();
        LOG.Info(L"Generate mod completed!");
        LOG.NewLine();

    }

}