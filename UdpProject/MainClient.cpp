#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>
#include <windows.h>
#include <iostream>

#include "MainClient.h"
using namespace std;

int main() {
	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		cout << "Time Client: Error at WSAStartup()\n";
		return 0;
	}

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket) {
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(LOCALHOST);
	server.sin_port = htons(TIME_PORT);

	bool command = true;
	while (command)
		command = handle(connSocket, server);

	cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);
	return 1;
}

bool handle(SOCKET socket, sockaddr_in adder) {
	/* handle the user input and the commands the user requests */
	char req[BUFFER_SIZE], res[BUFFER_SIZE];
	cout << "Please Enter Your Choice:" << endl;
	cout << " 1. Get Current Time And Date" << endl;
	cout << " 2. Get Current Time" << endl;
	cout << " 3. Get Time Since Epoch" << endl;
	cout << " 4. Get Delay Estimation" << endl;
	cout << " 5. Measure RTT" << endl;
	cout << " 6. Get Time Without Seconds" << endl;
	cout << " 7. Get Year" << endl;
	cout << " 8. Get Month And Day" << endl;
	cout << " 9. Get Second Since Beginning Of The Current Month" << endl;
	cout << "10. Get Week Of Year" << endl;
	cout << "11. Get Daylight Savings" << endl;
	cout << "12. Get Time Without Date In City" << endl;
	cout << "13. Measure Time Lap" << endl;
	cout << "14. Exit" << endl;
	cout << endl << endl << ">> ";
	int action;
	cin >> action;

	if (action == static_cast<int>(Commands::Delay))
		GetDelay(socket, adder, req, res);
	else if (action == static_cast<int>(Commands::RTT))
		GetRTT(socket, adder, req, res);
	else {
		if (action == static_cast<int>(Commands::CityTime)) {
			cout << "you can choose one of these cities: tokyo, melbourne, san_francisco, porto or Any other city that will be treated as UTC" << endl;
			char city[100];
			cout << "enter a city name: ";
			cin.ignore();
			cin.getline(city, 100);
			stringReplace(city, ' ', '-');
			sprintf(req, "%d %s", action - 1, city);
		}
		else if (action == static_cast<int>(Commands::Exit))
			return false;
		else
			sprintf(req, "%d", action - 1);
		sendRecvMessage(socket, adder, req, res);
	}
	return true;
}

void GetDelay(SOCKET socket, sockaddr_in adder, char req[BUFFER_SIZE], char res[BUFFER_SIZE]) {
	/* create the measurments for the delay between the client and the server */
	int length = 100;
	ULONGLONG time = 0, temp = 0;
	ULONGLONG* timeArray = new ULONGLONG[length];
	sprintf(req, "%d %d", static_cast<int>(Commands::Delay) - 1, length);
	for (int i = 0; i < length; i++) {
		timeArray[i] = GetTickCount64();
		int send = sendMessage(socket, adder, req);
	}
	
	for (int i = 0; i < length; i++) {
		int recv = recvMessage(socket, res);
		sscanf(res, "%d", &temp);
		time += temp - timeArray[i];
	}
	cout << "The Average Delay of the client to the server is: " << (time / (float) length) << " nano seconds" << endl << endl;
}

void GetRTT(SOCKET socket, sockaddr_in adder, char req[BUFFER_SIZE], char res[BUFFER_SIZE]) {
	/* create the measurment for the Round Trip Time */
	int length = 100;
	ULONGLONG avg = 0;  // DWORD
	ULONGLONG startTime, endTime;
	sprintf(req, "%d", static_cast<int>(Commands::RTT) - 1);
	for (int i = 0; i < length; i++) {
		startTime = GetTickCount64();
		int send = sendMessage(socket, adder, req);
		int recv = recvMessage(socket, res);
		endTime = GetTickCount64();
		avg += endTime - startTime;
	}
	cout << "The Average Round Trip Time from the client to the server is: " << (avg / (float) length) << " nano seconds" << endl << endl;
}

void sendRecvMessage(SOCKET socket, sockaddr_in adder, char req[BUFFER_SIZE], char res[BUFFER_SIZE]) {
	/* send a message and recv the response from the server */
	int send = sendMessage(socket, adder, req);
	int recv = recvMessage(socket, res);
	cout << res << endl << endl;
}

int sendMessage(SOCKET socket, sockaddr_in adder, char req[BUFFER_SIZE]) {
	/* send a message to the server */
	int bytesSent = sendto(socket, req, (int)strlen(req), 0, (const sockaddr*)&adder, sizeof(adder));
	if (SOCKET_ERROR == bytesSent) {
		cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
		closesocket(socket);
		WSACleanup();
		return bytesSent;
	}
	return 0;
}

int recvMessage(SOCKET socket, char res[BUFFER_SIZE]) {
	/* recv a message from the server */
	int bytesRecv = recv(socket, res, BUFFER_SIZE, 0);
	if (SOCKET_ERROR == bytesRecv) {
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(socket);
		WSACleanup();
		return bytesRecv;
	}

	res[bytesRecv] = '\0';
	return 0;
}

void stringReplace(char req[BUFFER_SIZE], char replacedChar, char replacedBy) {
	/* replace all cherecters that match replaceChar with replafceBy in the string req */
	char temp[BUFFER_SIZE];
	int i;
	for (i = 0; req[i]; i++) {
		if (req[i] == replacedChar)
			temp[i] = replacedBy;
		else
			temp[i] = req[i];
	}
	temp[i] = '\0';
	for (i = 0; temp[i]; i++)
		req[i] = temp[i];
	req[i] = '\0';
}