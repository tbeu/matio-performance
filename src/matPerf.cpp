#include <iostream>
#include <windows.h>
#include <ctime>
#include <string>
#include "mat.h"
#include "matio.h"

using namespace std;

typedef void (*ReadMatFile_Proc)(const char* file, bool bInfoOnly);

static void ReadMatFileMATLAB(const char* file, bool bInfoOnly)
{
	MATFile *pmat;
	mxArray *pa;
	const char *name;
	int varCnt = 0;

	cout << "Try to read all variables " << (bInfoOnly ? "info in: " : "in: ") << file << endl;

	pmat = matOpen(file, "r");
	if (pmat == NULL) 
	{
		cout << "Failed to open!" << endl;
		return;
	}

	while ((pa = bInfoOnly ? matGetNextVariableInfo(pmat, &name) : matGetNextVariable(pmat, &name)) != NULL) 
	{
		varCnt++;
		mxDestroyArray(pa);
	}

	if (matClose(pmat) != 0) 
	{
		cout << "Failed to close! " << endl;
		return;
	}

	cout << varCnt << " variables found and read in file..." << endl;
	return;
}

static void ReadMatFileMatio(const char* file, bool bInfoOnly)
{
	mat_t *pmat;
	matvar_t *pa;
	int varCnt = 0;
	
	cout << "Try to read all variables " << (bInfoOnly ? "info in: " : "in: ") << file << endl;

	pmat = Mat_Open(file, MAT_ACC_RDONLY);
	if (pmat == NULL) 
	{
		cout << "Failed to open!" << endl;
		return;
	}

	while ((pa = bInfoOnly ? Mat_VarReadNextInfo(pmat) : Mat_VarReadNext(pmat)) != NULL)
	{
		varCnt++;
		Mat_VarFree(pa);
	}
	
	if (Mat_Close(pmat) != 0) 
	{
		cout << "Failed to close! " << endl;
		return;
	}
	cout << varCnt << " variables found and read in file..." << endl;
	return;
}

int main(int argc, char *argv[])
{
	Mat_LogInit("matioPerformance");

	string pathToFiles = "..\\data\\";
	string fileExtension = "*.mat";
	WIN32_FIND_DATA search_data;

	memset(&search_data, 0, sizeof(WIN32_FIND_DATA));

	LARGE_INTEGER frequency;
	if (0 == QueryPerformanceFrequency(&frequency))
		return EXIT_FAILURE;

	// -i flag: Read variable info only (and skip data)
	bool bInfoOnly;
	if (argc > 1 && 0 == strcmp(argv[1], "-i"))
		bInfoOnly = true;
	else if (argc > 2 && 0 == strcmp(argv[2], "-i"))
		bInfoOnly = true;
	else
		bInfoOnly = false;

	// -m flag: Use MATLAB API
	ReadMatFile_Proc ReadMatFile;
	if (argc > 1 && 0 == strcmp(argv[1], "-m"))
		ReadMatFile = ReadMatFileMATLAB;
	else if (argc > 2 && 0 == strcmp(argv[2], "-m"))
		ReadMatFile = ReadMatFileMATLAB;
	else
		ReadMatFile = ReadMatFileMatio;

	HANDLE handle = FindFirstFile((pathToFiles+fileExtension).c_str(), &search_data);
	cout << "API: " << (ReadMatFile == ReadMatFileMatio ? "Matio" : "MATLAB") << endl;

	LARGE_INTEGER startTime, endTime;
	QueryPerformanceCounter(&startTime);
	while (INVALID_HANDLE_VALUE != handle)
	{
		ReadMatFile((pathToFiles+search_data.cFileName).c_str(), bInfoOnly);
		if (FindNextFile(handle, &search_data) == FALSE)
			break;
	}
	QueryPerformanceCounter(&endTime);

	FindClose(handle);
	cout << (bInfoOnly ? "Reading info" : "Reading") << " done in: " << fixed << double(endTime.QuadPart - startTime.QuadPart)/(double)frequency.QuadPart << endl;
	//system("pause");
	return EXIT_SUCCESS;
}
