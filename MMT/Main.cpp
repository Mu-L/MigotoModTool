#include <io.h>
#include "GlobalConfigs.h"
#include "GlobalFunctions.h"

//��ʼ��easylogpp
INITIALIZE_EASYLOGGINGPP
//��ʼ��ȫ������
GlobalConfigs G;
//��ʼ����־
MMTLogger LOG;

std::int32_t wmain(std::int32_t argc, wchar_t* argv[])
{
    setlocale(LC_ALL, "Chinese-simplified");
    std::wstring fullPath = argv[0];
    std::wstring applicationLocation = MMTString_GetFolderPathFromFilePath(fullPath);

    //���ȳ�ʼ����־���ã��ǳ���Ҫ
    boost::posix_time::ptime currentTime = boost::posix_time::second_clock::local_time();
    std::string logFileName = "Logs\\" + boost::posix_time::to_iso_string(currentTime) + ".log";
    el::Configurations logConfigurations;
    logConfigurations.setToDefault();
    logConfigurations.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "false");
    logConfigurations.set(el::Level::Global, el::ConfigurationType::ToFile, "true");
    logConfigurations.set(el::Level::Global, el::ConfigurationType::Filename, logFileName);
    el::Loggers::reconfigureAllLoggers(logConfigurations);

    LOG.Info(L"Running : " + fullPath);
    LOG.Info("Running in Release mode.");
    LOG.NewLine();

    //��ʼ����־��
    LOG = MMTLogger(applicationLocation);
    //��ʼ��ȫ������
    G = GlobalConfigs(applicationLocation);

#ifdef _DEBUG 
    LOG.NewLine();
    //UnityExtractCS();

    //ExtractFromBuffer_CS_WW();
    ReverseOutfitCompilerCompressed();
    //combineMods(L"");
#else
    LOG.Info(L"Running Command: " + G.RunCommand);
    LOG.NewLine();

    //������ȡģ��
    if (G.RunCommand == L"merge") {
        //�������ΪMerge����ȷ����������һ��FrameAnalysis�ļ���
        if (G.FrameAnalyseFolder == L"") {
            LOG.Error("Can't find any FrameAnalysis folder in your 3Dmigoto folder,please try press F8 to dump a new one with Hunting open.");
        }

        //����ÿ����Ϸ����ȡ���̶���һ�������Էֿ�����
        if (G.GameName == L"WW") {
            ExtractFromWW();
        }
        else if (G.GameName == L"HI3") {
            ExtractFromBuffer_VS();
        }
        else if (G.GameName == L"GI") {
            ExtractFromBuffer_VS();
        }
        else if (G.GameName == L"HSR") {
            ExtractFromBuffer_VS();
        }
        else if (G.GameName == L"ZZZ") {
            ExtractFromBuffer_VS();
        }
        else if (G.GameName == L"SnB" || G.GameName == L"KBY") {
            Extract_VS_UE4();
        }
    }
    //����Mod
    else if (G.RunCommand == L"split") {
        if (G.GameName == L"WW") {
            Generate_CS_WW_Body();
        }
        else if (G.GameName == L"SnB" || G.GameName == L"KBY") {
            Generate_VS_UE4();
        }
        else {
            UnityGenerate();
        }
    }
    else if (G.RunCommand == L"mergeReverse") {
        if (G.GameName == L"HI3") {
            ExtractFromBuffer_VS_Reverse();
        }
        else if (G.GameName == L"GI") {
            ExtractFromBuffer_VS_Reverse();
        }
        else if (G.GameName == L"HSR") {
            ExtractFromBuffer_VS_Reverse();
        }
        else if (G.GameName == L"ZZZ") {
            ExtractFromBuffer_VS_Reverse();
        }
    }
    else if (G.RunCommand == L"reverseOutfitCompiler") {
        ReverseOutfitCompilerCompressed();
    }
    else if (G.RunCommand == L"reverseSingle") {
        ReverseSingle();
    }
    else if (G.RunCommand == L"reverseMerged") {
        ReverseMerged();
    }

    LOG.Success();
#endif
    return 0;
}
