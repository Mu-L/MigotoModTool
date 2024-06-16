#include "GlobalConfigs.h"
#include "VertexBufferTxtFileDetect.h"
#include "IndexBufferTxtFile.h"
#include "FmtData.h"
#include "GlobalFunctions.h"
#include "IndexBufferBufFile.h"
#include "FrameAnalysisData.h"
#include "ExtractUtil.h"

void ExtractFromBuffer_VS_Reverse() {
    for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        ExtractConfig extractConfig = pair.second;
        //���ó�ʼ������
        std::wstring OutputDrawIBFolder = G.OutputFolder + DrawIB + L"\\";
        std::filesystem::create_directories(OutputDrawIBFolder);

        if (extractConfig.GameType == L"Auto") {
            //������ȡ������ʹ���Զ�����ʶ��
            LOG.Error("Reverse extract doesn't support auto type detect yet,please find your taget mod's game type by extract it's original game model and find the gametype in generated file tmp.json in OutputFolder.");
            // extractConfig.GameType = UnityAutoDetectGameType(DrawIB);
        }

        D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[MMTString_ToByteString(extractConfig.GameType)];
        LOG.Info("GameType: " + d3d11GameType.GameType);

        extractConfig.WorkGameType = MMTString_ToByteString(extractConfig.GameType);

        //ͳ�����е�FrameAnalysis�ļ���
        //����ͨ��txt�ļ����ҵ�pointlist��trianglelist������
        std::unordered_map<std::wstring, int> PointlistIndexVertexCountMap;
        std::unordered_map<std::wstring, int> TrianglelistIndexVertexCountMap;
        int TranglelistMaxVertexCount = 0;
        std::vector<std::wstring> TrianglelistIndices;
        FrameAnalysisData FAData(G.WorkFolder);
        std::vector<std::wstring> FrameAnalysisFileList = FAData.FrameAnalysisFileNameList;
        for (std::wstring FileName : FrameAnalysisFileList) {
            if (!FileName.ends_with(L".txt")) {
                continue;
            }
            std::wstring FilePath = G.WorkFolder + FileName;
            if (FileName.find(L"-vb0") != std::string::npos) {
                //LOG.Info(L"Try detect pointlist file: " + FileName);
                VertexBufferDetect vbDetect(FilePath);
                if (vbDetect.Topology == L"pointlist") {
                    LOG.Info(L"Processing pointlist: " + FilePath);
                    PointlistIndexVertexCountMap[vbDetect.Index] = vbDetect.fileBufferVertexCount;
                }
            }
            else if (FileName.find(L"-ib") != std::string::npos && FileName.find(DrawIB) != std::string::npos) {
                LOG.Info(L"Processing ib file: " + FileName);
                IndexBufferTxtFile indexBufferFileData = IndexBufferTxtFile(FilePath,false);
                //LOG.Info(L"IB File Topology: " + indexBufferFileData.Topology);
                if (indexBufferFileData.Topology == L"trianglelist") {
                    std::vector<std::wstring> tmpNameList = FAData.FindFrameAnalysisFileNameListWithCondition(indexBufferFileData.Index + L"-vb0", L".txt");
                    if (tmpNameList.size() == 0) {
                        LOG.Info(L"Special Type, Continue : Can't find vb0 for -ib:" + FileName);
                        continue;
                    }
                    std::wstring vb0FileName = tmpNameList[0];
                    LOG.Info(L"Processing trianglelist: " + vb0FileName);

                    VertexBufferDetect vertexBufferDetect(G.WorkFolder + vb0FileName);
                    //VertexBufferFileData vertexBufferFileData = VertexBufferFileData(wheelConfig.WorkFolder + vb0FileName, d3D11ElementAttribute, false, false);
                    //LOG.Info(L"VertexCount(Buffer) Str: " + std::to_wstring(vertexBufferDetect.fileBufferVertexCount));
                    
                    
                    //TODO ���ﵽ����ʹ��չʾ��VertexCount���ػ���Buffer�ĺ��أ�
                    //TODO ���ʹ��Buffer�ģ������������ͷ���޷���ȡ��
                    //TODO ��������ٲ���һ��ԭ��Mod����

                    //�����Ѿ�ȷ����ֻ����Buffer��ȷ��������
                    //������ʱ��Buffer�����һ���0��Ϊ������
                    //��Ҫһ������ȥ����β�����е�0x00
                    int vertexCount = vertexBufferDetect.fileBufferVertexCount;
                    if (vertexCount != vertexBufferDetect.fileBufferRealVertexCount) {
                        vertexCount = vertexBufferDetect.fileBufferRealVertexCount;
                        LOG.Info("Meet Null Patched Buffer file, try read real size again.");
                    }

                    LOG.Info(L"VertexCount: " + std::to_wstring(vertexCount));
                    if (vertexCount > TranglelistMaxVertexCount) {
                        TranglelistMaxVertexCount = vertexCount;
                        LOG.Info(L"Set trianglelist max vertex count to: " + std::to_wstring(vertexCount));
                    }

                    TrianglelistIndices.push_back(vertexBufferDetect.Index);
                    TrianglelistIndexVertexCountMap[vertexBufferDetect.Index] = vertexCount;
                }
            }
        }

        LOG.NewLine();

        LOG.Info(L"PointlistIndexVertexNumberMap:");
        std::wstring PointlistExtractIndex = L"";
        for (const auto& pair : PointlistIndexVertexCountMap) {
            std::wstring index = pair.first;
            int vertexCount = pair.second;
            LOG.Info(L"Index: " + index + L", VertexCount: " + std::to_wstring(vertexCount));
            if (vertexCount == TranglelistMaxVertexCount) {
                PointlistExtractIndex = index;
            }
        }
        LOG.Info(L"max vertex count pointlist index: " + PointlistExtractIndex);
        LOG.NewLine();

        // ���������trianglelistIndexVertexNumberMap
        LOG.Info(L"TrianglelistIndexVertexNumberMap:");
        for (const auto& pair : TrianglelistIndexVertexCountMap) {
            LOG.Info(L"Index:" + pair.first + L", VertexCount: " + std::to_wstring(pair.second));
        }
        LOG.NewLine();


        int MaxTrianglelistStride = 0;
        std::wstring TrianglelistExtractIndex;
        std::wstring TexcoordExtractSlot = MMTString_ToWideString(d3d11GameType.ElementNameD3D11ElementMap["TEXCOORD"].ExtractSlot);
        LOG.Info(L"TexcoordExtractSlot: " + TexcoordExtractSlot);

        for (const auto& pair : TrianglelistIndexVertexCountMap) {
            std::wstring Index = pair.first;
            std::vector<std::wstring> filenames = FAData.FindFrameAnalysisFileNameListWithCondition( Index + L"-" + TexcoordExtractSlot, L".txt");
            if (filenames.size() == 0) {
                LOG.Info(Index + L"'s slot " + TexcoordExtractSlot + L" can't find. skip this.");
                continue;
            }
            std::wstring filename = filenames[0];
            VertexBufferDetect vbDetect(G.WorkFolder + filename);
            if (vbDetect.fileRealStride > MaxTrianglelistStride) {
                MaxTrianglelistStride = vbDetect.fileRealStride;
                TrianglelistExtractIndex = Index;
            }
        }
        LOG.Info(L"MaxTrianglelistStride: " + std::to_wstring(MaxTrianglelistStride));
        LOG.Info(L"TrianglelistExtractIndex: " + TrianglelistExtractIndex);

        std::string VertexLimitVB = MMTString_ToByteString(FAData.FindFrameAnalysisFileNameListWithCondition(TrianglelistExtractIndex + L"-vb0", L".txt")[0].substr(11, 8));
        extractConfig.VertexLimitVB = VertexLimitVB;
        LOG.NewLine();

        //����Categoryֱ�ӴӶ�Ӧ������ȡ��ӦBuffer�ļ�
        LOG.Info("Extract from Buffer file: ");
        std::vector<std::unordered_map<int, std::vector<std::byte>>> categoryFileBufList;
        std::vector<std::string> ElementList;
        std::unordered_map<std::string, std::string> CategoryHashMap;
        for (const auto& pair : d3d11GameType.CategorySlotMap) {
            std::string Category = pair.first;
            std::string CategorySlot = pair.second;
            std::string CategoryTopology = d3d11GameType.CategoryTopologyMap[Category];
            std::wstring ExtractIndex = PointlistExtractIndex;

            if (CategoryTopology == "trianglelist") {
                ExtractIndex = TrianglelistExtractIndex;
            }

            //��ȡ��������Ӧ��λ��RealStride
            std::vector<std::wstring> filenames = FAData.FindFrameAnalysisFileNameListWithCondition( ExtractIndex + L"-" + MMTString_ToWideString(CategorySlot), L".txt");
            if (filenames.size() == 0) {
                LOG.Error(ExtractIndex + L"'s extract slot " + TexcoordExtractSlot + L" can't find.");
            }
            std::wstring filename = filenames[0];
            CategoryHashMap[Category] = MMTString_ToByteString(filename.substr(11, 8));
            VertexBufferDetect vbDetect(G.WorkFolder + filename);
            int stride = vbDetect.fileShowStride;

            //�ж��Ƿ���Ҫ����Ĭ�ϵ�Blendweights
            bool patchBlendWeights = false;
            std::vector<std::string> addElementList = vbDetect.realElementNameList;
            LOG.NewLine();
            LOG.Info(L"Extract from: " + filename);
            LOG.Info("AddElementList: ");
            for (std::string elementName : addElementList) {
                LOG.Info(elementName);
            }

            LOG.Info("file show stride: " + std::to_string(stride));
            LOG.Info("elementlist stride: " + std::to_string(d3d11GameType.getElementListStride(addElementList)));
            if (d3d11GameType.getElementListStride(addElementList) != stride) {
                addElementList.pop_back();
            }
            if (d3d11GameType.getElementListStride(addElementList) != stride) {
                LOG.Error("Read real elementlist is not equalt to stride.wrong parse type!");
            }

            if (addElementList.size() == 1 && "blendindices" == boost::algorithm::to_lower_copy(addElementList[0])) {
                addElementList = { "blendweights","blendindices" };
                patchBlendWeights = true;
                LOG.Info("Detect only BLENDINDICES, auto patch BLENDWEIGHTS 1,0,0,0");
            }
            ElementList.insert(ElementList.end(), addElementList.begin(), addElementList.end());


            //��ȡ������
            std::wstring bufFileName = filename.substr(0, filename.length() - 4) + L".buf";
            int bufFileSize = MMTFile_GetFileSize(G.WorkFolder + bufFileName);
            int vertexNumber = bufFileSize / stride;
            LOG.Info("Stride:" + std::to_string(stride));
            std::unordered_map<int, std::vector<std::byte>> fileBuf = MMTFile_ReadBufMapFromFile(G.WorkFolder + bufFileName, vertexNumber);
            LOG.Info(L"Extract from: " + bufFileName + L" VertexNumber:" + std::to_wstring(vertexNumber));
            LOG.Info("Category:" + Category + " CategorySlot:" + CategorySlot + " CategoryTopology:" + CategoryTopology);

            //�Զ���ȫBLENDWEIGHT 1,0,0,0  Ŀǰ��֧��R32G32B32A32_FLOAT���Ͳ�ȫ��ĿǰҲֻ��������һ������
            std::unordered_map<int, std::vector<std::byte>> patchedFileBuf;
            if (Category == "Blend" && patchBlendWeights) {

                for (const auto& pair : fileBuf) {
                    std::vector<std::byte> patchValue = {
                        std::byte{0x00}, std::byte{0x00}, std::byte{0x80}, std::byte{0x3f} ,
                        std::byte{0x00}, std::byte{0x00}, std::byte{0x00} , std::byte{0x00},
                        std::byte{0x00}, std::byte{0x00}, std::byte{0x00} , std::byte{0x00},
                        std::byte{0x00}, std::byte{0x00}, std::byte{0x00} , std::byte{0x00} };
                    int num = pair.first;
                    std::vector<std::byte> bufValue = pair.second;
                    patchValue.insert(patchValue.end(), bufValue.begin(), bufValue.end());
                    patchedFileBuf[num] = patchValue;
                }
                categoryFileBufList.push_back(patchedFileBuf);

            }
            else {
                categoryFileBufList.push_back(fileBuf);

            }


        }
        extractConfig.CategoryHashMap = CategoryHashMap;
        LOG.NewLine();
        //Buffer�ļ���ϳ�һ��
        std::vector<std::byte> finalVB0Buf;
        std::unordered_map<int, std::vector<std::byte>> tmpFileBuf;
        for (int i = 0; i < TranglelistMaxVertexCount; i++) {
            for (std::unordered_map<int, std::vector<std::byte>>& tmpFileBuf : categoryFileBufList) {
                finalVB0Buf.insert(finalVB0Buf.end(), tmpFileBuf[i].begin(), tmpFileBuf[i].end());
            }
        }

        LOG.Info("ElementList:");
        std::vector<std::string> TmpElementList;
        for (std::string ElementName : ElementList) {
            LOG.Info("ElementName: " + ElementName);
            TmpElementList.push_back(boost::algorithm::to_upper_copy(ElementName));
        }
        extractConfig.TmpElementList = TmpElementList;



        //����FMT�ļ�
        //TODO FMT�������
        FmtFileData fmtFileData;
        fmtFileData.ElementNameList = ElementList;
        fmtFileData.d3d11GameType = d3d11GameType;
        fmtFileData.Stride = d3d11GameType.getElementListStride(ElementList);
        //fmtFileData.Format = L"DXGI_FORMAT_R32_UINT";
        LOG.NewLine();


        //IB��Buffer�ļ������������
        LOG.Info(L"Start to read trianglelist IB file info");
        // ��ȷ�����ڲ�ͬ��trianglelist indices��ib�ļ�������first index��ȷ�ϡ�
        std::map<int, std::wstring> firstIndexFileNameMap;
        for (std::wstring index : TrianglelistIndices) {
            std::vector<std::wstring> trianglelistIBFileNameList = FAData.FindFrameAnalysisFileNameListWithCondition(index + L"-ib", L".txt");
            std::wstring trianglelistIBFileName;
            if (trianglelistIBFileNameList.empty()) {
                continue;
            }
            trianglelistIBFileName = trianglelistIBFileNameList[0];
            //LOG.Info(L"trianglelistIBFileName: " + trianglelistIBFileName);
            IndexBufferTxtFile indexBufferFileData = IndexBufferTxtFile(G.WorkFolder + trianglelistIBFileName,false);
            std::wstring firstIndex = indexBufferFileData.FirstIndex;
            //LOG.Info(L"firstIndex: " + firstIndex);
            firstIndexFileNameMap[std::stoi(firstIndex)] = trianglelistIBFileName;
        }
        LOG.Info(L"Output and see the final first index and trianglelist file:");
        std::vector<std::string> firstIndexList;
        for (auto it = firstIndexFileNameMap.begin(); it != firstIndexFileNameMap.end(); ++it) {
            LOG.Info(L"firstIndex: " + std::to_wstring(it->first) + L", trianglelistIBFileName: " + it->second);
            firstIndexList.push_back(std::to_string(it->first));
        }

        LOG.NewLine();

        //���ھͿ�������ˣ�����ÿ��MatchFirstIndex����������ɡ�
        int outputCount = 1;
        std::vector<std::string> MatchFirstIndexList;
        std::vector<std::string> PartNameList;
        for (const auto& pair : firstIndexFileNameMap) {
            std::wstring IBReadFileName = pair.second;
            std::wstring IBReadBufferFileName = IBReadFileName.substr(0, IBReadFileName.length() - 4) + L".buf";
            std::wstring IBReadFilePath = G.WorkFolder + IBReadFileName;
            IndexBufferTxtFile ibFileData(IBReadFilePath,false);
            //TODO ����Ҫ��ȡtxt��ʽ��ib�ļ���Ȼ��ת����buffer��ʽ��ib����ȥ��Сֵ���������
            //Ҳ���ԣ���ȥ��СֵҲ���ˣ�����ȫ�����ŵ�һ��IB�����ˣ�����Ҫ��ȥ��Сֵ��ֱ��������С�


            std::wstring Format = ibFileData.Format;
            std::wstring IBReadBufferFilePath = G.WorkFolder + IBReadBufferFileName;
            std::wstring OutputIBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".ib";
            LOG.Info(IBReadBufferFilePath);

            IndexBufferBufFile ibBufFile(IBReadBufferFilePath, Format);

            int minNumber = ibBufFile.MinNumber;
            int maxNumber = ibBufFile.MaxNumber;

            int readLength = 2;
            if (boost::algorithm::to_lower_copy(Format) == L"dxgi_format_r16_uint") {
                readLength = 2;
            }
            if (boost::algorithm::to_lower_copy(Format) == L"dxgi_format_r32_uint") {
                readLength = 4;
            }
            std::ifstream ReadIBFile(IBReadBufferFilePath, std::ios::binary);
            
            std::vector<uint16_t> IBR16DataList = {};
            std::vector<uint32_t> IBR32DataList = {};
            
            char* data = new char[readLength];
            
            while (ReadIBFile.read(data, readLength)) {
                if (boost::algorithm::to_lower_copy(Format) == L"dxgi_format_r16_uint") {
                    std::uint16_t value = MMTFormat_CharArrayToUINT16_T(data);
                    value = value - static_cast<std::uint16_t>(minNumber);
                    IBR16DataList.push_back(value);
            
                }
                if (boost::algorithm::to_lower_copy(Format) == L"dxgi_format_r32_uint") {
                    std::uint32_t value = MMTFormat_CharArrayToUINT32_T(data);
                    value = value - static_cast<std::uint32_t>(minNumber);
                    IBR32DataList.push_back(value);
                }
            }
            
            ReadIBFile.close();
            
            if (boost::algorithm::to_lower_copy(Format) == L"dxgi_format_r16_uint") {
                IBR32DataList = std::vector<uint32_t>(IBR16DataList.size());
                std::transform(IBR16DataList.begin(), IBR16DataList.end(), IBR32DataList.begin(),
                    [](uint16_t value) { return static_cast<uint32_t>(value); });
            }
            
            LOG.Info(L"IB file format: " + boost::algorithm::to_lower_copy(Format));
            LOG.Info(L"IB file length: " + std::to_wstring(IBR32DataList.size()));
            std::ofstream file(OutputIBBufFilePath, std::ios::binary);
            for (const auto& data : IBR32DataList) {
                uint32_t paddedData = data; 
                file.write(reinterpret_cast<const char*>(&paddedData), sizeof(uint32_t));
            }
            file.close(); 

            MatchFirstIndexList.push_back(MMTString_ToByteString(ibFileData.FirstIndex));
            PartNameList.push_back(std::to_string(outputCount));



            //���FMT�ļ�
            std::wstring OutputFmtFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".fmt";
            fmtFileData.Format = L"DXGI_FORMAT_R32_UINT";//����������ն������ΪR32_UINT����������ת��
            fmtFileData.OutputFmtFile(OutputFmtFilePath);
            
            //���VB�ļ�
            //VB0�ļ�Ҫ�и�������
            LOG.Info("MinNumber:" + std::to_string(minNumber));
            LOG.Info("MaxNumber:" + std::to_string(maxNumber));

            std::vector<std::byte> outputVBBuf = MMTFormat_GetRange_Byte(
                finalVB0Buf,
                minNumber * fmtFileData.Stride,
                (maxNumber + 1)* fmtFileData.Stride);

            LOG.Info("finalVB0Buf size:" + std::to_string(finalVB0Buf.size()));
            LOG.Info("outputVBBuf size:" + std::to_string(outputVBBuf.size()));

            std::wstring OutputVBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".vb";
            std::ofstream outputVBFile(OutputVBBufFilePath, std::ofstream::binary);
            outputVBFile.write(reinterpret_cast<const char*>(outputVBBuf.data()), outputVBBuf.size());
            outputVBFile.close();

            outputCount++;
        }

        extractConfig.MatchFirstIndexList = MatchFirstIndexList;
        extractConfig.PartNameList = PartNameList;
        extractConfig.saveTmpConfigs(OutputDrawIBFolder);
        //���tmp.json

        ExtractUtil_MoveAllTextures(G.WorkFolder, DrawIB, OutputDrawIBFolder);
    }
}