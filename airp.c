#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>
#include<stdbool.h>
#include <math.h>
//max elements in queue
#define MAX 3
//enum to know if a plane arrivess or departs
typedef enum action_tag {ARRIVE, DEPART} Action_type;

//user defined data type for plane
typedef struct plane_tag
{
    int id; // identification of plane
    int tm; //time of arrival in queue
}Plane_type;

//queue struct usinf linked list
typedef struct queue
{
    int count; //counter
    int front;
    int rear;
    Plane_type p[MAX];//entry
}Queue_type;

//initialization of queue
void Initialize(Queue_type *qptr) //pq = plane in queue
{
    qptr->count = 0;
    qptr->front = 0;
    qptr->rear = -1;
}

void AddQueue(Plane_type item, Queue_type *ptrq)
{
    //overflow
    if (ptrq->count >= MAX) 
        printf("queue full\n");

    ptrq->count++; //counter
    //enqueue
    ptrq->rear = (ptrq->rear + 1) % MAX;
    ptrq->p[ptrq->rear] = item;
}

//dequeue
void DeleteQueue(Plane_type *item, Queue_type *ptrq)
{
    if (ptrq->count <= 0)
    {
       printf("\nQueue is empty.\n");
       return;
    }
    else
    {
        ptrq->count--;
        *item = ptrq->p[ptrq->front];
        ptrq->front = (ptrq->front + 1) % MAX;
    }
}

int Size(Queue_type *qptr)
{
    return qptr->count; //returns size of current queue
}
//boolean type to check if queue is empty or not
bool Empty(Queue_type *qptr)
{
    return (qptr->count <= 0);
}
//boolean type to check if queue is full or not
bool Full(Queue_type *qptr)
{
    return (qptr->count >= MAX);
}
//generates starting point foe pseudo-random number
void Randomize()
{
    srand((unsigned int) (time(NULL)%10000));
}

//Poisson Distribution for pseudo-random number
int RandomNum(double expectedValue)
{
    int n = 0;
    double em; //limit
    double x;
    em = exp(-expectedValue);//exponential
    x = rand()/(double) RAND_MAX; //0-32767
    while (x>em)
    {
        n++;
        x *= rand()/(double)RAND_MAX;
    }
    return n;
    
}

