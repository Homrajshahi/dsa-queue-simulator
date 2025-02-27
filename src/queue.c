#include <stdlib.h>
<<<<<<< HEAD
#include "queue.h"
#include "vehicle.h"
=======
#include <math.h>
#include "queue.h"
>>>>>>> b73988b (solving a problem of dequw)

Queue* createQueue(Direction direction, LanePosition lane) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) return NULL;

    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->direction = direction;
    queue->lane = lane;
    queue->waitingTime = 0;
<<<<<<< HEAD

    // Set priority lane flags based on lane and direction
    queue->isPriorityLane = (lane == LANE_CENTER) &&
                           (direction == DIRECTION_NORTH || // AL2
                            direction == DIRECTION_SOUTH || // CL2
                            direction == DIRECTION_EAST ||  // BL2
                            direction == DIRECTION_WEST);   // DL2

    // Initialize all items to NULL
=======
    queue->isPriorityLane = (lane == LANE_CENTER) &&
                           (direction == DIRECTION_NORTH || direction == DIRECTION_SOUTH ||
                            direction == DIRECTION_EAST || direction == DIRECTION_WEST);

>>>>>>> b73988b (solving a problem of dequw)
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
<<<<<<< HEAD

    // Update vehicle's priority lane status
    vehicle->isPriorityLane = queue->isPriorityLane;

    // Adjust waiting time based on vehicle type and lane
    if (vehicle->type > 0) { // Emergency vehicles
        queue->waitingTime += 2;
    } else if (queue->isPriorityLane && queue->size > 5) {
        queue->waitingTime += 1.5; // Priority lanes with high congestion
=======
    vehicle->isPriorityLane = queue->isPriorityLane;

    if (vehicle->type > 0) {
        queue->waitingTime += 2;
    } else if (queue->isPriorityLane && queue->size > 5) {
        queue->waitingTime += 1.5;
>>>>>>> b73988b (solving a problem of dequw)
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

<<<<<<< HEAD
    // Update waiting time based on vehicle type
=======
>>>>>>> b73988b (solving a problem of dequw)
    if (vehicle) {
        if (vehicle->type > 0) {
            queue->waitingTime = fmax(0, queue->waitingTime - 2);
        } else if (queue->isPriorityLane && queue->size > 5) {
            queue->waitingTime = fmax(0, queue->waitingTime - 1.5);
        } else {
            queue->waitingTime = fmax(0, queue->waitingTime - 1);
        }
    }
<<<<<<< HEAD

=======
>>>>>>> b73988b (solving a problem of dequw)
    return vehicle;
}

int getSize(Queue* queue) {
    return queue ? queue->size : 0;
}

Vehicle* peekFront(Queue* queue) {
    if (!queue || isEmpty(queue)) return NULL;
    return queue->items[queue->front];
}
<<<<<<< HEAD
=======

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

    // Left lane: Always turn left to the right lane of the next road
    if (vehicle->startLane == LANE_LEFT) {
        switch (vehicle->startDirection) {
            case DIRECTION_SOUTH: // Road A (South) -> Road B (West)
                vehicle->endDirection = DIRECTION_WEST;
                vehicle->endLane = LANE_RIGHT;
                break;
            case DIRECTION_EAST: // Road C (East) -> Road A (South)
                vehicle->endDirection = DIRECTION_SOUTH;
                vehicle->endLane = LANE_RIGHT;
                break;
            case DIRECTION_NORTH: // Road D (North) -> Road C (East)
                vehicle->endDirection = DIRECTION_EAST;
                vehicle->endLane = LANE_RIGHT;
                break;
            case DIRECTION_WEST: // Road B (West) -> Road D (North)
                vehicle->endDirection = DIRECTION_NORTH;
                vehicle->endLane = LANE_RIGHT;
                break;
        }
        vehicle->turning = true; // Mark for turning, handled by simulator
    }
    // Center lane: Randomly turn left or right to the right lane of the next road
    else if (vehicle->startLane == LANE_CENTER) {
        int turnChoice = rand() % 2; // 0 = left, 1 = right
        switch (vehicle->startDirection) {
            case DIRECTION_SOUTH: // Road A -> Road D (North) or Road C (East)
                vehicle->endDirection = turnChoice ? DIRECTION_EAST : DIRECTION_NORTH;
                vehicle->endLane = LANE_RIGHT;
                break;
            case DIRECTION_EAST: // Road C -> Road B (West) or Road D (North)
                vehicle->endDirection = turnChoice ? DIRECTION_WEST : DIRECTION_NORTH;
                vehicle->endLane = LANE_RIGHT;
                break;
            case DIRECTION_NORTH: // Road D -> Road B (West) or Road A (South)
                vehicle->endDirection = turnChoice ? DIRECTION_WEST : DIRECTION_SOUTH;
                vehicle->endLane = LANE_RIGHT;
                break;
            case DIRECTION_WEST: // Road B -> Road C (East) or Road A (South)
                vehicle->endDirection = turnChoice ? DIRECTION_NORTH : DIRECTION_SOUTH;
                vehicle->endLane = LANE_RIGHT;
                break;
        }
        vehicle->turning = true; // Mark for turning, handled by simulator
    }
    // Right lane: No straight movement, used for incoming traffic
}

