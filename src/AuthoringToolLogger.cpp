#include <sdw.h>

#if SDW_PLATFORM == SDW_PLATFORM_WINDOWS
#define UGetcwd _wgetcwd
#else
#define UGetcwd getcwd
#endif

int UMain(int argc, UChar* argv[])
{
	const u32 uMaxPath = 4096;
	UString sCurrentWorkingDirName(uMaxPath, USTR('\0'));
	if (UGetcwd(&*sCurrentWorkingDirName.begin(), uMaxPath) == nullptr)
	{
		UPrintf(USTR("ERROR: getcwd error\n\n"));
		sCurrentWorkingDirName.clear();
	}
	else
	{
		sCurrentWorkingDirName.resize(UCslen(sCurrentWorkingDirName.c_str()));
	}
	UString sModuleDirName = UGetModuleDirName();
	UString sAuthoringToolNameFileName = sModuleDirName + USTR("/AuthoringToolName.txt");
	UString sAuthoringToolName = sModuleDirName + USTR("/AuthoringToolOrig.exe");
	FILE* fp = UFopen(sAuthoringToolNameFileName.c_str(), USTR("rb"), false);
	if (fp != nullptr)
	{
		fseek(fp, 0, SEEK_END);
		u32 uNameSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		char* pName = new char[uNameSize + 1];
		fread(pName, 1, uNameSize, fp);
		fclose(fp);
		pName[uNameSize] = '\0';
		UString sName = AToU(pName);
		delete[] pName;
		if (!sName.empty())
		{
			sAuthoringToolName = sModuleDirName + USTR("/") + sName;
		}
	}
	UString sCommand = sAuthoringToolName;
	for (int i = 1; i < argc; i++)
	{
		sCommand += Format(USTR(" \"%") PRIUS USTR("\""), argv[i]);
	}
	wstring sCommandW = UToW(sCommand);
	UString sLogFileName = sModuleDirName + USTR("/AuthoringTool.log.txt");
	fp = UFopen(sLogFileName.c_str(), USTR("ab"));
	if (fp != nullptr)
	{
		fseek(fp, 0, SEEK_END);
		u32 uLogSize = ftell(fp);
		if (uLogSize == 0)
		{
			fwrite("\xFF\xFE", 2, 1, fp);
		}
		if (!sCurrentWorkingDirName.empty())
		{
			wstring sCurrentWorkingDirNameW = UToW(sCurrentWorkingDirName);
#if SDW_PLATFORM == SDW_PLATFORM_WINDOWS
			fu16printf(fp, L"CD /D \"%ls\"\r\n", sCurrentWorkingDirNameW.c_str());
#else
			fu16printf(fp, L"cd \"%ls\"\r\n", sCurrentWorkingDirNameW.c_str());
#endif
		}
		fu16printf(fp, L"%ls\r\n\r\n", sCommandW.c_str());
		fclose(fp);
	}
	int nResult = 0;
#if SDW_PLATFORM == SDW_PLATFORM_WINDOWS
	nResult = _wsystem(sCommandW.c_str());
#else
	string sCommandA = UToA(sCommand);
	nResult = system(sCommandA.c_str());
#endif
	return nResult;
}
