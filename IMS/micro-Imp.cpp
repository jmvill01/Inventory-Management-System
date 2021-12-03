/*----------------------------------------------------------------------
 | Name: micro-Imp.cpp (microservice-Implementation.cpp)
 |
 | Microservice: Hash Function - Returns Identification Integer
 |
 | Desc: This File sends objects created to the microservice
 |		 and reads the corresponding hash value.
 |
 | Relevant Files: hashFunction.csv - contains microservice input
 |				   hashReturn.csv - contains microservice output
----------------------------------------------------------------------*/

#include <iostream>
#include <windows.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include "Functions.h"

using namespace std;

ifstream fileResult;
ofstream fileOutput;
STARTUPINFO startInfo = { 0 };
PROCESS_INFORMATION processInfo = { 0 };

//------------------------------------------------ Writes Object to File/Calls Microservice ------------------------------------------------
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

	bool success = CreateProcess(TEXT("C:/Users/jmvil/source/repos/my-creations/IMS/Debug/hash-Function.exe"),
				   NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &startInfo, &processInfo);

	if (success) {
		cout << "Process Successful" << endl;
	}
	else {
		cout << "Error in starting process" << endl;
	}
}

//------------------------------------------------ Reads/Returns Microservice Output ------------------------------------------------
int hashFunctionReturn() {
	fileResult.open("hashReturn.csv");

	if (fileResult.is_open()) {

		Sleep(100);
		string data;
		getline(fileResult, data);
		fileResult.close();
		int intData = stoi(data);

		return intData;
	}
	else {
		cout << "Could not open hashReturn.csv";
	}
}