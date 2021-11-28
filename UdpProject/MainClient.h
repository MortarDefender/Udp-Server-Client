#pragma once
#define BUFFER_SIZE 255
#define TIME_PORT	27015
#define LOCALHOST	"127.0.0.1"

enum class Commands { Delay = 4, RTT = 5, CityTime = 12, Exit = 14 };

int main();
bool handle(SOCKET socket, sockaddr_in adder);
int recvMessage(SOCKET socket, char res[BUFFER_SIZE]);
int sendMessage(SOCKET socket, sockaddr_in adder, char req[BUFFER_SIZE]);
void GetRTT(SOCKET socket, sockaddr_in adder, char req[BUFFER_SIZE], char res[BUFFER_SIZE]);
void GetDelay(SOCKET socket, sockaddr_in adder, char req[BUFFER_SIZE], char res[BUFFER_SIZE]);
void sendRecvMessage(SOCKET socket, sockaddr_in adder, char req[BUFFER_SIZE], char res[BUFFER_SIZE]);

void stringReplace(char req[BUFFER_SIZE], char replacedChar, char replacedBy);
