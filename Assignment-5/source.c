/* Assignment-4
 * CS29003
 * Name: Aditya Narayan
 * Roll No: 13EC30001
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define PARTICLE_COUNT 5
#define MIN_RADIUS 0.02
#define MAX_RADIUS 0.05
#define TIME_LIMIT 10.0
#define DELTA 0.001

double sim_time = 0;
const double time_inc = 0.010;

FILE *gnuplotPipe;
FILE *logFile[PARTICLE_COUNT];
FILE *gnuplotGraphPipe[PARTICLE_COUNT];

typedef enum COLOR{
    black = 0,
    red = 16711680 ,
    orange = 16753755,
    yellow = 16776705,
    green = 65280 ,
    blue = 255,
    indigo = 4915380,
    violet = 15630956,
}COLOR;

inline COLOR getCOLOR(int index)
{
    int rgb[] = {0, 16711680, 16753755, 16776705, 65280, 255, 4915380, 15630956};
    return rgb[index];
}


inline double randomdouble(double min, double max) {
    double random = ((double) rand()) / (double) RAND_MAX;
    double diff = max - min;
    double r = random * diff;
    return min + r;
}

typedef struct Ball {
    double tstamp;
    int id;
    double px, py, vx, vy, radius;
    COLOR colour;
}Ball;

inline Ball *initBallRandom(int id)
{
    Ball *ball = (Ball *)malloc(sizeof(Ball));
    ball->id = id;
    ball->tstamp = sim_time;
    ball->px = randomdouble(0.1, 0.9);
    ball->py = randomdouble(0.1, 0.9);
    ball->vx = randomdouble(-0.5, 0.5);
    ball->vy = randomdouble(-0.5, 0.5);
    ball->radius = MIN_RADIUS;
    ball->colour = getCOLOR(rand() % 8);
    return ball;
}

inline Ball *initBall(int id, double px, double py, double vx, double vy, double radius, COLOR colour)
{
    Ball *ball = (Ball *)malloc(sizeof(Ball));
    ball->id = id;
    ball->tstamp = sim_time;
    ball->px = px;
    ball->py = py;
    ball->vx = vx;
    ball->vx = vy;
    ball->radius = radius;
    ball->colour = colour;
    return ball;
}

typedef struct Interaction{
    double tstamp;
    Ball *interactee;
    Ball *interactor;
    int interactor_collision_count;       
}Interaction;

typedef struct LinkedList{
    Interaction *event;
    struct LinkedList *next;
    struct LinkedList *prev;
}LinkedList;

LinkedList *initLinkedListObject(Interaction *event)
{
    LinkedList *linkedListObject = (LinkedList *)malloc(sizeof(LinkedList));
    linkedListObject->event = event;
    linkedListObject->next = NULL;
    linkedListObject->prev = NULL;
    return linkedListObject;
}

typedef struct Heap{
    int *node;
    LinkedList **list;
    int *collision_count;
}Heap;

inline Heap *initHeap()
{
    int i;
    Heap *heap = (Heap *)malloc(sizeof(Heap));
    heap->node = (int *)malloc(sizeof(int) * PARTICLE_COUNT);
    heap->list = (LinkedList **)malloc(sizeof(LinkedList *) * PARTICLE_COUNT);
    heap->collision_count = (int *)malloc(sizeof(int) * PARTICLE_COUNT);
    for(i = 0; i < PARTICLE_COUNT; i++){
        heap->collision_count[i] = 0;
        heap->list[i] = NULL;
        heap->node[i] = -1;
    }
    return heap;
}

inline int heapify(Heap *heap, int interactee)
{
	int index = PARTICLE_COUNT + interactee;
	while(index/2){
		if(heap->node[index] == -1){
			heap->node[index] = interactee;	
		}
		else if(heap->list[heap->node[index]]){
			if(heap->list[heap->node[index]]->event->tstamp > heap->list[interactee]->event->tstamp){
				heap->node[index] = interactee;	
			}
		}
		index /= 2;
	}
}

LinkedList *calcMinima(Heap *heap, LinkedList *list)
{
    if(list == NULL){
        return NULL;
    }
    while(list && list->event->interactor != NULL && list->event->interactor_collision_count != heap->collision_count[list->event->interactor->id]){
        list = list->next;
    }
    LinkedList *minima = list;
    LinkedList *head = list;
    while(list != NULL){
        if(list->event->interactor != NULL){
            if(list->event->interactor_collision_count != heap->collision_count[list->event->interactor->id]){
                if(list->prev != NULL){
                    list->prev->next = list->next;
                }
                if(list->next != NULL){
                    list->next->prev = list->prev;
                }
                continue;
            }
        }
        if(list->event->tstamp < minima->event->tstamp){
            minima = list;
        }
        list = list->next;
    }
    if(minima == NULL){
        return minima;
    }
    if(minima->prev != NULL){
        minima->prev->next = minima->next;
    }
    if(minima->next != NULL){
        minima->next->prev = minima->prev;
    }
    while(head == minima){
        head = head->next;
    }
    minima->next = head;
    minima->prev = NULL;
    return minima;
}

inline int insertToHeap(Heap *heap, Interaction *event)
{
    if(heap == NULL || event == NULL){
        return -1;
    }
    LinkedList *linkedListObject = initLinkedListObject(event);
    if(heap->list[event->interactee->id] == NULL){
        heap->list[event->interactee->id] = linkedListObject;
        linkedListObject->next = NULL;
    }
    else{
        if(heap->list[event->interactee->id]->event->tstamp > event->tstamp){
            linkedListObject->next = heap->list[event->interactee->id];
            heap->list[event->interactee->id] = linkedListObject;
            linkedListObject->next->prev = linkedListObject;
        }
        else{
            linkedListObject->next = heap->list[event->interactee->id]->next;
            linkedListObject->prev = heap->list[event->interactee->id];
            if(linkedListObject->next != NULL){
                linkedListObject->next->prev = linkedListObject;
            }
            heap->list[event->interactee->id]->next = linkedListObject;   
        }
    }
    return 0;
}

inline int removeFromHeap(Heap *heap, int interactee)
{
    if(heap == NULL){
        return -1;
    }
    heap->list[interactee] = NULL;
    heap->collision_count[interactee] = heap->collision_count[interactee] + 1;
    return 0;
}

inline Interaction *getNextEvent(Heap *heap)
{
    int i, min_index;
    Interaction *nextEvent;
    while(1){
        for(i = 0; i != PARTICLE_COUNT; i++){
            if(heap->list[i] != NULL){
                nextEvent = heap->list[i]->event;
                min_index = i;
                break;
            }
        }
        if(min_index == PARTICLE_COUNT){
            return NULL;
        }
        for(i = min_index + 1; i != PARTICLE_COUNT; i++){
            if(heap->list[i] != NULL){
                if(heap->list[i]->event->tstamp < nextEvent->tstamp){
                    nextEvent = heap->list[i]->event;
                    min_index = i;
                }
            }
        }
        if(nextEvent->interactor != NULL && nextEvent->interactor_collision_count != heap->collision_count[nextEvent->interactor->id]){
            heap->list[min_index] = calcMinima(heap, heap->list[min_index]);
        }
        else{
            return nextEvent;
        }
    }
}

inline FILE *initPipe()
{
    FILE *gnuplotPipe;
    gnuplotPipe = popen("gnuplot", "w");
    return gnuplotPipe;
}

inline FILE *initGraph(int ball_id)
{
    FILE *graphPlotPipe;
    graphPlotPipe = popen("gnuplot", "w");
    fprintf(graphPlotPipe, "set terminal latex\n");
    fprintf(graphPlotPipe, "set output \"ball-%d.tex\"\n", ball_id);
    fprintf(graphPlotPipe, "set xrange [0:1]\n");
   	fprintf(graphPlotPipe, "set yrange [-0.01:1.01]\n");
   	fprintf(graphPlotPipe, "plot '-' with lines\n");
    return graphPlotPipe;
}

inline FILE *initGraphPipe(int ball_id)
{
    FILE *graphPlotPipe;
    graphPlotPipe = popen("gnuplot -persistent", "w");
    fprintf(graphPlotPipe, "set xrange [0:1]\n");
   	fprintf(graphPlotPipe, "set yrange [-0.01:1.01]\n");
   	fprintf(graphPlotPipe, "plot '-' with lines\n");
    return graphPlotPipe;
}

inline int drawGraph(Ball **ball)
{
    if(ball == NULL){
        return -1;
    }
    int i;
    for(i = 0; i < PARTICLE_COUNT; i++){
        fprintf(logFile[i], "%lf %lf\n", ball[i]->px, ball[i]->py);
        fprintf(gnuplotGraphPipe[i], "%lf %lf\n", ball[i]->px, ball[i]->py);
        fflush(logFile[i]);
        fflush(gnuplotGraphPipe[i]);
    }
    return 0;
}

inline int saveGraph(Ball **ball)
{
    if(ball == NULL){
        return -1;
    }
    int i;
    for(i = 0; i < PARTICLE_COUNT; i++){
        fprintf(logFile[i], "e");
        fflush(logFile[i]);
    }
    return 0;
}

inline int showGraph(Ball **ball)
{
    if(ball == NULL){
        return -1;
    }
    int i;
    for(i = 0; i < PARTICLE_COUNT; i++){
        fprintf(gnuplotGraphPipe[i], "e");
        fflush(gnuplotGraphPipe[i]);
    }
    return 0;
}

inline int drawBalls(Ball **ball)
{
    if(ball == NULL){
        return -1;
    }
    int i, flag_write;
    fprintf(gnuplotPipe, "set xrange [0:1]\n");
    fprintf(gnuplotPipe, "set yrange [-0.01:1.01]\n");
    fprintf(gnuplotPipe, "plot '-' with circles lc rgb variable fs transparent fill solid noborder\n");
    for(i = 0; i < PARTICLE_COUNT; i++){
        flag_write = fprintf(gnuplotPipe, "%lf %lf %lf %d\n", ball[i]->px, ball[i]->py, ball[i]->radius - DELTA, ball[i]->colour);
        if(!flag_write){
            return -1;
        }
    }
    fprintf(gnuplotPipe, "e\n");
    fflush(gnuplotPipe);
    return 0;
}

inline int advanceBalls(Ball **ball, double now)
{
    if(ball == NULL){
        return -1;
    }
    int i;
    for(i = 0; i < PARTICLE_COUNT; i++){
        ball[i]->px += ball[i]->vx * (now - ball[i]->tstamp);
        ball[i]->py += ball[i]->vy * (now - ball[i]->tstamp);
        ball[i]->tstamp = now;
        if((ball[i]->px - ball[i]->radius) <= 0 || (ball[i]->px + ball[i]->radius) >= 1.0){
            ball[i]->vx = -ball[i]->vx;
        }
        if((ball[i]->py - ball[i]->radius) <= 0 || (ball[i]->py + ball[i]->radius) >= 1.0){
            ball[i]->vy = -ball[i]->vy;
        }
    }
    return 0;
}

inline int simulateTo(Ball **ball, double now)
{
    if(ball == NULL){
        return -1;
    }
    while(sim_time <= now){
        sim_time += time_inc;
        advanceBalls(ball, sim_time);
        drawBalls(ball);
        drawGraph(ball);
        usleep(time_inc * 1000000);
    }
    return 0;
}

inline Interaction *eventBallCollide(Heap *heap, Ball *ball_i, Ball *ball_j)
{
    if(ball_i == ball_j){
        return NULL;
    }
    double relPX = ball_j->px - ball_i->px;
    double relPY = ball_j->py - ball_i->py;
    double relVX = ball_j->vx - ball_i->vx;
    double relVY = ball_j->vy - ball_i->vy;
    double relVrelP = relPX * relVX + relPY * relVY;
    if(relVrelP >= 0){
        return NULL;
    }
    double relVrelV = relVX * relVX + relVY * relVY;
    double relPrelP = relPX * relPX + relPY * relPY;
    double sigma = ball_j->radius + ball_i->radius + DELTA;
    double d = (relVrelP * relVrelP) - relVrelV * (relPrelP - sigma*sigma);
    if(d < 0){
        return NULL;
    }
    double timeToCollision = -(relVrelP + sqrt(d)) / relVrelV;
    if(timeToCollision > 10){
        return NULL;
    }

    Interaction *collisionEvent = (Interaction *)malloc(sizeof(Interaction));
    collisionEvent->tstamp = sim_time + timeToCollision;
    collisionEvent->interactee = ball_i;
    collisionEvent->interactor = ball_j;
    collisionEvent->interactor_collision_count = heap->collision_count[ball_j->id];
    return collisionEvent;
}

inline Interaction *eventWallCollideY(Heap *heap, Ball *ball)
{
    if(ball == NULL){
        return NULL;
    }
    double timeToCollision;
    if(ball->vy > 0){
        timeToCollision = (1.0 - ball->radius - ball->py)/ball->vy;
    }
    else{
        timeToCollision = fabs((ball->py - ball->radius)/ball->vy);
    }
    if(timeToCollision > 10){
        return NULL;
    }
    Interaction *collisionEvent = (Interaction *)malloc(sizeof(Interaction));
    collisionEvent->interactee = ball;
    collisionEvent->interactor = NULL;
    collisionEvent->tstamp = sim_time + timeToCollision;
    collisionEvent->interactor_collision_count = heap->collision_count[ball->id];
    return collisionEvent;
}


inline Interaction *eventWallCollideX(Heap *heap, Ball *ball)
{
    if(ball == NULL){
        return NULL;
    }
    double timeToCollision;
    if(ball->vx> 0){
        timeToCollision = (1.0 - ball->radius - ball->px)/ball->vx;
    }
    else{
        timeToCollision = fabs((ball->px - ball->radius)/ball->vx);
    }
    if(timeToCollision > 10){
        return NULL;
    }
    Interaction *collisionEvent = (Interaction *)malloc(sizeof(Interaction));
    collisionEvent->interactee = ball;
    collisionEvent->interactor = NULL;
    collisionEvent->tstamp = sim_time + timeToCollision;
    collisionEvent->interactor_collision_count = heap->collision_count[ball->id];
    return collisionEvent;
}


inline int resolveCollision(Interaction *collisionEvent)
{
    if(collisionEvent == NULL){
        return -1;
    }
    Ball *ball_i, *ball_j;
    ball_i = collisionEvent->interactee;
    ball_j = collisionEvent->interactor;
    if(ball_j != NULL){
        double relPX = ball_j->px - ball_i->px;
        double relPY = ball_j->py - ball_i->py;
        double relVX = ball_j->vx - ball_i->vx;
        double relVY = ball_j->vy - ball_i->vy;
        double relVrelP = relPX * relVX + relPY * relVY;
        double sigma = ball_j->radius + ball_i->radius;
        double J = relVrelP / sigma;
        double Jx = J * relPX / sigma;
        double Jy = J * relPY / sigma;
        ball_i->vx += Jx;
        ball_i->vy += Jy;
        ball_j->vx -= Jx;
        ball_j->vy -= Jy;
    }
    return 0;
}

inline int scheduleEvent(Ball **ball, Heap *heap, int index)
{
    int j;
    insertToHeap(heap, eventWallCollideX(heap, ball[index]));
    insertToHeap(heap, eventWallCollideY(heap, ball[index]));
    for(j = 0; j < PARTICLE_COUNT; j++){
        insertToHeap(heap, eventBallCollide(heap, ball[index], ball[j]));
    }
}

int main()
{
    int i, j;
    srand(time(NULL));

    Heap *heap;
    Ball **ball;
    Interaction *nextCollision;

    heap = initHeap();
    ball = (Ball **)malloc(PARTICLE_COUNT * sizeof(Ball *));
    gnuplotPipe = initPipe();

    for(i = 0; i < PARTICLE_COUNT; i++){
    	logFile[i] = initGraph(i);
    	gnuplotGraphPipe[i] = initGraphPipe(i);
    }
    
    for(i = 0; i < PARTICLE_COUNT; i++){
        ball[i] = initBallRandom(i);
        insertToHeap(heap, eventWallCollideX(heap, ball[i]));
        insertToHeap(heap, eventWallCollideY(heap, ball[i]));
        for(j = 0; j < i; j++){
            insertToHeap(heap, eventBallCollide(heap, ball[i], ball[j]));
        }
    }

    while(sim_time < TIME_LIMIT){
        nextCollision = getNextEvent(heap);
        simulateTo(ball, nextCollision->tstamp);
        printf("Collision at t=%lf\n", sim_time);
        resolveCollision(nextCollision);
        removeFromHeap(heap, nextCollision->interactee->id);
        scheduleEvent(ball, heap, nextCollision->interactee->id);
        if(nextCollision->interactor != NULL){
            removeFromHeap(heap, nextCollision->interactor->id);
            scheduleEvent(ball, heap, nextCollision->interactor->id);
        }
    }

    fprintf(gnuplotPipe, "quit\n");
    saveGraph(ball);
    showGraph(ball);
    return 0;
}