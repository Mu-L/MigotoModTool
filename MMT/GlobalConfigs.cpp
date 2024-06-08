#include "GlobalConfigs.h"


//���ṩ�������е�·��
GlobalConfigs::GlobalConfigs(std::wstring ApplicationLocation) {
    //��ʼ��GIMI��ʽ��Ҫ����Ƭ����
    this->GIMIPartNameAliasMap[L"1"] = L"Head";
    this->GIMIPartNameAliasMap[L"2"] = L"Body";
    this->GIMIPartNameAliasMap[L"3"] = L"Dress";
    this->GIMIPartNameAliasMap[L"4"] = L"Extra";
    this->GIMIPartNameAliasMap[L"5"] = L"Extra1";
    this->GIMIPartNameAliasMap[L"6"] = L"Extra2";
    this->GIMIPartNameAliasMap[L"7"] = L"Extra3";
    this->GIMIPartNameAliasMap[L"8"] = L"Extra4";
    this->GIMIPartNameAliasMap[L"9"] = L"Extra5";
    this->GIMIPartNameAliasMap[L"10"] = L"Extra6";
    this->GIMIPartNameAliasMap[L"12"] = L"Extra7";
    this->GIMIPartNameAliasMap[L"13"] = L"Extra8";
    this->GIMIPartNameAliasMap[L"14"] = L"Extra9";
    this->GIMIPartNameAliasMap[L"15"] = L"Extra10";
    this->GIMIPartNameAliasMap[L"16"] = L"Extra11";
    this->GIMIPartNameAliasMap[L"17"] = L"Extra12";
    this->GIMIPartNameAliasMap[L"18"] = L"Extra13";

    //��ʼ������·��
    this->ApplicationRunningLocation = ApplicationLocation;
    LOG.Info(L"ApplicationLocation: " + ApplicationLocation);
    LOG.Info(L"Start to initialize global config:");

    //��ʼ������·��
    this->Path_MainJson = this->ApplicationRunningLocation + L"\\Configs\\Main.json";

    //��ȡ��ǰ��������Ϸ����
    LOG.Info(L"Start to read from Main.json:");
    LOG.Info(L"Main.json Path:" + this->Path_MainJson);
    json MainJsonData = MMTJson_ReadJsonFromFile(this->Path_MainJson);
    this->GameName = MMTString_ToWideString(MainJsonData["GameName"]);
    LOG.Info(L"GameName:" + GameName);

    //������Ϸ���ƣ������GamesĿ¼�µľ�����ϷĿ¼
    this->Path_Game_Folder = this->ApplicationRunningLocation + L"\\Games\\" + GameName + L"\\";
    this->Path_Game_3DmigotoFolder = Path_Game_Folder + L"3Dmigoto\\";
    this->Path_Game_TypesFolder = Path_Game_Folder + L"Types\\";
    this->Path_Game_VSCheckJson = Path_Game_Folder + L"VSCheck.json";
    this->Path_Game_ConfigJson = Path_Game_Folder + L"Config.json";
    this->Path_Game_SettingJson = Path_Game_Folder + L"Setting.json";

    //TODO ��ȡ������Ϸ����
    std::vector<std::wstring> GameTypeFileNameList;
    for (const auto& entry : std::filesystem::directory_iterator(this->Path_Game_TypesFolder)) {
        if (!(entry.is_regular_file())) {
            continue;
        }
        std::wstring filename = entry.path().filename().wstring();
        if (!filename.ends_with(L".json")) {
            continue;
        }
        GameTypeFileNameList.push_back(filename);
    }
    LOG.Info("GameTypeFileNameList size: " + std::to_string(GameTypeFileNameList.size()));

    for (std::wstring GameTypeFileName : GameTypeFileNameList) {
        LOG.Info(GameTypeFileName);
        std::wstring GameTypeFilePath = this->Path_Game_TypesFolder + GameTypeFileName;
        LOG.Info(L"GameTypeFilePath: " + GameTypeFilePath);
        json GameTypeJson = MMTJson_ReadJsonFromFile(GameTypeFilePath);

        D3D11GameType d3d11GameType;
        d3d11GameType.GameType = MMTString_ToByteString(GameTypeFileName.substr(0, (GameTypeFileName.length() - 5)));
        if (GameTypeJson.contains("PatchBLENDWEIGHTS")) {
            d3d11GameType.PatchBLENDWEIGHTS = GameTypeJson["PatchBLENDWEIGHTS"];
        }
        d3d11GameType.CategoryDrawCategoryMap = GameTypeJson["CategoryDrawCategoryMap"];
        //����鿴����
        for (const auto& pair: d3d11GameType.CategoryDrawCategoryMap) {
            LOG.Info("Category:" + pair.first + " DrawCategory:" + pair.second );
        }
        d3d11GameType.OrderedFullElementList = GameTypeJson["OrderedFullElementList"];

        std::vector<json> d3d11JsonList = GameTypeJson["D3D11ElementList"];
        for (json d3d11Json : d3d11JsonList) {
            D3D11Element d3d11Element;
            d3d11Element.SemanticName = d3d11Json["SemanticName"];
            d3d11Element.SemanticIndex = d3d11Json["SemanticIndex"];
            d3d11Element.Format = d3d11Json["Format"];
            d3d11Element.InputSlot = d3d11Json["InputSlot"];
            d3d11Element.InputSlotClass = d3d11Json["InputSlotClass"];
            d3d11Element.InstanceDataStepRate = d3d11Json["InstanceDataStepRate"];
            std::string ByteWidthStr = d3d11Json["ByteWidth"];
            d3d11Element.ByteWidth = std::stoi(ByteWidthStr);
            d3d11Element.ExtractSlot = d3d11Json["ExtractSlot"];
            d3d11Element.ExtractTechnique = d3d11Json["ExtractTechnique"];
            d3d11Element.Category = d3d11Json["Category"];
            std::string ElementName = "";
            if (d3d11Element.SemanticIndex != "0") {
                ElementName = d3d11Element.SemanticName + d3d11Element.SemanticIndex;
            }
            else {
                ElementName = d3d11Element.SemanticName;
            }

            d3d11GameType.ElementNameD3D11ElementMap[ElementName] = d3d11Element;
            d3d11GameType.CategorySlotMap[d3d11Element.Category] = d3d11Element.ExtractSlot;
            d3d11GameType.CategoryTopologyMap[d3d11Element.Category] = d3d11Element.ExtractTechnique;
        }
        this->CurrentD3d11GameTypeList.push_back(d3d11GameType);
        this->GameTypeName_D3d11GameType_Map[d3d11GameType.GameType] = d3d11GameType;
        //LOG.Info(this->GameTypeName_D3d11GameType_Map[d3d11GameType.GameType].GameType);
    }

    //LOG.Error("Stop");


    this->OutputFolder = this->Path_Game_3DmigotoFolder + L"Mods\\output\\";
    LOG.Info(L"OutputFolder:" + OutputFolder);
    std::filesystem::create_directories(OutputFolder);

    this->LoaderFolder = this->Path_Game_3DmigotoFolder;
    LOG.Info(L"LoaderFolder:" + LoaderFolder);
    
    std::wstring latestFrameAnalyseFolder = MMTFile_FindMaxPrefixedDirectory(LoaderFolder, L"FrameAnalysis-");
    if (latestFrameAnalyseFolder == L"") {
        LOG.Warning(L"Can't find any FrameAnalysisFolder.");
    }
    this->FrameAnalyseFolder = latestFrameAnalyseFolder;
    LOG.Info(L"FrameAnalyseFolder:" + FrameAnalyseFolder);

    this->WorkFolder = LoaderFolder + FrameAnalyseFolder + L"\\";
    LOG.Info(L"WorkFolder:" + WorkFolder);
    LOG.NewLine();

    //��ȡ��ǰ�������������
    this->Path_RunInputJson = this->ApplicationRunningLocation + L"\\Configs\\RunInput.json";
    json runInputJson = MMTJson_ReadJsonFromFile(this->Path_RunInputJson);
    std::string runCommand = runInputJson["RunCommand"];
    this->RunCommand = MMTString_ToWideString(runCommand);

    //TODO ����Ķ�ȡ��Ҫ�޸ģ�ʵ�ڲ��о������ε���
    if (std::filesystem::exists(this->Path_Game_ConfigJson)) {
        //������Ϸ���ͣ���ȡconfigsĿ¼�¶�Ӧ�������ļ�
        json configJsonData = MMTJson_ReadJsonFromFile(this->Path_Game_ConfigJson);
        for (const auto& obj : configJsonData) {
            ExtractConfig config;
            //�Ȱ�DrawIB��ȡ����
            config.DrawIB = MMTString_ToWideString(obj["DrawIB"]);
            LOG.Info(L"Start to read config for DrawIB:" + config.DrawIB);


            //Merge��4��������
            //config.UseDefaultColor = obj["UseDefaultColor"];
            //LOG.LogOutput(L"UseDefaultColor:" + std::to_wstring(config.UseDefaultColor));
            
            config.GameType = MMTString_ToWideString(obj["GameType"]);
            LOG.Info(L"GameType:" + config.GameType);

            config.SwitchKey = MMTString_ToWideString(obj["SwitchKey"]);
            LOG.Info(L"SwitchKey:" + config.SwitchKey);
            config.TangentAlgorithm = MMTString_ToWideString(obj["TangentAlgorithm"]);
            LOG.Info(L"TangentAlgorithm:" + config.TangentAlgorithm);

            config.ColorAlgorithm = MMTString_ToWideString(obj["ColorAlgorithm"]);
            LOG.Info(L"ColorAlgorithm:" + config.ColorAlgorithm);

            //color map
            config.ColorMap = obj["Color"];
            LOG.Info(L"ColorMap:");
            for (const auto& pair : config.ColorMap) {
                LOG.Info(L"Color: " + MMTString_ToWideString(pair.first) + L"  value: " + MMTString_ToWideString(pair.second));
            }
            //texture map
            config.TextureMap = obj["TextureDict"];
            LOG.Info(L"TextureDict:");
            for (const auto& pair : config.TextureMap) {
                LOG.Info(L"Texture: " + MMTString_ToWideString(pair.first) + L"  hash: " + MMTString_ToWideString(pair.second));
            }


            //Reverse Normal and Tangent
            config.NormalReverseX = obj["NormalReverseX"];
            config.NormalReverseY = obj["NormalReverseY"];
            config.NormalReverseZ = obj["NormalReverseZ"];

            config.TangentReverseX = obj["TangentReverseX"];
            config.TangentReverseY = obj["TangentReverseY"];
            config.TangentReverseZ = obj["TangentReverseZ"];
            config.TangentReverseW = obj["TangentReverseW"];
            LOG.NewLine();

            //�������ж��Ƿ����tmpConfig,ֻ���ڴ��ڵ�����²Ż���ж�ȡ�������ֹ�ָ��
            std::wstring tmpConfigPath = OutputFolder + config.DrawIB + L"\\tmp.json";
            LOG.Info(tmpConfigPath);
            if (std::filesystem::exists(tmpConfigPath)) {
                json tmpJsonData = MMTJson_ReadJsonFromFile(tmpConfigPath);

                //Read the config Merge generated.

                LOG.Info(L"Start to read tmp config.");
                config.VertexLimitVB = tmpJsonData["VertexLimitVB"];
                LOG.Info(L"VertexLimitVB:" + MMTString_ToWideString(config.VertexLimitVB));

                config.WorkGameType = tmpJsonData["WorkGameType"];
                LOG.Info(L"WorkGameType:" + MMTString_ToWideString(config.WorkGameType));

                config.TmpElementList = tmpJsonData["TmpElementList"];
                LOG.Info(L"TmpElementList:");
                for (const auto& pair : config.TmpElementList) {
                    LOG.Info(MMTString_ToWideString(pair));
                }

                config.CategoryHashMap = tmpJsonData["CategoryHash"];
                LOG.Info(L"CategoryHash:");
                for (const auto& pair : config.CategoryHashMap) {
                    LOG.Info(L"Category: " + MMTString_ToWideString(pair.first) + L"  Hash: " + MMTString_ToWideString(pair.second));
                }
                config.MatchFirstIndexList = tmpJsonData["MatchFirstIndex"];
                LOG.Info(L"MatchFirstIndex:");
                for (const auto& pair : config.MatchFirstIndexList) {
                    LOG.Info(MMTString_ToWideString(pair));
                }
                config.PartNameList = tmpJsonData["PartNameList"];
                LOG.Info(L"PartNameList:");
                for (const auto& pair : config.PartNameList) {
                    LOG.Info(MMTString_ToWideString(pair));
                }

                LOG.NewLine();
            }

            DrawIB_ExtractConfig_Map[config.DrawIB] = config;
        }
        LOG.Info(L"Read config for every DrawIB success.");
        LOG.NewLine();


    }
    else {
        LOG.Info(L"Can't find config file: " + this->Path_Game_ConfigJson);
    }

}


