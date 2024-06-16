#include "MigotoFormatUtils.h"
#include "MMTFormatUtils.h"


std::unordered_map<std::wstring, std::vector<std::byte>> RecalculateColor(std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig, std::unordered_map<std::string, int> categoryStrideMap) {

    
    D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[basicConfig.WorkGameType];

    //��ȡPosition���������
    std::vector<std::byte> PositionCategoryValues = finalVBCategoryDataMap[L"Position"];

    //��ȡPOSITION��TANGENT
    std::vector<std::vector<double>> POSITION_VALUES;
    std::vector<std::vector<double>> NORMAL_VALUES;

    int positionStride = 0;
    //��ΪPOSITION���������TEXCOORD�����Ķ�̬��������������ֱ�ӻ�ȡOrderedElementList��ͳ�Ƴ�Position�ĳ���
    for (std::string elementName : d3d11GameType.OrderedFullElementList) {
        D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];
        if (d3d11Element.Category == "Position") {
            int byteWidth = d3d11Element.ByteWidth;
            positionStride += byteWidth;

        }
    }
    LOG.Info(L"PositionStride: " + std::to_wstring(positionStride));
    for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride)
    {
        std::vector<std::byte> POSITION_X = MMTFormat_GetRange_Byte(PositionCategoryValues, i, i + 4);
        //LOG.LogOutput(L"POSITION_X: " + std::to_wstring(bytesToFloat(POSITION_X)));
        std::vector<std::byte> POSITION_Y = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 4, i + 8);
        std::vector<std::byte> POSITION_Z = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 8, i + 12);
        std::vector<double> POSITION_DOUBLE = { MMTFormat_ByteVectorToFloat(POSITION_X), MMTFormat_ByteVectorToFloat(POSITION_Y) ,MMTFormat_ByteVectorToFloat(POSITION_Z) };
        POSITION_VALUES.push_back(POSITION_DOUBLE);

        std::vector<std::byte> NORMAL_X = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 12, i + 16);
        std::vector<std::byte> NORMAL_Y = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 16, i + 20);
        std::vector<std::byte> NORMAL_Z = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 20, i + 24);
        std::vector<double> NORMAL_DOUBLE = { MMTFormat_ByteVectorToFloat(NORMAL_X), MMTFormat_ByteVectorToFloat(NORMAL_Y) ,MMTFormat_ByteVectorToFloat(NORMAL_Z) };
        NORMAL_VALUES.push_back(NORMAL_DOUBLE);
    }
    LOG.Info(L"Start to CalculateAverageNormalsAndStoreTangent");
    //�Ѷ�����ͬ�ķ��� 3ά���� ���Ȼ���һ������Ϊ����ֵΪ�������ֵ����ͬ����
    //Ȼ���ÿ����Ӧ��TANGENT���������ֵ��Ϊǰ3λ��0��Ϊ���һλ

    //�����ƽ�ַ��ߵ�TANGENT
    std::vector<std::vector<double>> TANGENT_VALUES = MMTFormat_NormalNormalizeStoreTangent(POSITION_VALUES, NORMAL_VALUES);
    LOG.Info(L"TANGENT_VALUES size: " + std::to_wstring(TANGENT_VALUES.size()));
    LOG.Info(L"CalculateAverageNormalsAndStoreTangent success");


    //׼��һ���µ����ڷ���
    std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;

    //����Ҫȷ��Color���ĸ���λ��
    D3D11Element colorElement = d3d11GameType.ElementNameD3D11ElementMap["COLOR"];
    std::wstring colorCategory = MMTString_ToWideString(colorElement.Category);
    std::vector<std::byte> colorCategoryValues = finalVBCategoryDataMap[colorCategory];

    //ȷ��Color���ڲ�λ�Ĳ���
    int colorCategoryStride = categoryStrideMap[MMTString_ToByteString(colorCategory)];

    //ȷ��Color���ڵĲ�λƫ��
    int colorOffset = 0;

    //TODO ȷ��Color��ByteWidth,�������ǹ̶�Ϊ4����Ϊֻ����һ���������ʵҲ�ò���������������������ʹ��
    //int colorByteWidth = 4;

    for (std::string elementName : basicConfig.TmpElementList) {
        D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];

        //���ȱ�������color���ڵĲ�λ
        if (d3d11Element.Category == MMTString_ToByteString(colorCategory)) {
            if (elementName != "COLOR") {
                colorOffset += d3d11Element.ByteWidth;
            }
            else {
                break;
            }
        }
    }

    //��ʼ����
    std::vector<std::byte> newTexcoordCategoryValues;
    int count = 0;
    for (std::size_t i = 0; i < colorCategoryValues.size(); i = i + colorCategoryStride)
    {
        //�Ȼ�ȡCOLORǰ������ֵ
        std::vector<std::byte> beforeValues = MMTFormat_GetRange_Byte(colorCategoryValues, i, i + colorOffset);
        std::vector<std::byte> ColorValues = MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 4);
        std::vector<std::byte> afterValues = MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset + 4, i + colorCategoryStride);
        //��׼����COLORֵ
        std::vector<std::byte> newColorValues;
        std::vector<double> TANGENT_DOUBLES = TANGENT_VALUES[count];

        //R,G,B�ֱ�洢��һ��֮��Normal�ķ���
        newColorValues.push_back(MMTFormat_PackNumberToByte(static_cast<uint32_t>(round(TANGENT_DOUBLES[0] * 255))));
        newColorValues.push_back(MMTFormat_PackNumberToByte(static_cast<uint32_t>(round(TANGENT_DOUBLES[1] * 255))));
        newColorValues.push_back(MMTFormat_PackNumberToByte(static_cast<uint32_t>(round(TANGENT_DOUBLES[2] * 255))));

        //Alphaͨ���̶�Ϊ128
        newColorValues.push_back(ColorValues[3]);

        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), beforeValues.begin(), beforeValues.end());
        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), newColorValues.begin(), newColorValues.end());
        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), afterValues.begin(), afterValues.end());

        count++;
    }

    newVBCategoryDataMap = finalVBCategoryDataMap;
    newVBCategoryDataMap[colorCategory] = newTexcoordCategoryValues;
    return newVBCategoryDataMap;

}





