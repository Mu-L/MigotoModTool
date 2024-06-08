#include "VertexBufferBufFile.h"
#include "MMTFileUtils.h"
#include "MMTStringUtils.h"
#include <fstream>

VertexBufferBufFile::VertexBufferBufFile() {

}


VertexBufferBufFile::VertexBufferBufFile(std::wstring readVBBufFilePath, D3D11GameType d3d11GameType, std::vector<std::string> elementList) {

    int SplitStride = d3d11GameType.getElementListStride(elementList);
    std::vector<std::string> CategoryList = d3d11GameType.getCategoryList(elementList);
    std::unordered_map<std::string, int> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(elementList);

    uint64_t VBFileSize = MMTFile_GetFileSize(readVBBufFilePath);
    uint64_t vbFileVertexNumber = VBFileSize / SplitStride;

    //��ȡ����
    std::vector<std::byte> buffer(VBFileSize);
    std::ifstream VBFile(MMTString_ToByteString(readVBBufFilePath), std::ios::binary);
    VBFile.read(reinterpret_cast<char*>(buffer.data()), VBFileSize);
    VBFile.close();

    //ͨ�����ͣ���ȡ��ȡ���ȣ����ﻹ�������
    std::vector<int> readLengths;
    for (const std::string& category : CategoryList) {
        readLengths.push_back(CategoryStrideMap[category]);
    }

    int offset = 0;
    while (offset < buffer.size()) {
        for (size_t i = 0; i < CategoryList.size(); ++i) {
            const std::string& category = CategoryList[i];
            // ָ��Ҫ��ȡ���ֽڳ���
            int readLength = readLengths[i];
            std::vector<std::byte> categoryDataPatch;
            // Ԥ�����㹻���ڴ�ռ�
            categoryDataPatch.reserve(readLength);
            // ʹ��std::copy������ѭ�������Ч��
            std::copy(buffer.begin() + offset, buffer.begin() + offset + readLength, std::back_inserter(categoryDataPatch));

            // �ҵ�tmp�����
            std::unordered_map<std::wstring, std::vector<std::byte>>& tmpVBCategoryDataMap = this->CategoryVBDataMap;

            // �ҵ�tmp�������Ӧ��category������
            std::vector<std::byte>& categoryData = tmpVBCategoryDataMap[MMTString_ToWideString(category)];


            // ��categoryDataPatch������ֱ�Ӳ���categoryData
            categoryData.insert(categoryData.end(), categoryDataPatch.begin(), categoryDataPatch.end());

            offset += readLength;
        }

    }



};