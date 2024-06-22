#include "ModFormatExtra.h"
#include <boost/algorithm/cxx11/any_of.hpp>


ModFormat_INI::ModFormat_INI(std::wstring IniFilePath) {
    //�������ǰ����еĲ�ͬ��3Dmigoto��ini���Ͷ�����Ϊ������Section�������Lv3�ļܹ�����ģ�黯�������������
    std::vector<M_SectionLine> migotoSectionLineList = Parse_Util_ParseMigotoSectionLineList(IniFilePath);
    this->MigotoSectionLineList = migotoSectionLineList;

    //�г�Ҫ����������,Ȼ����ݵõ���migotoSectionLineList�����Section���н���
    std::vector<M_Variable> global_M_VariableList;
    std::vector<M_Key> global_M_KeyList;
    std::vector<M_Resource> global_M_ResourceList;
    std::vector<M_TextureOverride> global_M_TextureOverrideList;
    for (M_SectionLine migotoSectionLine : migotoSectionLineList) {
        //Ȼ���������ִ�о���Ľ�������
        if (migotoSectionLine.SectionName == L"constants") {
            std::vector<M_Variable> m_variable_list = Parse_Basic_ConstantsSection(migotoSectionLine);
            global_M_VariableList.insert(global_M_VariableList.end(), m_variable_list.begin(), m_variable_list.end());
        }
        else if (migotoSectionLine.SectionName.starts_with(L"key")) {
            M_Key m_key = Parse_Basic_KeySection(migotoSectionLine);
            global_M_KeyList.push_back(m_key);
        }
        else if (migotoSectionLine.SectionName == L"present") {
            std::vector<M_Variable> m_variable_list = Parse_Basic_ConstantsSection(migotoSectionLine);
            global_M_VariableList.insert(global_M_VariableList.end(), m_variable_list.begin(), m_variable_list.end());
        }
        else if (migotoSectionLine.SectionName.starts_with(L"resource")) {
            M_Resource m_resource = Parse_Basic_ResourceSection(migotoSectionLine);
            global_M_ResourceList.push_back(m_resource);
        }
        else if (migotoSectionLine.SectionName.starts_with(L"textureoverride")) {
            M_TextureOverride m_textureoverride = Parse_Basic_TextureOverrideSection(migotoSectionLine);
            global_M_TextureOverrideList.push_back(m_textureoverride);
        }
        else {
            LOG.Info(L"Out Size: " + std::to_wstring(migotoSectionLine.SectionLineList.size()));
            LOG.Info(L"Unrecognized Section: " + migotoSectionLine.SectionName);
        }
        LOG.NewLine();
    }
    this->Global_M_KeyList = global_M_KeyList;
    this->Global_M_VariableList = global_M_VariableList;
    this->Global_M_ResourceList = global_M_ResourceList;
    this->Global_M_TextureOverrideList = global_M_TextureOverrideList;
    LOG.Info(L"Parse all section over.");
    LOG.NewLine();
    // ��Resource���б���ResourceName:Resource��Map��
    // �������ֱ��ͨ�����ֻ�ȡResource������ܶ࣬����������дһ������
    std::unordered_map<std::wstring, M_Resource> global_ResourceName_Resource_Map;
    for (M_Resource resource : global_M_ResourceList) {
        global_ResourceName_Resource_Map[resource.ResourceName] = resource;
    }
    this->Global_ResourceName_Resource_Map = global_ResourceName_Resource_Map;
    LOG.NewLine();
}


