#include <iostream>
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <list>
#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include <sstream>
#include "Functions.h"

using namespace std;

//-------------------------- Defines Events --------------------------
#define SBUTTON 1
#define CBUTTON 2
#define LB_INV 3
#define ABUTTON 4
#define DBUTTON 5
#define EBUTTON 6
#define DELCONBUTTON 7
#define SAVECBUTTON 8

//-------------------------- Control Variables --------------------------
HWND txtSerNum;
HWND lblIms;
HWND lblSerNum;
HWND changeBtn;
HWND saveBtn;
HWND cancelBtn;
HWND deleteBtn;
HWND txtObjectDesc;
HWND txtObjectName;
HWND iListBox;

const int STORAGE_MAX = 1000;
int listIndex = 0;
//-------------------------- Inventory Item/List --------------------------
struct Inventory {
	WCHAR objName[50];
	WCHAR objDesc[1000];
	int serialNumber;
};

Inventory inventoryList[STORAGE_MAX];

//-------------------------- Function Identifiers --------------------------

void displayButtons(string btnClicked);

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void AddControls(HWND);


int main(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

	WNDCLASSW wc = { 0 };
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"IMS";
	wc.lpfnWndProc = WindowProcedure;

	if (!RegisterClassW(&wc))
		return -1;

	CreateWindowW(L"IMS", L"IMS Window", WS_OVERLAPPEDWINDOW | 
		WS_VISIBLE, 100, 100, 1300, 675, NULL, NULL, NULL, NULL);

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {

	case WM_CREATE:
		AddControls(hWnd);
		break;

	case WM_COMMAND: //-------------------------- Events --------------------------
		switch (LOWORD(wp)) {

		//-------------------------- Add Button Click --------------------------
		case ABUTTON: { 
			clearText(txtObjectName, txtObjectDesc, txtSerNum);
			displayButtons("add");
			break;
		}
		
		//-------------------------- Save Button Click --------------------------
		case SBUTTON: {	
			Inventory invItem;
			GetWindowText(txtObjectName, invItem.objName, 50);
			GetWindowText(txtObjectDesc, invItem.objDesc, 1000);
			writeToFile(invItem.objName);
			invItem.serialNumber = hashFunctionReturn();
			inventoryList[listIndex] = invItem;
			LRESULT lResult = SendMessageW(iListBox, LB_ADDSTRING, NULL, 
				(LPARAM)inventoryList[listIndex].objName);
			SendMessageW(iListBox, LB_SETITEMDATA, lResult, listIndex);
			clearText(txtObjectName, txtObjectDesc, txtSerNum);
			listIndex++;
			break;
		}

		//-------------------------- Cancel Button Click --------------------------
		case CBUTTON: { 
			clearText(txtObjectName, txtObjectDesc, txtSerNum);
			break;
		}

		//-------------------------- List Box Select Event --------------------------
		case LB_INV: { 
			switch (HIWORD(wp)) {

			case LBN_SELCHANGE: {
	
				int indexItem = (int)SendMessageW(iListBox, LB_GETCURSEL, 0, 0);
				string strSerNum = to_string(inventoryList[indexItem].serialNumber);
				
				const size_t newsizew = strlen(strSerNum.c_str()) + 1;
				size_t convertedChars = 0;
				wchar_t* wcstring = new wchar_t[newsizew];
				mbstowcs_s(&convertedChars, wcstring, newsizew, strSerNum.c_str(), _TRUNCATE);

				SetWindowTextW(txtObjectName, inventoryList[indexItem].objName);
				SetWindowTextW(txtObjectDesc, inventoryList[indexItem].objDesc);
				SetWindowTextW(txtSerNum, wcstring);
				break;
			}
				break;
			}
			break;
		}
		
		//-------------------------- Delete Button Click --------------------------
		case DBUTTON: { 
			clearText(txtObjectName, txtObjectDesc, txtSerNum);
			displayButtons("delete");
			break;
		}
		
		//---------------- Remove Item from List and ListBox ---------------- 
		case DELCONBUTTON: { 
			int answer;
			int indexItem = (int)SendMessageW(iListBox, LB_GETCURSEL, 0, 0);
			WCHAR objName[50];
			GetWindowText(txtObjectName, objName, 50);
			answer = MessageBoxW(hWnd, L"Would you like to delete this item ? ", L"Warning", MB_YESNO | MB_ICONQUESTION);

			if (answer == IDYES) {
				if (indexItem == STORAGE_MAX - 1) {
					inventoryList[indexItem] = {};
				}
				else {
					for (int i = 0; i < STORAGE_MAX - 1; i++) {
						inventoryList[indexItem] = inventoryList[indexItem + 1];
					}
					inventoryList[STORAGE_MAX - 1] = {};
				}
				LRESULT lResult = SendMessageW(iListBox, LB_DELETESTRING, indexItem, NULL);
				listIndex--;
				clearText(txtObjectName, txtObjectDesc, txtSerNum);
			}
			break;
		}

		//-------------------------- Save Change Button Click --------------------------
		case SAVECBUTTON: { 
			Inventory invItem;
			int indexItem = (int)SendMessageW(iListBox, LB_GETCURSEL, 0, 0);
			GetWindowText(txtObjectName, invItem.objName, 50);
			GetWindowText(txtObjectDesc, invItem.objDesc, 1000);
			invItem.serialNumber = inventoryList[indexItem].serialNumber;
			inventoryList[indexItem] = invItem;
			LRESULT lResult = SendMessageW(iListBox, LB_RESETCONTENT, NULL, NULL);
			
			int index = 0;
			while (wcscmp(inventoryList[index].objName, L"")) {
				LRESULT lResult = SendMessageW(iListBox, LB_ADDSTRING, NULL, (LPARAM)inventoryList[index].objName);
				SendMessageW(iListBox, LB_SETITEMDATA, lResult, index);
				index++;
			}

			clearText(txtObjectName, txtObjectDesc, txtSerNum);
			break;
		}

		//-------------------------- Edit Button Click --------------------------
		case EBUTTON: { 
			clearText(txtObjectName, txtObjectDesc, txtSerNum);
			displayButtons("edit");
			break;
			}
		}

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
}

void AddControls(HWND hWnd) { //-------------------------- GUI Controls/Components --------------------------
	lblIms = CreateWindowW(L"static", L"Inventory Management System", WS_VISIBLE | WS_CHILD, 100, 25, 100, 50, hWnd, NULL, NULL, NULL);

	//------------------------------------------------ Add/Delete/Edit ------------------------------------------------
	CreateWindowW(L"Button", L"Add", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 100, 100, 50, hWnd, (HMENU)ABUTTON, NULL, NULL);
	CreateWindowW(L"Button", L"Delete", WS_VISIBLE | WS_CHILD | WS_BORDER, 250, 100, 100, 50, hWnd, (HMENU)DBUTTON, NULL, NULL);
	CreateWindowW(L"Button", L"Edit", WS_VISIBLE | WS_CHILD | WS_BORDER, 400, 100, 100, 50, hWnd, (HMENU)EBUTTON, NULL, NULL);

	//------------------------------------------------ Inventory Box ------------------------------------------------
	CreateWindowW(L"static", L"Inventory:", WS_VISIBLE | WS_CHILD, 100, 200, 100, 50, hWnd, NULL, NULL, NULL);
	iListBox = CreateWindowW(WC_LISTBOX, L"viewList", WS_VISIBLE | WS_CHILD | WS_BORDER |
		LBS_NOTIFY, 100, 225, 680, 400, hWnd, (HMENU)LB_INV, NULL, NULL);

	//------------------------------------------------ Description Box ------------------------------------------------
	CreateWindowW(L"static", L"Description:", WS_VISIBLE | WS_CHILD, 800, 200, 100, 50, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"static", L"Object Name:", WS_VISIBLE | WS_CHILD, 800, 250, 100, 50, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"static", L"Object Description:", WS_VISIBLE | WS_CHILD, 800, 300, 100, 50, hWnd, NULL, NULL, NULL);
	lblSerNum = CreateWindowW(L"static", L"ID Number:", WS_CHILD, 800, 475, 100, 50, hWnd, NULL, NULL, NULL);
	txtSerNum = CreateWindowW(L"edit", L"", WS_CHILD, 900, 475, 100, 20, hWnd, NULL, NULL, NULL);
	txtObjectName = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE |
		ES_AUTOVSCROLL | ES_AUTOHSCROLL, 900, 250, 200, 20, hWnd, NULL, NULL, NULL);
	txtObjectDesc = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE |
		ES_AUTOVSCROLL, 900, 300, 300, 150, hWnd, NULL, NULL, NULL);

	//------------------------------------------------ Description Box Buttons ------------------------------------------------
	saveBtn = CreateWindowW(L"Button", L"Save", WS_CHILD | WS_BORDER, 800, 550, 100, 50, hWnd, (HMENU)SBUTTON, NULL, NULL);
	changeBtn = CreateWindowW(L"Button", L"Save Change", WS_CHILD | WS_BORDER, 800, 550, 100, 50, hWnd, (HMENU)SAVECBUTTON, NULL, NULL);
	cancelBtn = CreateWindowW(L"Button", L"Cancel", WS_CHILD | WS_BORDER, 1100, 550, 100, 50, hWnd, (HMENU)CBUTTON, NULL, NULL);
	deleteBtn = CreateWindowW(L"Button", L"Delete", WS_CHILD | WS_BORDER, 1100, 550, 100, 50, hWnd, (HMENU)DELCONBUTTON, NULL, NULL);
}

void displayButtons(string btnClicked) {
	if (btnClicked == "add") {
		ShowWindow(cancelBtn, 5);
		ShowWindow(saveBtn, 5);
		ShowWindow(deleteBtn, 0);
		ShowWindow(changeBtn, 0);
		ShowWindow(txtSerNum, 0);
		ShowWindow(lblSerNum, 0);
	}
	else if (btnClicked == "delete") {
		ShowWindow(cancelBtn, 0);
		ShowWindow(saveBtn, 0);
		ShowWindow(deleteBtn, 5);
		ShowWindow(changeBtn, 0);
		ShowWindow(txtSerNum, 0);
		ShowWindow(lblSerNum, 0);
	}
	else {
		ShowWindow(cancelBtn, 5);
		ShowWindow(saveBtn, 0);
		ShowWindow(deleteBtn, 0);
		ShowWindow(changeBtn, 5);
		ShowWindow(txtSerNum, 5);
		ShowWindow(lblSerNum, 5);
	}
}
