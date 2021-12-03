/*----------------------------------------------------------------------
 | Name: ims-Functions.cpp (inventory-management-system-Functions.cpp)
 |
 | Desc: Holds all functions regarding the use of the Inventory 
 |		 Management System
----------------------------------------------------------------------*/

#include <iostream>
#include <windows.h>
#include <fstream>
#include <string.h>
#include <sstream>
#include "Functions.h"

using namespace std;

void clearText(HWND inputName, HWND inputDescription, HWND txtSerNum) {
	wchar_t nullify[1] = L"";

	SetWindowTextW(inputName, nullify);
	SetWindowTextW(inputDescription, nullify);
	SetWindowTextW(txtSerNum, nullify);
}