/* BOEY JIAN WEN 014545 */
#ifndef AIRPORT_H_INCLUDED
#define AIRPORT_H_INCLUDED
#include <stdbool.h>

struct Planes
{
    char* airline;
    int hours;
    int minutes;
    unsigned int passengers;
    bool isD;
    int landELT;
    int fuel;
    bool emergencyLanding;
    size_t runway;
    int waitingTime;
    struct Planes* ptrNext;
};

struct FlightHistory
{
    char* airline;
    int hours;
    int minutes;
    unsigned int passengers;
    bool isD;
    int fuel;
    bool emergencyLanding;
    size_t runway;
    struct FlightHistory* ptrNext;
};

//timemanipulation.c
void initTime(struct Planes* ,int ,int ,int* ,int*); //Initialise simulation time
void ticktock(int* ,int* ); //Adds 1 minute on completion of all operations
void printTime(FILE* ,int ,int ); //Prints simulated time
void waitnfuelEdit(struct Planes** ); //Increase waiting time for each plane in queue, decrease in range time, and decrease fuel on each landing planes

//operations.c
void tokenise(char* ,char* ,int* ,int* ,unsigned int* ,int* ,unsigned int* ,bool ); //Tokenise file buffer input
bool planeExist(struct Planes* ,const char* ); //Check queues to see whether specified plane serial is already in any queue
bool isEmpty(struct Planes* ); //Checks specified plane queue
bool isHistEmpty(struct FlightHistory* ); //Check if none of the planes have landed or took off
bool airportIsNotEmpty(struct Planes* ,struct Planes* ,struct Planes* ,struct Planes* ,struct Planes* ); //Check whether airport has been cleared
bool toEnqueueLanding(struct Planes* ); //Check if landing plane is in range
void printRunwayQueue(FILE* ,struct Planes* ,struct Planes* ,const int ); //Prints runway queue status
bool isEmergency(struct Planes* ,struct Planes* ,int* ,const size_t ,const size_t ); //Check if emergency landing is required and get it's assigned runway
void emerPlaneSerial(FILE* ,struct Planes* ,char* ,int ,int); //Prints emergency status and get emergency plane landing serial number
int compareWaitTime(struct Planes* ,struct Planes* ,const size_t ,const size_t ,int ,int); //Determine the plane to land or take off based on their queueing time, landing has priority
void printDequeueDepart(FILE* ,struct Planes* ,int ,int ); //Prints status of departed planes
void printDequeueLand(FILE* ,struct Planes* ,int ,int ); //Prints status of landed planes without emergency
void statusPrintLog(FILE* ,struct FlightHistory* ,char* ); //Prints status of departed or landed planes from log
void statusPrintDeparture(FILE* ,struct Planes* ,char* ); //Prints status of departed planes
void statusPrintLanding(FILE* ,struct Planes* ,char* ); //Prints status of landed planes
bool planeExistInLog(struct FlightHistory* ,const char* ); //Check if plane has already departed or landed and is off the runway
bool departOnSchedule(struct Planes* ,int ,int ); //Departure is always on time or later, never earlier
void statusPrintOutRange(FILE* ,struct Planes* ,char* ); //Landing planes out of range but control tower surely knows

//endequeue.c
void flightLog(struct FlightHistory** ,struct FlightHistory** ,struct Planes* ,bool ,int ,int ); //Stores departed and landed planes
void enqueuePlanesTemp(struct Planes** ,struct Planes** ,const char* ,int ,int ,unsigned int ,int ,unsigned int ); //Stores all LANDING planes temporarily, not in runway
void enqueuePlanesLanding(struct Planes** ,struct Planes** ,struct Planes* ,size_t ); //When landing planes are in range, planes are assigned to their runway
void enqueuePlanesDeparture(struct Planes** ,struct Planes** ,const char* ,int ,int ,unsigned int ,size_t ); //Stores all DEPARTURE aircraft information
void dequeuePlanes(struct Planes** ,struct Planes** ); //Normal dequeue-ing of planes, used in both temporary queue and normal dequeue
void dequeueEmergency(struct Planes** ,struct Planes** ,char* ); //Dequeue emergency landing planes

#endif // AIRPORT_H_INCLUDED