//����COLOR��Position�����е������һ���㷨ͬʱ����Position��Texcoord
std::unordered_map<std::wstring, std::vector<std::byte>> ResetColor(std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig, std::unordered_map<std::string, int> categoryStrideMap) {
    //��ȡ����RGB������ֵ
    std::wstring rgb_r = MMTString_ToWideString(basicConfig.ColorMap["rgb_r"]);
    std::wstring rgb_g = MMTString_ToWideString(basicConfig.ColorMap["rgb_g"]);
    std::wstring rgb_b = MMTString_ToWideString(basicConfig.ColorMap["rgb_b"]);
    std::wstring rgb_a = MMTString_ToWideString(basicConfig.ColorMap["rgb_a"]);

    //׼�������������滻
    int rgb_r_num = -1;
    int rgb_g_num = -1;
    int rgb_b_num = -1;
    int rgb_a_num = -1;

    //���Խ��и�ʽת��
    if (L"default" != rgb_r) {
        rgb_r_num = std::stoi(rgb_r);
    }
    if (L"default" != rgb_g) {
        rgb_g_num = std::stoi(rgb_g);
    }
    if (L"default" != rgb_b) {
        rgb_b_num = std::stoi(rgb_b);
    }
    if (L"default" != rgb_a) {
        rgb_a_num = std::stoi(rgb_a);
    }

    //�������default�Ǹɴ�ɶҲ���ֱ�ӷ���
    if (rgb_a_num == -1 && rgb_g_num == -1 && rgb_b_num == -1 && rgb_a_num == -1) {
        return finalVBCategoryDataMap;
    }

    //׼��һ���µ����ڷ���
    std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;

    //����Ҫȷ��Color���ĸ���λ��
    D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[basicConfig.WorkGameType];
    D3D11Element colorElement = d3d11GameType.ElementNameD3D11ElementMap["COLOR"];
    std::wstring colorCategory = MMTString_ToWideString(colorElement.Category);
    std::vector<std::byte> colorCategoryValues = finalVBCategoryDataMap[colorCategory];

    //׼������������д��
    std::byte patchRgb_R = MMTFormat_PackNumberToByte(rgb_r_num);
    std::byte patchRgb_G = MMTFormat_PackNumberToByte(rgb_g_num);
    std::byte patchRgb_B = MMTFormat_PackNumberToByte(rgb_b_num);
    std::byte patchRgb_A = MMTFormat_PackNumberToByte(rgb_a_num);

    //ȷ��Color���ڲ�λ�Ĳ���
    int colorCategoryStride = categoryStrideMap[MMTString_ToByteString(colorCategory)];

    //ȷ��Color���ڵĲ�λƫ��
    int colorOffset = 0;

    //TODO ȷ��Color��ByteWidth,�������ǹ̶�Ϊ4����Ϊֻ����һ���������ʵҲ�ò���������������������ʹ��
    //int colorByteWidth = 4;

    for (std::string elementName : basicConfig.TmpElementList) {
        D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];

        //���ȱ�������color���ڵĲ�λ
        if (d3d11Element.Category == MMTString_ToByteString(colorCategory)) {
            if (elementName != "COLOR") {
                colorOffset += d3d11Element.ByteWidth;
            }
            else {
                break;
            }
        }
    }

    //��ʼ����
    std::vector<std::byte> newTexcoordCategoryValues;
    for (std::size_t i = 0; i < colorCategoryValues.size(); i = i + colorCategoryStride)
    {
        //�Ȼ�ȡCOLORǰ������ֵ
        std::vector<std::byte> beforeValues = MMTFormat_GetRange_Byte(colorCategoryValues, i, i + colorOffset);
        std::vector<std::byte> ColorValues = MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 4);
        std::vector<std::byte> afterValues = MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset + 4, i + colorCategoryStride);
        //��׼����COLORֵ
        std::vector<std::byte> newColorValues;

        if (rgb_r_num != -1) {
            newColorValues.push_back(patchRgb_R);
        }
        else {
            newColorValues.push_back(MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 1)[0]);
        }

        if (rgb_g_num != -1) {
            newColorValues.push_back(patchRgb_G);
        }
        else {
            newColorValues.push_back(MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset + 1, i + colorOffset + 2)[0]);
        }

        if (rgb_b_num != -1) {
            newColorValues.push_back(patchRgb_B);
        }
        else {
            newColorValues.push_back(MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset + 2, i + colorOffset + 3)[0]);
        }

        if (rgb_a_num != -1) {
            newColorValues.push_back(patchRgb_A);
        }
        else {
            newColorValues.push_back(MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset + 3, i + colorOffset + 4)[0]);
        }

        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), beforeValues.begin(), beforeValues.end());
        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), newColorValues.begin(), newColorValues.end());
        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), afterValues.begin(), afterValues.end());
    }

    newVBCategoryDataMap = finalVBCategoryDataMap;
    newVBCategoryDataMap[colorCategory] = newTexcoordCategoryValues;
    return newVBCategoryDataMap;
};




