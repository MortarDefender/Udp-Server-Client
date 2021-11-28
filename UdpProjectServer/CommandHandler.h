#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <time.h>

#define BUFFER_SIZE 255

enum class Commands { Time, TimeWDate, TimeEpoch, ClientDelay, RTT, HourMin, Year, MonthDay, TimeBeginningMonth, WeekYear, Daylight, TimeCity, TimeLap };
enum class Cities { tokyo = 6, melbourne = 8, san_francisco = 14, porto = 22, UTC = 21 };

class Handler {
	private:
		bool timeLapActive;
		time_t StartTimeLap;
		ULONGLONG* timeArray;
		int packetsAmount, packetIndex;

		Handler(const Handler&) = delete;

		// commands
		void GetTime(char ans[BUFFER_SIZE]);
		void GetTimeWithoutDate(char ans[BUFFER_SIZE]);
		void GetTimeSinceEpoch(char ans[BUFFER_SIZE]);
		void GetClientToServerDelayEstimation(char req[BUFFER_SIZE], char res[BUFFER_SIZE], SOCKET socket, sockaddr client_addr, int client_addr_len);
		void MeasureRTT(char ans[BUFFER_SIZE]);
		void GetTimeWithoutDateOrSeconds(char ans[BUFFER_SIZE]);
		void GetYear(char ans[BUFFER_SIZE]);
		void GetMonthAndDay(char ans[BUFFER_SIZE]);
		void GetSecondSinceBeginigOfMonth(char ans[BUFFER_SIZE]);
		void GetWeekOfYear(char ans[BUFFER_SIZE]);
		void GetDaylightSavings(char ans[BUFFER_SIZE]);
		void GetTimeWithoutDateInCity(char ans[BUFFER_SIZE], Cities city, char cityName[20]);
		void MeasureTimeLap(char ans[BUFFER_SIZE]);

	public:
		Handler();
		~Handler();
		
		bool handleRequest(char req[BUFFER_SIZE], char ans[BUFFER_SIZE], SOCKET socket, sockaddr client_addr, int client_addr_len);
};