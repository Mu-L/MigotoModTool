#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

class IndexBufferBufFile {
public:

	//��С�Ķ�����ֵ
	uint32_t MinNumber = -1;

	//���Ķ�����ֵ
	uint32_t MaxNumber = -1;

	//�ܹ��м�������
	uint32_t NumberCount = -1;

	//�����б�
	std::vector<uint32_t> NumberList;


	IndexBufferBufFile(std::wstring FileReadPath,std::wstring Format);

	void SaveToFile_UINT32(std::wstring FileWritePath, uint32_t Offset);
};