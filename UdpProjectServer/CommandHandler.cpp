#define _CRT_SECURE_NO_WARNINGS
#include "CommandHandler.h"
#include <iostream>
#include <windows.h>

Handler::Handler() : timeLapActive(false), StartTimeLap(0), packetsAmount(-1), packetIndex(0), timeArray(nullptr) {}

Handler::~Handler() {}


bool Handler::handleRequest(char req[BUFFER_SIZE], char ans[BUFFER_SIZE], SOCKET socket, sockaddr client_addr, int client_addr_len) {
	/* handle the requests from the user to the server. activate the function needed using a protocol  */
	int command;
	char city[20] = "--";
	sscanf(req, "%d", &command);


	switch (command) {
		case static_cast<int>(Commands::Time):
			GetTime(ans);
			break;
		case static_cast<int>(Commands::TimeWDate):
			GetTimeWithoutDate(ans);
			break;
		case static_cast<int>(Commands::TimeEpoch):
			GetTimeSinceEpoch(ans);
			break;
		case static_cast<int>(Commands::ClientDelay):
			GetClientToServerDelayEstimation(req, ans, socket, client_addr, client_addr_len);
			break;
		case static_cast<int>(Commands::RTT):
			MeasureRTT(ans);
			break;
		case static_cast<int>(Commands::HourMin) :
			GetTimeWithoutDateOrSeconds(ans);
			break;
		case static_cast<int>(Commands::Year):
			GetYear(ans);
			break;
		case static_cast<int>(Commands::MonthDay):
			GetMonthAndDay(ans);
			break;
		case static_cast<int>(Commands::TimeBeginningMonth):
			GetSecondSinceBeginigOfMonth(ans);
			break;
		case static_cast<int>(Commands::WeekYear) :
			GetWeekOfYear(ans);
			break;
		case static_cast<int>(Commands::Daylight):
			this->GetDaylightSavings(ans);
			break;
		case static_cast<int>(Commands::TimeLap) :
			this->MeasureTimeLap(ans);
			break;
		case static_cast<int>(Commands::TimeCity):
			sscanf(req, "%d %s", &command, city);
			if (strcmp(city, "--") != 0) {
				if (strcmp(city, "tokyo") == 0)
					this->GetTimeWithoutDateInCity(ans, Cities::tokyo, city);
				else if (strcmp(city, "melbourne") == 0)
					this->GetTimeWithoutDateInCity(ans, Cities::melbourne, city);
				else if (strcmp(city, "san-francisco") == 0)
					this->GetTimeWithoutDateInCity(ans, Cities::san_francisco, city);
				else if (strcmp(city, "porto") == 0)
					this->GetTimeWithoutDateInCity(ans, Cities::porto, city);
				else
					this->GetTimeWithoutDateInCity(ans, Cities::UTC, city);
			}
			else
				sprintf(ans, "error in time city: there is no city in the command to get a city time.");
			break;
		default:
			sprintf(ans, "There is no option %d", command);
			break;
	}
	return command != static_cast<int>(Commands::ClientDelay);
}


