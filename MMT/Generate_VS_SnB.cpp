//#include "GlobalConfigs.h"
//#include <chrono>
//#include "D3D11ElementAttributeClass.h"
//#include <boost/algorithm/string.hpp>
//#include <boost/date_time/gregorian/gregorian.hpp>
//#include "MigotoIniFile.h"
//#include "GenerateUtil.h"
//#include "MMTStringUtils.h"
//#include "MMTFormatUtils.h"
//#include "MMTFileUtils.h"
//
//
//void UE4Split(GlobalConfigs& wheelConfig) {
//    for (const auto& pair : wheelConfig.drawIBBasicConfigMap) {
//        std::wstring drawIB = pair.first;
//        ExtractConfig basicConfig = pair.second;
//        basicConfig.GameType = MMTString_ToWideString(basicConfig.WorkGameType);
//        D3D11ElementAttributeClass D3D11ElementAttribute(basicConfig.GameType);
//
//        std::wstring timeStr = MMTString_GetFormattedDateTimeForFilename().substr(0, 10);
//        std::wstring splitReadFolder = wheelConfig.OutputFolder + drawIB + L"/";
//        std::wstring splitOutputFolder = wheelConfig.OutputFolder + timeStr + L"/" + drawIB + L"/";
//        std::filesystem::create_directories(splitOutputFolder);
//
//        //����PartName������Ӧ������ļ��У��������ֱ����
//        for (std::string partName : basicConfig.PartNameList) {
//            std::wstring partNameFolder = splitOutputFolder + MMTString_ToWideString(partName) + L"/";
//            std::filesystem::create_directories(partNameFolder);
//        }
//
//        bool findValidFile = false;
//        for (std::string partName : basicConfig.PartNameList) {
//            std::wstring VBFileName = MMTString_ToWideString(partName) + L".vb";
//            if (std::filesystem::exists(splitReadFolder + VBFileName)) {
//                findValidFile = true;
//                break;
//            }
//        }
//        if (!findValidFile) {
//            LOG.Info(L"Detect didn't export vb file for DrawIB: " + drawIB + L" , so skip this drawIB generate.");
//            continue;
//        }
//
//        // ����Ҫ��ȡ���ָ�ã�������Ҫͳ��TmpElementList���ܳ��������ָ�,ֻͳ����ȡ��
//        int SplitStride = 0;
//        for (std::string elementName : basicConfig.TmpElementList) {
//            D3D11Element elementObject = D3D11ElementAttribute.ElementNameD3D11ElementMap[MMTString_ToWideString(elementName)];
//            if (elementObject.Extract) {
//                SplitStride = SplitStride + elementObject.ByteWidth;
//            }
//        }
//        LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));
//
//
//        //��ȡBLENDWEIGHTS������
//        std::wstring blendElementName;
//        if (boost::algorithm::any_of_equal(basicConfig.TmpElementList, "BLENDWEIGHT")) {
//            blendElementName = L"BLENDWEIGHT";
//        }
//        else {
//            blendElementName = L"BLENDWEIGHTS";
//        }
//        int blendElementByteWidth = D3D11ElementAttribute.ElementNameD3D11ElementMap[blendElementName].ByteWidth;
//        int blendIndicesByteWidth = D3D11ElementAttribute.ElementNameD3D11ElementMap[L"BLENDINDICES"].ByteWidth;
//        LOG.Info(L"Get BlendWeigths width: " + std::to_wstring(blendElementByteWidth));
//        LOG.Info(L"Get BlendIndices width: " + std::to_wstring(blendIndicesByteWidth));
//
//        // �����CategoryList�����ǰ���d3d11Element�������б�������ģ���Ҫ�ֳ�����
//        // ���ǵ�tmpElementList�϶�����ȷ��˳����Ϊ��merge��ʱ���������
//        // ���������ռ�����vb�ļ��е����ݣ�ÿ�����ð���category���з��࣬������Ҫ��ǰ�����ÿ��category�Ĳ���
//        std::unordered_map<std::wstring, int> CategoryStrideMap;
//        std::vector<std::wstring> CategoryList;
//        for (std::string elementName : basicConfig.TmpElementList) {
//            D3D11Element elementObject = D3D11ElementAttribute.ElementNameD3D11ElementMap[MMTString_ToWideString(elementName)];
//            int byteWidth = elementObject.ByteWidth;
//            std::wstring elementCategory = elementObject.Category;
//            int categoryStride = CategoryStrideMap[elementCategory];
//            if (categoryStride == NULL) {
//                categoryStride = byteWidth;
//            }
//            else {
//                categoryStride = categoryStride + byteWidth;
//            }
//            CategoryStrideMap[elementCategory] = categoryStride;
//
//            if (!boost::algorithm::any_of_equal(CategoryList, elementCategory)) {
//                CategoryList.push_back(elementCategory);
//            }
//
//        }
//
//        //����鿴ÿ��Category�Ĳ���
//        for (const auto& pair : CategoryStrideMap) {
//            const std::wstring& key = pair.first;
//            int value = pair.second;
//            LOG.Info(L"Category: " + key + L", CategoryStride: " + std::to_wstring(value));
//        }
//        LOG.NewLine();
//
//        //�鿴CategoryList
//        LOG.Info(L"CategoryList:");
//        for (std::wstring categoryName : CategoryList) {
//            LOG.Info(L"Ordered CategoryName: " + categoryName);
//        }
//        LOG.NewLine();
//
//        //(1) ת�������ÿ��IB�ļ�
//        //����ÿ��PartName��Ӧ��Offset������IB�ļ���Offset���á�
//        //�����ö�ȡib�ļ���ʹ�õ�Format,��1.fmt�ļ����Զ���ȡ
//        std::wstring IBReadDxgiFormat = MMTFile_FindMigotoIniAttributeInFile(splitReadFolder + L"1.fmt", L"format");
//        uint64_t IBOffset = 0;
//        for (std::string partName : basicConfig.PartNameList) {
//            std::wstring IBFileName = MMTString_ToWideString(partName) + L".ib";
//            std::wstring readIBFileName = splitReadFolder + IBFileName;
//            std::wstring writeIBFileName = splitOutputFolder + MMTString_ToWideString(partName) + L"/" + IBFileName;
//            LOG.Info(L"Converting IB file: " + IBFileName);
//            //TODO ��Ȼÿһ��IB��index����һ��������Mod����ô��û�п���ÿ�����ǵ����Ĵ�0��ʼ���أ�
//            MMTFile_ConvertIndexBufferFileToR32_UINT(readIBFileName, writeIBFileName, 0, IBReadDxgiFormat);
//
//            LOG.Info(L"Set partName: " + MMTString_ToWideString(partName) + L" offset to: " + std::to_wstring(IBOffset));
//            std::wstring readVBFileName = MMTString_ToWideString(partName) + L".vb";
//            std::ifstream readVBFile(MMTString_ToByteString(splitReadFolder + readVBFileName), std::ios::binary);
//            readVBFile.seekg(0, std::ios::end);
//            std::streampos VBFileSize = readVBFile.tellg();
//            uint64_t vbFileVertexNumber = VBFileSize / SplitStride;
//            IBOffset = IBOffset + vbFileVertexNumber;
//        }
//        LOG.Info(L"Output ib file over");
//        LOG.NewLine();
//
//
//
//        //(2)��ȡÿ��partName��Ӧ��vb�ļ����ָ�ɸ���Category�ļ������
//        for (std::string partName : basicConfig.PartNameList) {
//            std::unordered_map<std::wstring, std::vector<std::byte>> finalVBElementDataMap;
//            std::wstring readVBFileName = MMTString_ToWideString(partName) + L".vb";
//            std::ifstream readVBFile(MMTString_ToByteString(splitReadFolder + readVBFileName), std::ios::binary);
//            readVBFile.seekg(0, std::ios::end);
//            std::streampos VBFileSize = readVBFile.tellg();
//            uint64_t drawNumber = VBFileSize / SplitStride;
//            LOG.Info(L"Processing VB file: " + readVBFileName + L" size is: " + std::to_wstring(VBFileSize) + L" byte." + L" vertex number is: " + std::to_wstring(drawNumber));
//
//            // ���ļ�ָ�붨λ���ļ���ͷ,Ȼ���ٶ�ȡ
//            readVBFile.seekg(0);
//            std::vector<std::byte> readVBFileBuffer(VBFileSize);
//            readVBFile.read(reinterpret_cast<char*>(readVBFileBuffer.data()), VBFileSize);
//            readVBFile.close();
//
//            int offset = 0;
//            while (offset < readVBFileBuffer.size()) {
//                for (std::string elementName: basicConfig.TmpElementList) {
//                    D3D11Element elementObject = D3D11ElementAttribute.ElementNameD3D11ElementMap[MMTString_ToWideString(elementName)];
//
//                    std::vector<std::byte> elementDataPatch;
//                    if (elementObject.Extract) {
//                        elementDataPatch.reserve(elementObject.ByteWidth);
//                        std::copy(readVBFileBuffer.begin() + offset, readVBFileBuffer.begin() + offset + elementObject.ByteWidth, std::back_inserter(elementDataPatch));
//                    }
//                    else {
//                        elementDataPatch = std::vector<std::byte>(elementObject.ByteWidth, std::byte{ 0x00 });
//                    }
//
//                    // Ԥ�ȷ����㹻�Ŀռ�
//                    std::vector<std::byte>& elementData = finalVBElementDataMap[MMTString_ToWideString(elementName)];
//                    // ��categoryDataPatch������ֱ�Ӳ���categoryData
//                    elementData.insert(elementData.end(), elementDataPatch.begin(), elementDataPatch.end());
//
//                    if (elementObject.Extract) {
//                        offset += elementObject.ByteWidth;
//                    }
//                }
//
//            }
//
//            LOG.Info(L"Size Check:");
//            for (const auto& pair : finalVBElementDataMap) {
//                LOG.Info(L"ElementName:" + pair.first + L" " + std::to_wstring(pair.second.size()));
//            }
//            LOG.NewLine();
//
//            //��ת��ΪfinalVBCategoryDataMap
//            std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap;
//            std::unordered_map<std::wstring, std::vector<std::byte>> categoryDataCache;
//
//            for (int i = 0; i < drawNumber; i++) {
//                for (const std::string& elementName : basicConfig.TmpElementList) {
//                    const D3D11Element& d3d11Element = D3D11ElementAttribute.ElementNameD3D11ElementMap[MMTString_ToWideString(elementName)];
//                    const std::vector<std::byte>& elementData = finalVBElementDataMap[MMTString_ToWideString(elementName)];
//                    int offset = i * d3d11Element.ByteWidth;
//                    const std::vector<std::byte>& patchData = MMTFormat_GetRange_Byte(elementData, offset, offset + d3d11Element.ByteWidth);
//
//                    std::vector<std::byte>& categoryData = categoryDataCache[d3d11Element.Category];
//                    if (categoryData.empty()) {
//                        // Ԥ�ȷ����㹻���ڴ�ռ�
//                        categoryData.reserve(drawNumber * d3d11Element.ByteWidth);
//                    }
//                    categoryData.insert(categoryData.end(), patchData.begin(), patchData.end());
//                }
//            }
//
//            // ������Ľ�����뵽 finalVBCategoryDataMap ��
//            for (const auto& entry : categoryDataCache) {
//                finalVBCategoryDataMap.emplace(entry.first, entry.second);
//            }
//
//            LOG.Info(L"Start to patch 1 to NORMAL value in UE4.");
//            // ===================NORMAL��1===================
//            std::wstring NormalCategoryName = L"Normal";
//            std::vector<std::byte> NormalCategoryValues = finalVBCategoryDataMap[NormalCategoryName];
//            std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;
//            std::vector<std::byte> patchValue = MMTFormat_PackNumberOneByte(0x7F);
//
//            std::vector<std::byte> newNormalCategoryValues;
//            int NormalCategoryStride = CategoryStrideMap[NormalCategoryName]; //7
//            int NORMAL_ElementStride = D3D11ElementAttribute.ElementNameD3D11ElementMap[L"NORMAL"].ByteWidth;//3
//            int TANGENT_ElementStride = D3D11ElementAttribute.ElementNameD3D11ElementMap[L"TANGENT"].ByteWidth;//4
//            LOG.Info(L"NormalCategoryStride: " + std::to_wstring(NormalCategoryStride));
//            LOG.Info(L"NORMAL_ElementStride: " + std::to_wstring(NORMAL_ElementStride));
//            LOG.Info(L"TANGENT_ElementStride: " + std::to_wstring(TANGENT_ElementStride));
//
//            for (std::size_t i = 0; i < NormalCategoryValues.size(); i = i + NormalCategoryStride)
//            {
//                std::vector<std::byte> NormalValues = MMTFormat_GetRange_Byte(NormalCategoryValues, i, i + NORMAL_ElementStride);
//                NormalValues.insert(NormalValues.end(), patchValue.begin(), patchValue.end());
//                std::vector<std::byte> TangentValues = MMTFormat_GetRange_Byte(NormalCategoryValues, i + NORMAL_ElementStride, i + NORMAL_ElementStride + TANGENT_ElementStride);
//                //�µ�Position�����ֵ
//                std::vector<std::byte> new_NORMAL_TANGENT_Values;
//                new_NORMAL_TANGENT_Values.insert(new_NORMAL_TANGENT_Values.end(), NormalValues.begin(), NormalValues.end());
//                new_NORMAL_TANGENT_Values.insert(new_NORMAL_TANGENT_Values.end(), TangentValues.begin(), TangentValues.end());
//
//                //׷�ӵ��µ�Normal��
//                newNormalCategoryValues.insert(newNormalCategoryValues.end(), new_NORMAL_TANGENT_Values.begin(), new_NORMAL_TANGENT_Values.end());
//            }
//            newVBCategoryDataMap = finalVBCategoryDataMap;
//            newVBCategoryDataMap[NormalCategoryName] = newNormalCategoryValues;
//            LOG.Info(L"newNormalCategoryValues Size: " + std::to_wstring(newNormalCategoryValues.size()));
//
//            finalVBCategoryDataMap = newVBCategoryDataMap;
//            LOG.Info(L"Patch 1 to NORMAL value in UE4 over.");
//            LOG.NewLine();
//
//
//            // =========================��תNORMAL��TANGENT================================
//            LOG.Info(L"Start to flip NORMAL and TANGENT values.");
//            newVBCategoryDataMap.clear();
//            NormalCategoryValues = finalVBCategoryDataMap[NormalCategoryName];
//            newNormalCategoryValues.clear();
//
//            //���� NormalCategoryStride + 1 ��Ϊ�����¼���һ��float��ȥ��
//            for (std::size_t i = 0; i < NormalCategoryValues.size(); i = i + NormalCategoryStride + 1)
//            {
//                //��ȡPosition��Tangentֵ
//                std::vector<std::byte> NormalValueX = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 0, i + 1);
//                std::vector<std::byte> NormalValueY = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 1, i + 2);
//                std::vector<std::byte> NormalValueZ = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 2, i + 3);
//                std::vector<std::byte> NormalValueW = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 3, i + 4);
//
//                std::vector<std::byte> TangentValueX = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 4, i + 5);
//                std::vector<std::byte> TangentValueY = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 5, i + 6);
//                std::vector<std::byte> TangentValueZ = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 6, i + 7);
//                std::vector<std::byte> TangentValueW = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 7, i + 8);
//
//                //���ݲ������з�ת
//                std::vector<std::byte> NormalValueX_Reversed;
//                std::vector<std::byte> NormalValueY_Reversed;
//                std::vector<std::byte> NormalValueZ_Reversed;
//                if (basicConfig.NormalReverseX) {
//                    NormalValueX_Reversed = MMTFormat_ReverseSNORMValue(NormalValueX);
//                }
//                else {
//                    NormalValueX_Reversed = NormalValueX;
//                }
//
//                if (basicConfig.NormalReverseY) {
//                    NormalValueY_Reversed = MMTFormat_ReverseSNORMValue(NormalValueY);
//
//                }
//                else {
//                    NormalValueY_Reversed = NormalValueY;
//                }
//
//                if (basicConfig.NormalReverseZ) {
//                    NormalValueZ_Reversed = MMTFormat_ReverseSNORMValue(NormalValueZ);
//
//                }
//                else {
//                    NormalValueZ_Reversed = NormalValueZ;
//                }
//
//                //�Ż�Normal��λ
//                std::vector<std::byte> FlipNormalValues;
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueX_Reversed.begin(), NormalValueX_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueY_Reversed.begin(), NormalValueY_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueZ_Reversed.begin(), NormalValueZ_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueW.begin(), NormalValueW.end());
//
//                std::vector<std::byte> TangentValueX_Reversed;
//                std::vector<std::byte> TangentValueY_Reversed;
//                std::vector<std::byte> TangentValueZ_Reversed;
//                std::vector<std::byte> TangentValueW_Reversed;
//                if (basicConfig.TangentReverseX) {
//                    TangentValueX_Reversed = MMTFormat_ReverseSNORMValue(TangentValueX);
//                }
//                else {
//                    TangentValueX_Reversed = TangentValueX;
//                }
//                if (basicConfig.TangentReverseY) {
//                    TangentValueY_Reversed = MMTFormat_ReverseSNORMValue(TangentValueY);
//                }
//                else {
//                    TangentValueY_Reversed = TangentValueY;
//                }
//                if (basicConfig.TangentReverseZ) {
//                    TangentValueZ_Reversed = MMTFormat_ReverseSNORMValue(TangentValueZ);
//                }
//                else {
//                    TangentValueZ_Reversed = TangentValueZ;
//                }
//                if (basicConfig.TangentReverseW) {
//                    TangentValueW_Reversed = MMTFormat_ReverseSNORMValue(TangentValueW);
//                }
//                else {
//                    TangentValueW_Reversed = TangentValueW;
//                }
//
//                //�Ż�Tangent��λ
//                std::vector<std::byte> FlipTangentValues;
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueX_Reversed.begin(), TangentValueX_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueY_Reversed.begin(), TangentValueY_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueZ_Reversed.begin(), TangentValueZ_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueW_Reversed.begin(), TangentValueW_Reversed.end());
//
//
//                //�µ�Position�����ֵ
//                std::vector<std::byte> newPositionValues;
//                newPositionValues.insert(newPositionValues.end(), FlipNormalValues.begin(), FlipNormalValues.end());
//                newPositionValues.insert(newPositionValues.end(), FlipTangentValues.begin(), FlipTangentValues.end());
//                newNormalCategoryValues.insert(newNormalCategoryValues.end(), newPositionValues.begin(), newPositionValues.end());
//            }
//
//            newVBCategoryDataMap = finalVBCategoryDataMap;
//            newVBCategoryDataMap[NormalCategoryName] = newNormalCategoryValues;
//            finalVBCategoryDataMap = newVBCategoryDataMap;
//            LOG.Info(L"Flip NORMAL and TANGENT values over.");
//            LOG.NewLine();
//
//            // TODO ����Blender����ĵ������ã�������Ҫ����NORMAL��TANGENT�ķ���
//            // ���ǲ�֪��������ô��������ֻ��ͨ������۲쵽�˹��ɣ�
//            newVBCategoryDataMap.clear();
//            NormalCategoryValues = finalVBCategoryDataMap[NormalCategoryName];
//            newNormalCategoryValues.clear();
//
//            //���� NormalCategoryStride + 1 ��Ϊ�����¼���һ��float��ȥ��
//            for (std::size_t i = 0; i < NormalCategoryValues.size(); i = i + NormalCategoryStride + 1)
//            {
//                //��ȡPosition��Tangentֵ
//                std::vector<std::byte> NormalValueX = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 0, i + 1);
//                std::vector<std::byte> NormalValueY = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 1, i + 2);
//                std::vector<std::byte> NormalValueZ = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 2, i + 3);
//                std::vector<std::byte> NormalValueW = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 3, i + 4);
//                std::vector<std::byte> TangentValueX = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 4, i + 5);
//                std::vector<std::byte> TangentValueY = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 5, i + 6);
//                std::vector<std::byte> TangentValueZ = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 6, i + 7);
//                std::vector<std::byte> TangentValueW = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 7, i + 8);
//
//                // ��תNORMAL��ǰ��λ��������ŵ�TANGENT��x,y,z����תTANGENT��W
//                // TANGENT��ǰ��λֱ�ӷŵ�NORMAL��ǰ��λ
//                std::vector<std::byte> NormalValueX_Reversed;
//                std::vector<std::byte> NormalValueY_Reversed;
//                std::vector<std::byte> NormalValueZ_Reversed;
//                std::vector<std::byte> NormalValueW_Reversed;
//                std::vector<std::byte> TangentValueX_Reversed;
//                std::vector<std::byte> TangentValueY_Reversed;
//                std::vector<std::byte> TangentValueZ_Reversed;
//                std::vector<std::byte> TangentValueW_Reversed;
//
//                NormalValueX_Reversed = TangentValueX;
//                NormalValueY_Reversed = TangentValueY;
//                NormalValueZ_Reversed = TangentValueZ;
//
//                //������ȡ�������NORMAL��W����
//                NormalValueW_Reversed = NormalValueW;
//                TangentValueX_Reversed = MMTFormat_ReverseSNORMValue(NormalValueX);
//                TangentValueY_Reversed = MMTFormat_ReverseSNORMValue(NormalValueY);
//                TangentValueZ_Reversed = MMTFormat_ReverseSNORMValue(NormalValueZ);
//                TangentValueW_Reversed = TangentValueW;
//
//                //�Ż�Normal��λ
//                std::vector<std::byte> FlipNormalValues;
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueX_Reversed.begin(), NormalValueX_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueY_Reversed.begin(), NormalValueY_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueZ_Reversed.begin(), NormalValueZ_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueW_Reversed.begin(), NormalValueW_Reversed.end());
//                std::vector<std::byte> FlipTangentValues;
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueX_Reversed.begin(), TangentValueX_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueY_Reversed.begin(), TangentValueY_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueZ_Reversed.begin(), TangentValueZ_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueW_Reversed.begin(), TangentValueW_Reversed.end());
//
//                //�µ�Position�����ֵ
//                std::vector<std::byte> newPositionValues;
//                //newPositionValues.insert(newPositionValues.end(), NORMAL_TANGENT_Values.begin(), NORMAL_TANGENT_Values.end());
//                newPositionValues.insert(newPositionValues.end(), FlipNormalValues.begin(), FlipNormalValues.end());
//                newPositionValues.insert(newPositionValues.end(), FlipTangentValues.begin(), FlipTangentValues.end());
//
//                newNormalCategoryValues.insert(newNormalCategoryValues.end(), newPositionValues.begin(), newPositionValues.end());
//            }
//
//            newVBCategoryDataMap = finalVBCategoryDataMap;
//            newVBCategoryDataMap[NormalCategoryName] = newNormalCategoryValues;
//            finalVBCategoryDataMap = newVBCategoryDataMap;
//            LOG.Info(L"Flip NORMAL and TANGENT values over.");
//            LOG.NewLine();
//
//
//            //(2) ���濪ʼ����ֱ�����������Buf�ļ�
//            LOG.NewLine();
//            for (const auto& pair : finalVBCategoryDataMap) {
//                const std::wstring& categoryName = pair.first;
//                const std::vector<std::byte>& categoryData = pair.second;
//                LOG.Info(L"Output buf file, current category: " + categoryName + L" Length:" + std::to_wstring(categoryData.size() / drawNumber));
//
//                //���û���ǾͲ����
//                if (categoryData.size() == 0) {
//                    LOG.Warning(L"Current category's size is 0, can't output, skip this.");
//                    continue;
//                }
//
//                // ��������ļ�·��
//                std::wstring outputDatFilePath = splitOutputFolder + MMTString_ToWideString(partName)+ L"/" + MMTString_ToWideString(partName) + L"_" + basicConfig.DrawIB + L"_" + categoryName + L".buf";
//                // ������ļ�
//                std::ofstream outputFile(MMTString_ToByteString(outputDatFilePath), std::ios::binary);
//
//
//                //���ȱʧBLENDWEIGHTSԪ�أ�����Ҫ�ֶ�ɾ��BLEND��λ�е�BLENDԪ��
//                if (D3D11ElementAttribute.PatchBLENDWEIGHTS && categoryName == L"Blend") {
//
//                    LOG.Info(L"Detect element: " + blendElementName + L" need to delete , now will delete it.");
//                    //ȥ��BLENDWEIGHTS��������б�
//                    std::vector<std::byte> newBlendCategoryData;
//
//                    //������ֻ��BLENDINDICES��ӵ�Ԫ���б�
//                    int blendWeightCount = 1;
//                    int blendIndicesCount = 1;
//                    for (std::byte singleByte : categoryData) {
//                        //std::cout << "��ǰblendWeightCount: " << blendWeightCount << std::endl;
//                        //std::cout << "��ǰblendIndicesCount: " << blendIndicesCount << std::endl;
//                        if (blendWeightCount <= blendElementByteWidth) {
//                            blendWeightCount += 1;
//                        }
//                        else {
//                            //Ȼ��ʼ��ӵ�newBlendCategoryData
//                            if (blendIndicesCount <= blendIndicesByteWidth) {
//                                //std::cout << "ִ�з������" << std::endl;
//                                newBlendCategoryData.push_back(singleByte);
//                                blendIndicesCount += 1;
//
//
//                                //������֮�����Ҫ���̹��㣬���������ж�
//                                if ((blendWeightCount == blendElementByteWidth + 1) && (blendIndicesCount == blendIndicesByteWidth + 1)) {
//                                    blendWeightCount = 1;
//                                    blendIndicesCount = 1;
//                                }
//                            }
//                            else {
//                                blendWeightCount = 1;
//                                blendIndicesCount = 1;
//
//                            }
//                        }
//
//
//                    }
//                    outputFile.write(reinterpret_cast<const char*>(newBlendCategoryData.data()), newBlendCategoryData.size());
//                }
//                else {
//                    // ��std::vecto������д���ļ�
//                    outputFile.write(reinterpret_cast<const char*>(categoryData.data()), categoryData.size());
//                }
//                outputFile.close();
//
//                LOG.Info(L"Write " + categoryName + L" data into file: " + outputDatFilePath);
//            }
//            LOG.NewLine();
//
//
//        }
//        LOG.Info(L"Read and split vb file over.");
//        LOG.NewLine();
//       
//
//        //(3) ��������Wheel��ʽ��ini�ļ�
//        // TODO �Ҹо�����ҪVertexLimitRaise�ˣ�����˵ÿ��vb��λ����ҪVertexLimitRaise
//        // ����һ���ܵ�ini�ļ������Ʒֿ���ini�ļ� 
//        std::wstring outputIniFileName = splitOutputFolder + basicConfig.DrawIB  + L".ini";
//        std::wofstream outputIniFile(outputIniFileName);
//
//        //IB SKIP����
//        outputIniFile << L"[TextureOverride_" + basicConfig.DrawIB + L"_IB_SKIP]" << std::endl;
//        outputIniFile << L"hash = " + basicConfig.DrawIB << std::endl;
//        outputIniFile << "handling = skip" << std::endl;
//        outputIniFile << std::endl;
//
//        //vb0��VertexLimitRaise
//         //3.VertexLimitRaise����
//        //std::string VertexLimitVB = basicConfig.VertexLimitVB;
//        ////������drawNumber��ʵ�ֶ�̬����
//        //outputIniFile << L"[TextureOverride_VB_" + basicConfig.DrawIB + L"_" + std::to_wstring(CategoryStrideMap[L"Position"]) + L"_" + std::to_wstring(IBOffset) + L"_VertexLimitRaise]" << std::endl;
//        //outputIniFile << L"hash = " + MMTString_ToWideString(VertexLimitVB) << std::endl << std::endl;
//        //outputIniFile << std::endl;
//        outputIniFile.close();
//        
//        //����Ҫ��ÿһ�������ɵ�����ini�ļ�
//        for (int i = 0; i < basicConfig.PartNameList.size(); ++i) {
//            std::string partName = basicConfig.PartNameList[i];
//            LOG.Info(L"Start to output UE4 ini file.");
//            std::wstring outputIniFileName = splitOutputFolder + MMTString_ToWideString(partName) + L"/" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L".ini";
//            std::wofstream outputIniFile(outputIniFileName);
//
//            //��������֧��
//            bool generateSwitchKey = false;
//            std::wstring activateFlagName = L"ActiveFlag_" + basicConfig.DrawIB;
//            std::wstring switchVarName = L"SwitchVar_" + basicConfig.DrawIB;
//            std::wstring replace_prefix = L"";
//            if (basicConfig.SwitchKey != L"") {
//                generateSwitchKey = true;
//                replace_prefix = L"  ";
//                //��Ӷ�ӦConstants��KeySwitch����
//                outputIniFile << "[Constants]" << std::endl;
//                outputIniFile << "global persist $" << switchVarName << " = 1" << std::endl;
//                outputIniFile << "global $" << activateFlagName << " = 0" << std::endl;
//                outputIniFile << std::endl;
//
//                outputIniFile << "[Key" << switchVarName << "]" << std::endl;
//                outputIniFile << "condition = $" << activateFlagName << " == 1" << std::endl;
//                outputIniFile << "key = " << basicConfig.SwitchKey << std::endl;
//                outputIniFile << "type = cycle" << std::endl;
//                outputIniFile << "$" << switchVarName << " = 0,1" << std::endl;
//                outputIniFile << std::endl;
//
//                outputIniFile << "[Present]" << std::endl;
//                outputIniFile << "post $" << activateFlagName << " = 0" << std::endl;
//                outputIniFile << std::endl;
//            }
//            LOG.Info(L"Generate Switch Key ini :" + std::to_wstring(generateSwitchKey));
//
//            //4.IBOverride����
//            std::string IBFirstIndex = basicConfig.MatchFirstIndexList[i];
//            outputIniFile << L"[Resource_BakIB]"<< std::endl;
//            outputIniFile << L"[TextureOverride_IB_" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L"]" << std::endl;
//            outputIniFile << L"hash = " + basicConfig.DrawIB << std::endl;
//            outputIniFile << L"Resource_BakIB = ref ib" << std::endl;
//            outputIniFile << L"match_first_index = " + MMTString_ToWideString(IBFirstIndex) << std::endl;
//            if (generateSwitchKey) {
//                outputIniFile << L"if $" + switchVarName + L" == 1" << std::endl;
//            }
//            outputIniFile << replace_prefix << L"ib = Resource_IB_" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) << std::endl;
//            //6.TextureOverride VB����
//            for (std::wstring categoryName : CategoryList) {
//                std::string categoryHash = basicConfig.CategoryHashMap[MMTString_ToByteString(categoryName)];
//                std::string categorySlot = D3D11ElementAttribute.CategorySlotMap[MMTString_ToByteString(categoryName)];
//                LOG.Info(L"categoryName: " + categoryName + L" categorySlot: " + MMTString_ToWideString(categorySlot));
//                if (categoryName == L"Position") {
//                    outputIniFile << MMTString_ToWideString(categorySlot) + L" = copy Resource_VB_" + categoryName << std::endl;
//                }
//                else {
//                    outputIniFile << MMTString_ToWideString(categorySlot) + L" = copy Resource_VB_" + categoryName << std::endl;
//                }
//            }
//            
//            outputIniFile << replace_prefix << "drawindexed = auto" << std::endl;
//            outputIniFile << L"ib = Resource_BakIB" << std::endl;
//
//            if (generateSwitchKey) {
//                outputIniFile << "endif" << std::endl;
//            }
//            outputIniFile << std::endl;
//
//
//            //1.д��VBResource����
//            for (std::wstring categoryName : CategoryList) {
//                std::wstring fileName = MMTString_ToWideString(partName) + L"_" + basicConfig.DrawIB + L"_" + categoryName + L".buf";
//                std::wstring filePath = splitOutputFolder + MMTString_ToWideString(partName) + L"/" + fileName;
//                int fileSize = MMTFile_GetFileSize(filePath);
//                
//                outputIniFile << L"[Resource_VB_" + categoryName + L"]" << std::endl;
//                outputIniFile << L"type = Buffer" << std::endl;
//                outputIniFile << L"byte_width = " << std::to_wstring(fileSize) << std::endl;
//
//                if (categoryName == L"Blend" && D3D11ElementAttribute.PatchBLENDWEIGHTS) {
//                    int finalBlendStride = CategoryStrideMap[categoryName] - blendElementByteWidth;
//                    outputIniFile << "stride = " << std::to_wstring(finalBlendStride) << std::endl;
//
//                }
//                //����Normal��λ��Ҫ+1����Ϊ����������д��NORMAL����Ϊ3������Ϸ���4����һ��1
//                else if (categoryName == L"Normal") {
//                    outputIniFile << "stride = " << CategoryStrideMap[categoryName] + 1 << std::endl;
//                }
//                else {
//                    outputIniFile << "stride = " << CategoryStrideMap[categoryName] << std::endl;
//
//                }
//
//                outputIniFile << "filename = " << fileName << std::endl << std::endl;
//            }
//
//            //2.д��IBResource����
//            outputIniFile << L"[Resource_IB_" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L"]" << std::endl;
//            outputIniFile << "type = Buffer" << std::endl;
//            outputIniFile << "format = DXGI_FORMAT_R32_UINT" << std::endl;
//            outputIniFile << "filename = " << MMTString_ToWideString(partName) + L".ib" << std::endl << std::endl;
//
//            //д��ر��ļ�
//            outputIniFile.close();
//        
//        }
//
//        //VertexShaderCheck����
//        LOG.Info(L"Start to generate vertex shader check.");
//        //generate basic_check lines if this game need use basic_check technique.
//        if (wheelConfig.VertexShaderCheck) {
//            Analysis_ModifyShaderCheckByDrawIB(basicConfig.DrawIB, wheelConfig);
//        }
//        LOG.NewLine();
//
//        LOG.NewLine();
//        LOG.Info(L"Generate mod completed!");
//        LOG.NewLine();
//
//    }
//
//}
//
