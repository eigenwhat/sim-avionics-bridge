//
//  XPAvionicsBridge.cpp
//  sim-avionics-bridge
//
//  Created by Joao Cabral on 05/11/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#if IBM
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <io.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#else
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <syslog.h>
#include <sys/select.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/errno.h>

#endif

/* Some type definitions for windows */
#if IBM
typedef int int32_t;
typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef int socklen_t;
#endif

/* The X-Plane SDK include files */

#include <XPLMProcessing.h>
#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include <XPLMNavigation.h>
#include <XPLMMenus.h>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>

#include <XPWidgets.h>
#include <XPStandardWidgets.h>
#include <XPUIGraphics.h>
#include <XPLMPlugin.h>

#include "Network.h"
#include "Utilities.h"

extern int errno;

const char *lhsDefaultMPDestinationIP = "172.30.1.255";
#define XPFLIGHTDATA_SENT_UPDATES_PER_SECOND 10.0

XPLMFlightLoopID flightLoop = NULL;

/* For Windows, define DLLMain */
#if IBM
BOOL APIENTRY DllMain( HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}
#endif

XPLMDataRef lhsXPDRLatitude=NULL;
XPLMDataRef lhsXPDRLongitude=NULL;
XPLMDataRef lhsXPDRGroundSpeed=NULL;
XPLMDataRef lhsXPDRTrueHeading=NULL;
XPLMDataRef lhsXPDRTrueTrack=NULL;
XPLMDataRef lhsXPDRMagVar=NULL;

void LHSSetDatarefs(void) {
  
  lhsXPDRLatitude = XPLMFindDataRef("sim/flightmodel/position/latitude"); // Double
  lhsXPDRLongitude = XPLMFindDataRef("sim/flightmodel/position/longitude"); // Double
  lhsXPDRGroundSpeed = XPLMFindDataRef("sim/flightmodel/position/groundspeed"); // Float
  lhsXPDRTrueHeading = XPLMFindDataRef("sim/flightmodel/position/psi"); // Float
  lhsXPDRTrueTrack = XPLMFindDataRef("sim/flightmodel/position/hpath"); // Float
  lhsXPDRMagVar = XPLMFindDataRef("sim/flightmodel/position/magnetic_variation"); // Float
}

void LHSSendSimData(void) {
  
  double lat = XPLMGetDatad(lhsXPDRLatitude);
  double lon = XPLMGetDatad(lhsXPDRLongitude);
  double gs = XPLMGetDataf(lhsXPDRGroundSpeed);
  double hdg = XPLMGetDataf(lhsXPDRTrueHeading);
  double trk = XPLMGetDataf(lhsXPDRTrueTrack);
  double mvar = XPLMGetDataf(lhsXPDRMagVar);

  char *adbpMessage = ADBPMessage(XPFLIGHTDATA_SENT_UPDATES_PER_SECOND * 10, hdg, gs, lat, lon);
  
  ssize_t s;
  if((s=SendUDPPacket(adbpMessage, strlen(adbpMessage), 50000))<strlen(adbpMessage)) {
    XPLMDebugString("XPAB: Error sending ADBPMessage!\n");
  };
 
  char nmeaMessages[1400];
  memset(nmeaMessages,0,1400);
  strcat(nmeaMessages, NMEARMCMessage(lat, lon, gs, trk, mvar));
  strcat(nmeaMessages, NMEAGLLMessage(lat, lon));
  strcat(nmeaMessages, NMEAHDTMessage(hdg));
     
  if((s=SendUDPPacket(nmeaMessages, strlen(nmeaMessages), 10110))<strlen(nmeaMessages)) {
    XPLMDebugString("XPAB: Error sending nmeaMessages!\n");
  };
}

float LHSRunLoop(float   inElapsedSinceLastCall,
                 float   inElapsedTimeSinceLastFlightLoop,
                 int     inCounter,
                 void *  inRefcon) {
  LHSSendSimData();
  return ((float)(1.0 / XPFLIGHTDATA_SENT_UPDATES_PER_SECOND));
}

