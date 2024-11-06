//
//  FSAvionicsBridge.cpp
//  sim-avionics-bridge
//
//  Created by Joao Cabral on 03/11/2024.
//

#define STRSAFE_NO_DEPRECATE

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <SimConnect.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "FSFlightData.h"
#include "Network.h"
#include "Utilities.h"

using namespace std;

enum FLIGHTDATA_DATA_DEFINE_ID {
  FLIGHTDATA_DEFINITION
};

enum FLIGHTDATA_DATA_REQUEST_ID {
  FLIGHTDATA_REQUEST
};

struct FSFlightDataPropertyDefinition {
  const char* pszName;
  const char* pszUnits;
  SIMCONNECT_DATATYPE eDataType;
};

static const char* TITLE_STRING = "MSFS Avionics Bridge";
static const char* VERSION_STRING = "0.1";

// This table must align with the sizes and types defined in the FSFlightDataFields struct.
const FSFlightDataPropertyDefinition flightVariables[] = {
  { "TITLE",                      NULL,                SIMCONNECT_DATATYPE_STRING256 },
  { "PLANE LATITUDE",             "Degrees",           SIMCONNECT_DATATYPE_FLOAT64 },
  { "PLANE LONGITUDE",            "Degrees",           SIMCONNECT_DATATYPE_FLOAT64 },
  { "PLANE ALTITUDE",             "Feet",              SIMCONNECT_DATATYPE_FLOAT64 },
  { "PLANE HEADING DEGREES TRUE", "Degrees",           SIMCONNECT_DATATYPE_FLOAT64 },
  { "GPS GROUND TRUE TRACK",      "Radians",           SIMCONNECT_DATATYPE_FLOAT64 },
  { "MAGVAR",                     "Degrees",           SIMCONNECT_DATATYPE_FLOAT64 },
  { "GPS GROUND SPEED",           "Meters per second", SIMCONNECT_DATATYPE_FLOAT64 }
};

FSFlightData flightData;
HANDLE hSimConnect = NULL;
int shouldQuit = 0;

enum EVENT_ID {
  EVENT_SIM_START
};

void RequestData(void) {
  flightData = FSFlightData();
  
  // HRESULT hr =
  SimConnect_RequestDataOnSimObject(hSimConnect,
                                    FLIGHTDATA_REQUEST,
                                    FLIGHTDATA_DEFINITION,
                                    SIMCONNECT_OBJECT_ID_USER,
                                    SIMCONNECT_PERIOD_VISUAL_FRAME);
}

void AddDefinitions() {
  for (unsigned int i = 0; i < ARRAYSIZE(flightVariables); ++i)
  {
    const FSFlightDataPropertyDefinition& prop = flightVariables[i];
    SimConnect_AddToDataDefinition(hSimConnect, FLIGHTDATA_DEFINITION, prop.pszName, prop.pszUnits, prop.eDataType);
  }
}

void CALLBACK DispatchHandler(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext) {
  
  switch (pData->dwID) {
    case SIMCONNECT_RECV_ID_OPEN: {
      RequestData();
      break;
    }
      
    case SIMCONNECT_RECV_ID_EVENT: {
      SIMCONNECT_RECV_EVENT* evt = (SIMCONNECT_RECV_EVENT*)pData;
      switch (evt->uEventID) {
        case EVENT_SIM_START:
          RequestData();
          break;
        default:
          break;
      }
      break;
    }
      
    /*
    case SIMCONNECT_RECV_ID_EXCEPTION: {
      SIMCONNECT_RECV_EXCEPTION* evt = (SIMCONNECT_RECV_EXCEPTION*)pData;
      if(evt->dwException!=29) {
        printf("Received exception %d for %d/%d\n", evt->dwException, evt->dwSendID, evt->dwIndex);
      } else {
        // SIMCONNECT_EXCEPTION_DUPLICATE_ID
      }
      break;
    }
      
      
    case SIMCONNECT_RECV_ID_CLIENT_DATA: {
      SIMCONNECT_RECV_CLIENT_DATA *pObjData = (SIMCONNECT_RECV_CLIENT_DATA*)pData;
      break;
    }
    */
      
    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA: {
      SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;
      switch (pObjData->dwRequestID) {
        case FLIGHTDATA_REQUEST: {
          FSFlightDataFields* pUserData = (FSFlightDataFields*)&pObjData->dwData;
          if (SUCCEEDED(StringCbLengthA(&pUserData->title[0], sizeof(pUserData->title), NULL))) // security check {
            flightData.updateFields(pUserData);
          break;
        }
        default: {
          break;
        }
      }
      break; // SIMCONNECT_RECV_ID_SIMOBJECT_DATA
    }
      
    case SIMCONNECT_RECV_ID_QUIT: {
      shouldQuit = 1;
      break;
    }
      
    default:
      // printf("Received Event:%d\n", pData->dwID);
      break;
  }
}

void SendSimData(void) {
 
  char *adbpMessage = flightData.ADBPMSG();
  SendUDPPacket(adbpMessage, strlen(adbpMessage), 50000);
  
  char nmeaMessages[1400];
  memset(nmeaMessages,0,1400);
  strcat(nmeaMessages, flightData.NMEARMCMSG());
  strcat(nmeaMessages, flightData.NMEAGLLMSG());
  strcat(nmeaMessages, flightData.NMEAHDTMSG());
  SendUDPPacket(nmeaMessages, strlen(nmeaMessages), 10110);
}

void RunLoop(void) {
  
  HRESULT hr;
  
  if(SUCCEEDED(SimConnect_Open(&hSimConnect, TITLE_STRING, NULL, 0, 0, 0))) {
    
    printf("Connected to Flight Simulator! Running ...\n");
    AddDefinitions();
    hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_START, "SimStart");
    double lastUpdateTime = 0;
    double updatesInterval = 1 / FSFLIGHTDATA_SENT_UPDATES_PER_SECOND;
    while (0 == shouldQuit) {
      double timeNow = SystemUnixTimeNow();
      SimConnect_CallDispatch(hSimConnect, DispatchHandler, NULL);
      if(timeNow - lastUpdateTime > updatesInterval) {
        SendSimData();
        lastUpdateTime = timeNow;
      }
      Sleep(1);
    }
    printf("Disconnecting from Flight Simulator ...\n");
    hr = SimConnect_Close(hSimConnect);
    printf("Shutting down UDP Interface ...\n");
    ShutdownUDPInterface();
  } else {
    printf("Failed to connect to Flight Simulator (SimConnect)! Exiting ...\n");
  }
}

// int main(int argc, char* argv[]) {
int __cdecl _tmain(int argc, _TCHAR* argv[]) {
  
  printf("\nStarting %s %s\n",TITLE_STRING, VERSION_STRING);
  
  if(argc != 2) {
    printf("First parameter should be the remote IPv4 address! Exiting ...\n");
    return 1;
  }
  
  printf("Initializing UDP Interface ...\n");
  switch(InitializeUDPInterface(argv[1])) {
    case 0:
      printf("UDP interface is ready.\n");
      break;
    case 1:
      printf("Error initializing WSAStartup! Exiting ...\n");
      return 2;
    case 2:
      printf("Error creating UDP socket! Exiting ...\n");
      return 3;
    case 3:
      printf("Invalid remote IPv4 address! Exiting ...\n");
      return 4;
    default:
      break;
  }
  RunLoop();
  return 0;
}


