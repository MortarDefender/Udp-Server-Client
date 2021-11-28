#define _CRT_SECURE_NO_WARNINGS

using namespace std; 
#include <iostream>
#include <string.h>



#include "CommandHandler.h"

#define TIME_PORT	27015
#define LOCALHOST	"127.0.0.1"


void main() {
	Handler* h = new Handler();

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}

	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (INVALID_SOCKET == m_socket) {
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;
	serverService.sin_port = htons(TIME_PORT);
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService))) {
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0, bytesRecv = 0;
	char sendBuff[BUFFER_SIZE], recvBuff[BUFFER_SIZE];

	cout << "Time Server: Wait for clients' requests.\n";

	while (true) {
		bytesRecv = recvfrom(m_socket, recvBuff, BUFFER_SIZE, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv) {
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}
		recvBuff[bytesRecv] = '\0';
		cout << "res: " << recvBuff << endl;
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

		bool command = h->handleRequest(recvBuff, sendBuff, m_socket, client_addr, client_addr_len);
		if (command) {
			bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
			if (SOCKET_ERROR == bytesSent) {
				cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
				closesocket(m_socket);
				WSACleanup();
				return;
			}

			cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
		}
	}

	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}