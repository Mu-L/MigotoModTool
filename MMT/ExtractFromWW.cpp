#include "GlobalConfigs.h"
#include "GlobalFunctions.h"
#include "IndexBufferTxtFile.h"
#include "ConstantBufferBufFile.h"
#include "FrameAnalysisData.h"

void ExtractFromWW() {
    for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        ExtractConfig extractConfig = pair.second;
        LOG.NewLine();
        LOG.Info("I suggest this test Setting in your d3dx.ini for dump WW:\nanalyse_options = dump_rt dump_tex dump_cb dump_vb dump_ib buf txt");
        LOG.NewLine();
        LOG.Info("Extract Buffer from Weathering Wave:");

        std::wstring MatchedGameType = extractConfig.GameType;
        if (extractConfig.GameType == L"Auto") {
            LOG.Info(L"Try auto detect game type:" );
            std::vector<std::wstring> MatchGameTypeList;
            for (const auto& pair: G.GameTypeName_D3d11GameType_Map) {
                D3D11GameType d3d11GameType = pair.second;
                FrameAnalysisData FAData(G.WorkFolder);
                std::vector<std::wstring> FrameAnalyseFileNameList = FAData.FrameAnalysisFileNameList;
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

                }
                
                LOG.NewLine();


                LOG.NewLine();
                int MatchedDrawNumber = 0;
                std::wstring MatchedDrawNumberCSIndex = L"";
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
                    
                        if (MatchNumber == drawNumber) {
                            //��Ⲣ����DrawComputeShader�������жϾ���Ҫ���ĸ���λ��ȡ
                            if (filename.find(L"1ff924db9d4048d1") != std::wstring::npos) {
                                DrawComputeShader = "1ff924db9d4048d1";

                            }
                            else if (filename.find(L"4d0760c2c7406824") != std::wstring::npos) {
                                DrawComputeShader = "4d0760c2c7406824";
                            }
                            LOG.Info("Find Match Number File! Set draw compute shader to : " + DrawComputeShader);
                            MatchedDrawNumberCSIndex = filename.substr(0, 6);
                        }

                }
                LOG.Info(L"MatchedDrawNumber: " + std::to_wstring(MatchedDrawNumber));
                LOG.Info(L"MatchedDrawNumberCSIndex: " + MatchedDrawNumberCSIndex);

                //������ҵ�MaxDrawNumberIndex��˵��ʹ��CS���м��㣬˵����ǰ���Ϳ϶���WW_Body
                if (MatchedDrawNumberCSIndex != L"") {
                    MatchedGameType = MMTString_ToWideString("WW_Body");
                    MatchGameTypeList.push_back(MMTString_ToWideString("WW_Body"));
                    break;
                }

                //����Ҳ�����Ӧ����ֵ��˵����ʹ��CS���м��㣬˵��ΪObject���ͣ�����������object����ƥ��
                //��ÿ����λ�Ŀ�Ƚ��м�飬��ͨ����continue��ȫ��ͨ��������б�
                
                LOG.Info("Start to match object types:");
                // ����object����Ӧ�ô�vb0�ж�ȡ��������
                // ���ƶ�������ƥ�䵽���ǵ�DrawNumber�����ö�Ӧ������
                std::wstring PositionExtractFileName = L"";
                MatchNumber = 0;
                std::unordered_map<std::string, int> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(d3d11GameType.OrderedFullElementList);
                int PositionStride = CategoryStrideMap["Position"];
                for (std::wstring filename : FrameAnalyseFileNameList) {
                    if (!filename.ends_with(L".txt")) {
                        continue;
                    }
                    if (filename.find(L"-ib=" + DrawIB) == std::wstring::npos) {
                        continue;
                    }

                    IndexBufferTxtFile ibFileData(G.WorkFolder + filename, false);
                    //ע��:�����г����˲����������Ͷ��Drawʱ����ЩDraw��ʹ����ͼ��λ�Ҷ�������Ҳ�޷����ϣ�HashֵҲ��ͬ�����
                    //����������ȡ����Mod����ʱ������Ҫȷ��ps-t0��λ����ͼȷʵ����
                    //�����������˵������������Ⱦ��ͼ���Ǹ���λ��
                    std::vector<std::wstring> Pst0_TextureDDSFileList = FAData.FindFrameAnalysisFileNameListWithCondition(ibFileData.Index + L"-ps-t0=", L".dds");
                    std::vector<std::wstring> Pst0_TextureJPGFileList = FAData.FindFrameAnalysisFileNameListWithCondition(ibFileData.Index + L"-ps-t0=", L".jpg");
                    if (Pst0_TextureDDSFileList.size() == 0 && Pst0_TextureJPGFileList.size() == 0) {
                        LOG.Info(L"Can't find ps-t0 or jpg texture for index:" + ibFileData.Index + L" it will not be a valid object type, so skip this.");
                        continue;
                    }

                    VSExtractIndex = ibFileData.Index;
                    PositionExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + L"-vb0=", L".buf")[0];
                    LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
                    int PositionFileSize = MMTFile_GetFileSize(G.WorkFolder + PositionExtractFileName);
                    MatchNumber = PositionFileSize / PositionStride;
                    LOG.Info("Match DrawNumber: " + std::to_string(MatchNumber));

                    LOG.Info(filename);
                    if (ibFileData.FirstIndex == L"") {
                        LOG.Info("Can't find FirstIndex attribute in this file, so skip this.");
                        continue;
                    }
                }
               
                LOG.NewLine();



                std::wstring TexcoordExtractSlot = L"-vs-t0=";
                std::wstring NormalExtractSlot = L"-vs-t1=";
                LOG.Info(L"Texcoord Extract Slot: " + TexcoordExtractSlot);
                LOG.Info(L"Normal Extract Slot: " + NormalExtractSlot);
                LOG.NewLine();
                //�ռ�������λ�����ݣ�����ϳ�VB0������
                TexcoordExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + TexcoordExtractSlot, L".buf")[0];
                std::wstring NormalExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + NormalExtractSlot, L".buf")[0];

                //����ı������ŵ������ʼ����
                LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
                LOG.Info(L"TexcoordExtractFileName: " + TexcoordExtractFileName);
                LOG.Info(L"NormalExtractFileName: " + NormalExtractFileName);

                int NormalStride = MMTFile_GetFileSize(G.WorkFolder + NormalExtractFileName) / MatchNumber;
                int TexcoordStride = MMTFile_GetFileSize(G.WorkFolder + TexcoordExtractFileName) / MatchNumber;

                if (NormalStride != CategoryStrideMap["Normal"]) {
                    continue;
                }
                if (TexcoordStride != CategoryStrideMap["Texcoord"]) {
                    continue;
                }

                //�����������������ܶ��ϣ�������ֱ�Ӽ��뼴��
                MatchedGameType = MMTString_ToWideString(pair.first);
                MatchGameTypeList.push_back(MMTString_ToWideString(pair.first));

            }

            //Ȼ����ݴ�С������Ӧѡ��
            if (MatchGameTypeList.size() == 1) {
                LOG.NewLine();
                LOG.Info(L"GameType Matched: " + MatchedGameType);
                LOG.NewLine();

                if (MatchedGameType == L"WW_Body") {
                    ExtractFromBuffer_CS_WW_Body(DrawIB, L"WW_Body");
                }
                else {
                    ExtractFromBuffer_VS_WW_Object(DrawIB, MatchedGameType);

                }
            }
            else if (MatchGameTypeList.size() > 1) {
                std::wstring GameTypeListStr = L"";
                for (std::wstring GameType: MatchGameTypeList) {
                    GameTypeListStr = GameTypeListStr + GameType + L" ";
                }
                //�г����п��ܵ�����
                LOG.Error(L"More than one GameType Matched: " + GameTypeListStr);
            }
            else {
                LOG.Error("Unknown GameType! Please contanct NicoMico to add support for this type!");
            }

        }
        else {
            if (MatchedGameType == L"WW_Body") {
                ExtractFromBuffer_CS_WW_Body(DrawIB, L"WW_Body");
            }
            else {
                ExtractFromBuffer_VS_WW_Object(DrawIB, MatchedGameType);

            }
        }



    }
}