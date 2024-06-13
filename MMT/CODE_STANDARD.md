# Project design
- C++ 20��׼
- ʹ��Boost��
- ʹ��Json for morden C++ �⴦��Json�ļ� https://github.com/nlohmann/json
- ʹ��easyloggingpp�⴦����־��¼
- Visual Studio 2022 & Platform Set V143

# ����淶
- ����ʹ��string��������������ſ���ʹ��wstring������������
- ���й�������MMT��ͷ��Utils��β��������Ҫ���д��.h ��.cpp�У�Ҫ��������ճ��.h��.cpp�ļ�������������Ŀ��ʹ��
- ����ʱ�õ�ʲôincludeʲô����Ҫ��ǰinclude
- ��ֹʹ��using namespace��ֹ�����ռ������̫���Ļ����ù�������а������򻯽ӿڣ������Բ���ʹ��using namespace
- �������еķ���Ҫ��MMT��ͷ+��������+�»��ߣ��շ�����������MMTString_To_Wide_String() , MMTFile_Exists()
- ����ÿһ�����棬������ȥ�����������˵Ŀ��Գ���Ѱ�����������������Ҫ����������������Լ��Ĵ�����־��档
- �������еĺ������������ǿ��ַ������ͣ������Ʊ�����W��β
- ��ֹʹ��boost::algorithm::split����Ϊ������bug�޷���L"=="��Ϊ�ָ��������ָ�wstring.
- �����ļ�Ҫʹ��std::filesystem::copy_file
- Boost.log��easyloggingpp��ֻ��ʹ��std::string������ʹ��std::wstring.
- ����һ��������������Ŀ�������Ƕ����Ŀ֮���ļ����ͷ���������࣬�ᵼ��Visual Studio�޷���ȷ�����������ӣ��޷���ȷ��ʹ�ö�Ӧ�ķ��š�

# Build from source code
MMT's core idea is designed for extremely fast speed process for everything in making d3d11 buffer replace game mod, 
so it's core algorithm is writen in C++ and it's GUI is writen in C# WinForm, 
all code is designed for fast development, fast run and fast modify for release new version ,etc..., 
anyway, time is valuable, tool need to be fast.

Some preconfig you will need to have:
- VisualStudio 2022 VS143 toolset. 
- Install Boost 1.84.0  (or latest version)
- (1) Download Boost 1.84.0 and unzip it to somewhere.
- (2) Open your terminal and switch into your boost unziped folder.
- (3) to initialize boost config, type this command in your terminal: .\bootstrap.bat
- (4) then type this in your terminal to build boost library for MMT:

- .\b2.exe variant=release link=static runtime-link=static threading=multi runtime-debugging=off
- (5) Change your project's setting path, on my computer i put the unziped boost on C:\AppLibraries, so for my computer it's path looks like below,but you need to change it to your own path:
The following directory should be added to compiler include paths:

    C:\AppLibraries\boost_1_84_0

The following directory should be added to linker library paths:

    C:\AppLibraries\boost_1_84_0\stage\lib

After these steps above ,you will be able to build MMT on X64 DEBUG or RELEASE, x32-bit is also work but not official supported by MMT for low speed reason.

# MMT Third party used:
- easylogging
- Json for morden C++ (nlohmann json)
- Boost.
MMT-GUI will need these packages, you need to install it in NuGet manager.
- Newtonsoft.Json


# Join Development and you can make it better!