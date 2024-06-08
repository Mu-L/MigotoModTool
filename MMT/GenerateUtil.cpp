#include "GenerateUtil.h"
#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MigotoFormatUtils.h"
#include "VertexBufferBufFile.h"

//�������������ʹ�õĶ�ȡ���ָ��ؼ���
void SplitUtil::readSplitRecalculate() {
    //�������ļ���ȡtangent���㷽��
    std::wstring TangentAlgorithm = basicConfig.TangentAlgorithm;
    
    this->CategoryList = d3d11GameType.getCategoryList(basicConfig.TmpElementList);
    this->CategoryStrideMap = d3d11GameType.getCategoryStrideMap(basicConfig.TmpElementList);
    //����鿴ÿ��Category�Ĳ���
    for (const auto& pair : CategoryStrideMap) {
        const std::string& key = pair.first;
        int value = pair.second;
        LOG.Info(L"Category: " + MMTString_ToWideString(key) + L", CategoryStride: " + std::to_wstring(value));
    }
    LOG.NewLine();

    //�鿴CategoryList
    LOG.Info(L"CategoryList:");
    for (std::string categoryName: CategoryList) {
        LOG.Info(L"Ordered CategoryName: " + MMTString_ToWideString(categoryName));
    }
    LOG.NewLine();

    //��ȡvb�ļ���ÿ��vb�ļ�������category�ֿ�װ�ز�ͬcategory������
    int SplitStride = d3d11GameType.getElementListStride(basicConfig.TmpElementList);
    LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));

    for (std::string partName : basicConfig.PartNameList) {
        partNameOffsetMap[MMTString_ToWideString(partName)] = drawNumber;
        LOG.Info(L"Set partName: " + MMTString_ToWideString(partName) + L" offset to drawNumber: " + std::to_wstring(drawNumber));
        std::wstring VBFileName = MMTString_ToWideString(partName) + L".vb";

        uint64_t VBFileSize = MMTFile_GetFileSize(splitReadFolder + VBFileName);
        uint64_t vbFileVertexNumber = VBFileSize / SplitStride;
        //��ӵ�drawNumber
        drawNumber = drawNumber + vbFileVertexNumber;
        LOG.Info(L"Processing VB file: " + VBFileName + L" size is: " + std::to_wstring(VBFileSize) + L" byte." + L" vertex number is: " + std::to_wstring(vbFileVertexNumber));
        
        VertexBufferBufFile vbBufFile(splitReadFolder + VBFileName,d3d11GameType,basicConfig.TmpElementList);
        partName_VBCategoryDaytaMap[partName] = vbBufFile.CategoryVBDataMap;
    }


    LOG.Info(L"Combine and put partName_VBCategoryDaytaMap's content back to finalVBCategoryDataMap");
    //��partName_VBCategoryDaytaMap������ݣ�����finalVBCategoryDataMap����ϳ�һ����������ʹ��
    for (std::string partName : basicConfig.PartNameList) {
        std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = partName_VBCategoryDaytaMap[partName];

        for (size_t i = 0; i < CategoryList.size(); ++i) {
            const std::string& category = CategoryList[i];
            std::vector<std::byte> tmpCategoryData = tmpVBCategoryDataMap[MMTString_ToWideString(category)];

            std::vector<std::byte>& finalCategoryData = finalVBCategoryDataMap[MMTString_ToWideString(category)];
            finalCategoryData.insert(finalCategoryData.end(), tmpCategoryData.begin(), tmpCategoryData.end());

        }
    }
    LOG.NewLine();
    //������Ƿָ��ȫ������

    //����ģ���ϼ��� TANGENT�ؼ���
    if (TangentAlgorithm == L"average_normal") {
        std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = finalVBCategoryDataMap;
        //��������
        std::unordered_map<std::wstring, std::vector<std::byte>> finalfixTangentVBCategoryDataMap = TANGENT_averageNormal(tmpVBCategoryDataMap,d3d11GameType);
        //������ɺ�ֵ��ȥ
        finalVBCategoryDataMap = finalfixTangentVBCategoryDataMap;
    }

    //COLORֵ�ؼ���
    if (basicConfig.ColorAlgorithm == L"average_normal") {
        std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = finalVBCategoryDataMap;
        //��������
        std::unordered_map<std::wstring, std::vector<std::byte>> finalfixTangentVBCategoryDataMap = RecalculateColor(tmpVBCategoryDataMap, basicConfig, CategoryStrideMap);
        //������ɺ�ֵ��ȥ
        finalVBCategoryDataMap = finalfixTangentVBCategoryDataMap;
        
    }
  
    LOG.Info(std::to_wstring(finalVBCategoryDataMap[L"Position"].size() / 40));
    // ����鿴drawNumber:
    LOG.Info(L"Set draw number to: " + std::to_wstring(drawNumber));
    LOG.NewLine();

    LOG.Info(L"Start to flip NORMAL and TANGENT Values:");
    std::unordered_map<std::wstring, std::vector<std::byte>> finalfixTangentVBCategoryDataMap;
    finalfixTangentVBCategoryDataMap = ReverseNormalTangentValues(finalVBCategoryDataMap, basicConfig);
    finalVBCategoryDataMap = finalfixTangentVBCategoryDataMap;
    LOG.NewLine();

    LOG.Info(L"Start to reset COLOR Values:");
    std::unordered_map<std::wstring, std::vector<std::byte>> finalfixColorVBCategoryDataMap;
    finalfixColorVBCategoryDataMap = ResetColor(finalVBCategoryDataMap, basicConfig, CategoryStrideMap);
    finalVBCategoryDataMap = finalfixColorVBCategoryDataMap;
    LOG.NewLine();
    // ������ǰ�ÿ�� vb�����ݶ�ȡ���б��Ȼ���ÿ��vb�޸�Color��Tangent�󣬷ָbuf
}


