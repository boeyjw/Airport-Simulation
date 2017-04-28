/* BOEY JIAN WEN 014545 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Airport.h"

void enqueuePlanesTemp(struct Planes** ptrHead, struct Planes** ptrTail, const char* pSerial, int h, int m, unsigned int pas, int elt, unsigned int rFuel)
{
    struct Planes* ptrNew = malloc(sizeof(struct Planes));
    struct Planes* ptrPrev;
    struct Planes* ptrCur;

    if(ptrNew != NULL)
    {
        ptrNew->airline = malloc(sizeof(pSerial) + 1);
        strcpy(ptrNew->airline, pSerial); //Store plane serial
        ptrNew->hours = h; //Store scheduled landing time, hour
        ptrNew->minutes = m; //Store scheduled landing time, minutes
        ptrNew->passengers = pas; //Stored number of passengers on board
        ptrNew->isD = false; //Boolean to check whether plane is a departure plane, landing planes are false by default
        ptrNew->landELT = elt; //Store "in range" time
        ptrNew->fuel = rFuel; //Store amount of fuel left

        if(rFuel > 0) //Chances are planes in range may lack fuel
        {
            ptrNew->emergencyLanding = false;
        }
        else
        {
            ptrNew->emergencyLanding = true;
        }

        ptrNew->ptrNext = NULL;
        ptrPrev = NULL;
        ptrCur = *ptrHead;

        while(ptrCur != NULL && elt > ptrCur->landELT) //Arrange planes base don "in range" time in ascending order
        {
            ptrPrev = ptrCur;
            ptrCur = ptrCur->ptrNext;
        }

        if(ptrPrev == NULL)
        {
            ptrNew->ptrNext = *ptrHead;
            *ptrHead = ptrNew;
        }
        else
        {
            ptrPrev->ptrNext = ptrNew;
            ptrNew->ptrNext = ptrCur;
            (*ptrTail)->ptrNext = ptrNew;
        }

        *ptrTail = ptrNew;
    }
    else
    {
        puts("Out of memory");
    }

}

void enqueuePlanesLanding(struct Planes** ptrHead, struct Planes** ptrTail, struct Planes* ptrTemp, size_t run)
{
    struct Planes* ptrNew = malloc(sizeof(struct Planes));

    if(ptrNew != NULL)
    {
        ptrNew->airline = malloc(sizeof(ptrTemp->airline) + 1);
        strcpy(ptrNew->airline, ptrTemp->airline); //Store plane serial
        ptrNew->hours = ptrTemp->hours; //Store scheduled landing time, hours
        ptrNew->minutes = ptrTemp->minutes; //Store scheduled landing time, minutes
        ptrNew->passengers = ptrTemp->passengers; //Store number of passengers on board
        ptrNew->isD = false; //Boolean to check whether plane is a departure plane, landing planes are false by default
        ptrNew->fuel = ptrTemp->fuel; //Store amount of fuel left in plane
        ptrNew->runway = run; //Store assigned runway for plane
        ptrNew->waitingTime = 0; //Store the time the plane is on queue, 0 by default when just assigned runway
        ptrNew->emergencyLanding = ptrTemp->emergencyLanding; //Store boolean on whether plane is on emergency status
        ptrNew->ptrNext = NULL;

        if(isEmpty(*ptrHead))
        {
            *ptrHead = ptrNew;
        }
        else
        {
            (*ptrTail)->ptrNext = ptrNew;
        }

        *ptrTail = ptrNew;
    }
    else
    {
        puts("Out of memory");
    }
}

void enqueuePlanesDeparture(struct Planes** ptrHead, struct Planes** ptrTail, const char* pSerial, int h, int m, unsigned int pas, size_t run)
{
    struct Planes* ptrNew = malloc(sizeof(struct Planes));
    struct Planes* ptrPrev;
    struct Planes* ptrCur;

    if(ptrNew != NULL)
    {
        ptrNew->airline = malloc(sizeof(pSerial) + 1);
        strcpy(ptrNew->airline, pSerial); //Store plane serial
        ptrNew->hours = h; //Store plane scheduled depart time, hours
        ptrNew->minutes = m; //Store plane scheduled depart time, minutes
        ptrNew->passengers = pas; //Store number of passengers on board
        ptrNew->isD = true; //Boolean to check whether plane is a departure plane, dequeue planes are true by default
        ptrNew->runway = run; //Store assigned runway
        ptrNew->waitingTime = 0; //Store the time the plane is on queue, 0 by default when just assigned runway

        ptrNew->ptrNext = NULL;
        ptrPrev = NULL;
        ptrCur = *ptrHead;

        while(ptrCur != NULL && ((h == ptrCur->hours && m > ptrCur->minutes) || h > ptrCur->hours)) //Store departure planes based on scheduled departure time in ascending order
        {
            ptrPrev = ptrCur;
            ptrCur = ptrCur->ptrNext;
        }

        if(ptrPrev == NULL)
        {
            ptrNew->ptrNext = *ptrHead;
            *ptrHead = ptrNew;
        }
        else
        {
            ptrPrev->ptrNext = ptrNew;
            ptrNew->ptrNext = ptrCur;
            (*ptrTail)->ptrNext = ptrNew;
        }

        *ptrTail = ptrNew;
    }
    else
    {
        puts("Out of memory");
    }
}

void dequeuePlanes(struct Planes** ptrTempHead, struct Planes** ptrTempTail) //Dequeue the first plane on queue
{
    struct Planes* ptrTemp;

    ptrTemp = *ptrTempHead;
    *ptrTempHead = (*ptrTempHead)->ptrNext;
    free(ptrTemp);

    if(*ptrTempHead == NULL)
    {
        *ptrTempTail = NULL;
    }
}

void dequeueEmergency(struct Planes** ptrHeadL, struct Planes** ptrTailL, char* pSerial) //Searches for emergency status plane in landing queue and dequeue them
{
    struct Planes* ptrTemp;
    struct Planes* ptrPrev;
    struct Planes* ptrCur;

    if((strcmp(pSerial, (*ptrHeadL)->airline) == 0))
    {
        ptrTemp = *ptrHeadL;
        *ptrHeadL = (*ptrHeadL)->ptrNext;
        free(ptrTemp);
    }
    else
    {
        ptrPrev = *ptrHeadL;
        ptrCur = (*ptrHeadL)->ptrNext;

        while(ptrCur != NULL && (strcmp(pSerial, (*ptrHeadL)->airline) == 0))
        {
            ptrPrev = ptrCur;
            ptrCur = ptrCur->ptrNext;
        }

        if(ptrCur != NULL)
        {
            ptrTemp = ptrCur;
            ptrPrev->ptrNext = ptrCur->ptrNext;
            free(ptrTemp);
        }
    }

    if(*ptrHeadL == NULL)
    {
        *ptrTailL = NULL;
    }


}

void flightLog(struct FlightHistory** ptrRoot, struct FlightHistory** ptrTailRoot, struct Planes* ptrHead, bool isEmer, int tH, int tM)
{
    struct FlightHistory* ptrNew = malloc(sizeof(struct FlightHistory));

    if(ptrNew != NULL)
    {
        ptrNew->airline = malloc(sizeof(ptrHead->airline) + 1);
        strcpy(ptrNew->airline, ptrHead->airline); //Store plane serial
        ptrNew->hours = tH; //Store plane scheduled landing/departure time, hours
        ptrNew->minutes = tM; //Store plane scheduled landing/departure time, minutes
        ptrNew->passengers = ptrHead->passengers; //Store number of passengers on board
        ptrNew->isD = ptrHead->isD; //Determines whether plane has landed or departed
        ptrNew->runway = ptrHead->runway; //Store assigned runway
        if(!ptrHead->isD) //If it's landed plane, store fuel left and emergency status
        {
            ptrNew->fuel = ptrHead->fuel;
            ptrNew->emergencyLanding = isEmer;
        }
        ptrNew->ptrNext = NULL;

        if(isHistEmpty(*ptrRoot))
        {
            *ptrRoot = ptrNew;
        }
        else
        {
            (*ptrTailRoot)->ptrNext = ptrNew;
        }

        *ptrTailRoot = ptrNew;
    }
    else
    {
        puts("Out of memory");
    }
}
