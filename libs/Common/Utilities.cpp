//
//  Utilities.cpp
//  sim-avionics-bridge
//
//  Created by Joao Cabral on 03/11/2024.
//

#include <cstring>
#include <cstdio>
#include <cmath>
#include <time.h>
#include <sys/time.h>

#include "Utilities.h"

double SystemUnixTimeNow(void) {
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv,&tz);
  return (double)tv.tv_sec + ((double)tv.tv_usec / 1000000.0);
}

char *ADBPTimeNowUTC(void) {

  static char _adbp_time_now_utc_[16];
  double timenow = SystemUnixTimeNow();
  time_t timeonly = (time_t)timenow % (60 * 60 * 24);
  time_t hours = timeonly / (60 * 60);
  time_t minssecs = timeonly % (60 * 60);
  time_t mins = minssecs / 60;
  time_t secs = minssecs % 60;
  snprintf(_adbp_time_now_utc_,16,"%02d:%02d:%02d",(int)hours,(int)mins,(int)secs);
  return _adbp_time_now_utc_;
};

char *ADBPDateNowUTC(void) {
  static char _adbp_date_now_utc_[16];
  time_t timenow = (time_t)SystemUnixTimeNow();
  struct tm ts;
#if defined(_WIN32)
  if(!gmtime_s(&ts,&timenow)) {
#else
    if(gmtime_r(&timenow,&ts)!=NULL) {
#endif
    snprintf(_adbp_date_now_utc_,16,"%02d/%02d/%02d",(int)ts.tm_mday,(int)ts.tm_mon+1,((int)ts.tm_year+1900) % 100);
  } else {
    _adbp_date_now_utc_[0]='\0';
  }
  return _adbp_date_now_utc_;
}

char *ADBPMessage(int frequency, double trueHeading, double groundSpeed, double latitude, double longitude) {
  static char message[1400];
  static unsigned int seqID=1;
  double timestamp  = SystemUnixTimeNow() * 1000.0;
 
 snprintf(message,1400,"<?xml version=\"1.0\" encoding=\"utf-8\"?><method name=\"publishAvionicParameters\"><refreshperiod>%d</refreshperiod><context><key name=\"SEQUENCE_ID\" value=\"%u\"/><key name=\"TOTAL_MSG_PARTS\" value=\"1\"/></context><parameters><parameter name=\"HDGT\" type=\"0\" validity=\"1\" value=\"%.1f\" time=\"%.0f\"/><parameter name=\"GROUNDSPEED\" type=\"0\" validity=\"1\" value=\"%.1f\" time=\"%.0f\"/><parameter name=\"LATITUDE\" type=\"0\" validity=\"1\" value=\"%f\" time=\"%.0f\"/><parameter name=\"LATFINE\" type=\"0\" validity=\"1\" value=\"0.0\" time=\"%.0f\"/><parameter name=\"LONGITUDE\" type=\"0\" validity=\"1\" value=\"%f\" time=\"%.0f\"/><parameter name=\"LONGFINE\" type=\"0\" validity=\"1\" value=\"0.0\" time=\"%.0f\"/><parameter name=\"SYSTEMDATE\" type=\"8\" validity=\"1\" value=\"%s\"/><parameter name=\"SYSTEMTIME\" type=\"0\" validity=\"1\" value=\"%s\"/></parameters></method>",
          frequency,
          seqID++,
          trueHeading, timestamp,
          groundSpeed, timestamp,
          latitude, timestamp, timestamp,
          longitude, timestamp, timestamp,
          ADBPDateNowUTC(),
          ADBPTimeNowUTC()
          );
  return message;
}

char *NMEATimeNowUTC(void) {

  static char _nmea_time_now_utc_[16];
  double timenow = SystemUnixTimeNow();
  time_t timeonly = (time_t)timenow % (60 * 60 * 24);
  time_t hours = timeonly / (60 * 60);
  time_t minssecs = timeonly % (60 * 60);
  time_t mins = minssecs / 60;
  time_t secs = minssecs % 60;
  double subsecs = timenow - floor(timenow);
  snprintf(_nmea_time_now_utc_,16,"%02d%02d%02d%.3f",(int)hours,(int)mins,(int)secs,subsecs);
  return _nmea_time_now_utc_;
};

char *NMEADateNowUTC(void) {

  static char _nmea_date_now_utc_[16];
  time_t timenow = (time_t)SystemUnixTimeNow();
  struct tm ts;
#if defined(_WIN32)
  if(!gmtime_s(&ts,&timenow)) {
#else
    if(gmtime_r(&timenow,&ts)!=NULL) {
#endif
      snprintf(_nmea_date_now_utc_,16,"%02d%02d%02d",(int)ts.tm_mday,(int)ts.tm_mon+1,((int)ts.tm_year+1900) % 100);
  } else {
    _nmea_date_now_utc_[0]='\0';
  }
  return _nmea_date_now_utc_;
};

char *NMEARMCMessage(double latitude, double longitude, double groundSpeed, double trueTrack, double magVar) {
  
  static char message[256];
  snprintf(message,256,"$GPRMC,%s,A,%02.5f,%c,%03.5f,%c,%.3f,%.2f,%s,%.5f,%c,D,S",
           NMEATimeNowUTC(),
           abs(latitude),
           latitude < 0 ? 'S' : 'N',
           abs(longitude),
           longitude < 0 ? 'W' : 'E',
           groundSpeed,
           trueTrack,
           NMEADateNowUTC(),
           abs(magVar),
           magVar < 0 ? 'W' : 'E');
  NMEAAddChecksumTo(message);
  strcat(message,"\n");
  return message;
}

char *NMEAGLLMessage(double latitude, double longitude) {
  static char message[256];
  snprintf(message,256,"$GPGLL,%02.5f,%c,%03.5f,%c,%s,A,D",
           abs(latitude),
           latitude < 0 ? 'S' : 'N',
           abs(longitude),
           longitude < 0 ? 'W' : 'E',
           NMEATimeNowUTC());
  NMEAAddChecksumTo(message);
  strcat(message,"\n");
  return message;
}

char *NMEAHDTMessage(double trueHeading) {
  static char message[256];
  snprintf(message,256,"$GPHDT,%0.3f,T",trueHeading);
  NMEAAddChecksumTo(message);
  strcat(message,"\n");
  return message;
}

char NMEAChecksumOf(char *message) {
  char checksum=message[1];
  char *cp=&message[1];
  while(*cp!='\0' && *cp!='*') {
    if(*(cp+1)!='\0') {
      checksum ^= *(cp+1);
    }
    cp++;
  }
  return checksum;
}

void NMEAAddChecksumTo(char *message) {
  char checksum = NMEAChecksumOf(message);
  size_t mlen = strlen(message);
  if(message[mlen-1] != '*') {
    strcat(message,"*");
  }
  char checksumString[4];
  snprintf(checksumString,4,"%02X", checksum);
  strcat(message,checksumString);
}

