#include "GlobalConfigs.h"
#include "VertexBufferTxtFileDetect.h"
#include "IndexBufferTxtFile.h"
#include "FmtData.h"
#include "GlobalFunctions.h"
#include "ConstantBufferBufFile.h"
#include <set>
#include "FrameAnalysisData.h"
#include "ExtractUtil.h"


void ExtractFromBuffer_CS_WW_Body(std::wstring DrawIB,std::wstring GameType) {
    ExtractConfig extractConfig = G.DrawIB_ExtractConfig_Map[DrawIB];
    D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[MMTString_ToByteString(GameType)];
    LOG.Info("GameType: " + d3d11GameType.GameType);
    std::wstring OutputDrawIBFolder = G.OutputFolder + DrawIB + L"\\";
    std::filesystem::create_directories(OutputDrawIBFolder);
    FrameAnalysisData FAData(G.WorkFolder);
    std::vector<std::wstring> FrameAnalyseFileNameList = FAData.FrameAnalysisFileNameList;
    std::map<int, std::wstring> matchFirstIndexIBFileNameMap;
    std::wstring VSExtractIndex;
    // ��vs-t0�ж�ȡ����������
    // Ȼ��������Ĺ��������ƶ�������ƥ�䵽���ǵ�DrawNumber�����ö�Ӧ�������Ŷԣ�����������������
    std::wstring TexcoordExtractFileName = L"";
    int MatchNumber = 0;
    for (std::wstring filename : FrameAnalyseFileNameList) {
        if (!filename.ends_with(L".txt")) {
            continue;
        }
        if (filename.find(L"-ib=" + DrawIB) == std::wstring::npos) {
            continue;
        }
        IndexBufferTxtFile ibFileData(G.WorkFolder + filename, false);

        VSExtractIndex = ibFileData.Index;


        TexcoordExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + L"-vs-t0=", L".buf")[0];
        int TexcoordFileSize = MMTFile_GetFileSize(G.WorkFolder + TexcoordExtractFileName);
        MatchNumber = TexcoordFileSize / 16;
        LOG.Info("Match DrawNumber: " + std::to_string(MatchNumber));


        LOG.Info(filename);
        if (ibFileData.FirstIndex == L"") {
            LOG.Info("Can't find FirstIndex attribute in this file, so skip this.");
            continue;
        }
        int matchFirstIndex = std::stoi(ibFileData.FirstIndex);

        matchFirstIndexIBFileNameMap[matchFirstIndex] = filename;
    }
    for (const auto& pair : matchFirstIndexIBFileNameMap) {
        LOG.Info("Match First Index: " + std::to_string(pair.first));
    }
    LOG.NewLine();


    LOG.NewLine();
    int MaxDrawNumber = 0;
    std::wstring MaxDrawNumberCSIndex = L"";
    std::string DrawComputeShader = "";
    for (std::wstring filename : FrameAnalyseFileNameList) {
        if (!filename.ends_with(L".buf")) {
            continue;
        }
        if (filename.find(L"-cs=") == std::wstring::npos) {
            continue;
        }
        //��ʱֻ��Ҫcs-cb0�͹���
        if (filename.find(L"-cs-cb0=") == std::wstring::npos) {
            continue;
        }
        //��Ҫ���˵���������ComputeShader���ļ���
        if (filename.find(L"1ff924db9d4048d1") == std::wstring::npos && filename.find(L"4d0760c2c7406824") == std::wstring::npos) {
            continue;
        }

        LOG.Info(filename);
        std::wstring filepath = G.WorkFolder + filename;
        //LOG.Info(filepath);

        ConstantBufferBufFile cbFileData(filepath);
        LOG.Info("CB[0].W CS Calculate Time: " + std::to_string(cbFileData.lineCBValueMap[0].W));
        LOG.Info("CB[0].Y Draw Number1: " + std::to_string(cbFileData.lineCBValueMap[0].Y));
        LOG.Info("CB[0].Z Draw Number2: " + std::to_string(cbFileData.lineCBValueMap[0].Z));
        LOG.Info("CB[1].X Draw Number3: " + std::to_string(cbFileData.lineCBValueMap[1].X));

        //�������cb0.w�� cb0.y��ͬ����Ϊcb0.w����cb1.x
        //�����ͬ��Ϊcb0.w + cb0.x
        int drawNumber = 0;
        if (cbFileData.lineCBValueMap[0].W == cbFileData.lineCBValueMap[0].Y) {
            drawNumber = cbFileData.lineCBValueMap[0].W + cbFileData.lineCBValueMap[1].X;
            LOG.Info("CB[0].W == CB[0].Y   DrawNumber = CB[0].W + CB[1].X = " + std::to_string(drawNumber));
        }
        else {
            drawNumber = cbFileData.lineCBValueMap[0].W + cbFileData.lineCBValueMap[0].Y;
            LOG.Info("CB[0].W != CB[0].Y   DrawNumber = CB[0].W + CB[0].Y = " + std::to_string(drawNumber));
        }
        LOG.NewLine();

        //LOG.Info("Draw Number: " + std::to_string(drawNumber));

        //����ֻ��drawNumber�������Ǵ�vs-t0���ҵ���DrawNumberʱ���Ž����滻
        if (drawNumber > MaxDrawNumber) {
            MaxDrawNumber = drawNumber;
            if (MatchNumber == drawNumber) {
                //��Ⲣ����DrawComputeShader�������жϾ���Ҫ���ĸ���λ��ȡ
                if (filename.find(L"1ff924db9d4048d1") != std::wstring::npos) {
                    DrawComputeShader = "1ff924db9d4048d1";

                }
                else if (filename.find(L"4d0760c2c7406824") != std::wstring::npos) {
                    DrawComputeShader = "4d0760c2c7406824";
                }
                LOG.Info("Find Match Number File! Set draw compute shader to : " + DrawComputeShader);
                MaxDrawNumberCSIndex = filename.substr(0, 6);
            }
        }

    }
    LOG.Info(L"MaxDrawNumber: " + std::to_wstring(MaxDrawNumber));
    LOG.Info(L"MaxDrawNumberIndex: " + MaxDrawNumberCSIndex);

    /*
        1.��DrawComputeShaderΪ1ff924db9d4048d1ʱCS����λ�������£�
            cs-t3 stride=8    33145   ֻ���ǳ��ȸ�Ϊ4��BLENDWEIGHT��BLENDINDICES
            cs-t4 stride=8    33145   NORMAL,COLOR
            cs-t5 stride=12   33145   POSITION
        2.��DrawComputeShaderΪ4d0760c2c7406824ʱCS����λ�������£�
            cs-t4 stride=8    33145   ֻ���ǳ��ȸ�Ϊ4��BLENDWEIGHT��BLENDINDICES
            cs-t5 stride=8    33145   NORMAL,COLOR
            cs-t6 stride=12   33145   POSITION
    */
    std::wstring PositionExtractSlot = L"-cs-t5=";
    std::wstring NormalExtractSlot = L"-cs-t4=";
    std::wstring BlendExtractSlot = L"-cs-t3=";
    if (DrawComputeShader == "1ff924db9d4048d1") {
        PositionExtractSlot = L"-cs-t5=";
        NormalExtractSlot = L"-cs-t4=";
        BlendExtractSlot = L"-cs-t3=";
    }
    else if (DrawComputeShader == "4d0760c2c7406824") {
        PositionExtractSlot = L"-cs-t6=";
        NormalExtractSlot = L"-cs-t5=";
        BlendExtractSlot = L"-cs-t4=";
    }
    else {
        LOG.Info("Can't find DrawComputeShader, use default setting.");
    }
    LOG.Info(L"Position Extract Slot: " + PositionExtractSlot);
    LOG.Info(L"Normal Extract Slot: " + NormalExtractSlot);
    LOG.Info(L"Blend Extract Slot: " + BlendExtractSlot);
    LOG.NewLine();
    //�ռ�������λ�����ݣ�����ϳ�VB0������
    std::wstring PositionExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(MaxDrawNumberCSIndex + PositionExtractSlot, L".buf")[0];
    std::wstring NormalColorExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(MaxDrawNumberCSIndex + NormalExtractSlot, L".buf")[0];
    std::wstring BlendExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(MaxDrawNumberCSIndex + BlendExtractSlot, L".buf")[0];

    //����ı������ŵ������ʼ����
    LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
    LOG.Info(L"NormalExtractFileName: " + NormalColorExtractFileName);
    LOG.Info(L"BlendExtractFileName: " + BlendExtractFileName);
    LOG.Info(L"TexcoordExtractFileName: " + TexcoordExtractFileName);

    std::unordered_map<int, std::vector<std::byte>> PositionBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + PositionExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> NormalTangentBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + NormalColorExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> BlendBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + BlendExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> TexcoordBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + TexcoordExtractFileName, MatchNumber);

    std::wstring CategoryHash_Position = PositionExtractFileName.substr(13, 8);
    std::wstring CategoryHash_NormalColor = NormalColorExtractFileName.substr(13, 8);
    std::wstring CategoryHash_Blend = BlendExtractFileName.substr(13, 8);
    std::wstring CategoryTexcoord = TexcoordExtractFileName.substr(13, 8);

    extractConfig.CategoryHashMap["Position"] = MMTString_ToByteString(CategoryHash_Position);
    extractConfig.CategoryHashMap["Normal"] = MMTString_ToByteString(CategoryHash_NormalColor);
    extractConfig.CategoryHashMap["Blend"] = MMTString_ToByteString(CategoryHash_Blend);
    extractConfig.CategoryHashMap["Texcoord"] = MMTString_ToByteString(CategoryTexcoord);


    std::vector<std::byte> finalVB0Buf;
    for (int i = 0; i < MatchNumber; i++) {
        finalVB0Buf.insert(finalVB0Buf.end(), PositionBufMap[i].begin(), PositionBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), NormalTangentBufMap[i].begin(), NormalTangentBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), BlendBufMap[i].begin(), BlendBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), TexcoordBufMap[i].begin(), TexcoordBufMap[i].end());
    }

    FmtFileData fmtFileData;
    fmtFileData.ElementNameList = d3d11GameType.OrderedFullElementList;
    fmtFileData.d3d11GameType = d3d11GameType;
    LOG.NewLine();

    LOG.Info("Start to go through every IB file:");
    //����ÿһ��IB�����
    int outputCount = 1;
    std::vector<std::string> MatchFirstIndexList;
    std::vector<std::string> PartNameList;

    for (const auto& pair : matchFirstIndexIBFileNameMap) {
        std::wstring IBFileName = pair.second;
        std::wstring Index = IBFileName.substr(0, 6);

        std::wstring IBReadBufferFileName = IBFileName.substr(0, IBFileName.length() - 4) + L".buf";
        std::wstring IBReadFilePath = G.WorkFolder + IBFileName;
        IndexBufferTxtFile ibFileData(IBReadFilePath, true);

        MatchFirstIndexList.push_back(MMTString_ToByteString(ibFileData.FirstIndex));
        PartNameList.push_back(std::to_string(outputCount));

        LOG.Info(IBFileName);
        LOG.Info(L"MatcheFirstIndex: " + ibFileData.FirstIndex + L"  PartName:" + std::to_wstring(outputCount));
        LOG.Info("MinNumber: " + std::to_string(ibFileData.MinNumber) + "\t\tMaxNumber:" + std::to_string(ibFileData.MaxNumber));

        std::wstring Format = ibFileData.Format;
        std::wstring IBReadBufferFilePath = G.WorkFolder + IBReadBufferFileName;
        std::unordered_map<int, std::vector<std::byte>> IBFileBuf = MMTFile_ReadIBBufFromFile(IBReadBufferFilePath, Format);
        int FirstIndex = std::stoi(ibFileData.FirstIndex);
        int IndexCount = std::stoi(ibFileData.IndexCount);

        int EndIndex = FirstIndex + IndexCount;
        std::vector<std::byte> finalIBBuf;
        std::vector<std::byte> tmpIBBuf;
        for (int i = FirstIndex; i < EndIndex; i++) {
            tmpIBBuf = IBFileBuf[i];
            finalIBBuf.insert(finalIBBuf.end(), tmpIBBuf.begin(), tmpIBBuf.end());
        }
        fmtFileData.Format = Format;

        //�ֱ����fmt,ib,vb
        std::wstring OutputIBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".ib";
        std::wstring OutputVBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".vb";
        std::wstring OutputFmtFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".fmt";

        //���FMT�ļ�
        fmtFileData.OutputFmtFile(OutputFmtFilePath);
        //���IB�ļ�
        std::ofstream outputIBFile(OutputIBBufFilePath, std::ofstream::binary);
        outputIBFile.write(reinterpret_cast<const char*>(finalIBBuf.data()), finalIBBuf.size());
        outputIBFile.close();
        //���VB�ļ�
        std::ofstream outputVBFile(OutputVBBufFilePath, std::ofstream::binary);
        outputVBFile.write(reinterpret_cast<const char*>(finalVB0Buf.data()), finalVB0Buf.size());
        outputVBFile.close();

        outputCount++;


    }


    //���Tmp.json
    extractConfig.MatchFirstIndexList = MatchFirstIndexList;
    extractConfig.PartNameList = PartNameList;
    extractConfig.TmpElementList = d3d11GameType.OrderedFullElementList;
    extractConfig.WorkGameType = MMTString_ToByteString(GameType);

    extractConfig.saveTmpConfigs(OutputDrawIBFolder);

    ExtractUtil_MoveAllTextures(G.WorkFolder, DrawIB, OutputDrawIBFolder);
}