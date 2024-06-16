#include "GlobalConfigs.h"
#include "VertexBufferTxtFile.h"
#include "IndexBufferTxtFile.h"
#include "D3d11GameType.h"
#include <set>
#include "VertexBufferTxtFileDetect.h"
#include "GlobalFunctions.h"
#include "FrameAnalysisData.h"


std::wstring UnityAutoDetectGameType(std::wstring DrawIB) {
	FrameAnalysisData FAData(G.WorkFolder);
	std::vector<std::wstring> trianglelistIndexList = FAData.ReadTrianglelistIndexList(DrawIB);
	std::vector<std::wstring> pointlistIndexList = FAData.ReadPointlistIndexList();
	if (pointlistIndexList.size() < 1) {
		LOG.Warning(L"Can't find any pointlist file!");
	}
	LOG.NewLine();

    //�������е�trianglelist��vb�ļ���ʶ��ÿ���ļ����ܵ�d3d11�����б��ŵ�set��ȥ��
	std::set<std::wstring> trianglelistElementSet;
	std::unordered_map<std::wstring, int > trianglelistIndexVertexNumberMap;
	std::unordered_map<std::wstring, VertexBufferDetect> trianglelistIndexAndSlotVertexBufferMap;

	//����VertexCountNumberֻ����Ѱ�Ҷ�Ӧpointlist
	int maxTrianglelistVertexCountNumber = 0;
	for (std::wstring trianglelistIndex: trianglelistIndexList) {
		//����Index��ȡ���е�VB�ļ����б�
		std::vector<std::wstring> trianglelistVBFileNameList = FAData.FindFrameAnalysisFileNameListWithCondition(trianglelistIndex + L"-vb", L".txt");

		bool allmatch = true;
		LOG.Info(L"Try to find max real stride and vertex count trianglelist index: ");

		//TODO ����Ҫȷ�����������ԣ�ʶ��ʱ����Ҫʹ��ʲô��ΪVertexCount?�Ǵ�Buffer���ȡ���Ǵ�Txt���ȡ��
		for (std::wstring trianglelistVBFileName: trianglelistVBFileNameList) {
			LOG.NewLine();
			LOG.Info(trianglelistVBFileName);
			std::wstring trignalelistVBFilePath = G.WorkFolder + trianglelistVBFileName;
			LOG.Info(trignalelistVBFilePath);
			VertexBufferDetect vertexBufferDetect(trignalelistVBFilePath);
			trianglelistIndexAndSlotVertexBufferMap[trianglelistIndex + L"-vb" + std::to_wstring(vertexBufferDetect.vbSlotNumber)] = vertexBufferDetect;

			int TrianglelistFileVertexCount = vertexBufferDetect.fileShowVertexCount;

			trianglelistIndexVertexNumberMap[trianglelistIndex] = TrianglelistFileVertexCount;
			LOG.Info(L"Try match: RealStride: " + std::to_wstring(vertexBufferDetect.fileRealStride) + L"  ShowStride:" + std::to_wstring(vertexBufferDetect.fileShowStride));
			LOG.Info(L"Try match: File Buffer VertexCount: " + std::to_wstring(TrianglelistFileVertexCount) + L"  CurrentMaxVertexCount:" + std::to_wstring(maxTrianglelistVertexCountNumber));

			if (TrianglelistFileVertexCount > maxTrianglelistVertexCountNumber) {
				maxTrianglelistVertexCountNumber = TrianglelistFileVertexCount;
			}

			for (std::string elementName: vertexBufferDetect.realElementNameList) {
				LOG.Info("Add into trianglelistElementSet: " + elementName);
				trianglelistElementSet.insert(MMTString_ToWideString(elementName));
			}
		}
		LOG.NewLine();
	}

	LOG.Info(L"Detected trianglelist possible element list: ");
	for (std::wstring elementName : trianglelistElementSet) {
		LOG.Info(elementName);
	}
	LOG.NewLine();
	
	std::vector <std::wstring> maxVertexCountTrianglelistIndexList;
	for (const auto& pair:trianglelistIndexVertexNumberMap) {
		std::wstring index = pair.first;
		int vertexCount = pair.second;
		if (vertexCount == maxTrianglelistVertexCountNumber) {
			maxVertexCountTrianglelistIndexList.push_back(index);
		}

		LOG.Info(L"Trianglelist Index: " +pair.first + L" VertexCount: " + std::to_wstring(pair.second));
	}
	LOG.NewLine();
	LOG.Info(L"maxTrianglelistVertexCountNumber: " + std::to_wstring(maxTrianglelistVertexCountNumber));
	LOG.NewLine();

	//�����MaxVertexCount��index�����п��ܵ�d3d11Element�Լ����ǵĺͲ�λ�����������������͵�ƥ��
	std::vector<D3D11Element> possibleTrianglelistD3D11ElementTypeList;
	for (const auto& pair : trianglelistIndexAndSlotVertexBufferMap) {
		std::wstring indexAndSlot = pair.first;
		VertexBufferDetect vertexBufferDetect = pair.second;
		std::wstring index = indexAndSlot.substr(0, 6);
		std::wstring slot = indexAndSlot.substr(7, 3);

		if (vertexBufferDetect.realElementList.size() >= 1) {
			//������ǰ��realElementList;
			for (D3D11Element realD3D11Element : vertexBufferDetect.realElementList) {

				//�жϵ�ǰ���realD3D11Element�Ƿ���possible�����
				bool findSameD3D11Element = false;
				for (D3D11Element possibleD3D11Element : possibleTrianglelistD3D11ElementTypeList) {
					if (realD3D11Element.SemanticName == possibleD3D11Element.SemanticName &&
						realD3D11Element.SemanticIndex == possibleD3D11Element.SemanticIndex &&
						realD3D11Element.ExtractSlot == possibleD3D11Element.ExtractSlot
						) {
						findSameD3D11Element = true;
						break;
					}
				}

				//��������ڣ�����ӽ�ȥ
				if (!findSameD3D11Element) {
					realD3D11Element.ExtractSlot = MMTString_ToByteString(slot);
					possibleTrianglelistD3D11ElementTypeList.push_back(realD3D11Element);
				}

			}
		}

	}
	LOG.Info("possibleTrianglelistD3D11ElementTypeList;");
	for (D3D11Element d3d11Element: possibleTrianglelistD3D11ElementTypeList) {
		LOG.Info(d3d11Element.SemanticName + d3d11Element.SemanticIndex);
	}
	LOG.NewLine();


	LOG.Info(L"Start to parse all pointlist vb file to detect possible d3d11Element type.");
	//�������е�pointlist��vb�ļ���ʶ��ÿ��Ԫ�ؿ��ܵ�d3d11�����б�
	std::unordered_map<std::wstring, VertexBufferDetect> pointlistIndexAndSlotVertexBufferMap;
	std::unordered_map<std::wstring, int > pointlistIndexVertexNumberMap;
	std::wstring matchPointlistIndex = L"";
	std::set<std::wstring> pointlistElementSet;

	for (std::wstring pointlistIndex : pointlistIndexList) {
		//����Index��ȡ���е�VB�ļ����б�
		LOG.NewLine();
		LOG.Info(L"Parsing pointlist index: " + pointlistIndex);
		std::vector<std::wstring> pointlistVBFileNameList = FAData.FindFrameAnalysisFileNameListWithCondition(pointlistIndex + L"-vb", L".txt");
	
		LOG.Info(L"Start to parse pointlist FileNameList. ");
		for (std::wstring pointlistVBFileName : pointlistVBFileNameList) {
			std::wstring pointlistVBFilePath = G.WorkFolder + pointlistVBFileName;
			VertexBufferDetect vertexBufferDetect(pointlistVBFilePath);
			LOG.Info(L"Parse pointlist elementlist over.");
			pointlistIndexAndSlotVertexBufferMap[pointlistIndex + L"-vb" + std::to_wstring(vertexBufferDetect.vbSlotNumber)] = vertexBufferDetect;
			if (vertexBufferDetect.fileBufferVertexCount == maxTrianglelistVertexCountNumber) {
				if (vertexBufferDetect.vbSlotNumber == 0) {
					pointlistIndexVertexNumberMap[pointlistIndex] = vertexBufferDetect.fileBufferVertexCount;
					matchPointlistIndex = pointlistIndex;
					LOG.Info(L"Find match pointlistIndex: " + pointlistIndex);
				}

				for (std::string elementName : vertexBufferDetect.realElementNameList) {
					LOG.Info("pointlistElementSet.insert(elementName): " + elementName);
					pointlistElementSet.insert(MMTString_ToWideString(elementName));
				}
			}
			
		}
	}

	if (matchPointlistIndex == L"") {
		LOG.Warning(L"Can't find matchPointlistIndex this may be a Object type.");
	}

	LOG.Info(L"matchPointlistIndex: " + matchPointlistIndex + L" VertexCount:" + std::to_wstring(pointlistIndexVertexNumberMap[matchPointlistIndex]));
	// ������Ҫ����ʶ�𵽵���Щpointlist��trianglelist��VertexBufferDetect���d3d11ElementMap��������GameType��ƥ��
	// һ��һ������ֱ���������һ��GameType,Ȼ������Ĳ���ֱ�ӵ���֮ǰ�ķ��������ˡ�
	LOG.NewLine();


	LOG.Info(L"Detected pointlist element list: ");
	for (std::wstring elementName : pointlistElementSet) {
		LOG.Info(elementName);
	}
	LOG.NewLine();

	LOG.Info(L"Try to match GameType:");
	GlobalConfigs wheelConfig;
	wheelConfig.ApplicationRunningLocation = G.ApplicationRunningLocation;
	std::vector<D3D11GameType> gameD3D11ElementAttributeList = G.CurrentD3d11GameTypeList;
	LOG.Info( G.GameName + L" Total GameType list size: " + std::to_wstring(gameD3D11ElementAttributeList.size()));

	std::vector<std::wstring> matchedGameTypeList;
	for (D3D11GameType d3d11GameType: gameD3D11ElementAttributeList) {
		LOG.NewLine();
		LOG.Info("Current GameType: " + d3d11GameType.GameType);

		int pointlistDifferentCount = 0;
		std::set<std::wstring> pointlistMatchedElementNameSet;
		for (std::string d3d11ElementName:d3d11GameType.OrderedFullElementList) {
			std::string lowerCased3d11ElementName = boost::algorithm::to_lower_copy(d3d11ElementName);

			LOG.Info("Current Matching elementName: " + d3d11ElementName);

			//�������Ҫ����BLENDWEIGHTS��������������Աȣ����羵����������
			if (d3d11ElementName == "BLENDWEIGHTS" && d3d11GameType.PatchBLENDWEIGHTS) {
				continue;
			}
			//��ȡ��Ӧd3d11Element�����жԱ�byteWidth��to_lower֮���format
			D3D11Element gameTypeD3D11Element = d3d11GameType.ElementNameD3D11ElementMap[d3d11ElementName];
			//�������pointlistIndex���ȶԱ�pointlist��
			if (matchPointlistIndex != L"") {
				for (const auto& pair: pointlistIndexAndSlotVertexBufferMap) {
					std::wstring indexAndSlot = pair.first;
					VertexBufferDetect vertexBufferDetect = pair.second;

					std::wstring index = indexAndSlot.substr(0, 6);
					std::wstring slot = indexAndSlot.substr(7, 3);

					//LOG.LogOutput(L"Index:" + index);
					//LOG.LogOutput(L"Slot:" + slot);

					//Indexƥ�䣬���ҵ���Ӧ��pointlist�ļ���vb�ļ�����ƥ��
					if (index == matchPointlistIndex ) {
						//LOG.LogOutput(L"Index is match: " + index);

						//ֻ�а�������ȥ�ж�
						if (vertexBufferDetect.elementNameD3D11ElementMap.contains(lowerCased3d11ElementName)) {
							D3D11Element matchD3D11Element = vertexBufferDetect.elementNameD3D11ElementMap[lowerCased3d11ElementName];

							//ֻ��Slotƥ���ϲ��У�ƥ�䲻�ϲ���
							if (slot == MMTString_ToWideString(gameTypeD3D11Element.ExtractSlot)) {
								LOG.Info(L"ExtractVBFile is match: " + slot);

								if (gameTypeD3D11Element.ExtractTechnique == matchD3D11Element.ExtractTechnique) {
									if (gameTypeD3D11Element.ByteWidth == matchD3D11Element.ByteWidth) {
										LOG.Info("Current Matching elementName: " + d3d11ElementName + " matched!");
										pointlistMatchedElementNameSet.insert(MMTString_ToWideString(lowerCased3d11ElementName));
									}
									else {
										LOG.Info(lowerCased3d11ElementName + " can't match: ByteWidth not equal");
										pointlistDifferentCount++;
										break;
									}
								}
								else {
									//��ȡ�����Ͷ��Բ��ϣ��϶�����ѽ�����Ǳ��������ﲻһ����һ����pointlist��
									if (gameTypeD3D11Element.ExtractTechnique == "pointlist") {
										LOG.Info(lowerCased3d11ElementName + " can't match: ExtractTechnique not equal" + gameTypeD3D11Element.ExtractTechnique +"  " + matchD3D11Element.ExtractTechnique);

										pointlistDifferentCount++;
										break;
									}
								}
							}
							else {
								LOG.Info(L"ExtractVBFile not match: " + slot);
								LOG.Info(d3d11ElementName + " can't match: ExtractVBFile not match!");
								pointlistDifferentCount++;
								break;
							}

						}
						else {
							// �������������ģ�����Ԥ����Texcooord1������ʵ����û��
							// ����������VB0��COLOR����������û���ˣ��Ǿ��ǲ�������
							// ��������ֻ��Texcoord1��ʼ�Ŀ���û�У������Ķ�������

							//  ����texcoordʵ���ϵ�extractTech������pointlist����ô��ֱ�Ӿ�˵û����
							//  ������������жϣ�����pointlist�Ĳ��ֲ���˵û��
							if (gameTypeD3D11Element.ExtractTechnique == "pointlist" && slot == MMTString_ToWideString(gameTypeD3D11Element.ExtractSlot)) {

								if (lowerCased3d11ElementName.starts_with("texcoord") && !lowerCased3d11ElementName.ends_with("texcoord")) {
									//����TEXCOORD1��ʼ������TEXCOORD��������
									LOG.Info(lowerCased3d11ElementName + " can't match: technique is pointlist but not contains in : " + MMTString_ToByteString(indexAndSlot));
									LOG.Info(L"Because it's not TEXCOORD so it allowed to be matched.");

								}
								
								else {
									pointlistDifferentCount++;
									break;
									//����������Ԫ�أ�����COLOR���Ǿ͵���
									LOG.Info(lowerCased3d11ElementName + " can't match: technique is pointlist but not contains in : " + MMTString_ToByteString(indexAndSlot));
								}
								
							}

						}
						
					}
					
				}
			}

			
			
		}

		if (pointlistDifferentCount == 0) {
			LOG.Info("GameType " + d3d11GameType.GameType + " Pointlist  Matched!");
			LOG.NewLine();
		}



		LOG.Info(L"Start to match trianglelist d3d11 element:");
		std::set<std::wstring> trianglelistPossibleElementName;
		for (std::string d3d11ElementName : d3d11GameType.OrderedFullElementList) {
			D3D11Element d3d11Eleemnt = d3d11GameType.ElementNameD3D11ElementMap[d3d11ElementName];

			//��Ϊ�����Ǹ�trianglelist��ӣ���������ֻ��trianglelist��Ҫ�ɡ�
			if (d3d11Eleemnt.ExtractTechnique == "trianglelist") {
				std::string lowerCased3d11ElementName = boost::algorithm::to_lower_copy(d3d11ElementName);

				if (!pointlistMatchedElementNameSet.contains(MMTString_ToWideString(lowerCased3d11ElementName))) {
					trianglelistPossibleElementName.insert(MMTString_ToWideString(d3d11ElementName));
					LOG.Info("trianglelist element need to match: " + lowerCased3d11ElementName);
				}
			}

		}
		LOG.NewLine();

		int trianglelistDifferentCount = 0;
		for (std::wstring elementName: trianglelistPossibleElementName) {
			//��ȡ��Ӧd3d11Element�����жԱ�byteWidth��to_lower֮���format
			D3D11Element gameTypeD3D11Element = d3d11GameType.ElementNameD3D11ElementMap[MMTString_ToByteString(elementName)];

			//����ı����ǽ��жԱ�D3D11Element�Ƿ��ڶ�Ӧ��Index����ڣ���������Ҫ����ȫ�µĶԱȷ�����possibleTrianglelistD3D11ElementTypeList
			bool findExistsTrianglelsit = false;
			for (D3D11Element d3d11Element: possibleTrianglelistD3D11ElementTypeList) {

				std::string lowerSemanticName = boost::algorithm::to_lower_copy(gameTypeD3D11Element.SemanticName);

				if (
					lowerSemanticName == d3d11Element.SemanticName &&
					gameTypeD3D11Element.SemanticIndex == d3d11Element.SemanticIndex &&
					gameTypeD3D11Element.ExtractSlot == d3d11Element.ExtractSlot &&
					gameTypeD3D11Element.ByteWidth == d3d11Element.ByteWidth
					) {
					LOG.Info("Trianglelist Element Matched: " + lowerSemanticName + gameTypeD3D11Element.SemanticIndex);
					findExistsTrianglelsit = true;
					break;
				}
				else {
					if (lowerSemanticName == d3d11Element.SemanticName) {
						LOG.Info(L"Can't match reason:");
						LOG.Info("GameType SemanticName:" + lowerSemanticName + "  current SemanticName: " + d3d11Element.SemanticName);
						LOG.Info("GameType SemanticIndex:" + gameTypeD3D11Element.SemanticIndex + "  current SemanticIndex: " + d3d11Element.SemanticIndex);
						LOG.Info("GameType ExtractVBFile:" + gameTypeD3D11Element.ExtractSlot + "  current ExtractVBFile: " + d3d11Element.ExtractSlot);
						LOG.Info("GameType ByteWidth:" + std::to_string(gameTypeD3D11Element.ByteWidth) + "  current SemanticName: " + std::to_string(d3d11Element.ByteWidth));
					}
				}
			}

			if (!findExistsTrianglelsit) {

				if (gameTypeD3D11Element.SemanticName == "TEXCOORD" && gameTypeD3D11Element.SemanticIndex != "0") {
					//ֻ��ΪTEXCOORD��SemanticIndex��Ϊ0��ʱ�򣬼�TEXCOORD1���֣����Բ�ƥ�䡣
				}
				else {
					trianglelistDifferentCount++;
				}

			}

		}
		
		if (trianglelistDifferentCount == 0) {
			LOG.Info("GameType " + d3d11GameType.GameType + " Trianglelist  Matched!");
			LOG.NewLine();
		}




		if (pointlistDifferentCount == 0 && trianglelistDifferentCount == 0) {
			LOG.Info("GameType " + d3d11GameType.GameType + "  Matched!");
			//matchedGameTypeList.push_back(MMTString_ToWideString(d3d11GameType.GameType));
			if (matchPointlistIndex != L"") {
				//��pointlist�ж���������£�����ѡ��pointlist����
				//��ô�ж��Ƿ�Ϊpointlist��ֱ�ӻ�ȡPOSITION��extractTechnique��
				if (d3d11GameType.ElementNameD3D11ElementMap["POSITION"].ExtractTechnique == "pointlist") {
					matchedGameTypeList.push_back(MMTString_ToWideString(d3d11GameType.GameType));
				}
			}
			else {
				//�����ڲ�Ϊpointlist������£��ǿ���ֱ�Ӽ���
				matchedGameTypeList.push_back(MMTString_ToWideString(d3d11GameType.GameType));
			}
		}
	}

	LOG.NewLine();
	
	if (matchedGameTypeList.size() > 1) {
		for (std::wstring gameType: matchedGameTypeList) {
			LOG.Info(L"Matched gameType: " + gameType);
		}
		LOG.Info(L"More than one game type matched! Only the first matched type work!");
	}
	else if (matchedGameTypeList.size() == 1) {
		LOG.Info(L"Auto matchend game type: " + matchedGameTypeList[0]);
	}
	else {
		LOG.Error(L"No any game type matched!Please contact NicoMico to fix this.");
	}
	

    //�������
    LOG.Info(L"GameType Detect complete!");
	return matchedGameTypeList[0];
}




