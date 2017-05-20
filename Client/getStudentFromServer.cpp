#include <iostream>
#include <windows.h>
#include "Student.h"

using namespace std;

Student getStudentFromServer(HANDLE hReadPipe, HANDLE hWritePipe, HANDLE hEnableReadClient, int num, int command)
{
	Student student;
	WriteFile(hWritePipe, &command, sizeof(command), NULL, NULL);
	HANDLE hEnableReadServer = OpenEvent(EVENT_MODIFY_STATE, FALSE, "EnableReadServer");
	SetEvent(hEnableReadServer);
	WriteFile(hWritePipe, &num, sizeof(num), NULL, NULL);
	hEnableReadServer = OpenEvent(EVENT_MODIFY_STATE, FALSE, "EnableReadServer");
	SetEvent(hEnableReadServer);
	WaitForSingleObject(hEnableReadClient, INFINITE);
	ReadFile(hReadPipe, &student, sizeof(student), NULL, NULL);
	CloseHandle(hEnableReadServer);
	return student;
}