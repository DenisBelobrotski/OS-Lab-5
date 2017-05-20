#include <iostream>
#include <windows.h>
#include "Student.h"

using namespace std;

Student getStudentFromServer(HANDLE hReadPipe, HANDLE hWritePipe, HANDLE hEnableReadClient, int num, int command);

int main(int args, char *argv[]) {
	setlocale(LC_ALL, "rus");

	HANDLE hReadPipe;
	HANDLE hWritePipe;
	HANDLE hEnableReadServer;
	HANDLE hEnableReadClient;

	Student student;
	int command;
	int num;
	bool exit = false;
	int answer;

	hEnableReadClient = CreateEvent(NULL, NULL, FALSE, "EnableReadClient");

	hReadPipe = (HANDLE)atoi(argv[1]);
	hWritePipe = (HANDLE)atoi(argv[2]);
	
	while (!exit)
	{
		printf("%s (%s, %s, %s): ", "Enter command", "1 - change", "2 - read", "3 - exit");
		scanf("%d", &command);
		switch (command) {
			case 1:
				printf("Enter record-book number:\n");
				scanf("%d", &num);
				command = 2;
				student = getStudentFromServer(hReadPipe, hWritePipe, hEnableReadClient, num, command);
				printf("%s: %d\n%s: %s\n%s: %lf\n", "Record-book number", student.num, "Name", student.name, "Grade", student.grade);
				printf("%s: ", "Enter new record-book number");
				scanf("%d", &student.num);
				printf("%s: ", "Enter new name");
				scanf("%s", &student.name);
				printf("%s: ", "Enter new grade");
				scanf("%lf", &student.grade);
				printf("%s (%s, %s): ", "Do you want to send new values to the server?", "1 - Yes", "0 - No");
				scanf("%d", &answer);

				if (answer != 1) 
				{

				}
				else
				{
					command = 1;
					WriteFile(hWritePipe, &command, sizeof(int), NULL, NULL);
					hEnableReadServer = OpenEvent(EVENT_MODIFY_STATE, FALSE, "EnableReadServer");
					SetEvent(hEnableReadServer);
					WriteFile(hWritePipe, &student, sizeof(Student), NULL, NULL);
					hEnableReadServer = OpenEvent(EVENT_MODIFY_STATE, FALSE, "EnableReadServer");
					SetEvent(hEnableReadServer);
					CloseHandle(hEnableReadServer);
				}

				break;
			case 2:
				printf("%s: ", "Enter record-book number");
				scanf("%d", &num);

				student = getStudentFromServer(hReadPipe, hWritePipe, hEnableReadClient, num, command);
				printf("%s: %d\n%s: %s\n%s: %lf\n", "Record-book number", student.num, "Name", student.name, "Grade", student.grade);

				break;
			default:
				WriteFile(hWritePipe, &command, sizeof(command), NULL, NULL);
				hEnableReadServer = OpenEvent(EVENT_MODIFY_STATE, FALSE, "EnableReadServer");
				SetEvent(hEnableReadServer);
				exit = true;

				break;
		}
	}

	CloseHandle(hReadPipe);
	CloseHandle(hWritePipe);
	CloseHandle(hEnableReadClient);

	return 0;
}