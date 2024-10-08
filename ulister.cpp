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
#include <stdio.h>
#include "ulister.h"
#include "total.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
HINSTANCE  hInst;
HANDLE     hViewerLibrary;
int        numInstances;
extern int keepinmemory;
extern wchar_t ininoloadtypes[1024];
extern wchar_t inionlyloadtypes[1024];
extern wchar_t ininopreviewtypes[1024];
extern wchar_t inionlypreviewtypes[1024];

///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HINSTANCE hinst, unsigned long reason, void* lpReserved) {
    switch(reason) {
		case DLL_PROCESS_ATTACH:
			hInst=hinst;
			hViewerLibrary=NULL;
			numInstances=0;
			iniparse();
			break;
		case DLL_PROCESS_DETACH:
			if(hViewerLibrary)FreeLibrary((HINSTANCE)hViewerLibrary);
			hViewerLibrary=NULL;
			break;
    }
    return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) HWND __stdcall ListLoadW(HWND ParentWin,wchar_t* FileToLoad,int ShowFlags) {
    HWND        hViewWnd;
    if(!CheckFile(FileToLoad,inionlyloadtypes,ininoloadtypes))return NULL;
    hViewWnd=CreateLister(ParentWin);
    if(!IsWindow(hViewWnd))return NULL;
    ALLMYDATA *mydata;
    mydata=(ALLMYDATA *)GetWindowLongPtr(hViewWnd, GWLP_USERDATA);
	if(mydata)LoadFile(mydata->oiWindow,FileToLoad);
    return hViewWnd;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) HWND __stdcall ListLoad(HWND ParentWin,char* FileToLoad,int ShowFlags) {
	wchar_t path[MAX_PATH]=L"";
	MultiByteToWideChar(CP_ACP,0,FileToLoad,-1,path,MAX_PATH);	
	return ListLoadW(ParentWin,path,ShowFlags);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) int __stdcall ListLoadNextW(HWND ParentWin,HWND ListWin,wchar_t* FileToLoad,int ShowFlags) {
    if(!CheckFile(FileToLoad,inionlyloadtypes,ininoloadtypes))return LISTPLUGIN_ERROR;
    ALLMYDATA *mydata;
    mydata=(ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if(mydata)LoadFile(mydata->oiWindow,FileToLoad);
    return LISTPLUGIN_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) int __stdcall ListLoadNext(HWND ParentWin,HWND ListWin,char* FileToLoad,int ShowFlags) {
	wchar_t path[MAX_PATH]=L"";
	MultiByteToWideChar(CP_ACP,0,FileToLoad,-1,path,MAX_PATH);	
	ListLoadNextW(ParentWin,ListWin,path,ShowFlags);
	return LISTPLUGIN_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)void __stdcall ListCloseWindow(HWND ListWin) {
    if(IsWindow(ListWin)) {
        ALLMYDATA *mydata;
        mydata=(ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
		if(mydata){
			SendMessage(mydata->oiWindow,SCCVW_SAVEOPTIONS,0,0L);
			SendMessage(mydata->oiWindow,SCCVW_CLOSEFILE,0,0L);
			DestroyWindow(mydata->oiWindow);
			DestroyWindow(mydata->waWindow);
			numInstances--;
			if((hViewerLibrary != NULL)&&(keepinmemory==0)&&(numInstances==0)) {
				FreeLibrary((HINSTANCE)hViewerLibrary);
				hViewerLibrary=NULL;
			}
			delete mydata;
		}
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListSearchText(HWND ListWin,char* SearchString,int SearchParameter) {
    ALLMYDATA *mydata;
    mydata=(ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if(mydata){

		if(SearchParameter & lcs_findfirst) {

			SCCVWSEARCHINFO40  locSearchInfo;

			locSearchInfo.dwSize = sizeof(SCCVWSEARCHINFO40);
			strncpy(locSearchInfo.siText,SearchString,79);
			locSearchInfo.siTextLen = strlen(locSearchInfo.siText);

			locSearchInfo.siType =(SearchParameter & lcs_matchcase)? SCCVW_SEARCHCASE:SCCVW_SEARCHNOCASE;
			locSearchInfo.siFrom = SCCVW_SEARCHCURRENT;
			locSearchInfo.siDirection = (SearchParameter & lcs_backwards)?SCCVW_SEARCHBACK:SCCVW_SEARCHFORWARD;

			if(SendMessage(mydata->oiWindow,SCCVW_SEARCH,0,(LPARAM)(PSCCVWSEARCHINFO40)&locSearchInfo)!=0)
				MessageBox(mydata->oiWindow, SearchString, "Not found:", MB_OK);
		} else
		if(SearchParameter & lcs_backwards) {
				if(SendMessage(mydata->oiWindow,SCCVW_SEARCHNEXT,SCCVW_SEARCHBACK,0)!=0)
					MessageBox(mydata->oiWindow, SearchString, "Not found:", MB_OK);
		} else
			if(SendMessage(mydata->oiWindow,SCCVW_SEARCHNEXT,SCCVW_SEARCHFORWARD,0)!=0)
				MessageBox(mydata->oiWindow, SearchString, "Not found:", MB_OK);
	}
    return LISTPLUGIN_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListPrint(HWND ListWin,char* FileToPrint,char* DefPrinter,int PrintFlags,RECT* Margins) {
    ALLMYDATA *mydata;
    mydata=(ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if(mydata)
    SendMessage(mydata->oiWindow,SCCVW_PRINT,0,0);
    return LISTPLUGIN_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)int __stdcall ListSendCommand(HWND ListWin,int Command,int Parameter) {
    ALLMYDATA *mydata;
    mydata=(ALLMYDATA *)GetWindowLongPtr(ListWin, GWLP_USERDATA);
	if(mydata)
    switch(Command) {
		case lc_copy:
			SendMessage(mydata->oiWindow,SCCVW_COPYTOCLIP,0,0);
			return LISTPLUGIN_OK;
		case lc_selectall:
			SendMessage(mydata->oiWindow,SCCVW_SELECTALL,0,0);
			return LISTPLUGIN_OK;
    }
    return LISTPLUGIN_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)HBITMAP __stdcall ListGetPreviewBitmapW(wchar_t* FileToLoad,int width,int height,char* contentbuf,int contentbuflen) {
    if(!CheckFile(FileToLoad,inionlypreviewtypes,ininopreviewtypes))return NULL;    
    HBITMAP bitmap=getpreview(FileToLoad,width,height);
    return bitmap;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport)HBITMAP __stdcall ListGetPreviewBitmap(char* FileToLoad,int width,int height,char* contentbuf,int contentbuflen) {
	wchar_t path[MAX_PATH]=L"";
	MultiByteToWideChar(CP_ACP,0,FileToLoad,-1,path,MAX_PATH);	
	return ListGetPreviewBitmapW(path,width,height,contentbuf,contentbuflen);
}
