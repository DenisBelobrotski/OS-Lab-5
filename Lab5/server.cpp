#include <iostream>
#include <fstream>
#include <windows.h>
#include "student.h"

using namespace std;

ifstream getIFStream(char* fileName);
ofstream getOFStream(char* fileName);

int main() 
{
	setlocale(LC_ALL, "rus");

	char lpszAppName[] = "Client.exe";
	char* lpszCommandLine = new char[1000];
	char* fileName = new char[260];
	int studentsNum;
	int num;
	int command;
	bool exit = false;
	int lastReadIndex;
	int answer;
	Student* students;
	Student student;
	ofstream fout;
	ifstream fin;

	HANDLE hReadPipe;
	HANDLE hWritePipe;
	HANDLE hEnableReadServer;
	HANDLE hEnableReadClient;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	printf("%s: ", "Enter file name");
	scanf("%s", fileName);
	fout = getOFStream(fileName);

	hEnableReadServer = CreateEvent(NULL, NULL, FALSE, "EnableReadServer");

	HANDLE hInheritReadPipe;
	HANDLE hInheritWritePipe;

	CreatePipe(&hReadPipe, &hWritePipe, NULL, 0);

	DuplicateHandle(GetCurrentProcess(), hReadPipe, GetCurrentProcess(), &hInheritReadPipe, 0, TRUE, DUPLICATE_SAME_ACCESS);
	DuplicateHandle(GetCurrentProcess(), hWritePipe, GetCurrentProcess(), &hInheritWritePipe, 0, TRUE, DUPLICATE_SAME_ACCESS);
	
	printf("%s: ", "Enter number of students");
	scanf("%d", &studentsNum);
	fout << studentsNum;
	students = new Student[studentsNum];
	for (int i = 0; i < studentsNum; i++)
	{
		printf("%s: ", "Enter record-book number");
		scanf("%d", &(students[i].num));
		printf("%s: ", "Enter name");
		scanf("%s", students[i].name);
		printf("%s: ", "Enter grade");
		scanf("%lf", &(students[i].grade));
		fout.write((char*)&students[i], sizeof(Student));
	}
	fout.close();

	printf("%s:\n", "Original file");
	fin = getIFStream(fileName);
	fin >> studentsNum;
	for (int i = 0; i < studentsNum; i++) {
		printf("%s: %d\n%s: %s\n%s: %lf\n", "Record-book number", students[i].num, "Name", students[i].name, "Grade", students[i].grade);
	}
	fin.close();

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	sprintf(lpszCommandLine, "%s %d %d", lpszAppName, (int)hInheritReadPipe, (int)hInheritWritePipe);
	CreateProcess(NULL, lpszCommandLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	while (!exit) {
		
		WaitForSingleObject(hEnableReadServer, INFINITE);
		ReadFile(hInheritReadPipe, &command, sizeof(command), NULL, NULL);
		switch (command) {
			case 1:
				WaitForSingleObject(hEnableReadServer, INFINITE);
				ReadFile(hReadPipe, &student, sizeof(Student), NULL, NULL);

				fin = getIFStream(fileName);
				fin >> studentsNum;
				for (int i = 0; i < studentsNum; i++) {
					fin.read((char*)&students[i], sizeof(Student));
				}
				fin.close();

				fout = getOFStream(fileName);
				students[lastReadIndex] = student;
				fout << studentsNum;
				for (int i = 0; i < studentsNum; i++) {
					fout.write((char*)&students[i], sizeof(Student));
				}
				fout.close();

				break;
			case 2:
				WaitForSingleObject(hEnableReadServer, INFINITE);
				ReadFile(hReadPipe, &num, sizeof(int), NULL, NULL);
				hEnableReadClient = OpenEvent(EVENT_MODIFY_STATE, FALSE, "EnableReadClient");

				fin = getIFStream(fileName);
				fin >> studentsNum;
				for (int i = 0; i < studentsNum; i++) {
					fin.read((char*)&student, sizeof(Student));
					if (student.num == num) {
						WriteFile(hWritePipe, &student, sizeof(Student), NULL, NULL);
						lastReadIndex = i;
						break;
					}
				}
				fin.close();

				SetEvent(hEnableReadClient);

				break;
			default:
				printf("%s:\n", "Changed file");
				fin = getIFStream(fileName);
				fin >> studentsNum;
				for (int i = 0; i < studentsNum; i++) {
					printf("%s: %d\n%s: %s\n%s: %lf\n", "Record-book number", students[i].num, "Name", students[i].name, "Grade", students[i].grade);
				}
				fin.close();
				printf("%s (%s, %s): ", "Do you want to exit?", "1 - Yes", "0 - No");
				scanf("%d", &answer);
				if (answer == 1)
					exit = true;

				break;
		}
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hInheritReadPipe);
	CloseHandle(hInheritWritePipe);
	CloseHandle(hEnableReadServer);
	CloseHandle(hEnableReadClient);

	return 0;
}