void SplitUtil::outputModFiles() {
    //(2) ���濪ʼ����ֱ�����������Buf�ļ�
    LOG.NewLine();

    //��ɾ����һ�����ɵ� buf�ļ� �� ib�ļ� ini�ļ�
    MMTFile_DeleteFilesWithSuffix(splitOutputFolder, L".buf");
    MMTFile_DeleteFilesWithSuffix(splitOutputFolder, L".ib");
    //��ɾ����һ�����ɵ�
    MMTFile_DeleteFilesWithSuffix(splitOutputFolder, L".ini");

    //��ȡBLENDWEIGHTS������
    std::wstring blendElementName;
    if (boost::algorithm::any_of_equal(basicConfig.TmpElementList, "BLENDWEIGHT")) {
        blendElementName = L"BLENDWEIGHT";
    }
    else {
        blendElementName = L"BLENDWEIGHTS";
    }
    blendElementByteWidth = d3d11GameType.ElementNameD3D11ElementMap[MMTString_ToByteString(blendElementName)].ByteWidth;
    int blendIndicesByteWidth = d3d11GameType.ElementNameD3D11ElementMap["BLENDINDICES"].ByteWidth;

    LOG.Info(L"Get BlendWeigths width: " + std::to_wstring(blendElementByteWidth));
    LOG.Info(L"Get BlendIndices width: " + std::to_wstring(blendIndicesByteWidth));

    for (const auto& pair : finalVBCategoryDataMap) {
        const std::wstring& categoryName = pair.first;
        const std::vector<std::byte>& categoryData = pair.second;
        LOG.Info(L"Output buf file, current category: " + categoryName + L" Length:" + std::to_wstring(categoryData.size() / drawNumber));

        //���û���ǾͲ����
        if (categoryData.size() == 0) {
            LOG.Info(L"Current category's size is 0, can't output, skip this.");
            continue;
        }

        std::wstring categoryGeneratedName = L"";
        
        categoryGeneratedName = basicConfig.DrawIB + categoryName;
            
        this->categoryUUIDMap[MMTString_ToByteString(categoryName)] = MMTString_ToByteString(categoryGeneratedName);
       

        // ��������ļ�·��
        std::wstring outputDatFilePath = splitOutputFolder + categoryGeneratedName + L".buf";
        // ������ļ�
        std::ofstream outputFile(MMTString_ToByteString(outputDatFilePath), std::ios::binary);


        //���ȱʧBLENDWEIGHTSԪ�أ�����Ҫ�ֶ�ɾ��BLEND��λ�е�BLENDԪ��
        if (d3d11GameType.PatchBLENDWEIGHTS && categoryName == L"Blend") {

            LOG.Info(L"Detect element: " + blendElementName + L" need to delete , now will delete it.");
            //ȥ��BLENDWEIGHTS��������б�
            std::vector<std::byte> newBlendCategoryData;

            //������ֻ��BLENDINDICES��ӵ�Ԫ���б�
            int blendWeightCount = 1;
            int blendIndicesCount = 1;
            for (std::byte singleByte : categoryData) {
                //std::cout << "��ǰblendWeightCount: " << blendWeightCount << std::endl;
                //std::cout << "��ǰblendIndicesCount: " << blendIndicesCount << std::endl;
                if (blendWeightCount <= blendElementByteWidth) {
                    blendWeightCount += 1;
                }
                else {
                    //Ȼ��ʼ��ӵ�newBlendCategoryData
                    if (blendIndicesCount <= blendIndicesByteWidth) {
                        //std::cout << "ִ�з������" << std::endl;
                        newBlendCategoryData.push_back(singleByte);
                        blendIndicesCount += 1;


                        //������֮�����Ҫ���̹��㣬���������ж�
                        if ((blendWeightCount == blendElementByteWidth + 1) && (blendIndicesCount == blendIndicesByteWidth + 1)) {
                            blendWeightCount = 1;
                            blendIndicesCount = 1;
                        }
                    }
                    else {
                        blendWeightCount = 1;
                        blendIndicesCount = 1;

                    }
                }


            }
            outputFile.write(reinterpret_cast<const char*>(newBlendCategoryData.data()), newBlendCategoryData.size());
        }
        else {
            // ��std::vecto������д���ļ�
            outputFile.write(reinterpret_cast<const char*>(categoryData.data()), categoryData.size());
        }
        outputFile.close();

        LOG.Info(L"Write " + categoryName + L" data into file: " + outputDatFilePath);
    }
    LOG.NewLine();

    //�����ö�ȡib�ļ���ʹ�õĲ���,��fmt�ļ����Զ���ȡ
    //��ȡbody_part0.fmt�ļ��е�Format
    std::wstring readFormatFileName = splitReadFolder + L"1.fmt";
    std::wstring IBReadDxgiFormat = MMTFile_FindMigotoIniAttributeInFile(readFormatFileName, L"format");
    LOG.Info(L"Auto read IB format: " + IBReadDxgiFormat);
    LOG.Info(L"Default output IB format: DXGI_FORMAT_R32_UINT");
    LOG.NewLine();

    //��ȡib�ļ�,ת����ʽ��ֱ��д��
    for (std::string partName : basicConfig.PartNameList) {
        uint64_t offset = partNameOffsetMap[MMTString_ToWideString(partName)];
        std::wstring IBFileName = MMTString_ToWideString(partName) + L".ib";
        LOG.Info(L"Processing IB file: " + IBFileName);

        // ָ��Ҫ��ȡ���ֽڳ���
        int readLength = 2;
        if (IBReadDxgiFormat == L"DXGI_FORMAT_R16_UINT") {
            readLength = 2;
        }
        if (IBReadDxgiFormat == L"DXGI_FORMAT_R32_UINT") {
            readLength = 4;
        }
        //ѭ����ȡֱ���ļ�����
        std::ifstream ReadIBFile(MMTString_ToByteString(splitReadFolder + IBFileName), std::ios::binary);

        //R32_UINT�������Ժ���������������Թ̶�ʹ��R16_UINT������ܶ�
        std::vector<uint16_t> IBR16DataList = {};
        std::vector<uint32_t> IBR32DataList = {};

        // ����char������ȡ����
        char* data = new char[readLength];

        if (IBReadDxgiFormat == L"DXGI_FORMAT_R16_UINT") {
            while (ReadIBFile.read(data, readLength)) {
                std::uint16_t value = (static_cast<unsigned char>(data[1]) << 8) |
                    static_cast<unsigned char>(data[0]);
                value = value + static_cast<std::uint16_t>(offset);
                IBR16DataList.push_back(value);
            }
        }
        else if (IBReadDxgiFormat == L"DXGI_FORMAT_R32_UINT") {
            while (ReadIBFile.read(data, readLength)) {
                std::uint32_t value = (static_cast<unsigned char>(data[3]) << 24) |
                    (static_cast<unsigned char>(data[2]) << 16) |
                    (static_cast<unsigned char>(data[1]) << 8) |
                    static_cast<unsigned char>(data[0]);
                value = value + static_cast<std::uint32_t>(offset);
                IBR32DataList.push_back(value);
            }

        }
        else {
            LOG.Error(L"Can't find a valid DXGI FORMAT, current procesing DXGI FORMAT: " + IBReadDxgiFormat);
        }

        // ��ȡ��ɹر��ļ�
        ReadIBFile.close();

        std::wstring partNameGeneratedName = L"";
        
        partNameGeneratedName = this->basicConfig.DrawIB + wheelConfig.GIMIPartNameAliasMap[MMTString_ToWideString(partName)];
            
        this->partNameUUIDMap[partName] = MMTString_ToByteString(partNameGeneratedName);


        //ƴ��������ļ���
        std::wstring outputIBFileName = splitOutputFolder + partNameGeneratedName + L".ib";

        //����̶�ΪR32_UINT�����������ȡ����R16_UINT����Ҫ��һ��ת��
        if (IBReadDxgiFormat == L"DXGI_FORMAT_R16_UINT") {
            //ת��֮ǰ�ó�ʼ�����ȣ���Ȼ�ᱨ��
            IBR32DataList = std::vector<uint32_t>(IBR16DataList.size());
            std::transform(IBR16DataList.begin(), IBR16DataList.end(), IBR32DataList.begin(),
                [](uint16_t value) { return static_cast<uint32_t>(value); });
        }

        //��ʼ���
        LOG.Info(L"IB file length: " + std::to_wstring(IBR32DataList.size()));
        std::ofstream file(MMTString_ToByteString(outputIBFileName), std::ios::binary); // ���ļ�
        //д��ʱ����ȷ����4���ֽڵĳ���
        for (const auto& data : IBR32DataList) {
            uint32_t paddedData = data; // ԭʼ����
            file.write(reinterpret_cast<const char*>(&paddedData), sizeof(uint32_t));
        }
        file.close(); // �ر��ļ�

        //offset = offset + partNameOffsetMap[partName];
    }
}

SplitUtil::SplitUtil() {

}

SplitUtil::SplitUtil(GlobalConfigs wheelConfig, ExtractConfig basicConfig, D3D11GameType d3d11GameType) {
    this->wheelConfig = wheelConfig;
    this->basicConfig = basicConfig;
    this->d3d11GameType = d3d11GameType;
}

