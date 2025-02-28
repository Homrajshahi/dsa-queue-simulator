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

void setVehiclePath(Vehicle* vehicle) {
    if (!vehicle) return;

    vehicle->endDirection = vehicle->startDirection;
    vehicle->endLane = vehicle->startLane;
    vehicle->turning = false;
    vehicle->passedIntersection = false;

    if (vehicle->startLane == LANE_LEFT) { // L1 turns left
        switch (vehicle->startDirection) {
            case DIRECTION_SOUTH: vehicle->endDirection = DIRECTION_WEST; break;  // A -> B (BL3)
            case DIRECTION_WEST:  vehicle->endDirection = DIRECTION_SOUTH; break; // B -> A (AL3)
            case DIRECTION_EAST:  vehicle->endDirection = DIRECTION_NORTH; break; // C -> D (DL3)
            case DIRECTION_NORTH: vehicle->endDirection = DIRECTION_EAST; break;  // D -> C (CL3)
        }
        vehicle->turning = true;
        vehicle->endLane = LANE_RIGHT;
    }
    else if (vehicle->startLane == LANE_CENTER) { // L2 turns left or right with restrictions
        int turnChoice = rand() % 2; // 0 = left, 1 = right
        switch (vehicle->startDirection) {
            case DIRECTION_SOUTH: // A
                vehicle->endDirection = turnChoice ? DIRECTION_EAST : DIRECTION_NORTH; // CL3 or DL3
                break;
            case DIRECTION_WEST:  // B
                vehicle->endDirection = turnChoice ? DIRECTION_EAST : DIRECTION_SOUTH; // CL3 or AL3
                break;
            case DIRECTION_EAST:  // C
                vehicle->endDirection = turnChoice ? DIRECTION_NORTH : DIRECTION_WEST; // DL3 or BL3
                break;
            case DIRECTION_NORTH: // D
                vehicle->endDirection = turnChoice ? DIRECTION_SOUTH : DIRECTION_WEST; // AL3 or BL3
                break;
        }
        vehicle->turning = true;
        vehicle->endLane = LANE_RIGHT;
    }
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

bool canProceedThroughIntersection(Vehicle* vehicle, bool trafficLights[4]) {
    if (!vehicle) return false;

    if (vehicle->passedIntersection) return true;

    bool canProceed = false;

    switch (vehicle->startDirection) {
        case DIRECTION_SOUTH:  // Road A - t2
            if (vehicle->startLane == LANE_LEFT) { // AL1 -> BL3
                canProceed = trafficLights[1]; // t2
            } else if (vehicle->startLane == LANE_CENTER) { // AL2 -> DL3 or CL3
                canProceed = trafficLights[1]; // t2
            } else { // AL3 <- CL1 (t1), BL2 (t3), DL2 (t4)
                canProceed = trafficLights[1]; // t2
            }
            break;

        case DIRECTION_WEST:  // Road B - t3
            if (vehicle->startLane == LANE_LEFT) { // BL1 -> AL3
                canProceed = trafficLights[2]; // t3
            } else if (vehicle->startLane == LANE_CENTER) { // BL2 -> AL3 or CL3
                canProceed = trafficLights[2]; // t3
            } else { // BL3 <- AL1 (t2), CL2 (t1), DL2 (t4)
                canProceed = trafficLights[2]; // t3
            }
            break;

        case DIRECTION_EAST:  // Road C - t1
            if (vehicle->startLane == LANE_LEFT) { // CL1 -> DL3
                canProceed = trafficLights[0]; // t1
            } else if (vehicle->startLane == LANE_CENTER) { // CL2 -> BL3 or DL3
                canProceed = trafficLights[0]; // t1
            } else { // CL3 <- AL2 (t2), BL2 (t3), DL1 (t4)
                canProceed = trafficLights[0]; // t1
            }
            break;

        case DIRECTION_NORTH:  // Road D - t4
            if (vehicle->startLane == LANE_LEFT) { // DL1 -> CL3
                canProceed = trafficLights[3]; // t4
            } else if (vehicle->startLane == LANE_CENTER) { // DL2 -> BL3 or AL3
                canProceed = trafficLights[3]; // t4
            } else { // DL3 <- BL1 (t3), CL2 (t1), AL2 (t2)
                canProceed = trafficLights[3]; // t4
            }
            break;
    }

    if (vehicle->isPriorityLane && getQueueSize(vehicle->startDirection, LANE_CENTER) > 5) {
        return true;
    }

    if (isEmergencyVehicle(vehicle)) {
        return true;
    }

    return canProceed;
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