//����outputFolder�������浽outputfolder�����DrawIBΪ���Ƶ��ļ�����
void ExtractConfig::saveTmpConfigs(std::wstring outputPath) {
    LOG.Info(L"Start to save tmp config");

    //write to json file.
    json tmpJsonData;
    tmpJsonData["VertexLimitVB"] = VertexLimitVB;
    tmpJsonData["TmpElementList"] = TmpElementList;
    tmpJsonData["CategoryHash"] = CategoryHashMap;
    tmpJsonData["MatchFirstIndex"] = MatchFirstIndexList;
    tmpJsonData["PartNameList"] = PartNameList;
    tmpJsonData["WorkGameType"] = WorkGameType;

    std::wstring configCompletePath = outputPath + L"tmp.json";
    LOG.Info(L"configCompletePath: " + configCompletePath);
    // save json config into file.
    // ��������ļ������ڣ�����Զ������µ��ļ���д�룬���Բ��õ��ġ�
    if (std::filesystem::exists(configCompletePath)) {
        std::filesystem::remove(configCompletePath);
    }
    std::ofstream file(configCompletePath);
    std::string content = tmpJsonData.dump(4); // ʹ��������ʽ���浽�ļ�������Ϊ4���ո�
    file << content << std::endl; 
    file.close();
    LOG.Info(L"Json config has saved into file.");
   
    LOG.NewLine();
}



//�����ṩ�޲ι��죬������������ȫ�ֱ������ں������г�ʼ����
GlobalConfigs::GlobalConfigs() {
    
}





