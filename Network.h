//
//  Network.h
//  sim-avionics-bridge
//
//  Created by Joao Cabral on 03/11/2024.
//

#ifndef NETWORK_H
#define NETWORK_H

int InitializeUDPInterface(const char *destAddress);

void ShutdownUDPInterface(void);

ssize_t SendUDPPacket(void *data, size_t len, int port);

#endif /* NETWORK_H */