//TODO �����TANGENT���㲽������д�����������仯
std::unordered_map<std::wstring, std::vector<std::byte>> TANGENT_averageNormal(std::unordered_map<std::wstring, std::vector<std::byte>> inputVBCategoryDataMap, D3D11GameType d3d11GameType) {
    LOG.Info(L"Start to recalculate tangent use averageNormal algorithm:");
    std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;
    //��ȡPosition���������
    std::vector<std::byte> PositionCategoryValues = inputVBCategoryDataMap[L"Position"];

    //��ȡPOSITION��TANGENT
    std::vector<std::vector<double>> POSITION_VALUES;
    std::vector<std::vector<double>> NORMAL_VALUES;

    int positionStride = 0;
    //��ΪPOSITION���������TEXCOORD�����Ķ�̬��������������ֱ�ӻ�ȡOrderedElementList��ͳ�Ƴ�Position�ĳ���
    for (std::string elementName : d3d11GameType.OrderedFullElementList) {
        D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];
        if (d3d11Element.Category == "Position") {
            int byteWidth = d3d11Element.ByteWidth;
            positionStride += byteWidth;

        }
    }
    LOG.Info(L"PositionStride: " + std::to_wstring(positionStride));
    for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride)
    {
        std::vector<std::byte> POSITION_X = MMTFormat_GetRange_Byte(PositionCategoryValues, i, i + 4);
        //LOG.LogOutput(L"POSITION_X: " + std::to_wstring(bytesToFloat(POSITION_X)));
        std::vector<std::byte> POSITION_Y = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 4, i + 8);
        std::vector<std::byte> POSITION_Z = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 8, i + 12);
        std::vector<double> POSITION_DOUBLE = { MMTFormat_ByteVectorToFloat(POSITION_X), MMTFormat_ByteVectorToFloat(POSITION_Y) ,MMTFormat_ByteVectorToFloat(POSITION_Z) };
        POSITION_VALUES.push_back(POSITION_DOUBLE);

        std::vector<std::byte> NORMAL_X = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 12, i + 16);
        std::vector<std::byte> NORMAL_Y = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 16, i + 20);
        std::vector<std::byte> NORMAL_Z = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 20, i + 24);
        std::vector<double> NORMAL_DOUBLE = { MMTFormat_ByteVectorToFloat(NORMAL_X), MMTFormat_ByteVectorToFloat(NORMAL_Y) ,MMTFormat_ByteVectorToFloat(NORMAL_Z) };
        NORMAL_VALUES.push_back(NORMAL_DOUBLE);
    }
    LOG.Info(L"Start to CalculateAverageNormalsAndStoreTangent");
    //�Ѷ�����ͬ�ķ��� 3ά���� ���Ȼ���һ������Ϊ����ֵΪ�������ֵ����ͬ����
    //Ȼ���ÿ����Ӧ��TANGENT���������ֵ��Ϊǰ3λ��0��Ϊ���һλ

    //�����ƽ�ַ��ߵ�TANGENT
    std::vector<std::vector<double>> TANGENT_VALUES = MMTFormat_CalculateAverageNormalsAndStoreTangent(POSITION_VALUES, NORMAL_VALUES);
    LOG.Info(L"TANGENT_VALUES size: " + std::to_wstring(TANGENT_VALUES.size()));
    LOG.Info(L"CalculateAverageNormalsAndStoreTangent success");

    //����һ���µ�vector����װ�޸ĺ��Position���������
    std::vector<std::byte> newPositionCategoryValues;
    //���TANGENT���µ�newPositionCategoryValues
    int count = 0;
    for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride)
    {
        //TODO �����positionStride - 16�õ��ľ��ǳ���TANGENT֮���������element���ܳ���
        //��Ϊ��HI3Body2�У�Position����POSITION��NORMAL��COLOR��TANGENT��ɵģ���������������POSITION��NORMAL��TANGENT
        //������Ȼ������ʱ�����ˣ����Ǽ����Բ����ã��Ժ������޸İɡ�
        std::vector<std::byte> POSITION_NORMAL_VALUES = MMTFormat_GetRange_Byte(PositionCategoryValues, i, i + (positionStride - 16));

        std::vector<double> TANGENT_DOUBLES = TANGENT_VALUES[count];

        std::vector<std::byte> TANGENT_X;
        std::vector<std::byte> TANGENT_Y;
        std::vector<std::byte> TANGENT_Z;
        std::vector<std::byte> TANGENT_W;

        double tangent_double_x = TANGENT_DOUBLES[0];

        TANGENT_X = MMTFormat_PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[0]);
        TANGENT_Y = MMTFormat_PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[1]);
        TANGENT_Z = MMTFormat_PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[2]);
        //TANGENT��W����Ҫô��-1Ҫô��1�������ɵ�ǰ��������Ծ����ģ���������ʹ��Blender����ʱ�Դ��ķ�����һ��
        //TANGENT_W = GetRange(PositionCategoryValues, i + 36, i + 40);
        TANGENT_W = MMTFormat_PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[3]);

        //LOG.LogOutput(L"Tangent: " + formatedFloat(TANGENT_DOUBLES[0],10) + L"," + formatedFloat(TANGENT_DOUBLES[1],10) + L"," + formatedFloat(TANGENT_DOUBLES[2], 10) + L"," + formatedFloat(TANGENT_DOUBLES[3], 10));

        newPositionCategoryValues.insert(newPositionCategoryValues.end(), POSITION_NORMAL_VALUES.begin(), POSITION_NORMAL_VALUES.end());

        newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_X.begin(), TANGENT_X.end());
        newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_Y.begin(), TANGENT_Y.end());
        newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_Z.begin(), TANGENT_Z.end());
        newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_W.begin(), TANGENT_W.end());



        count = count + 1;
        //LOG.LogOutput(L"Count: " + std::to_wstring(count));
    }

    LOG.Info(L"Recalculate Tangent success! ");
    newVBCategoryDataMap = inputVBCategoryDataMap;
    newVBCategoryDataMap[L"Position"] = newPositionCategoryValues;
    return newVBCategoryDataMap;
};



