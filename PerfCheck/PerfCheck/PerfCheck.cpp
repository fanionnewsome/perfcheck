// PerfCheck.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <Pdh.h>
#include <PdhMsg.h>
#include <iostream>
#include <conio.h>
#include "sqlite3.h"
#include "Lib\libcurl\include\curl\curl.h"
#include "Lib\cereal\include\cereal\archives\binary.hpp"

// Use to convert bytes to MB
#define KB 1024
#define	MEMORY_AVAILABLE_BYTES  "\\Memory\\Available Bytes"
#define	MEMORY_AVAILABLE_KBYTES "\\Memory\\Available KBytes"
#define	MEMORY_AVAILABLE_MBYTES "\\Memory\\Available MBytes"

#pragma comment(lib, "pdh.lib")



#ifdef _DEBUG
#pragma comment(lib, "Lib/libcurl/lib/libcurl_a_debug.lib")
#else
#pragma comment(lib, "Lib/libcurl/lib/libcurl_a.lib")
#endif

CONST ULONG INTERVAL_MS = 60000;
CONST PWSTR BROWSE_DIALOG_CAPTION = L"Select a counter to monitor.";
PDH_HQUERY   query;

using namespace std;


void PerformCleanUp() {
	if (query) {
		PdhCloseQuery(query);
	}
}

void sendData()
{
	std::ostringstream os;
	{
		cereal::BinaryOutputArchive ar(os);
		
	} // the binary archives will flush their output 
	  // immediately, but it's better to
	  // use cereal archives in an RAII matter all the time

	std::string data = os.str();

	// create_packet(data.c_str(), data.size());
	// send out
}


int main()
{
	PDH_FMT_COUNTERVALUE counterCPUValue = {};
	PDH_FMT_COUNTERVALUE counterRAMValue = {};
	PDH_HCOUNTER counterCPU;
	PDH_STATUS   status;
	//LPSTR        ptrMessage;

	MEMORYSTATUSEX memorystat;
	memorystat.dwLength = sizeof(memorystat);
	GlobalMemoryStatusEx(&memorystat);

	/*curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL *curl = curl_easy_init();
	if (curl) {
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, "http://www.dynamsoft.com");
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();*/
	

	if (( status = PdhOpenQuery(NULL, 0, &query)) != ERROR_SUCCESS ) 
	{
		std::cout << "PdhOpenQuery: " << status << std::endl;
	}

	if ((status = PdhAddCounter(query, L"\\Processor(_Total)\\% Processor Time",
		0, &counterCPU)) != ERROR_SUCCESS)
	{
		wprintf(L"\nPdhAddCounter (cpu) %lx\n", status);
	}

	/*if ((status = PdhAddCounter(query, L"\\Memory\\Available MBytes",
		0, &counterRAM)) != ERROR_SUCCESS)
	{
		printf("PdhAddCounter (ram) %lx\n", status);
	}*/
	

	while (true) {

		if ((status = PdhCollectQueryData(query)) != ERROR_SUCCESS)
		{
			wprintf(L"PdhCollectQueryData %lx\n", status);
		}

		if (GlobalMemoryStatusEx(&memorystat) == 0)
		{
			wprintf(L"GlobalMemoryStatusEx %d\n", GetLastError());
		}

		if ((status = PdhGetFormattedCounterValue(counterCPU,
			PDH_FMT_LONG, 0, &counterCPUValue)) != ERROR_SUCCESS) {
			wprintf(L"PdhGetFormattedCounterValue(ram) %lx\n", status);
		}

		wprintf(L"cpu %3d%% \tmem %3d%% \tavail (%d mb)\n",
			counterCPUValue.longValue,
			memorystat.dwMemoryLoad,
			(int)memorystat.ullAvailPhys / (KB*KB));

		Sleep(INTERVAL_MS);
	}

	
	/*status = DisplayCounterDialog();


	if (status != ERROR_SUCCESS) 
	{
		if (status == PDH_DIALOG_CANCELLED)
		{
			wprintf(L"\nDialog canceled by user.");
		}
		else 
		{
			wprintf(L"\nPdhBrowseCounters failed with status 0x%x.", status);
		}
		PerformCleanUp();
	}*/
    return 0;
}