ValidModDetect::ValidModDetect(D3D11GameType d3d11GameTypeInput, std::vector<M_Resource> ResourceVBListInput, std::vector<M_TextureOverride> TextureOverrideIBListInput) {
    this->d3d11GameType = d3d11GameTypeInput;
    this->TextureOverrideIBList = TextureOverrideIBListInput;

    //����Ҫȷ����ǰ������ʶ�𵽵�Resource������ͬ����Ϊÿ�������һ��Resource
    if (d3d11GameType.CategorySlotMap.size() != ResourceVBListInput.size()) {
        LOG.Info("Try to parse with type:" + d3d11GameType.GameType + " but can't match category size.");
        LOG.Info("d3d11GameType.CategorySlotMap.size(): " + std::to_string(d3d11GameType.CategorySlotMap.size()));
        LOG.Info("ResourceVBList.size(): " + std::to_string(ResourceVBListInput.size()));
        this->ValidMod = false;
        return;
    }

    //���ݵ�ǰd3d11GameType��ͼ����Category��Strideƴ�ӳ�ElementList
    LOG.Info("Current Try Match With Type: " + d3d11GameType.GameType);
    if (ResourceVBListInput.size() == 3) {
        LOG.Info(L"Try Detect classic Position,Texcoord,Blend mod.");
        M_Resource ResourcePosition;
        M_Resource ResourceTexcoord;
        M_Resource ResourceBlend;

        std::vector<std::string> elementList;
        std::vector<M_Resource> vbResourceOrderedList;

        std::unordered_map<std::string, int> categoryStrideMap = d3d11GameType.getCategoryStrideMap(d3d11GameType.OrderedFullElementList);
        int positionStrie = categoryStrideMap["Position"];
        std::vector <std::string> positionElementListTmp = d3d11GameType.getCategoryElementList(d3d11GameType.OrderedFullElementList, "Position");
        LOG.NewLine();
        LOG.Info(L"Start to load Position category's element list:");
        LOG.Info(L"Position stride:" + std::to_wstring(positionStrie));

        for (M_Resource resourceVB : ResourceVBListInput) {
            int stride = std::stoi(resourceVB.Stride);
            LOG.Info(L"stride:" + std::to_wstring(stride));
            std::wstring resourceFileName = resourceVB.FileName;
            LOG.Info(L"resourceFileName:" + resourceFileName);

            //POSITION
            if (stride == positionStrie) {
                ResourcePosition = resourceVB;
                elementList.insert(elementList.end(), positionElementListTmp.begin(), positionElementListTmp.end());
                vbResourceOrderedList.push_back(ResourcePosition);
                break;

            }
        }


        int colorByteWidth = d3d11GameType.ElementNameD3D11ElementMap["COLOR"].ByteWidth;
        int texcoordByteWidth = d3d11GameType.ElementNameD3D11ElementMap["TEXCOORD"].ByteWidth;
        int texcoord1ByteWidth = d3d11GameType.ElementNameD3D11ElementMap["TEXCOORD1"].ByteWidth;
        LOG.NewLine();
        LOG.Info(L"Start to load TEXCOORD category's element list:");
        for (M_Resource resourceVB : ResourceVBListInput) {
            int stride = std::stoi(resourceVB.Stride);
            std::wstring resourceFileName = resourceVB.FileName;
            //TEXCOORD
            if (stride == texcoordByteWidth) {
                ResourceTexcoord = resourceVB;
                elementList.push_back("TEXCOORD");
                vbResourceOrderedList.push_back(ResourceTexcoord);

                break;

            }
            if (stride == colorByteWidth + texcoordByteWidth) {
                ResourceTexcoord = resourceVB;
                elementList.push_back("COLOR");
                elementList.push_back("TEXCOORD");
                vbResourceOrderedList.push_back(ResourceTexcoord);

                break;

            }
            if (stride == colorByteWidth + texcoordByteWidth + texcoord1ByteWidth) {
                ResourceTexcoord = resourceVB;
                elementList.push_back("COLOR");
                elementList.push_back("TEXCOORD");
                elementList.push_back("TEXCOORD1");
                vbResourceOrderedList.push_back(ResourceTexcoord);

                break;
            }
        }
        LOG.NewLine();
        LOG.Info(L"Start to load Blend category's element list:");
        for (M_Resource resourceVB : ResourceVBListInput) {
            LOG.Info(L"Current processing ResourceVB: " + resourceVB.FileName);

            int stride = std::stoi(resourceVB.Stride);
            std::wstring resourceFileName = resourceVB.FileName;

            std::vector<std::string> blendElementList = d3d11GameType.getCategoryElementList(d3d11GameType.OrderedFullElementList, "Blend");
            int blendStride = d3d11GameType.getElementListStride(blendElementList);
            LOG.Info(L"Blend stride:" + std::to_wstring(blendStride));
            LOG.Info(L"Current stride:" + std::to_wstring(stride));

            //BLEND
            if (stride == blendStride) {
                ResourceBlend = resourceVB;
                if (boost::algorithm::any_of_equal(blendElementList, "BLENDWEIGHT")) {
                    elementList.push_back("BLENDWEIGHT");
                }
                if (boost::algorithm::any_of_equal(blendElementList, "BLENDWEIGHTS")) {
                    elementList.push_back("BLENDWEIGHTS");
                }
                elementList.push_back("BLENDINDICES");

                vbResourceOrderedList.push_back(ResourceBlend);
                break;
            }
        }
        LOG.NewLine();

        /*LOG.LogOutput(L"current Element list detected");
        for (std::wstring elementName : elementList) {
            LOG.LogOutput(elementName);
        }*/

        this->ElementList = elementList;
        this->OrderedResourceVBList = vbResourceOrderedList;

    }
    else if (ResourceVBListInput.size() == 1) {
        int bufStride = 0;
        std::vector<std::string> elementList;
        LOG.Info(L"Detect only one ResourceVB, maybe this is a object mod.");
        M_Resource resourceVBObject = ResourceVBListInput[0];

        elementList.clear();
        bufStride = 0;
        for (const auto& pair : d3d11GameType.ElementNameD3D11ElementMap) {
            std::string elementName = pair.first;
            D3D11Element d3d11Element = pair.second;
            int byteWidth = d3d11Element.ByteWidth;
            bufStride += byteWidth;
            elementList.push_back(elementName);
            LOG.Info("Current ElementName: " + elementName + " ByteWidth: " + std::to_string(byteWidth));
        }
        if (std::stoi(resourceVBObject.Stride) == bufStride) {
            std::vector<M_Resource> vbResourceOrderedList;
            vbResourceOrderedList.push_back(resourceVBObject);
            this->ElementList = elementList;
            this->OrderedResourceVBList = vbResourceOrderedList;
        }


        //������ƥ��ȥ����TEXCOORD1�İ汾
        elementList.clear();
        bufStride = 0;
        for (const auto& pair : d3d11GameType.ElementNameD3D11ElementMap) {
            std::string elementName = pair.first;
            if (elementName == "TEXCOORD1") {
                continue;
            }
            D3D11Element d3d11Element = pair.second;
            int byteWidth = d3d11Element.ByteWidth;
            bufStride += byteWidth;
            elementList.push_back(elementName);
            LOG.Info("Current ElementName: " + elementName + " ByteWidth: " + std::to_string(byteWidth));
        }
        if (std::stoi(resourceVBObject.Stride) == bufStride) {
            std::vector<M_Resource> vbResourceOrderedList;
            vbResourceOrderedList.push_back(resourceVBObject);
            this->ElementList = elementList;
            this->OrderedResourceVBList = vbResourceOrderedList;
        }
    }
    else if (ResourceVBListInput.size() == 4) {
        LOG.Error("Wrong possible ResourceVB size, is your mod contains a extra Remapped Blend part? please remove it and try again.");
    }

    //ȷ������ʶ�������������Resource�б��С��ԭ������ȣ���������ʶ����һ����Ч��Mod
    if (this->OrderedResourceVBList.size() != ResourceVBListInput.size()) {
        this->ValidMod = false;
    }
    else {
        this->ValidMod = true;
    }

    //���ʶ���Category�б�������ȷ���������ȡVB����
    if (this->ValidMod) {
        //��������ElementList��ֹ˳�򲻶�
        this->ElementList = this->d3d11GameType.getReorderedElementList(this->ElementList);
        //����������ElementList�͵�ǰ��d3d11GameType�Ϳ���ȥ��ȡ����װFinalVB0�����ˡ�
        LOG.Info(L"read vb file ,combine final file");
        std::unordered_map<int, std::vector<byte>> vb0IndexNumberBytes;
        for (M_Resource resourceVBObject : this->OrderedResourceVBList) {

            LOG.Info(L"current  : " + resourceVBObject.ResourceName);
            LOG.Info(L"current processing vb file: " + resourceVBObject.FileName);
            resourceVBObject.FileName = resourceVBObject.NameSpace + L"\\" + resourceVBObject.FileName;
            LOG.Info(L"Show the resourceVBObject before read and output to vb file.");
            if (resourceVBObject.FileName == L"" || resourceVBObject.FileName.empty()) {
                LOG.Info(L"Can't find any FileNam in this ResourceVB.");
                continue;
            }
            std::wstring resourceVBFileName = resourceVBObject.FileName;
            int resourceVBStride = std::stoi(resourceVBObject.Stride);
            std::wstring resourceVBFilePath = resourceVBFileName;

            LOG.Info(L"current processing vb file path: " + resourceVBFilePath);

            std::ifstream resourceVBFile(resourceVBFilePath, std::ios::binary);
            resourceVBFile.seekg(0, std::ios::end);
            std::streampos resourceVBFileSize = resourceVBFile.tellg();
            resourceVBFile.seekg(0, std::ios::beg);
            LOG.Info(L"Current processing vb file size: " + std::to_wstring(resourceVBFileSize));
            int resourceVBVertexCount = (int)resourceVBFileSize / resourceVBStride;
            LOG.Info(L"Get vertex count number: " + std::to_wstring(resourceVBVertexCount));

            for (int i = 1; i <= resourceVBVertexCount; i++) {
                std::vector<byte> bytes(resourceVBStride);
                resourceVBFile.read(reinterpret_cast<char*>(bytes.data()), resourceVBStride);
                vb0IndexNumberBytes[i].insert(vb0IndexNumberBytes[i].end(), bytes.begin(), bytes.end());
            }
            resourceVBFile.close();
        }
        LOG.Info(L"Read vb file finished, combined to final format.");
        LOG.NewLine();
        this->VB0IndexNumberBytes = vb0IndexNumberBytes;

        //��װΪFinalVB0Bytes
        std::vector<byte> outputBytes;
        for (const auto& pair : this->VB0IndexNumberBytes) {
            std::vector<byte> indexBytes = pair.second;
            outputBytes.insert(outputBytes.end(), indexBytes.begin(), indexBytes.end());
        }
        this->finalVB0Bytes = outputBytes;

        //LOG.Info("outputBytes.size():  " + std::to_string(outputBytes.size()));
    }
}