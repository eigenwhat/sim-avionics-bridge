//
//  Utilities.h
//  sim-avionics-bridge
//
//  Created by Joao Cabral on 03/11/2024.
//

#ifndef UTILITIES_H
#define UTILITIES_H

double SystemUnixTimeNow(void);


char *ADBPTimeNowUTC(void);
char *ADBPDateNowUTC(void);
char *ADBPMessage(int frequency, double trueHeading, double groundSpeed, double latitude, double longitude);

char *NMEATimeNowUTC(void);
char *NMEADateNowUTC(void);
char *NMEARMCMessage(double latitude, double longitude, double groundSpeed, double trueTrack, double magVar);
char *NMEAGLLMessage(double latitude, double longitude);
char *NMEAHDTMessage(double trueHeading);
char NMEAChecksumOf(char *message);
void NMEAAddChecksumTo(char *message);

#endif /* UTILITIES_H */