bool isSafeDistance(Vehicle* v1, Vehicle* v2) {
    if (!v1 || !v2) return true;

    float distance = sqrt(pow(v2->x - v1->x, 2) + pow(v2->y - v1->y, 2));
    float requiredDistance = SAFE_DISTANCE * (v1->type > 0 || v2->type > 0 ? 1.5 : 1.0);
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

    return !(v1Right < v2Left || v1Left > v2Right || v1Bottom < v2Top || v1Top > v2Bottom);
}

int getQueueSize(Direction direction, LanePosition lane) {
    return (lane == LANE_CENTER && (direction == DIRECTION_NORTH || direction == DIRECTION_SOUTH ||
                                    direction == DIRECTION_EAST || direction == DIRECTION_WEST)) ? 6 : 0;
}

bool canProceedThroughIntersection(Vehicle* vehicle, bool trafficLights[4]) {
    if (!vehicle) return false;

    bool canProceed = false;
    switch (vehicle->startDirection) {
        case DIRECTION_SOUTH: canProceed = trafficLights[1]; break; // t2
        case DIRECTION_EAST:  canProceed = trafficLights[0]; break; // t1
        case DIRECTION_NORTH: canProceed = trafficLights[3]; break; // t4
        case DIRECTION_WEST:  canProceed = trafficLights[2]; break; // t3
    }

    if (vehicle->isPriorityLane && getQueueSize(vehicle->startDirection, LANE_CENTER) > 5) return true;
    if (isEmergencyVehicle(vehicle)) return true;
    return canProceed;
}

void updateVehiclePosition(Vehicle* vehicle) {
    if (!vehicle) return;

    if (!vehicle->hasPassed) {
        // Straight movement until turning or passing
        switch (vehicle->startDirection) {
            case DIRECTION_NORTH: vehicle->y -= vehicle->speed; break;
            case DIRECTION_EAST:  vehicle->x += vehicle->speed; break;
            case DIRECTION_SOUTH: vehicle->y += vehicle->speed; break;
            case DIRECTION_WEST:  vehicle->x -= vehicle->speed; break;
        }
    } else {
        // Continue straight after passing
        switch (vehicle->startDirection) {
            case DIRECTION_NORTH: vehicle->y -= vehicle->speed; break;
            case DIRECTION_EAST:  vehicle->x += vehicle->speed; break;
            case DIRECTION_SOUTH: vehicle->y += vehicle->speed; break;
            case DIRECTION_WEST:  vehicle->x -= vehicle->speed; break;
        }
    }
}
>>>>>>> b73988b (solving a problem of dequw)
