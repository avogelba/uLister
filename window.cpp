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
#include <CommCtrl.h>
#include <float.h>
#include <lomenu.h>
#include "ulister.h"

extern HINSTANCE  hInst;
extern HANDLE     hViewerLibrary;
extern int        numInstances;

///////////////////////////////////////////////////////////////////////////////////////////////////
void zoom(HWND hWnd,int dir) {
    SCCVWOPTIONSPEC40 locOptionSpec;
    VTDWORD zoom;
    locOptionSpec.dwSize=sizeof(SCCVWOPTIONSPEC40);
    locOptionSpec.dwId=SCCID_BMPZOOMEVENT;
    locOptionSpec.dwFlags=SCCVWOPTION_CURRENT;
    zoom=(dir>0)?SCCVW_ZOOM_IN:SCCVW_ZOOM_OUT;
    locOptionSpec.pData=&zoom;
    SendMessage(hWnd,SCCVW_SETOPTION,0,(LPARAM)&locOptionSpec);
    locOptionSpec.dwId=SCCID_VECZOOMEVENT;
    SendMessage(hWnd,SCCVW_SETOPTION,0,(LPARAM)&locOptionSpec);
    locOptionSpec.dwId = SCCID_FONTSCALINGFACTOR;
    locOptionSpec.pData = &zoom;
    SendMessage(hWnd,SCCVW_GETOPTION,0,(LPARAM)(PSCCVWOPTIONSPEC40)&locOptionSpec);
    zoom=(dir>0)?zoom*10/8:zoom*8/10;
    SendMessage(hWnd,SCCVW_SETOPTION,0,(LPARAM)&locOptionSpec);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK ParentWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	_control87(MCW_EM, MCW_EM);
    ALLMYDATA *mydata;
    mydata=(ALLMYDATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if(mydata)
    switch(message) {
		case WM_SETFOCUS:
		   SetFocus(mydata->oiWindow);
			break;
		case WM_SIZE:
			InvalidateRect(hWnd,NULL,0);
			if(!mydata)break;
			if(IsWindow(mydata->oiWindow)) {
				MoveWindow(mydata->oiWindow,0,0,LOWORD(lParam),HIWORD(lParam),true);
				ShowWindow(mydata->oiWindow,SW_SHOW);
			}
			break;
		case SCCVW_VIEWTHISFILE:
			return(loadthisfile(lParam)==0)?SCCVWERR_MESSAGEHANDLED:0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam); 
}
///////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK ViewWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	_control87(MCW_EM, MCW_EM);
    ALLMYDATA *mydata;
    mydata=(ALLMYDATA *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if(mydata){
		switch(message) {
			case SCCVW_VIEWTHISFILE:
				return(loadthisfile(lParam)==0)?SCCVWERR_MESSAGEHANDLED:0;
			case SCCVW_KEYDOWN:			
				PostMessage(mydata->ListerWindow, WM_KEYDOWN, lParam, 0);
				if((GetKeyState(VK_CONTROL)<0)&&((lParam==VK_OEM_PLUS)||(lParam==VK_ADD)))	zoom(hWnd,1);			
				if((GetKeyState(VK_CONTROL)<0)&&((lParam==VK_OEM_MINUS)||(lParam==VK_SUBTRACT)))zoom(hWnd,-1);
				break;
			case WM_MOUSEHWHEEL:
				PostMessage(hWnd, SCCVW_HSCROLL,(GET_WHEEL_DELTA_WPARAM(wParam)>0)?SCCSB_LINERIGHT:SCCSB_LINELEFT, 0);
				return 0;
		}
		return CallWindowProc(mydata->ViewWindowProc, hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam); 
}
///////////////////////////////////////////////////////////////////////////////////////////////////
HWND CreateLister(HWND ParentWin) {
    HWND        hViewWnd,waWnd;
    RECT r;
    WNDCLASS wc; 
    bool first=false;
    ALLMYDATA *mydata;
    mydata=new ALLMYDATA();
    numInstances++;
    if(!hViewerLibrary)hViewerLibrary=loadlib(L"SCCVW.DLL");
    if(!hViewerLibrary)return NULL;
    mydata->ListerWindow=ParentWin;
    bool quickview=WS_CHILD&GetWindowLongPtr(ParentWin, GWL_STYLE);
    wc.style =CS_HREDRAW | CS_VREDRAW; 
    wc.lpfnWndProc = (WNDPROC) ParentWindowProc; 
    wc.cbClsExtra = 0; 
    wc.cbWndExtra = 0; 
    wc.hInstance = hInst; 
    wc.hIcon = NULL; 
    wc.hCursor = NULL; 
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  NULL; 
    wc.lpszClassName = "WAwc"; 
    RegisterClass(&wc);
    GetClientRect(ParentWin,&r);
    waWnd = CreateWindow("WAwc",NULL,WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,r.left,r.top,r.right-r.left,r.bottom-r.top,ParentWin,0,hInst,NULL);	
    mydata->waWindow=waWnd;
    GetClientRect(waWnd,&r);
	hViewWnd = CreateWindow("SCCVIEWER",NULL,WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,r.left,r.top,r.right-r.left,r.bottom-r.top,waWnd,0,hInst,NULL);   
    mydata->oiWindow=hViewWnd;
    if(!IsWindow(hViewWnd))return NULL;
    mydata->ViewWindowProc = (WNDPROC)SetWindowLongPtr(hViewWnd,GWLP_WNDPROC, (LONG_PTR)ViewWindowProc);
	SetWindowLongPtr(hViewWnd, GWLP_USERDATA,(long) mydata);
	SetWindowLongPtr(waWnd, GWLP_USERDATA,(long) mydata);
    if(!quickview)SetFocus(hViewWnd);
    return waWnd;
}
