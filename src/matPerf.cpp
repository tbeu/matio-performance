#include <iostream>
#include <windows.h>
#include <ctime>
#include <string>

using namespace std;

#define MATIO

#if !defined(MATIO)
#include "mat.h"
static void ReadMatFile(const char* file)
{
	MATFile *pmat;
	mxArray *pa;
	const char *name;
	int varCnt = 0;

	
	cout << "Try to read all variables in: " << file << endl;

	pmat = matOpen(file, "r");
	if (pmat == NULL) 
	{
		cout << "Failed to open!" << endl;
		return;
	}

	while ((pa = matGetNextVariable(pmat, &name)) != NULL) 
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
#else
#include "matio.h"
static void ReadMatFile(const char* file)
{
	mat_t *pmat;
	matvar_t *pa;
	int varCnt = 0;
	
	cout << "Try to read all variables in: " << file << endl;

	pmat = Mat_Open(file, MAT_ACC_RDONLY);
	if (pmat == NULL) 
	{
		cout << "Failed to open!" << endl;
		return;
	}

	while ((pa = Mat_VarReadNext(pmat)) != NULL)
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
#endif

int main()
{
	string pathToFiles = "..\\data\\";
	string fileExtension = "*.mat";
	WIN32_FIND_DATA search_data;

	memset(&search_data, 0, sizeof(WIN32_FIND_DATA));

	HANDLE handle = FindFirstFile((pathToFiles+fileExtension).c_str(), &search_data);
	cout << "Tick!" << endl;
	
	clock_t startTime = clock();
	while (handle != INVALID_HANDLE_VALUE)
	{
		ReadMatFile((pathToFiles+search_data.cFileName).c_str());
		if (FindNextFile(handle, &search_data) == FALSE)
			break;
	}
	clock_t endTime = clock();
	cout << "Tock!" << endl;

	FindClose(handle);
	cout << "Done in: " << ((float)(endTime - startTime) / CLOCKS_PER_SEC) << endl;
	system("pause");
	return 0;
}