void Handler::GetTime(char ans[BUFFER_SIZE]) {
	/* gets the full time YYYY/MM/DD/hh/mm/ss */
	time_t currentTime = time(0);
	tm* timeStruct = localtime(&currentTime);
	sprintf(ans, "The Date and Time for now are: %d/%d/%d %d:%d:%d", 1900 + timeStruct->tm_year, 1 + timeStruct->tm_mon, timeStruct->tm_mday, timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
}

void Handler::GetTimeWithoutDate(char ans[BUFFER_SIZE]) {
	/* gets the time hh/mm/ss */
	time_t currentTime = time(0);
	tm* timeStruct = localtime(&currentTime);
	sprintf(ans, "The Time Is:  %d:%d:%d",timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec);
}

void Handler::GetTimeSinceEpoch(char ans[BUFFER_SIZE]) {
	/* gets the epoch time */
	time_t timer;
	time(&timer);
	sprintf(ans, "Time since Epoch for now is:  %lld", timer);
}

void Handler::GetClientToServerDelayEstimation(char req[BUFFER_SIZE], char res[BUFFER_SIZE], SOCKET socket, sockaddr client_addr, int client_addr_len) {  // not yet tested
	/* return the avreage of delay between the server and the client */
	int commandId;
	if (this->packetsAmount == -1) {
		sscanf(req, "%d %d", &commandId, &packetsAmount);
		this->timeArray = new ULONGLONG[packetsAmount];
	}
	timeArray[packetIndex] = GetTickCount64();
	packetIndex++;

	if (packetIndex == packetsAmount) {
		for (int i = 0; i < packetsAmount; i++) {
			sprintf(res, "%d", timeArray[i]);
			sendto(socket, res, (int)strlen(res), 0, (const sockaddr*)&client_addr, client_addr_len);
		}
		packetsAmount = -1;
		packetIndex = 0;
		delete timeArray;
		timeArray = nullptr;
	}
}

void Handler::MeasureRTT(char ans[BUFFER_SIZE]) {
	/* measure the Round Trip Time */
	ULONGLONG currentTime = GetTickCount64();
	sprintf(ans, "%lld", currentTime);
}

void Handler::GetTimeWithoutDateOrSeconds(char ans[BUFFER_SIZE]) {
	/* gets the time in the format HH/MM */
	time_t currentTime = time(0);
	tm* timeStruct = localtime(&currentTime);
	sprintf(ans, "The Time Is: %d:%d", timeStruct->tm_hour, timeStruct->tm_min);
}

void Handler::GetYear(char ans[BUFFER_SIZE]) {
	/* gets the year YYYY */
	time_t currentTime = time(0);
	tm* timeStruct = localtime(&currentTime);
	sprintf(ans, "The Year is: %d", 1900 + timeStruct->tm_year);
}

void Handler::GetMonthAndDay(char ans[BUFFER_SIZE]) {
	/* gets the date MM/DD */
	time_t currentTime = time(0);
	tm* timeStruct = localtime(&currentTime);
	sprintf(ans, "The Month And Day are: %d/%d", 1 + timeStruct->tm_mon, timeStruct->tm_mday);
}

void Handler::GetSecondSinceBeginigOfMonth(char ans[BUFFER_SIZE]) {
	/* gets the seconds from the start of the month */
	time_t currentTime = time(0);
	tm* currentTimeStruct = localtime(&currentTime);
	currentTimeStruct->tm_mday = 1;
	currentTimeStruct->tm_hour = 0;
	currentTimeStruct->tm_min = 0;
	currentTimeStruct->tm_sec = 0;
	sprintf(ans, "The amount of seconds from the start of the month is: %lld", currentTime - mktime(currentTimeStruct));
}

void Handler::GetWeekOfYear(char ans[BUFFER_SIZE]) {
	/* gets the amount of weeks from the start of the year */
	time_t currentTime = time(0);
	tm* timeStruct = localtime(&currentTime);
	sprintf(ans, "Week Of Year Is: %d", static_cast<int>(timeStruct->tm_yday / 7.0));
}

void Handler::GetDaylightSavings(char ans[BUFFER_SIZE]) {
	/* gets daylight savings. 1 if ther time is by summer clock otherwise 0 */
	time_t currentTime = time(0);
	tm* timeStruct = localtime(&currentTime);
	sprintf(ans, "daylight saving = %d", timeStruct->tm_isdst);
}

void Handler::GetTimeWithoutDateInCity(char ans[BUFFER_SIZE], Cities city, char cityName[20]) {
	/* gets the time <hh/mm/ss> in the city given by req [Tokio, Malboran, San-Francisco, Porto, utc] */
	time_t currentTime = time(0);
	tm* timeStruct = localtime(&currentTime);
	if (city == Cities::UTC)
		sprintf(cityName, "%s", "UTC");
	sprintf(ans, "The Time In %s is: %d:%d:%d", cityName,(timeStruct->tm_hour + static_cast<int>(city)) % 24, timeStruct->tm_min, timeStruct->tm_sec);
}

void Handler::MeasureTimeLap(char ans[BUFFER_SIZE]) {
	/* measure the difference between the first TimeLap command and this. timeout after 3 minutes */
	if (timeLapActive) {
		time_t timeout = time(0) - this->StartTimeLap;
		if (timeout > 180)
			sprintf(ans, "timeout. The last time the command has been activated is more than 3 minutes.");
		else
			sprintf(ans, "The difference between the last command is: %lld sec", timeout);
	}
	else {
		this->StartTimeLap = time(0);
		sprintf(ans, "Timer Has Been Started");
	}
	timeLapActive = !timeLapActive;
}
