#pragma once

class WuwaCSInfo {
public:
    //֮ǰ�Ѽ��㶥����ܺ�
    int Offset;
    //��ǰCS���������Ҳ���ǵ�ǰ�Ĳ����Ķ�����
    int CalculateTime;
    //ʹ���ĸ�Shader������Pre-Skinning
    std::wstring ComputeShaderHash;
};