void ExtractUtil_MoveAllTextures(std::wstring FrameAnalysisFolderPath, std::wstring DrawIB, std::wstring OutputFolder) {
	
	std::wstring PsTexturesFolder = OutputFolder + L"PsTextures\\";
	std::wstring PsTexturesUniqueFolder = OutputFolder + L"PsTexturesUnique\\";
	std::wstring PsHashTexturesFolder = OutputFolder + L"PsHashTextures\\";

	//Create PsTextureFolder and PsTexturesUniqueFolder
	std::filesystem::create_directories(PsTexturesFolder);
	std::filesystem::create_directories(PsTexturesUniqueFolder);
	std::filesystem::create_directories(PsHashTexturesFolder);

	LOG.Info(L"Start to move all Texture files: ");
	std::unordered_map<std::wstring, std::wstring> HashSlotTextureFileNameMap;
	std::unordered_map<std::wstring, std::wstring> HashTextureFileNameMap;
	FrameAnalysisData FAData(FrameAnalysisFolderPath);
	std::vector<std::wstring> trianglelistIndexList = FAData.ReadTrianglelistIndexList(DrawIB);
	for (const auto& index : trianglelistIndexList) {

		std::vector<std::wstring> pixelShaderTextureFileNameList = FAData.FindFrameAnalysisFileNameListWithCondition(index + L"-ps-t", L".dds");
		//std::vector<std::wstring> pixelShaderTextureFileNameList = wheelConfigs.findFileNameListWithCondition(index + L"-ps-t", L".dds");
		for (const auto& psTextureFileName : pixelShaderTextureFileNameList) {
			//���г��ļ����е�hashֵ
			// ��ȡ��һ���Ⱥŵ�����
			size_t equalSignIndex = psTextureFileName.find('=');
			size_t firstDashIndex = psTextureFileName.find('-');
			// ��ȡ�Ⱥź����һ�����ŵ�����
			size_t SecondDashIndex = psTextureFileName.find('-', equalSignIndex);


			// ���г� hash ֵ
			std::wstring slot = psTextureFileName.substr(firstDashIndex + 1, equalSignIndex - firstDashIndex - 1);
			std::wstring hashValue = psTextureFileName.substr(equalSignIndex + 1, SecondDashIndex - equalSignIndex - 1);
			std::wstring combineHash = hashValue + L"_" + slot;
			HashSlotTextureFileNameMap[combineHash] = psTextureFileName;
			HashTextureFileNameMap[hashValue] = psTextureFileName;

			//std::cout << "psTextureFileName:" << psTextureFileName << std::endl;
			std::wstring originalFileName = FrameAnalysisFolderPath + psTextureFileName;
			std::wstring destFileName = PsTexturesFolder + psTextureFileName;
			LOG.Info(originalFileName);
			if (!std::filesystem::exists(destFileName)) {
				std::filesystem::copy_file(originalFileName, destFileName, std::filesystem::copy_options::overwrite_existing);
			}
		}
	}
	LOG.Info(L"Start to move unique Texture files: ");
	for (const auto& pair : HashSlotTextureFileNameMap) {
		std::wstring filename = pair.second;
		std::wstring originalFileName = FrameAnalysisFolderPath + filename;
		LOG.Info(originalFileName);
		std::wstring destFileName = PsTexturesUniqueFolder + filename;

		if (!std::filesystem::exists(destFileName)) {
			std::filesystem::copy_file(originalFileName, destFileName, std::filesystem::copy_options::overwrite_existing);
		}
	}
	LOG.NewLine();
	LOG.Info(L"Start to move hash distinct Texture files: ");
	for (const auto& pair : HashTextureFileNameMap) {
		std::wstring filename = pair.second;
		std::wstring originalFileName = FrameAnalysisFolderPath + filename;
		LOG.Info(originalFileName);
		std::wstring destFileName = PsHashTexturesFolder + pair.first + L".dds";

		if (!std::filesystem::exists(destFileName)) {
			std::filesystem::copy_file(originalFileName, destFileName, std::filesystem::copy_options::overwrite_existing);
		}
	}
	LOG.NewLine();

}
