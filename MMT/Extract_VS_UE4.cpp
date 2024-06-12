#include <set>

#include "D3d11GameType.h"
#include "IndexBufferTxtFile.h"
#include "VertexBufferBufFile.h"
#include "VertexBufferTxtFile.h"
#include "GlobalConfigs.h"
#include "GlobalFunctions.h"
#include "FrameAnalysisData.h"


void Extract_VS_UE4() {
    for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        ExtractConfig extractConfig = pair.second;
        LOG.NewLine();
        LOG.Info("Extract Buffer from SnowBreak:");
        LOG.NewLine();

        std::wstring MatchedGameType = extractConfig.GameType;
        if (extractConfig.GameType == L"Auto") {
            //�����Զ�����ʶ�����¸�ֵGameType,�������ų���ͨ���Ա�ʵ��stride��Ԥ��stride�Ƿ���ͬ�����ų�ÿ�����͡�
            //���յõ����п��ܵ������б���������б��������ֹһ�����򱨴���ʾ���п��ܵ�����
            //���ֻ��һ�����ͣ���ʹ���Ǹ�������������ȡ

            std::vector<std::string> MatchedGameTypeList;
            LOG.Info("Start to auto detect game type:");
            for (const auto& pair: G.GameTypeName_D3d11GameType_Map) {
                std::string GameType = pair.first;
                D3D11GameType d3d11GameType = pair.second;
                LOG.Info("Try Match GameType: " + d3d11GameType.GameType);

                FrameAnalysisData FAData(G.WorkFolder);
                std::vector<std::wstring> FrameAnalyseFileNameList = FAData.FrameAnalysisFileNameList;
                LOG.Info("Read FrameAnalysis file name list success.");

                // ��vb0�ж�ȡ����������Ȼ��������Ĺ����У�UE4��VB0һ��̶�ΪPOSITION���ȹ̶�Ϊ2
                // ���ƶ�������ƥ�䵽���ǵ�DrawNumber�����ö�Ӧ������
                std::map<int, std::wstring> matchFirstIndexIBFileNameMap;
                std::wstring VSExtractIndex;
                std::wstring PositionExtractFileName = L"";
                int MatchNumber = 0;
                int POSITION_ByteWidth = d3d11GameType.ElementNameD3D11ElementMap["POSITION"].ByteWidth;
                for (std::wstring filename : FrameAnalyseFileNameList) {
                    if (!filename.ends_with(L".txt")) {
                        continue;
                    }
                    if (filename.find(L"-ib=" + DrawIB) == std::wstring::npos) {
                        continue;
                    }
                    IndexBufferTxtFile ibFileData(G.WorkFolder + filename, false);
                    VSExtractIndex = ibFileData.Index;
                    PositionExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + L"-vb0=", L".buf")[0];
                    int POSITION_FileSize = MMTFile_GetFileSize(G.WorkFolder + PositionExtractFileName);
                    MatchNumber = POSITION_FileSize / POSITION_ByteWidth;
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


                std::wstring NormalExtractSlot = L"-" + MMTString_ToWideString(d3d11GameType.CategorySlotMap["Normal"]) + L"=";
                std::wstring TexcoordExtractSlot = L"-" + MMTString_ToWideString(d3d11GameType.CategorySlotMap["Texcoord"]) + L"=";
                std::wstring ColorExtractSlot = L"-" + MMTString_ToWideString(d3d11GameType.CategorySlotMap["Color"]) + L"=";
                std::wstring BlendExtractSlot = L"-" + MMTString_ToWideString(d3d11GameType.CategorySlotMap["Blend"]) + L"=";
                LOG.Info(L"Normal Extract Slot: " + NormalExtractSlot);
                LOG.Info(L"Texcoord Extract Slot: " + TexcoordExtractSlot);
                LOG.Info(L"Color Extract Slot: " + ColorExtractSlot);
                LOG.Info(L"Blend Extract Slot: " + BlendExtractSlot);
                LOG.NewLine();
                //�ռ�������λ�����ݣ�����ϳ�VB0������
                std::wstring NormalExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + NormalExtractSlot, L".buf")[0];
                std::wstring TexcoordExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + TexcoordExtractSlot, L".buf")[0];
                std::wstring ColorExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + ColorExtractSlot, L".buf")[0];
                std::wstring BlendExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + BlendExtractSlot, L".buf")[0];

                //����ı������ŵ������ʼ����
                LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
                LOG.Info(L"NormalExtractFileName: " + NormalExtractFileName);
                LOG.Info(L"TexcoordExtractFileName: " + TexcoordExtractFileName);
                LOG.Info(L"ColorExtractFileName: " + ColorExtractFileName);
                LOG.Info(L"BlendExtractFileName: " + BlendExtractFileName);
                LOG.NewLine();

                int PositionRealStride = MMTFile_GetFileSize(G.WorkFolder + PositionExtractFileName) / MatchNumber;
                int NormalRealStride = MMTFile_GetFileSize(G.WorkFolder + NormalExtractFileName) / MatchNumber;
                int TexcoordRealStride = MMTFile_GetFileSize(G.WorkFolder + TexcoordExtractFileName) / MatchNumber;
                int ColorRealStride = MMTFile_GetFileSize(G.WorkFolder + ColorExtractFileName) / MatchNumber;
                int BlendRealStride = MMTFile_GetFileSize(G.WorkFolder + BlendExtractFileName) / MatchNumber;

                std::unordered_map<std::string, int> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(d3d11GameType.OrderedFullElementList);
                if (PositionRealStride != CategoryStrideMap["Position"]) {
                    continue;
                }
                else if (NormalRealStride != CategoryStrideMap["Normal"]) {
                    continue;
                }
                else if (TexcoordRealStride != CategoryStrideMap["Texcoord"]) {
                    continue;
                }
                else if (ColorRealStride != CategoryStrideMap["Color"]) {
                    continue;
                }
                else if (BlendRealStride != CategoryStrideMap["Blend"]) {
                    continue;
                }
                //�����ִ�е�����˵��������ӵ�ƥ�䵽�������б���
                MatchedGameTypeList.push_back(GameType);
            }

            if (MatchedGameTypeList.size() == 0) {
                LOG.Error("Can't find any GameType using auto game type detect, try to manually set GameType if you think this exists in our GameType,or contact NicoMico to add this new GameType.");
            }
            else if (MatchedGameTypeList.size() == 1) {
                MatchedGameType = MMTString_ToWideString(MatchedGameTypeList[0]);
                LOG.Info(L"Auto GameType detect success! Current GameType: " + MatchedGameType);
                LOG.NewLine();
            }
            else {
                std::string GameTypeListString = "";
                for (std::string matchedGameType : MatchedGameTypeList) {
                    GameTypeListString = GameTypeListString + matchedGameType + " ";
                }
                LOG.Error("More than one GameType detected: " + GameTypeListString + " Please manually set GameType!");
            }
        
        }

        LOG.Info(L"Start to extract using game type: " + MatchedGameType);
        ExtractFromBuffer_VS_UE4(DrawIB, MatchedGameType);

    }

};