#include <commons.h>

sem_t securityControl;
pthread_mutex_t securityControlLock;

sem_t boardingGate;
pthread_mutex_t boardingGateLock;

Plane plane[MAX_NUM_OF_PLANES];

int currentPlane;
int currentPassenger;