//ͨ����ȡ�Ĳ�������תNormal��X��Y��Z��������תTangent��X��Y��Z��W����
std::unordered_map<std::wstring, std::vector<std::byte>> ReverseNormalTangentValues(
    std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig) {
    LOG.Info(L"Start to flip NORMAL and TANGENT values");
    std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;
    std::vector<std::byte> PositionCategoryValues = finalVBCategoryDataMap[L"Position"];

    //����Ҫ�õ�TmpElementList��Ȼ��ͳ�Ƴ�����Position��λ��Ԫ������Щ
    D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[basicConfig.WorkGameType];
    std::vector<std::string> positionElementList;
    int positionStride = 0;
    for (std::string elementName : basicConfig.TmpElementList) {
        D3D11Element elementObject = d3d11GameType.ElementNameD3D11ElementMap[elementName];
        std::wstring category = MMTString_ToWideString(elementObject.Category);
        if (category == L"Position") {
            LOG.Info(L"Detect Position category element: " + MMTString_ToWideString(elementName) + L"  stride: " + std::to_wstring(elementObject.ByteWidth));
            positionElementList.push_back(elementName);
            positionStride += elementObject.ByteWidth;

        }
    }
    LOG.Info(L"PositionStride: " + std::to_wstring(positionStride));

    // Ȼ��̬�Ķ�ȡֵ��Ȼ��ֻ�޸�NORMAL��TANGENTֵ
    std::vector<std::byte> newPositionCategoryValues;
    for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride) {

        std::vector<std::byte> newPOSITIONValues;
        //����positionElementList������ֻ����NORMAL��TANGENT������ԭ�ⲻ���ŵ����б�
        int offset = 0;
        for (std::string elementName : positionElementList) {
            //LOG.LogOutput(L"Tmp  offset: " + std::to_wstring(offset));
            D3D11Element elementObject = d3d11GameType.ElementNameD3D11ElementMap[elementName];
            std::wstring format = MMTString_ToWideString(elementObject.Format);
            int elementStrie = elementObject.ByteWidth;

            //LOG.LogOutput(L"Tmp  format: " + format);
            //LOG.LogOutput(L"Tmp  elementStrie: " + std::to_wstring(elementStrie));


            std::vector<std::byte> TmpValues = MMTFormat_GetRange_Byte(PositionCategoryValues, i + offset, i + offset + elementStrie);
            if (elementName == "NORMAL") {
                // TODO �������������������ת��Ӧ������Ŀǰ����NORMAL���������ͣ�һ��R32G32B32_FLOAT һ��R16G16B16A16_FLOAT����A16ȫ��0
                if (format == L"R32G32B32_FLOAT") {
                    std::vector<std::byte> FlipedValues;

                    std::vector<std::byte> ValueR = MMTFormat_GetRange_Byte(TmpValues, 0, 4);
                    std::vector<std::byte> ValueG = MMTFormat_GetRange_Byte(TmpValues, 4, 8);
                    std::vector<std::byte> ValueB = MMTFormat_GetRange_Byte(TmpValues, 8, 12);

                    std::vector<std::byte> ValueR_Reversed = basicConfig.NormalReverseX ? MMTFormat_Reverse32BitFloatValue(ValueR) : ValueR;
                    std::vector<std::byte> ValueG_Reversed = basicConfig.NormalReverseY ? MMTFormat_Reverse32BitFloatValue(ValueG) : ValueG;
                    std::vector<std::byte> ValueB_Reversed = basicConfig.NormalReverseZ ? MMTFormat_Reverse32BitFloatValue(ValueB) : ValueB;

                    FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());

                    newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                }
                else if (format == L"R16G16B16A16_FLOAT") {
                    std::vector<std::byte> FlipedValues;

                    std::vector<std::byte> ValueR = MMTFormat_GetRange_Byte(TmpValues, 0, 2);
                    std::vector<std::byte> ValueG = MMTFormat_GetRange_Byte(TmpValues, 2, 4);
                    std::vector<std::byte> ValueB = MMTFormat_GetRange_Byte(TmpValues, 4, 6);
                    std::vector<std::byte> ValueA = MMTFormat_GetRange_Byte(TmpValues, 6, 8);

                    std::vector<std::byte> ValueR_Reversed = basicConfig.NormalReverseX ? MMTFormat_Reverse16BitShortValue(ValueR) : ValueR;
                    std::vector<std::byte> ValueG_Reversed = basicConfig.NormalReverseY ? MMTFormat_Reverse16BitShortValue(ValueG) : ValueG;
                    std::vector<std::byte> ValueB_Reversed = basicConfig.NormalReverseZ ? MMTFormat_Reverse16BitShortValue(ValueB) : ValueB;

                    FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());
                    //NORMAL��A������Զ��0������Ҫ��ת
                    FlipedValues.insert(FlipedValues.end(), ValueA.begin(), ValueA.end());

                    newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                }
                else {
                    newPOSITIONValues.insert(newPOSITIONValues.end(), TmpValues.begin(), TmpValues.end());
                    //LOG.Warning(L"Can't find any format for NORMAL vlaues,use original NORMAL values");
                }

            }
            else if (elementName == "TANGENT") {

                if (format == L"R32G32B32A32_FLOAT") {
                    std::vector<std::byte> FlipedValues;

                    std::vector<std::byte> ValueR = MMTFormat_GetRange_Byte(TmpValues, 0, 4);
                    std::vector<std::byte> ValueG = MMTFormat_GetRange_Byte(TmpValues, 4, 8);
                    std::vector<std::byte> ValueB = MMTFormat_GetRange_Byte(TmpValues, 8, 12);
                    std::vector<std::byte> ValueA = MMTFormat_GetRange_Byte(TmpValues, 12, 16);

                    std::vector<std::byte> ValueR_Reversed = basicConfig.TangentReverseX ? MMTFormat_Reverse32BitFloatValue(ValueR) : ValueR;
                    std::vector<std::byte> ValueG_Reversed = basicConfig.TangentReverseY ? MMTFormat_Reverse32BitFloatValue(ValueG) : ValueG;
                    std::vector<std::byte> ValueB_Reversed = basicConfig.TangentReverseZ ? MMTFormat_Reverse32BitFloatValue(ValueB) : ValueB;
                    std::vector<std::byte> ValueA_Reversed = basicConfig.TangentReverseW ? MMTFormat_Reverse32BitFloatValue(ValueA) : ValueA;

                    FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueA_Reversed.begin(), ValueA_Reversed.end());

                    newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                }
                else if (format == L"R16G16B16A16_FLOAT") {
                    std::vector<std::byte> FlipedValues;

                    std::vector<std::byte> ValueR = MMTFormat_GetRange_Byte(TmpValues, 0, 2);
                    std::vector<std::byte> ValueG = MMTFormat_GetRange_Byte(TmpValues, 2, 4);
                    std::vector<std::byte> ValueB = MMTFormat_GetRange_Byte(TmpValues, 4, 6);
                    std::vector<std::byte> ValueA = MMTFormat_GetRange_Byte(TmpValues, 6, 8);

                    std::vector<std::byte> ValueR_Reversed = basicConfig.TangentReverseX ? MMTFormat_Reverse16BitShortValue(ValueR) : ValueR;
                    std::vector<std::byte> ValueG_Reversed = basicConfig.TangentReverseY ? MMTFormat_Reverse16BitShortValue(ValueG) : ValueG;
                    std::vector<std::byte> ValueB_Reversed = basicConfig.TangentReverseZ ? MMTFormat_Reverse16BitShortValue(ValueB) : ValueB;
                    std::vector<std::byte> ValueA_Reversed = basicConfig.TangentReverseW ? MMTFormat_Reverse16BitShortValue(ValueA) : ValueA;

                    FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueA_Reversed.begin(), ValueA_Reversed.end());

                    newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                }
                else {
                    newPOSITIONValues.insert(newPOSITIONValues.end(), TmpValues.begin(), TmpValues.end());
                    //LOG.Warning(L"Can't find any format for TANGENT vlaues,use default TANGENT values.");
                }

            }
            else {
                //����ת�Ļ�ֱ����ӽ�ȥ������
                newPOSITIONValues.insert(newPOSITIONValues.end(), TmpValues.begin(), TmpValues.end());

            }
            offset += elementStrie;

        }

        //�������POSITION values��ӵ��ܵ�Position��λ��
        newPositionCategoryValues.insert(newPositionCategoryValues.end(), newPOSITIONValues.begin(), newPOSITIONValues.end());
    }

    LOG.Info(L"Size: newPositionCategoryValues : " + std::to_wstring(newPositionCategoryValues.size()));
    LOG.Info(L"Size: PositionCategoryValues : " + std::to_wstring(PositionCategoryValues.size()));

    newVBCategoryDataMap = finalVBCategoryDataMap;
    newVBCategoryDataMap[L"Position"] = newPositionCategoryValues;
    return newVBCategoryDataMap;
};