//Prints initail messages and initializes parameters
void Start(int *endtime, double *expectarrive, double *expectdepart)
{
    bool ok;

    printf("\nProgram that simulates an airport with only one runway.\n");
    printf("One plane can land or depart in each unit of time.\n");
    printf("Up to %d planes can be waiting to land or take off at any time.\n", MAX);
    printf("\nHow many units of time will the simulation run? ");
    scanf("%d", endtime);
    Randomize();
    do
    {
        printf("\nExpected number of arrivals per unit time? ");
        scanf("%lf", expectarrive);
        printf("\nExpected number of departures per unit time? ");
        scanf("%lf", expectdepart);

        if (*expectarrive < 0.0 || *expectdepart < 0.0)
        {
            printf("These numbers must be nonnegative.\n");
            ok = false;
        }
        else if (*expectarrive + *expectdepart > 1.0)
            {
                char c;
                printf("The airport will become saturated. Enter new numbers? ");
                scanf("%c", &c);
                if (c == 'y' || c == 'Y')
                ok = true;
                else ok = false;
                
            }
            else
                ok = true;
    } while (ok == false);
}
//make records for planes and update total number of planes
void NewPlane(Plane_type *p, int *nplanes, int curtime, Action_type kind)
{
    (*nplanes)++;
    p->id = *nplanes;
    p->tm = curtime;
    switch (kind)
    {
    case ARRIVE:
        printf("\tPlane %3d ready to land.\n", *nplanes);
        break;
    
    case DEPART:
        printf("\tPlane %3d ready to take off\n", *nplanes);
        break;
    }
}
//takes action if queue is full
void Refuse(Plane_type p, int *nrefuse, Action_type kind)
{
    switch (kind)
    {
    case ARRIVE:
        printf("\tPlane %3d directed to another airport.\n", p.id);
        break;
    
    case DEPART:
        printf("\tPlane %3d was told to try later.\n", p.id);
        break;
    }
    (*nrefuse)++;
}
//processes planes that are landing
void Land(Plane_type p, int curtime, int *nland, int *landwait)
{
    int wait;
    wait = curtime + p.tm;
    printf("%3d: Plane %3d landed; in queue %d units.\n", curtime, p.id, wait);
    (*nland)++;
    *landwait += wait;
}
//processes planes that are taking off
void Fly(Plane_type p, int curtime, int *ntakeoff, int *takeoffwait)
{
    int wait;
    wait = curtime + p.tm;
    printf("%3d: Plane %3d took off; in queue %d units.\n", curtime, p.id, wait);
    (*ntakeoff)++;
    *takeoffwait += wait;
}
//add time if runway is idle
void Idle(int curtime, int *idletime)
{
    printf("%3d : Runway is idle,\n", curtime);
    (*idletime)++;
}
//concludes program with printing results.
void Conclude(int nplanes, int nland, int ntakeoff, int nrefuse, int landwait, int takeoffwait, int idletime, int endtime, Queue_type *pt, Queue_type *pl)
{
    printf("Simulation has concluded after %d units.\n", endtime);
    printf("Total number of planes processed:  %3d\n", nplanes);
    printf(" Number of planes landed:          %3d\n", nland);
    printf(" Number of planes taken off:       %3d\n", ntakeoff);
    printf(" Number of plane refused use:      %3d\n", nrefuse);
    printf(" Number left ready to land:        %3d\n", Size(pl));
    printf(" Number left ready to take off:    %3d\n", Size(pt));
    if(endtime > 0)
        printf(" Percentage of time runway idle:   %6.2f\n",((double)idletime/endtime) * 100.0);
    if(nland > 0)
        printf(" Average wait time to land:        %6.2f\n", (double)landwait/nland);
    if(ntakeoff > 0)
        printf(" Average wait time to take off:    %6.2f\n", (double)takeoffwait/ntakeoff);

}

int main()
{
    //srand(time(NULL));
    Queue_type landing, takeoff;
    Queue_type *pl = &landing;
    Queue_type *pt = &takeoff;
    Plane_type plane;
    int curtime, endtime, i, idletime, landwait, nland, nplanes, nrefuse, ntakeoff, pri, takeoffwait;
    double expectarrive, expectdepart;

    Initialize(pl);
    Initialize(pt);
    nplanes = nland = ntakeoff = nrefuse = 0;
    landwait = takeoffwait = idletime = 0;

    Start(&endtime, &expectarrive, &expectdepart);
    for (curtime = 1; curtime <= endtime; curtime++)
    {
        pri = RandomNum(expectarrive); //psudo-random number for arrival
        for (i = 1; i <= pri; i++)
        {
            NewPlane(&plane, &nplanes, curtime, ARRIVE);
            if (Full(pl))
                Refuse(plane, &nrefuse, ARRIVE);
            else AddQueue(plane, pl);            
        }
        pri = RandomNum(expectdepart);//psudo-random number for departure
        for (i = 1; i <=pri; i++)
        {
            NewPlane(&plane, &nplanes, curtime, DEPART);
            if (Full(pt))
                Refuse(plane, &nrefuse, DEPART);
            else AddQueue(plane, pt); 
            
        }
        if (!Empty(pl))
        {
            DeleteQueue(&plane, pl);
            Land(plane, curtime, &nland, &landwait);
        } else if (!Empty(pt))
        {
            DeleteQueue(&plane, pt);
            Fly(plane, curtime, &ntakeoff, &takeoffwait);
        } else Idle(curtime, &idletime);        
    }
    Conclude(nplanes, nland, ntakeoff, nrefuse, landwait, takeoffwait, idletime, endtime, pt, pl);
    return 0;
}