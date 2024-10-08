/* 
	uLister is a powerful document viewer which can open more than 500 file formats.
	Copyright (C) 2011 Egor Vlaznev 
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <windows.h>
#include "ulister.h"

extern HINSTANCE  hInst;
extern HANDLE     hViewerLibrary;
extern int        numInstances;
int keepinmemory;
wchar_t ininoloadtypes[1024];
wchar_t inionlyloadtypes[1024];
wchar_t ininopreviewtypes[1024];
wchar_t inionlypreviewtypes[1024];


///////////////////////////////////////////////////////////////////////////////////////////////////
VTWORD gettype(wchar_t* FileToLoad) {
    typedef VTDWORD (*FIInitFUNC)(VTVOID);
    typedef VTWORD  (*FIIdFileExFUNC)(VTDWORD, VTVOID *, VTDWORD, VTWORD *,VTLPTSTR, VTWORD);
    typedef VTDWORD  (*FIDeInitFUNC)(VTVOID);

    SCCERR  err=SCCERR_UNKNOWN;
    VTDWORD dwFlags = FIFLAG_NORMAL;
    VTWORD  wType=FI_UNKNOWN;
    VTBYTE  pTypeName[ 256 ];


    FIInitFUNC FIInit;
    FIIdFileExFUNC FIIdFileEx;
    FIDeInitFUNC FIDeInit;

    HINSTANCE hinstDLL;

    hinstDLL = loadlib(L"SCCFI.DLL");
    if(hinstDLL) {
        FIInit = (FIInitFUNC)GetProcAddress(hinstDLL, "FIInit");
        FIIdFileEx = (FIIdFileExFUNC)GetProcAddress(hinstDLL, "FIIdFileEx");
        FIDeInit = (FIDeInitFUNC)GetProcAddress(hinstDLL, "FIDeInit");
        if(FIInit&&FIIdFileEx&&FIDeInit) {
            FIInit();
            err=FIIdFileEx(IOTYPE_UNICODEPATH, FileToLoad, dwFlags, &wType, (VTLPTSTR)pTypeName, 256 );
            FIDeInit();
        }
        FreeLibrary(hinstDLL);
    } else return FI_UNKNOWN;

    if(err != SCCERR_OK) return FI_UNKNOWN;
    return 	wType;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int CheckFile(wchar_t* FileToLoad,wchar_t* onlyload,wchar_t* noload) {
    VTWORD  wType;
    wType=gettype(FileToLoad);

    wchar_t ftype[10];
    _itow_s(wType,ftype,10,10);
    if(wcslen(onlyload)>0) {
        if(wcsstr(onlyload,ftype))return 1;
        return NULL;
    }
    if(wcslen(noload)>0) {
        if(wcsstr(noload,ftype))return NULL;
    }

    return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void LoadFile(HWND hViewWnd,wchar_t* FileToLoad) {
    SCCVWVIEWFILE80  locViewFile;
    locViewFile.dwSize = sizeof(SCCVWVIEWFILE80);
    locViewFile.dwSpecType = IOTYPE_UNICODEPATH;
    locViewFile.pSpec = (VTVOID *)FileToLoad;
    locViewFile.dwViewAs = 0;
    locViewFile.bUseDisplayName = FALSE;
    locViewFile.bDeleteOnClose = FALSE;
    locViewFile.dwFlags = 0;
    locViewFile.dwReserved1 = 0;
    locViewFile.dwReserved2 = 0;
    SendMessage(hViewWnd,SCCVW_VIEWFILE,0,(LPARAM)&locViewFile);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int loadthisfile(LPARAM lParam) {
    PSCCVWVIEWTHISFILE40    locVTFPtr40;
    PSCCVWVIEWTHISFILE80    locVTFPtr80;
    locVTFPtr40 = (PSCCVWVIEWTHISFILE40)lParam;
    locVTFPtr80 = (PSCCVWVIEWTHISFILE80)lParam;

    if(GetFileAttributesA((char *)locVTFPtr40->sViewFile.pSpec)!=INVALID_FILE_ATTRIBUTES) {
        char compath[MAX_PATH]="";
        char comdir[MAX_PATH]="";
        GetEnvironmentVariable("COMMANDER_PATH",comdir,MAX_PATH);
        char tcinipath[MAX_PATH]="";
        GetEnvironmentVariable("COMMANDER_INI",tcinipath,MAX_PATH);
        if(comdir==NULL)return 1;
        strcat_s(compath,MAX_PATH,comdir);
#ifdef WIN64
        strcat_s(compath,MAX_PATH,"\\TOTALCMD64.EXE");
#else
		strcat_s(compath,MAX_PATH,"\\TOTALCMD.EXE");
#endif
        char allpar[MAX_PATH*2+15]="/S=L ";
        strcat_s(allpar,MAX_PATH*2+15,(char *)locVTFPtr40->sViewFile.pSpec);
        strcat_s(allpar,MAX_PATH*2+15," /I=\"");
        strcat_s(allpar,MAX_PATH*2+15,tcinipath);
        strcat_s(allpar,MAX_PATH*2+15,"\"");
        ShellExecuteA(NULL,"open",compath,allpar,NULL,SW_RESTORE);
    } 
	else
    if(GetFileAttributesW((wchar_t *)locVTFPtr80->sViewFile.pSpec)!=INVALID_FILE_ATTRIBUTES){
		MessageBoxW(0,(wchar_t *)locVTFPtr80->sViewFile.pSpec,0,0);
        wchar_t compath[MAX_PATH]=L"";
        wchar_t comdir[MAX_PATH]=L"";
        GetEnvironmentVariableW(L"COMMANDER_PATH",comdir,MAX_PATH);
        wchar_t tcinipath[MAX_PATH]=L"";
        GetEnvironmentVariableW(L"COMMANDER_INI",tcinipath,MAX_PATH);
        if(comdir==NULL)return 1;
        wcscat_s(compath,MAX_PATH,comdir);
#ifdef WIN64
        wcscat_s(compath,MAX_PATH,L"\\TOTALCMD64.EXE");
#else
		wcscat_s(compath,MAX_PATH,L"\\TOTALCMD.EXE");
#endif
        wchar_t allpar[MAX_PATH*2+15]=L"/S=L ";
        wcscat_s(allpar,MAX_PATH*2+15,(wchar_t *)locVTFPtr80->sViewFile.pSpec);
        wcscat_s(allpar,MAX_PATH*2+15,L" /I=\"");
        wcscat_s(allpar,MAX_PATH*2+15,tcinipath);
        wcscat_s(allpar,MAX_PATH*2+15,L"\"");
        ShellExecuteW(NULL,L"open",compath,allpar,NULL,SW_RESTORE);
    }
    return GetLastError();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
HBITMAP getpreview(wchar_t* FileToLoad,int width,int height) {

	if(!hViewerLibrary)hViewerLibrary=loadlib(L"SCCVW.DLL");
	if(!hViewerLibrary)return NULL;
	numInstances++;
	HWND hViewWnd = CreateWindow("SCCVIEWER",NULL,WS_CLIPCHILDREN,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,0,hInst,NULL);
	if(!IsWindow(hViewWnd)){
		numInstances--;
		return NULL;
	}
	LoadFile(hViewWnd,FileToLoad);

	HDC OutputDC,FormatDC;
	HBITMAP bitmap,oldbitmap;
	SCCVWDRAWPAGE41  locDrawPage;
	BITMAPINFOHEADER  locBIH;


	FormatDC=GetDC(hViewWnd);
	OutputDC=CreateCompatibleDC(FormatDC);

	locBIH.biSize = sizeof(BITMAPINFOHEADER);
	locBIH.biWidth = width;
	locBIH.biHeight = height;
	locBIH.biPlanes = 1;
	locBIH.biBitCount = 24;
	locBIH.biCompression = BI_RGB;
	locBIH.biSizeImage = 0;
	locBIH.biXPelsPerMeter = 0;
	locBIH.biYPelsPerMeter = 0;
	locBIH.biClrUsed = 0;
	locBIH.biClrImportant = 0;

	bitmap = CreateDIBitmap(FormatDC,&locBIH,0,NULL,NULL,DIB_RGB_COLORS);

	oldbitmap=(HBITMAP)SelectObject(OutputDC,bitmap);
	Rectangle(OutputDC,0,0,width,height);

	locDrawPage.dwSize = sizeof(SCCVWDRAWPAGE41);
	locDrawPage.dwPageToDraw = 0;
	locDrawPage.dwReserved = 0;
	locDrawPage.dwFlags = SCCVW_DPFLAG_DETERMINEOUTPUTTYPE;
	locDrawPage.lUnitsPerInch = 1440;
	locDrawPage.lFormatWidth = 12240;
	locDrawPage.lFormatHeight = 15840;
	locDrawPage.lTop = 0;
	locDrawPage.lLeft = 0;
	locDrawPage.lBottom = height;
	locDrawPage.lRight = width;
	locDrawPage.hOutputDC = OutputDC;
	locDrawPage.hFormatDC = FormatDC;
	SendMessage(hViewWnd,SCCVW_INITDRAWPAGE,0,0);
	SendMessage(hViewWnd,SCCVW_DRAWPAGE,0,(LPARAM)(PSCCVWDRAWPAGE41)&locDrawPage);
	SelectObject(OutputDC,oldbitmap);
	DeleteDC(OutputDC);
	ReleaseDC(hViewWnd,FormatDC);
	SendMessage(hViewWnd,SCCVW_DEINITDRAWPAGE,0,0);
	SendMessage(hViewWnd,SCCVW_CLOSEFILE,0,0L);
	DestroyWindow(hViewWnd);
	numInstances--;
	if((hViewerLibrary != NULL)&&(keepinmemory==0)&&(numInstances==0)) {
		FreeLibrary((HINSTANCE)hViewerLibrary);
		hViewerLibrary=NULL;
	}
	return bitmap;
}
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
///////////////////////////////////////////////////////////////////////////////////////////////////
wchar_t* getlibpath(wchar_t *libname,wchar_t *libpath) {
	/*GetModuleFileNameW(hInst, libpath, MAX_PATH);
	if(wcsrchr(libpath, L'\\'))*wcsrchr(libpath, L'\\') = L'\0';
	wcscat_s(libpath, MAX_PATH,L"\\");
	wcscat_s(libpath, MAX_PATH,libname);
	if(GetFileAttributesW(libpath)!=INVALID_FILE_ATTRIBUTES)return libpath;*/
	GetModuleFileNameW(hInst, libpath, MAX_PATH);
	if(wcsrchr(libpath, L'\\'))*wcsrchr(libpath, L'\\') = L'\0';
	#if !defined _M_X64 && !defined __amd64__
		wcscat_s(libpath, MAX_PATH,L"\\redist32\\");
	#else
		wcscat_s(libpath, MAX_PATH,L"\\redist64\\");
	#endif
	wcscat_s(libpath, MAX_PATH,libname);
	if(GetFileAttributesW(libpath)!=INVALID_FILE_ATTRIBUTES)return libpath;
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int libexist(wchar_t *libname) {
	wchar_t path[MAX_PATH];
	return(int)getlibpath(libname,path);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
HINSTANCE loadlib(wchar_t * libname) {
	HANDLE lib;
	wchar_t path[MAX_PATH];
	if(getlibpath(libname,path))lib=LoadLibraryExW(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	else {
		MessageBox(NULL,"Outside In dlls not found in plugin dir. \n See readme.txt, install section.","ERROR",MB_OK);
		return 0;
	}
	return(HINSTANCE)lib;
}




