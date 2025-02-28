#include <stdlib.h>
#include <math.h>
#include "queue.h"

Queue* createQueue(Direction direction, LanePosition lane) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) return NULL;

    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->direction = direction;
    queue->lane = lane;
    queue->waitingTime = 0;

    queue->isPriorityLane = (lane == LANE_CENTER) &&
                           (direction == DIRECTION_SOUTH || // AL2
                            direction == DIRECTION_WEST ||  // BL2
                            direction == DIRECTION_EAST ||  // CL2
                            direction == DIRECTION_NORTH);  // DL2

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

    vehicle->isPriorityLane = queue->isPriorityLane;

    if (vehicle->type > 0) {
        queue->waitingTime += 2;
    } else if (queue->isPriorityLane && queue->size > 5) {
        queue->waitingTime += 1.5;
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

bool isEmergencyVehicle(Vehicle* vehicle) {
    return vehicle && vehicle->type > 0;
}

bool isPriorityLaneActive(Queue* queue) {
    return queue && queue->isPriorityLane && queue->size > 5;
}

// Next, let's fix the vehicle path setting logic
void setVehiclePath(Vehicle* vehicle) {
    float turnProbability = 0.6; // 60% chance to turn from left lane

    if (vehicle->startLane == LANE_LEFT) {
        // Left lane: 60% chance to turn left, 40% chance to go straight
        if ((float)rand() / RAND_MAX < turnProbability) {
            vehicle->turning = true;
            // Set end direction based on left turn
            switch (vehicle->startDirection) {
                case DIRECTION_SOUTH:
                    vehicle->endDirection = DIRECTION_WEST;  // A turns to B
                    break;
                case DIRECTION_WEST:
                    vehicle->endDirection = DIRECTION_SOUTH; // B turns to A
                    break;
                case DIRECTION_EAST:
                    vehicle->endDirection = DIRECTION_NORTH; // C turns to D
                    break;
                case DIRECTION_NORTH:
                    vehicle->endDirection = DIRECTION_EAST;  // D turns to C
                    break;
            }
        } else {
            // Go straight
            vehicle->turning = false;
            vehicle->endDirection = vehicle->startDirection;
        }
    } else if (vehicle->startLane == LANE_CENTER) {
        // Center lane: Always turn (either left or right)
        vehicle->turning = true;

        // 50% chance to turn left, 50% chance to turn right
        if ((float)rand() / RAND_MAX < 0.5) {
            // Turn left
            switch (vehicle->startDirection) {
                case DIRECTION_SOUTH:
                    vehicle->endDirection = DIRECTION_WEST;  // A turns to B
                    break;
                case DIRECTION_WEST:
                    vehicle->endDirection = DIRECTION_SOUTH; // B turns to A
                    break;
                case DIRECTION_EAST:
                    vehicle->endDirection = DIRECTION_NORTH; // C turns to D
                    break;
                case DIRECTION_NORTH:
                    vehicle->endDirection = DIRECTION_EAST;  // D turns to C
                    break;
            }
        } else {
            // Turn right
            switch (vehicle->startDirection) {
                case DIRECTION_SOUTH:
                    vehicle->endDirection = DIRECTION_EAST;  // A turns to C
                    break;
                case DIRECTION_WEST:
                    vehicle->endDirection = DIRECTION_NORTH; // B turns to D
                    break;
                case DIRECTION_EAST:
                    vehicle->endDirection = DIRECTION_SOUTH; // C turns to A
                    break;
                case DIRECTION_NORTH:
                    vehicle->endDirection = DIRECTION_WEST;  // D turns to B
                    break;
            }
        }
    } else { // LANE_RIGHT
        // Right lane: Always turn right
        vehicle->turning = true;
    }

    // Set end lane to the rightmost lane (LANE_RIGHT)
    vehicle->endLane = LANE_RIGHT;
}

bool isSafeDistance(Vehicle* v1, Vehicle* v2) {
    if (!v1 || !v2) return true;

    float v1CenterX = v1->x;
    float v1CenterY = v1->y;
    float v2CenterX = v2->x;
    float v2CenterY = v2->y;

    float distance = sqrt(pow(v2CenterX - v1CenterX, 2) +
                         pow(v2CenterY - v1CenterY, 2));

    float requiredDistance = SAFE_DISTANCE;
    if (v1->type > 0 || v2->type > 0) {
        requiredDistance *= 1.5;
    }

    return distance >= requiredDistance;
}

bool checkCollision(Vehicle* v1, Vehicle* v2) {
    if (!v1 || !v2) return false;

    float v1Left = v1->x - SAFE_DISTANCE/4;
    float v1Right = v1->x + SAFE_DISTANCE/4;
    float v1Top = v1->y - SAFE_DISTANCE/4;
    float v1Bottom = v1->y + SAFE_DISTANCE/4;

    float v2Left = v2->x - SAFE_DISTANCE/4;
    float v2Right = v2->x + SAFE_DISTANCE/4;
    float v2Top = v2->y - SAFE_DISTANCE/4;
    float v2Bottom = v2->y + SAFE_DISTANCE/4;

    return !(v1Right < v2Left || v1Left > v2Right ||
             v1Bottom < v2Top || v1Top > v2Bottom);
}

int getQueueSize(Direction direction, LanePosition lane) {
    if (lane == LANE_CENTER &&
        (direction == DIRECTION_SOUTH || // AL2
         direction == DIRECTION_WEST ||  // BL2
         direction == DIRECTION_EAST ||  // CL2
         direction == DIRECTION_NORTH)) { // DL2
        return 6;
    }
    return 0;
}

// First, let's define the traffic light control logic
bool canProceedThroughIntersection(Vehicle* vehicle,  bool trafficLights[4]) {
    // Emergency vehicles can always proceed
    if (vehicle->type == 1) return true;

    // Already passed the intersection
    if (vehicle->passedIntersection) return true;

    // Check traffic light states based on direction
    int trafficLight = -1;

    switch (vehicle->startDirection) {
        case DIRECTION_SOUTH: // A - controlled by t2 (index 1)
            trafficLight = 1;
            break;
        case DIRECTION_WEST:  // B - controlled by t3 (index 2)
            trafficLight = 0;
            break;
        case DIRECTION_EAST:  // C - controlled by t1 (index 0)
            trafficLight = 2;
            break;
        case DIRECTION_NORTH: // D - controlled by t4 (index 3)
            trafficLight = 3;
            break;
    }

    // If light is red, stop
    if (trafficLight  >= 0 && !trafficLights[trafficLight]) {
        return false;
    }

    return true;
}

void updateVehiclePosition(Vehicle* vehicle) {
    if (!vehicle) return;

    if (vehicle->turning && vehicle->progress < 1.0f) {
        vehicle->progress += vehicle->speed / 100.0f;
        if (vehicle->progress >= 1.0f) {
            vehicle->turning = false;
            vehicle->progress = 1.0f;
            vehicle->passedIntersection = true;
        }
    } else {
        switch (vehicle->startDirection) {
            case DIRECTION_SOUTH: vehicle->y -= vehicle->speed; break; // A
            case DIRECTION_WEST:  vehicle->x -= vehicle->speed; break; // B
            case DIRECTION_EAST:  vehicle->x += vehicle->speed; break; // C
            case DIRECTION_NORTH: vehicle->y += vehicle->speed; break; // D
        }
    }
}
