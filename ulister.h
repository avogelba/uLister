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

#ifdef _AMD64_
#define WIN64
#endif
#include <sccvw.h>

struct ALLMYDATA
{
    WNDPROC ViewWindowProc;
    HWND ListerWindow;
    HWND oiWindow;
    HWND waWindow;
};

wchar_t* getlibpath(wchar_t *libname,wchar_t *libpath);
int libexist(wchar_t *libname);
void iniparse();
HINSTANCE loadlib(wchar_t * libname);
void zoom(HWND hWnd,int dir);
int loadthisfile(LPARAM lParam);
LRESULT CALLBACK ParentWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ViewWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateLister(HWND ParentWin);
VTWORD gettype(wchar_t* FileToLoad);
int CheckFile(wchar_t* FileToLoad,wchar_t* onlyload,wchar_t* noload);
HBITMAP getpreview(wchar_t* FileToLoad,int width,int height);
void LoadFile(HWND hViewWnd,wchar_t* FileToLoad);


