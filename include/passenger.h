#ifndef PASSENGER_H
#define PASSENGER_H

void passenger_process(int shmID, int semID, int passengerID);
int verify_baggage(int weight, int isVip);

#endif