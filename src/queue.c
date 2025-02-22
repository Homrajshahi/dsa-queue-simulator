#include <stdlib.h>
#include "queue.h"
#include "vehicle.h"

Queue* createQueue(Direction direction, LanePosition lane) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) return NULL;

    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->direction = direction;
    queue->lane = lane;
    queue->waitingTime = 0;

    // Set priority lane flags based on lane and direction
    queue->isPriorityLane = (lane == LANE_CENTER) &&
                           (direction == DIRECTION_NORTH || // AL2
                            direction == DIRECTION_SOUTH || // CL2
                            direction == DIRECTION_EAST ||  // BL2
                            direction == DIRECTION_WEST);   // DL2

    // Initialize all items to NULL
    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        queue->items[i] = NULL;
    }
    return queue;
}

bool isEmpty(Queue* queue) {
    return (queue->size == 0);
}

bool isFull(Queue* queue) {
    return (queue->size == MAX_QUEUE_SIZE);
}

void enqueue(Queue* queue, Vehicle* vehicle) {
    if (!queue || !vehicle || isFull(queue)) return;

    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->items[queue->rear] = vehicle;
    queue->size++;

    // Update vehicle's priority lane status
    vehicle->isPriorityLane = queue->isPriorityLane;

    // Adjust waiting time based on vehicle type and lane
    if (vehicle->type > 0) { // Emergency vehicles
        queue->waitingTime += 2;
    } else if (queue->isPriorityLane && queue->size > 5) {
        queue->waitingTime += 1.5; // Priority lanes with high congestion
    } else {
        queue->waitingTime += 1;
    }
}

Vehicle* dequeue(Queue* queue) {
    if (!queue || isEmpty(queue)) return NULL;

    Vehicle* vehicle = queue->items[queue->front];
    queue->items[queue->front] = NULL;
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->size--;

    // Update waiting time based on vehicle type
    if (vehicle) {
        if (vehicle->type > 0) {
            queue->waitingTime = fmax(0, queue->waitingTime - 2);
        } else if (queue->isPriorityLane && queue->size > 5) {
            queue->waitingTime = fmax(0, queue->waitingTime - 1.5);
        } else {
            queue->waitingTime = fmax(0, queue->waitingTime - 1);
        }
    }

    return vehicle;
}

int getSize(Queue* queue) {
    return queue ? queue->size : 0;
}

Vehicle* peekFront(Queue* queue) {
    if (!queue || isEmpty(queue)) return NULL;
    return queue->items[queue->front];
}
