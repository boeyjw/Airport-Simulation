/* BOEY JIAN WEN 014545 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Airport.h"

int main(int argc, char* argv[])
{
    puts("Initialising program...");
    FILE* planeEntry = fopen(argv[1], "r"); //Open file to read and store all DEPARTURE available planes
    struct Planes* runway1h[2] = {NULL, NULL}; //Pointer to head of the linked list for runway 1
    struct Planes* runway1t[2] = {NULL, NULL}; //Pointer to tail of the linked list for runway 1
    struct Planes* runway2h[2] = {NULL, NULL}; //Pointer to head of the linked list for runway 2
    struct Planes* runway2t[2] = {NULL, NULL}; //Pointer to tail of the linked list for runway 2
    /* runway1/2h/t[0] is always pointing to a LANDING queue
     * runway1/2h/t[1] is always pointing to a DEPARTURE queue
     */
    struct Planes* tempLandingQueue[2] = {NULL, NULL};
     /* tempLandingQueue[0] points to the HEAD
      * tempLandingQueue[1] points to the TAIL
      */

    puts("Opening and reading from file...");
    if(planeEntry == NULL){
        perror("File input error");
        exit(EXIT_FAILURE); //Since there is no file input, terminate program
    }

    char buffer[30] = {'\0'}; //Stores file input
    char* tempBuffer = buffer; //Stores file buffer temporarily to avoid destroying the original file buffer, fail safe due to lack of strtok_r
    char planeSerial[8] = {'\0'}; //Stores file string per line
    unsigned int pasOnBoard = 0, leftFuel = 0;
    int itHours = 0, itMinutes = 0;
    int eLandTime = 0;
    int initHours = 0, initMinutes = 0; //Initial time is 0000 hours, to be adjusted to 1 minute before first aircraft L or T time
    bool runway1DHasOne = false, tempLHasOne = false, runwayHasOne = true; //Boolean to initialise simulation time
    size_t runwayElement[4] = {0};
    /* runwayElement[0] is for LANDING on RUNWAY 1
     * runwayElement[1] is for DEPARTURE on RUNWAY 1
     * runwayElement[2] is for LANDING on RUNWAY 2
     * runwayElement[3] is for DEPARTURE on RUNWAY 2
     */

    puts("Adding all planes...");
    //Read from file and enqueue all DEPARTURE planes schedule onto runways and all LANDING planes into temporary queue
    while((fgets(buffer, sizeof(buffer), planeEntry) != NULL) || (!feof(planeEntry))) //Get input line by line from file
    {
        //puts(buffer); /* DEBUG */
        strcpy(tempBuffer, buffer); //Temporarily hold buffer to avoid destroying original buffer

        if(runway1DHasOne && tempLHasOne && runwayHasOne) //Initialise simulation time, only accessed ONCE
        {
            initTime(runway1h[1], itHours, itMinutes, &initHours, &initMinutes);
            runwayHasOne = false;
        }

        if(buffer[0] == 'T') //If take off
        {
            tokenise(tempBuffer, planeSerial, &itHours, &itMinutes, &pasOnBoard, &eLandTime, &leftFuel, true); //Tokenise input string

            if(planeExist(runway1h[1], planeSerial) || planeExist(runway2h[1], planeSerial)) //If plane already exist in system, ignore line
            {
                printf("%s is already queued for departure\n", planeSerial);
                memset(planeSerial, '\0', sizeof(planeSerial));
                memset(buffer, '\0', sizeof(buffer));
                continue;
            }

            //Enqueues planes into respective runway, planes are enqueued based on length of queue on each runway
            if(isEmpty(runway1h[1]))
            {
                runwayElement[1]++;
                enqueuePlanesDeparture(&runway1h[1], &runway1t[1], planeSerial, itHours, itMinutes, pasOnBoard, 1);
                runway1DHasOne = true;
            }
            else if(isEmpty(runway2h[1]))
            {
                runwayElement[3]++;
                enqueuePlanesDeparture(&runway2h[1], &runway2t[1], planeSerial, itHours, itMinutes, pasOnBoard, 2);
            }
            else if(runwayElement[1] > runwayElement[3])
            {
                runwayElement[3]++;
                enqueuePlanesDeparture(&runway2h[1], &runway2t[1], planeSerial, itHours, itMinutes, pasOnBoard, 2);
            }
            else
            {
                runwayElement[1]++;
                enqueuePlanesDeparture(&runway1h[1], &runway1t[1], planeSerial, itHours, itMinutes, pasOnBoard, 1);
            }
        }
        else if(buffer[0] == 'L') //If landing
        {
            tokenise(tempBuffer, planeSerial, &itHours, &itMinutes, &pasOnBoard, &eLandTime, &leftFuel, false); //Tokenise input string
            if(planeExist(tempLandingQueue[0], planeSerial)) //If plane is already in system, ignore line
            {
                printf("%s is already queued for landing\n", planeSerial);
                memset(planeSerial, '\0', sizeof(planeSerial));
                memset(buffer, '\0', sizeof(buffer));
                continue;
            }
            enqueuePlanesTemp(&tempLandingQueue[0], &tempLandingQueue[1], planeSerial, itHours, itMinutes, pasOnBoard, eLandTime, leftFuel);
            tempLHasOne = true;
        }
        else
        {
            puts("Input is neither 'T' nor 'L', skipping input..."); //If plane is neither take off of landing, ignore
        }
        memset(planeSerial, '\0', sizeof(planeSerial));
        memset(buffer, '\0', sizeof(buffer));
    }
    puts("All planes added");
    fclose(planeEntry); //End reading file
    puts("Closing file and creating log file...");
    strtok(argv[1], ".");
    strtok(NULL, ".");
    strcat(argv[1], "Log.txt"); //Append xxxxLog.txt
    FILE* airportLog = fopen(argv[1], "w"); //Open xxxxLog.txt for writing
    struct FlightHistory* flightRoot[2] = {NULL, NULL};
    /* flightRoot[0] always points to HEAD
     * flightRoot[1] always points to TAIL
     */
    int endTick = 0, emerRunway = 0, waitTime = 0, i = 0;
    char emerSerial[10], sSerial[12], usSerial[12];

    if(airportLog == NULL)
    {
        perror("File write error:");
    }
    else
    {
        printf("Created %s file...\nLaunching simulator...\n", argv[1]);
    }

    puts("Welcome to Airport Simulator (2 runways) Version 1.41"); //Obligatory welcome message

    while(airportIsNotEmpty(runway1h[1], runway1h[0], runway2h[1], runway2h[0], tempLandingQueue[0])) //If airport is empty, end simulation
    {
        endTick++; //Simulation begins with at least 1 minute
        if(endTick > 1441) //Simulate 1 day
        {
            printf("%d minutes has passed\nSimulation forced exit\n", endTick - 1);
            fprintf(airportLog, "%d minutes has passed\nSimulation forced exit\n", endTick - 1);
            break;
        }
        ticktock(&initHours, &initMinutes);
        waitnfuelEdit(&runway1h[1]);
        waitnfuelEdit(&runway1h[0]);
        waitnfuelEdit(&runway2h[1]);
        waitnfuelEdit(&runway2h[0]);
        waitnfuelEdit(&tempLandingQueue[0]);


        if(!isEmpty(tempLandingQueue[0])) //Enqueue landing planes if temporary queue is not empty
        {
            while(toEnqueueLanding(tempLandingQueue[0])) //Check to see if planes is already in range and ready for landing
            {
                if(isEmpty(runway1h[0]))
                {
                    runwayElement[0]++;
                    enqueuePlanesLanding(&runway1h[0], &runway1t[0], tempLandingQueue[0], 1);
                }
                else if(isEmpty(runway2h[0]))
                {
                    runwayElement[2]++;
                    enqueuePlanesLanding(&runway2h[0], &runway2t[0], tempLandingQueue[0], 2);
                }
                else if(runwayElement[0] > runwayElement[2])
                {
                    runwayElement[2]++;
                    enqueuePlanesLanding(&runway2h[0], &runway2t[0], tempLandingQueue[0], 2);
                }
                else
                {
                    runwayElement[0]++;
                    enqueuePlanesLanding(&runway1h[0], &runway1t[0], tempLandingQueue[0], 1);
                }
                dequeuePlanes(&tempLandingQueue[0], &tempLandingQueue[1]);
            }
        }
        puts("============================================================================================"); //Separator between each minute
        fputs("============================================================================================", airportLog);
        printf("%s", "\nCurrent time is ");
        fprintf(airportLog, "\nCurrent time is ");
        printTime(airportLog, initHours, initMinutes);
        printRunwayQueue(airportLog, runway1h[1], runway1h[0], 1);
        printRunwayQueue(airportLog, runway2h[1], runway2h[0], 2);

        //Emergency dequeue
        if(isEmergency(runway1h[0], runway2h[0], &emerRunway, runwayElement[0], runwayElement[2])) //If any landing planes have 0 units of fuel, declare plane as emergency
        {
            if(emerRunway == 1)
            {
                emerPlaneSerial(airportLog, runway1h[0], emerSerial, initHours, initMinutes);
                flightLog(&flightRoot[0], &flightRoot[1], runway1h[0], true, initHours, initMinutes);
                dequeueEmergency(&runway1h[0], &runway1t[0], emerSerial);
                runwayElement[0]--;
            }
            else if(emerRunway == 2)
            {
                emerPlaneSerial(airportLog, runway2h[0], emerSerial, initHours, initMinutes);
                flightLog(&flightRoot[0], &flightRoot[1], runway2h[0], true, initHours, initMinutes);
                dequeueEmergency(&runway2h[0], &runway2t[0], emerSerial);
                runwayElement[2]--;
            }
            memset(emerSerial, '\0', sizeof(emerSerial));
        }
        else //If no emergencies, do normal take off and landings on each runway
        {
            //Runway 1 dequeue
            waitTime = compareWaitTime(runway1h[1], runway1h[0], runwayElement[1], runwayElement[0], initHours, initMinutes);
            if(waitTime != 0)
            {
                if(waitTime == 1 && departOnSchedule(runway1h[1], initHours, initMinutes))
                {
                    flightLog(&flightRoot[0], &flightRoot[1], runway1h[1], false, initHours, initMinutes);
                    printDequeueDepart(airportLog, runway1h[1], initHours, initMinutes);
                    dequeuePlanes(&runway1h[1], &runway1t[1]);
                    runwayElement[1]--;
                }
                else if(waitTime == 2)
                {
                    flightLog(&flightRoot[0], &flightRoot[1], runway1h[0], false, initHours, initMinutes);
                    printDequeueLand(airportLog, runway1h[0], initHours, initMinutes);
                    dequeuePlanes(&runway1h[0], &runway1t[0]);
                    runwayElement[0]--;
                }
            }

            //Runway 2 dequeue
            waitTime = compareWaitTime(runway2h[1], runway2h[0], runwayElement[3], runwayElement[2], initHours, initMinutes);
            if(waitTime != 0)
            {
                if(waitTime == 1 && departOnSchedule(runway2h[1], initHours, initMinutes))
                {
                    flightLog(&flightRoot[0], &flightRoot[1], runway2h[1], false, initHours, initMinutes);
                    printDequeueDepart(airportLog, runway2h[1], initHours, initMinutes);
                    dequeuePlanes(&runway2h[1], &runway2t[1]);
                    runwayElement[3]--;
                }
                else if(waitTime == 2)
                {
                    flightLog(&flightRoot[0], &flightRoot[1], runway2h[0], false, initHours, initMinutes);
                    printDequeueLand(airportLog, runway2h[0], initHours, initMinutes);
                    dequeuePlanes(&runway2h[0], &runway2t[0]);
                    runwayElement[2]--;
                }
            }
        }

        //Status check
        while(1)
        {
            puts("Status check?");
            printf("%s", "Input method (S MH 1234) to check/(N) to skip checking: ");
            fgets(sSerial, 11, stdin);

            if(toupper(sSerial[0]) == 'N')
            {
                break;
            }
            else if(toupper(sSerial[0]) == 'S')
            {
                i = 0;
                while(sSerial[i + 2] != '\0')
                {
                    usSerial[i] = toupper(sSerial[i + 2]);
                    i++;
                }
                usSerial[i - 1] = '\0';

                if(planeExistInLog(flightRoot[0], usSerial))
                {
                    statusPrintLog(airportLog, flightRoot[0], usSerial);
                }
                else if(planeExist(runway1h[1], usSerial))
                {
                    statusPrintDeparture(airportLog, runway1h[1], usSerial);
                }
                else if(planeExist(runway1h[0], usSerial))
                {
                    statusPrintLanding(airportLog, runway1h[0], usSerial);
                }
                else if(planeExist(runway2h[1], usSerial))
                {
                    statusPrintDeparture(airportLog, runway2h[1], usSerial);
                }
                else if(planeExist(runway2h[0], usSerial))
                {
                    statusPrintLanding(airportLog, runway2h[0], usSerial);
                }
                else if(planeExist(tempLandingQueue[0], usSerial))
                {
                    statusPrintOutRange(airportLog, tempLandingQueue[0], usSerial);
                }
                else
                {
                    printf("%s is not in system\n", usSerial);
                }
                memset(usSerial, '\0', sizeof(usSerial));
                continue;
            }
            else
            {
                puts("Invalid input");
                continue;
            }
        }
    }

    puts("\nAirport is now empty!");
    fputs("\nAirport is now empty!\n", airportLog);
    puts("Thank you for using Airport Simulator (2 runways) Version 1.40!");
    fputs("Thank you for using Airport Simulator (2 runways) Version 1.40!\n", airportLog);

    fclose(airportLog); //End writing file
    return 0;
}
