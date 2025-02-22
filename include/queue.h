#ifndef QUEUE_H
#define QUEUE_H

#include "vehicle.h"

#define MAX_QUEUE_SIZE 100
#define SAFE_DISTANCE 20

typedef struct {
    Vehicle* items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
    Direction direction;    // Associated direction (N,E,S,W)
    LanePosition lane; // Associated lane (Left,Center,Right)
    float waitingTime; // Cumulative waiting time for priority handling
    bool isPriorityLane; // Whether this is a priority lane (e.g., AL2)
} Queue;

// Queue operations
Queue* createQueue(Direction direction, LanePosition lane);
bool isEmpty(Queue* queue);
bool isFull(Queue* queue);
void enqueue(Queue* queue, Vehicle* vehicle);
Vehicle* dequeue(Queue* queue);
int getSize(Queue* queue);
Vehicle* peekFront(Queue* queue);

#endif // QUEUE_H
