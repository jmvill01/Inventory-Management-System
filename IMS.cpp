#include <iostream>
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <list>
#include <fstream>
#include <string>

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
#define WM_SETFONT                      0x0030

//-------------------------- Control Variables --------------------------
ifstream fileResult;
ofstream fileOutput;
HWND lblSerNum;
HWND lblIms;
HWND changeBtn;
HWND saveBtn;
HWND cancelBtn;
HWND deleteBtn;
HWND txtObjectDesc;
HWND txtObjectName;
HWND iListBox;

int listIndex = 0;
const int STORAGE_MAX = 1000;
STARTUPINFO startInfo = { 0 };
PROCESS_INFORMATION processInfo = { 0 };

//-------------------------- Inventory Item/List --------------------------
struct Inventory {
	WCHAR objName[50];
	WCHAR objDesc[1000];
	string serialNumber;
};

Inventory inventoryList[STORAGE_MAX];

//-------------------------- Function Identifiers --------------------------
string hashFunctionReturn();

void writeToFile(WCHAR[50]);

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

	CreateWindowW(L"IMS", L"IMS Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 1800, 1080, NULL, NULL, NULL, NULL);

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	wchar_t text[1] = L"";

	switch (msg) {

	case WM_CREATE:
		AddControls(hWnd);
		break;

	case WM_COMMAND: //-------------------------- Events --------------------------

		switch (LOWORD(wp)) {

		case ABUTTON: { //-------------------------- Add Button Click --------------------------
			SetWindowTextW(txtObjectName, text);
			SetWindowTextW(txtObjectDesc, text);
			ShowWindow(cancelBtn, 5);
			ShowWindow(saveBtn, 5);
			ShowWindow(deleteBtn, 0);
			ShowWindow(changeBtn, 0);
			break;
		}

		case SBUTTON: {	//-------------------------- Save Button Click --------------------------
			Inventory invItem;

			GetWindowText(txtObjectName, invItem.objName, 50);
			GetWindowText(txtObjectDesc, invItem.objDesc, 1000);
			inventoryList[listIndex] = invItem;
			LRESULT lResult = SendMessageW(iListBox, LB_ADDSTRING, NULL, (LPARAM)inventoryList[listIndex].objName);
			SendMessageW(iListBox, LB_SETITEMDATA, lResult, listIndex);

			writeToFile(invItem.objName);
			invItem.serialNumber = hashFunctionReturn();

			SetWindowTextW(txtObjectName, text);
			SetWindowTextW(txtObjectDesc, text);
			listIndex++;
			SetFocus(iListBox);
			break;
		}	 //-------------------------- Inserts Object Into ListBox --------------------------

		case CBUTTON: { //-------------------------- Cancel Button Click --------------------------
			SetWindowTextW(txtObjectName, text);
			SetWindowTextW(txtObjectDesc, text);
			SetFocus(iListBox);
			break;
		}

		case LB_INV: { //-------------------------- List Box Select Event --------------------------
			switch (HIWORD(wp)) {

			case LBN_SELCHANGE: {

				int indexItem = (int)SendMessageW(iListBox, LB_GETCURSEL, 0, 0);

				//WCHAR i = (WCHAR)SendMessageW(iListBox, LB_GETITEMDATA, indexItem, 0);

				SetWindowTextW(txtObjectName, inventoryList[indexItem].objName);
				SetWindowTextW(txtObjectDesc, inventoryList[indexItem].objDesc);

				break;
			}
							  break;
			}
			break;
		}

		case DBUTTON: { //-------------------------- Delete Button Click --------------------------
			SetWindowTextW(txtObjectName, text);
			SetWindowTextW(txtObjectDesc, text);
			ShowWindow(cancelBtn, 0);
			ShowWindow(saveBtn, 0);
			ShowWindow(deleteBtn, 5);
			ShowWindow(changeBtn, 0);
			break;
		}

		case DELCONBUTTON: { //---------------- Remove Item from List and ListBox ---------------- 

			int answer;
			int indexItem = (int)SendMessageW(iListBox, LB_GETCURSEL, 0, 0);

			//---------------- Add Message Box Functional ---------------- 
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
				SetWindowTextW(txtObjectName, text);
				SetWindowTextW(txtObjectDesc, text);
			}
			break;
		}

		case SAVECBUTTON: { //-------------------------- Save Change Button Click --------------------------
			Inventory invItem;
			int indexItem = (int)SendMessageW(iListBox, LB_GETCURSEL, 0, 0);
			GetWindowText(txtObjectName, invItem.objName, 50);
			GetWindowText(txtObjectDesc, invItem.objDesc, 1000);
			inventoryList[indexItem] = invItem;

			SendMessageW(iListBox, LB_SETITEMDATA, indexItem, (LPARAM)invItem.objName);
			SendMessageW(iListBox, LB_SETITEMDATA, indexItem, (LPARAM)invItem.objDesc);

			break;
		}

		case EBUTTON: { //-------------------------- Edit Button Click --------------------------
			SetWindowTextW(txtObjectName, text);
			SetWindowTextW(txtObjectDesc, text);
			ShowWindow(cancelBtn, 5);
			ShowWindow(saveBtn, 0);
			ShowWindow(deleteBtn, 0);
			ShowWindow(changeBtn, 5);
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

void AddControls(HWND hWnd) { //-------------------------- GUI Controls --------------------------
	lblIms = CreateWindowW(L"static", L"Inventory Management System", WS_VISIBLE | WS_CHILD | SS_CENTER, 750, 100, 100, 50, hWnd, NULL, NULL, NULL);
	/*font = CreateFontA(48, 0, 0, 0, 700, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, NULL, CLEARTYPE_QUALITY, FF_DONTCARE, "Arial");
	SetDlgItemTextW(lblIms, font, L"Inventory Management System");*/

	//------------------------------------------------ Add/Delete/Edit ------------------------------------------------
	CreateWindowW(L"Button", L"Add", WS_VISIBLE | WS_CHILD | WS_BORDER, 300, 300, 100, 50, hWnd, (HMENU)ABUTTON, NULL, NULL);
	CreateWindowW(L"Button", L"Delete", WS_VISIBLE | WS_CHILD | WS_BORDER, 450, 300, 100, 50, hWnd, (HMENU)DBUTTON, NULL, NULL);
	CreateWindowW(L"Button", L"Edit", WS_VISIBLE | WS_CHILD | WS_BORDER, 600, 300, 100, 50, hWnd, (HMENU)EBUTTON, NULL, NULL);

	//------------------------------------------------ Inventory Box ------------------------------------------------
	CreateWindowW(L"static", L"Inventory:", WS_VISIBLE | WS_CHILD, 300, 400, 100, 50, hWnd, NULL, NULL, NULL);
	iListBox = CreateWindowW(WC_LISTBOX, L"viewList", WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY, 300, 425, 880, 550, hWnd, (HMENU)LB_INV, NULL, NULL);

	//------------------------------------------------ Description Box ------------------------------------------------
	CreateWindowW(L"static", L"Description:", WS_VISIBLE | WS_CHILD, 1200, 400, 100, 50, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"static", L"Object Name:", WS_VISIBLE | WS_CHILD, 1200, 450, 100, 50, hWnd, NULL, NULL, NULL);
	CreateWindowW(L"static", L"Object Description:", WS_VISIBLE | WS_CHILD, 1200, 500, 100, 50, hWnd, NULL, NULL, NULL);
	lblSerNum = CreateWindowW(L"static", L"", WS_VISIBLE | WS_CHILD, 1200, 550, 100, 50, hWnd, NULL, NULL, NULL);
	txtObjectName = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 1300, 450, 200, 20, hWnd, NULL, NULL, NULL);
	txtObjectDesc = CreateWindowW(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 1300, 500, 300, 150, hWnd, NULL, NULL, NULL);

	//------------------------------------------------ Description Box Buttons ------------------------------------------------
	saveBtn = CreateWindowW(L"Button", L"Save", WS_CHILD | WS_BORDER, 1200, 900, 100, 50, hWnd, (HMENU)SBUTTON, NULL, NULL);
	changeBtn = CreateWindowW(L"Button", L"Save Change", WS_CHILD | WS_BORDER, 1200, 900, 100, 50, hWnd, (HMENU)SAVECBUTTON, NULL, NULL);
	cancelBtn = CreateWindowW(L"Button", L"Cancel", WS_CHILD | WS_BORDER, 1600, 900, 100, 50, hWnd, (HMENU)CBUTTON, NULL, NULL);
	deleteBtn = CreateWindowW(L"Button", L"Delete", WS_CHILD | WS_BORDER, 1600, 900, 100, 50, hWnd, (HMENU)DELCONBUTTON, NULL, NULL);

}

void writeToFile(WCHAR input[50]) {
	wstring objectName(input);
	string stringInput(objectName.begin(), objectName.end());
	fileOutput.open("hashFunction.csv", ios::out);

	if (fileOutput.is_open()) {

		fileOutput << stringInput;
		cout << stringInput << endl;
		fileOutput.close();
	}
	else {
		cout << "Error, could not open file." << endl;
	}

	bool success = CreateProcess(TEXT("C:/Users/jmvil/source/repos/my-creations/IMS/Debug/hash-Function.exe"), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &startInfo, &processInfo);
	if (success) {
		cout << "Process Success" << endl;
	}
	else {
		cout << "Error in starting process" << endl;
	}
	
}

string hashFunctionReturn() {
	fileResult.open("hashReturn.csv");

	if (fileResult.is_open()) {
		string data;
		getline(fileResult, data);
		//cout << data << endl;
		fileResult.close();

		return data;
	}
}