/* The default/required plugin handlers */

PLUGIN_API int XPluginStart ( char * outName, char * outSig, char * outDesc ) {

  XPLMEnableFeature("XPLM_USE_NATIVE_PATHS",1);
  
  /* Register our credentials */
  strcpy(outName,"XP Avionics Bridge 0.1");
  strcpy(outDesc,"XP Avionics Bridge 0.1");
  strcpy(outSig, "com.lhsystems.xp-avionics-bridge");
  
  /* Log the start of the plugin regardless of the state of HSXPLDEBUG so
   * that its version gets into Log.txt */
  char logstr[64];
  snprintf(logstr,64,"XPAB: %s XPluginStart()\n",outName);
  XPLMDebugString(logstr);

  // Sdt dataref pointers
  LHSSetDatarefs();
  
  // Setup flight loop
  XPLMCreateFlightLoop_t flparams;
  flparams.phase = xplm_FlightLoop_Phase_BeforeFlightModel;
  flparams.structSize = sizeof(XPLMCreateFlightLoop_t);
  flparams.callbackFunc = LHSRunLoop;
  flparams.refcon = NULL;
  flightLoop = XPLMCreateFlightLoop(&flparams);
  XPLMScheduleFlightLoop(flightLoop,-1,1);
  
  // Determine destinationIP
  char destinationIP[256];
  strcpy(destinationIP, lhsDefaultMPDestinationIP);
  
#if !defined(_WIN32)
  FILE *fp=fopen("Resources/plugins/XPAvionicsBridge.cfg","r");
#else
  FILE *fp=fopen("Resources\\plugins\\XPAvionicsBridge.cfg","r");
#endif
  if(fp!=NULL) {
    fscanf(fp,"%s",destinationIP);
    fclose(fp);
  }
  snprintf(logstr,64,"XPAB: Initializing UDP socket to IP address: %s\n", destinationIP);
  XPLMDebugString(logstr);

  switch(InitializeUDPInterface(lhsDefaultMPDestinationIP)) {
    case 0: break;
    case 1:
      XPLMDebugString("XPAB: Failed in WSAStartup!\n");
      break;
    case 2:
      XPLMDebugString("XPAB: Failed to inisialise UDP socket!\n");
      break;
    case 3:
      XPLMDebugString("XPAB: Invalid IPv4 Address!\n");
      break;
    case 4:
      XPLMDebugString("XPAB: Error in setsockopt broadcast!\n");
      break;
    default:
      XPLMDebugString("XPAB: Error initializing UDP interface!\n");
      break;
  };
  
  return 1;
}

PLUGIN_API void XPluginStop ( void ) {
  if(flightLoop!=NULL) {
    XPLMDestroyFlightLoop(flightLoop);
    flightLoop = NULL;
  }
}

PLUGIN_API int XPluginEnable ( void ) {
  LHSSetDatarefs();
  return 1;
}

PLUGIN_API void XPluginDisable ( void ) {
}

#define XPLM_MSG_PLANE_CRASHED          101
#define XPLM_MSG_PLANE_LOADED           102
#define XPLM_MSG_AIRPORT_LOADED         103
#define XPLM_MSG_SCENERY_LOADED         104
#define XPLM_MSG_AIRPLANE_COUNT_CHANGED 105
#define XPLM_MSG_PLANE_UNLOADED         106
#define XPLM_MSG_WILL_WRITE_PREFS       107
#define XPLM_MSG_LIVERY_LOADED          108

PLUGIN_API void XPluginReceiveMessage ( XPLMPluginID inFrom, long inMessage,
                                       void * inParam ) {
  
  int reloadDatarefs=0;
  switch(inMessage) {
    case(XPLM_MSG_PLANE_LOADED): {
      if(inParam == 0) {
        reloadDatarefs = 1;
      }
      break;
    }
    case(XPLM_MSG_AIRPORT_LOADED): {
      reloadDatarefs = 1;
      break;
    }
    default: break;
  }
  if (reloadDatarefs) {
    LHSSetDatarefs();
  }
}
