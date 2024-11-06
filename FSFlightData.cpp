//
//  FSFlightData.cpp
//  sim-avionics-bridge
//
//  Created by Joao Cabral on 03/11/2024.
//

#include "FSFlightData.h"
#include "Utilities.h"

#include <cstring>
#include <cstdio>
#include <cmath>

FSFlightData::FSFlightData() {
  memset(&fields,0,sizeof(struct FSFlightDataFields));
}

void FSFlightData::updateFields(FSFlightDataFields *newFields) {
  memcpy(&fields, newFields, sizeof(struct FSFlightDataFields));
}

char *FSFlightData::title(void) {
  return fields.title;
}

double FSFlightData::latitude(void) {
  return fields.latitude;
}

double FSFlightData::longitude(void) {
  return fields.longitude;
}

double FSFlightData::altitudeMSL(void) {
  return fields.altitudeMSL;
}

double FSFlightData::trueHeading(void) {
  return fields.trueHeading;
}

double FSFlightData::trueTrack(void) {
  return fields.trueTrack * 57.2957795;
}

double FSFlightData::magVar(void) {
  return fields.magVar;
}

double FSFlightData::groundSpeed(void) {
  return fields.groundSpeed;
}

char FSFlightData::latitudeNorthSouth(void) {
  if (fields.latitude < 0) {
    return 'S';
  }
  return 'N';
}

char FSFlightData::longitudeEastWest(void) {
  if (fields.longitude < 0) {
    return 'W';
  }
  return 'E';
}

char FSFlightData::magVarEastWest(void) {
  if (fields.magVar < 0) {
    return 'W';
  }
  return 'E';
}

char *FSFlightData::ADBPMSG(void) {
  return ADBPMessage((int)FSFLIGHTDATA_SENT_UPDATES_PER_SECOND * 10, trueHeading(),groundSpeed(),latitude(),longitude());
}

char *FSFlightData::NMEARMCMSG(void) {
  return NMEARMCMessage(latitude(),longitude(),groundSpeed(),trueTrack(),magVar());
}

char *FSFlightData::NMEAGLLMSG(void) {
  return NMEAGLLMessage(latitude(), longitude());
}

char *FSFlightData::NMEAHDTMSG(void) {
  return NMEAHDTMessage(trueHeading());
}

