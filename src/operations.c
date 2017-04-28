/* BOEY JIAN WEN 014545 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Airport.h"

void tokenise(char* tmpbuf, char pSerial[], int* h, int* m, unsigned int* pas, int* estiLT, unsigned int* rFuel, bool isDepart)
{
    char* tmpToken; //Initial multi-usage char* variable to store tokens

    tmpToken = strtok(tmpbuf, " "); //Tokenise to get company aircraft type
    tmpToken = strtok(NULL, " ");
    strcat(pSerial, tmpToken);
    strcat(pSerial, " ");
    tmpToken = strtok(NULL, " "); //Tokenise to get company aircraft type serial
    strcat(pSerial, tmpToken); //Concatenate XX0000

    tmpToken = strtok(NULL, " "); //Temporarily stores the time (hours) token
    *h = strtol(tmpToken, NULL, 10); //Convert to int for time (hours)

    tmpToken = strtok(NULL, " "); //Temporarily stores the time (minutes) token
    *m = strtol(tmpToken, NULL, 10); //Convert to int for time (minutes)

    if(isDepart)
    {
        tmpToken = strtok(NULL, " "); //Temporarily stores the number of passengers on board in the aircraft
        *pas = strtol(tmpToken, NULL, 10); //Convert to int for number of passengers on board
    }
    else
    {
        tmpToken = strtok(NULL, " ");
        *estiLT = strtol(tmpToken, NULL, 10);
        tmpToken = strtok(NULL, " ");
        *rFuel = strtol(tmpToken, NULL, 10);
        tmpToken = strtok(NULL, " "); //Temporarily stores the number of passengers on board in the aircraft
        *pas = strtol(tmpToken, NULL, 10); //Convert to int for number of passengers on board
    }
}

bool planeExist(struct Planes* ptrHead, const char* pSerial)
{
    while(ptrHead != NULL)
    {
        if((strcmp(ptrHead->airline, pSerial) == 0))
        {
            return true;
        }
        ptrHead = ptrHead->ptrNext;
    }
    return false;
}

bool isEmpty(struct Planes* ptrRunway)
{
    return ptrRunway == NULL;
}

bool isHistEmpty(struct FlightHistory* ptrHistory)
{
    return ptrHistory == NULL;
}

bool airportIsNotEmpty(struct Planes* ptrRunway1D, struct Planes* ptrRunway1L, struct Planes* ptrRunway2D, struct Planes* ptrRunway2L, struct Planes* ptrTemp)
{
    return (!(ptrRunway1D == NULL && ptrRunway1L == NULL && ptrRunway2D == NULL && ptrRunway2L == NULL && ptrTemp == NULL)); //Returns 0 if airport is empty, 1 if at least 1 plane is left
}

bool toEnqueueLanding(struct Planes* ptrTempHead)
{
    while(ptrTempHead != NULL)
    {
        if(ptrTempHead->landELT == 0)
        {
            return true;
        }
        ptrTempHead = ptrTempHead->ptrNext;
    }
    return false;
}

void printRunwayQueue(FILE* wr,struct Planes* ptrHeadD, struct Planes* ptrHeadL, const int run)
{
    printf("Runway %d\n", run);
    fprintf(wr, "Runway %d\n", run);
    puts("\tTake off queue:");
    fputs("\tTake off queue:\n", wr);
    if(isEmpty(ptrHeadD))
    {
        puts("\t<EMPTY>\n");
        fputs("\t<EMPTY>\n", wr);
    }
    else
    {
        while(ptrHeadD != NULL)
        {
            if(ptrHeadD->isD)
            {
                printf("\t%s in queue for %d minute(s) and is scheduled to depart on ", ptrHeadD->airline, ptrHeadD->waitingTime);
                fprintf(wr, "\t%s in queue for %d minute(s) and is scheduled to depart on ", ptrHeadD->airline, ptrHeadD->waitingTime);
                printTime(wr, ptrHeadD->hours, ptrHeadD->minutes);
            }
            ptrHeadD = ptrHeadD->ptrNext;
        }
    }

    puts("\n\tLanding queue:");
    fputs("\n\tLanding queue:\n", wr);
    if(isEmpty(ptrHeadL))
    {
        puts("\t<EMPTY>\n");
        fputs("\t<EMPTY>\n", wr);
    }
    else
    {
        while(ptrHeadL != NULL)
        {
            if(!ptrHeadL->isD)
            {
                printf("\t%s in queue for %d minute(s) and is scheduled to land on ", ptrHeadL->airline, ptrHeadL->waitingTime);
                fprintf(wr, "\t%s in queue for %d minute(s) and is scheduled to land on ", ptrHeadL->airline, ptrHeadL->waitingTime);
                printTime(wr, ptrHeadL->hours, ptrHeadL->minutes);
                printf("\t%d minute(s) of fuel remaining\n", ptrHeadL->fuel);
                fprintf(wr, "\t%d minute(s) of fuel remaining\n", ptrHeadL->fuel);
            }
            ptrHeadL = ptrHeadL->ptrNext;
        }
    }
}

bool isEmergency(struct Planes* ptrR1, struct Planes* ptrR2, int* run, const size_t re0, const size_t re2)
{
    if(ptrR1 == NULL && ptrR2 == NULL)
    {
        return false;
    }

    size_t count1 = 0, count2 = 0;
    bool ptr1 = false, ptr2 = false;

    while(ptrR1 != NULL)
    {
        count1++;
        if(ptrR1->fuel == 0)
        {
            ptr1 = true;
            break;
        }
        ptrR1 = ptrR1->ptrNext;
    }

    while(ptrR2 != NULL)
    {
        count2++;
        if(ptrR2->fuel == 0)
        {
            ptr2 = true;
            break;
        }
        ptrR2 = ptrR2->ptrNext;
    }

    if(ptr1 && ptrR2 == NULL)
    {
        *run = 1;
        return true;
    }
    else if(ptr2 && ptrR1 == NULL)
    {
        *run = 2;
        return true;
    }
    else if(re0 == re2 && (ptr1 && ptr2))
    {
        if(count1 == count2 || count1 < count2)
        {
            *run = 1;
        }
        else
        {
            *run = 2;
        }
        return true;
    }
    else if(re0 > re2 && (ptr1 && ptr2))
    {
        *run = 2;
        return true;
    }
    else if(re0 < re2 && (ptr1 && ptr2))
    {
        *run = 1;
        return true;
    }
    else
    {
        return false;
    }
}

void emerPlaneSerial(FILE* wr, struct Planes* ptrHead, char* epSerial, int tH, int tM)
{
    while(ptrHead != NULL)
    {
        if(ptrHead->fuel == 0)
        {
            strncpy(epSerial, ptrHead->airline, strlen(ptrHead->airline));
            break;
        }
        ptrHead = ptrHead->ptrNext;
    }

    printf("%s with %d passengers made an emergency landing on runway %u at ", ptrHead->airline, ptrHead->passengers, ptrHead->runway);
    fprintf(wr, "%s with %d passengers made an emergency landing on runway %u at ", ptrHead->airline, ptrHead->passengers, ptrHead->runway);
    printTime(wr, tH, tM);
    printf("%s", "Expected arrival time was ");
    fprintf(wr, "Expected arrival time was ");
    printTime(wr, ptrHead->hours, ptrHead->minutes);
}

int compareWaitTime(struct Planes* ptr1D, struct Planes* ptr1L, const size_t re1, const size_t re0, int h, int m)
{
    if(ptr1D == NULL && ptr1L == NULL)
    {
        return 0; //If there are no planes in the runway
    }

    if(ptr1D != NULL && ptr1L == NULL) //If no planes in landing queue, dequeue the departure queue by default
    {
        return 1;
    }
    else if(ptr1D == NULL && ptr1L != NULL) //If no planes in departure queue, dequeue the landing queue by default
    {
        return 2;
    }
    else if(ptr1D->waitingTime == ptr1L->waitingTime) //If both queue has planes, compare their time in queue, shortest time will be dequeued
    {
        if(re1 < re0 || (ptr1L->hours < h || (ptr1L->hours == h && ptr1L->minutes <= m)))
        {
            return 2;
        }
        else
        {
            return 1;
        }
    }
    else if(ptr1D->waitingTime < ptr1L->waitingTime || (ptr1L->hours < h || (ptr1L->hours == h && ptr1L->minutes <= m)))
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

void printDequeueDepart(FILE* wr, struct Planes* ptrHead, int tH, int tM)
{
    printf("%s took off on runway %d at ", ptrHead->airline, ptrHead->runway);
    fprintf(wr, "%s took off on runway %d at ", ptrHead->airline, ptrHead->runway);
    printTime(wr, tH, tM);
    printf("%s", "Expected departure time was ");
    fprintf(wr, "Expected departure time was ");
    printTime(wr, ptrHead->hours, ptrHead->minutes);
    puts("");
    fputs("", wr);
}

void printDequeueLand(FILE* wr, struct Planes* ptrHead, int tH, int tM)
{
    printf("%s with %u passengers landed on runway %d at ", ptrHead->airline, ptrHead->passengers, ptrHead->runway);
    fprintf(wr, "%s with %u passengers landed on runway %d at ", ptrHead->airline, ptrHead->passengers, ptrHead->runway);
    printTime(wr, tH, tM);
    printf("%s", "Expected landing time was ");
    fprintf(wr, "Expected landing time was ");
    printTime(wr, ptrHead->hours, ptrHead->minutes);
    puts("");
    fputs("", wr);
}

void statusPrintLog(FILE* wr,struct FlightHistory* ptrHist, char* pSerial)
{
    while(ptrHist != NULL)
    {
        if(strcmp(ptrHist->airline, pSerial) == 0)
        {
            break;
        }
        ptrHist = ptrHist->ptrNext;
    }

    if(ptrHist->isD)
    {
        printf("Status: %s with %u passengers took off at ", ptrHist->airline, ptrHist->passengers);
        fprintf(wr, "Status: %s with %u passengers took off at ", ptrHist->airline, ptrHist->passengers);
    }
    else if(!ptrHist->isD && ptrHist->emergencyLanding)
    {
        printf("Status: %s with %u passengers made an emergency landing at ", ptrHist->airline, ptrHist->passengers);
        fprintf(wr, "Status: %s with %u passengers made an emergency landing at ", ptrHist->airline, ptrHist->passengers);
    }
    else
    {
        printf("Status: %s with %u passengers landed with %d units of fuel remaining at ", ptrHist->airline, ptrHist->passengers, ptrHist->fuel);
        fprintf(wr, "Status: %s with %u passengers landed with %d units of fuel remaining at ", ptrHist->airline, ptrHist->passengers, ptrHist->fuel);
    }
    printTime(wr, ptrHist->hours, ptrHist->minutes);
}

void statusPrintDeparture(FILE* wr, struct Planes* ptrHead, char* pSerial)
{
    while(ptrHead != NULL)
    {
        if(strcmp(ptrHead->airline, pSerial) == 0)
        {
            break;
        }
        ptrHead = ptrHead->ptrNext;
    }

    printf("Status: %s is queued to take off on runway %d\n", ptrHead->airline, ptrHead->runway);
    fprintf(wr, "Status: %s is queued to take off on runway %d\n", ptrHead->airline, ptrHead->runway);
}

void statusPrintLanding(FILE* wr,struct Planes* ptrHead, char* pSerial)
{
    while(ptrHead != NULL)
    {
        if(strcmp(ptrHead->airline, pSerial) == 0)
        {
            break;
        }
        ptrHead = ptrHead->ptrNext;
    }

    printf("Status: %s is queued to land on runway %d and has %d minutes of fuel remaining\n", ptrHead->airline, ptrHead->runway, ptrHead->fuel);
    fprintf(wr, "Status: %s is queued to land on runway %d and has %d minutes of fuel remaining\n", ptrHead->airline, ptrHead->runway, ptrHead->fuel);
}

bool planeExistInLog(struct FlightHistory* ptrHist, const char* pSerial)
{
    while(ptrHist != NULL)
    {
        if(strcmp(ptrHist->airline, pSerial) == 0)
        {
            return true;
        }
        ptrHist = ptrHist->ptrNext;
    }

    return false;
}

bool departOnSchedule(struct Planes* ptrHead, int h, int m)
{
    if(ptrHead->isD)
    {
        if(ptrHead->hours < h || ptrHead->minutes <= m)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

void statusPrintOutRange(FILE* wr, struct Planes* ptrTemp, char* pSerial)
{
    while(ptrTemp != NULL)
    {
        if(strcmp(ptrTemp->airline, pSerial) == 0)
        {
            break;
        }
        ptrTemp = ptrTemp->ptrNext;
    }

    printf("Status: %s is will be in range in %d minutes and has %d units of fuel\n", ptrTemp->airline, ptrTemp->landELT, ptrTemp->fuel);
    fprintf(wr, "Status: %s is will be in range in %d minutes and has %d units of fuel\n", ptrTemp->airline, ptrTemp->landELT, ptrTemp->fuel);
}
