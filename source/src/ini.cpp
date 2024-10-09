#include <windows.h>
#include <stdio.h>

extern HINSTANCE  hInst;

int keepinmemory;
wchar_t ininoloadtypes[1024];
wchar_t inionlyloadtypes[1024];
wchar_t ininopreviewtypes[1024];
wchar_t inionlypreviewtypes[1024];


///////////////////////////////////////////////////////////////////////////////////////////////////
void iniparse() {
	wchar_t inioptdir[MAX_PATH]=L"";
	wchar_t inipath[MAX_PATH]=L"";
	wchar_t posdir[MAX_PATH]=L"";
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(hInst, path, MAX_PATH);
    if(wcsrchr(path, L'\\'))*wcsrchr(path, L'\\') = L'\0';
    wcscat_s(path,MAX_PATH,L"\\ulister.ini");

    if(GetFileAttributesW(path)==INVALID_FILE_ATTRIBUTES) {
        GetEnvironmentVariableW(L"COMMANDER_INI",path,MAX_PATH);
        if(wcsrchr(path, L'\\'))*wcsrchr(path, L'\\') = L'\0';
        wcscat_s(path,MAX_PATH,L"\\ulister.ini");
    }
    if(GetFileAttributesW(path)==INVALID_FILE_ATTRIBUTES) {
        GetEnvironmentVariableW(L"APPDATA",path,MAX_PATH);
        wcscat_s(path,MAX_PATH,L"\\ulister.ini");
    }
    wcscpy_s(inipath,MAX_PATH,path);

	wchar_t buf[1024]=L"";

	GetPrivateProfileStringW(L"uLister",L"keepinmemory",L"1",buf,1024,inipath);
	if(wcscmp(buf,L"1")==0)keepinmemory=1;else keepinmemory=0;
	GetPrivateProfileStringW(L"uLister",L"optionsdir",L"",inioptdir,1024,inipath);
	
	GetPrivateProfileStringW(L"uLister",L"noloadtypes",L"",ininoloadtypes,1024,inipath);
	GetPrivateProfileStringW(L"uLister",L"onlyloadtypes",L"",inionlyloadtypes,1024,inipath);
	GetPrivateProfileStringW(L"uLister",L"nopreviewtypes",L"",ininopreviewtypes,1024,inipath);
	GetPrivateProfileStringW(L"uLister",L"onlypreviewtypes",L"",inionlypreviewtypes,1024,inipath);

    if(wcslen(inioptdir)>0) {
        ExpandEnvironmentStringsW(inioptdir,posdir,MAX_PATH);
        SetEnvironmentVariableW(L"OIT_DATA_PATH",posdir);
    }
}
