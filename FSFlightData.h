//
//  FSFlightData.h
//  sim-avionics-bridge
//
//  Created by Joao Cabral on 03/11/2024.
//

#ifndef FSFLIGHTDATA_H
#define FSFLIGHTDATA_H

#define FSFLIGHTDATA_SENT_UPDATES_PER_SECOND 10.0

struct FSFlightDataFields {
  char title[256];
  double latitude;
  double longitude;
  double altitudeMSL;
  double trueHeading;
  double trueTrack;
  double magVar;
  double groundSpeed;
};

class FSFlightData {
  
private:
  
  struct FSFlightDataFields fields;
  
public:
  
  FSFlightData(void);
  
  void updateFields(FSFlightDataFields *fields);
  
  char *title(void);
  double latitude(void);
  double longitude(void);
  double altitudeMSL(void);
  double trueHeading(void);
  double trueTrack(void);
  double magVar(void);
  double groundSpeed(void);
  char latitudeNorthSouth(void);
  char longitudeEastWest(void);
  char magVarEastWest(void);

  char *ADBPMSG(void);
  char *NMEARMCMSG(void);
  char *NMEAGLLMSG(void);
  char *NMEAHDTMSG(void);
};

#endif
