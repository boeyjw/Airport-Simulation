/* BOEY JIAN WEN 014545 */
#include <stdio.h>
#include "Airport.h"

void initTime(struct Planes* ptrHeadR1D , int hl, int ml, int* h, int* m)
{
    if(ptrHeadR1D->hours == hl && ptrHeadR1D->minutes == ml) //If comparison between first departure and landing plane has same scheduled time
    {
        if(ml - 1 < 0)
        {
            if(ptrHeadR1D->hours - 1 < 0)
            {
                *h = 23;
            }
            else
            {
                *h = ptrHeadR1D->hours - 1;
            }
            *m = 60 + (ml - 1);
        }
        else
        {
            *h = ptrHeadR1D->hours;
            *m = ml - 1;
        }
    }
    else if(ptrHeadR1D->hours == hl) //If comparison between first departure and landing plane has same scheduled hours
    {
        if(ptrHeadR1D->minutes > ml)
        {
            if(ml - 1 < 0)
            {
                if(hl - 1 < 0)
                {
                    *h = 23;
                }
                else
                {
                    *h = hl - 1;
                }
                *m = 60 + (ml - 1);
            }
            else
            {
                *h = hl;
                *m = ml - 1;
            }
        }
        else if(ptrHeadR1D->minutes < ml)
        {
            if(ptrHeadR1D->minutes - 1 < 0)
            {
                if(ptrHeadR1D->hours - 1 < 0)
                {
                    *h = 23;
                }
                else
                {
                    *h = ptrHeadR1D->hours - 1;
                }
                *m = 60 + (ptrHeadR1D->minutes - 1);
            }
            else
            {
                *h = ptrHeadR1D->hours;
                *m = ptrHeadR1D->minutes - 1;
            }
        }
    }
    else if(ptrHeadR1D->hours < hl || ptrHeadR1D->hours > hl)
    {
        if(ptrHeadR1D->hours > hl)
        {
            if(ml - 1 < 0)
            {
                if(hl - 1 < 0)
                {
                    *h = 23;
                }
                else
                {
                    *h = hl - 1;
                }
                *m = 60 + (ml - 1);
            }
            else
            {
                *h = hl;
                *m = ml - 1;
            }
        }
        else
        {
            if(ptrHeadR1D->minutes - 1 < 0)
            {
                if(ptrHeadR1D->hours - 1 < 0)
                {
                    *h = 23;
                }
                else
                {
                    *h = ptrHeadR1D->hours - 1;
                }
                *m = 60 + ((int)ptrHeadR1D->minutes - 1);
            }
            else
            {
                *h = ptrHeadR1D->hours;
                *m = ptrHeadR1D->minutes - 1;
            }
        }
    }
}

void ticktock(int* h, int* m)
{
    if(*m + 1 == 60)
    {
        if(*h + 1 == 24)
        {
            *h = 0;
        }
        else
        {
            *h += 1;
        }
        *m = 0;
    }
    else
    {
        *m += 1;
    }
}

void printTime(FILE* wr, int h, int m)
{
    if(h <= 9 && m <= 9)
    {
        printf("0%d:0%d ", h, m);
        fprintf(wr, "0%d:0%d \n", h, m);
    }
    else if(h >= 9 && m <= 9)
    {
        printf("%d:0%d ", h, m);
        fprintf(wr, "%d:0%d \n", h, m);
    }
    else if(h <= 9 && m >= 9)
    {
        printf("0%d:%d ", h, m);
        fprintf(wr, "0%d:%d \n", h, m);
    }
    else
    {
        printf("%d:%d ", h, m);
        fprintf(wr, "%d:%d \n", h, m);
    }
    puts("");
}

void waitnfuelEdit(struct Planes** ptrRunway)
{
    if(*ptrRunway == NULL)
    {
        return;
    }

    struct Planes* ptrCur;
    ptrCur = *ptrRunway;

    if(ptrCur->isD)
    {
        while(ptrCur != NULL)
        {
            ptrCur->waitingTime += 1;
            ptrCur = ptrCur->ptrNext;
        }
    }
    else if(ptrCur->landELT > 0)
    {
        while(ptrCur != NULL)
        {
            if(ptrCur->landELT > 0)
            {
                ptrCur->landELT -= 1;
            }
            if(ptrCur->fuel > 0)
            {
                ptrCur->fuel -= 1;
            }
            ptrCur = ptrCur->ptrNext;
        }
    }
    else
    {
        while(ptrCur != NULL)
        {
            ptrCur->waitingTime += 1;
            if(ptrCur->fuel > 0)
            {
                ptrCur->fuel -= 1;
            }
            ptrCur = ptrCur->ptrNext;
        }
    }
}
