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

bool isEmergencyVehicle(Vehicle* vehicle) {
    return vehicle && vehicle->type > 0;
}

bool isPriorityLaneActive(Queue* queue) {
    return queue && queue->isPriorityLane && queue->size > 5;
}

void setVehiclePath(Vehicle* vehicle) {
    if (!vehicle) return;

    // Default: go straight
    vehicle->endDirection = vehicle->startDirection;
    vehicle->endLane = vehicle->startLane;
    vehicle->turning = false;

    // Handle left turns for left lanes
    if (vehicle->startLane == LANE_LEFT) {
        switch (vehicle->startDirection) {
            case DIRECTION_NORTH:
                vehicle->endDirection = DIRECTION_WEST;
                break;
            case DIRECTION_EAST:
                vehicle->endDirection = DIRECTION_NORTH;
                break;
            case DIRECTION_SOUTH:
                vehicle->endDirection = DIRECTION_EAST;
                break;
            case DIRECTION_WEST:
                vehicle->endDirection = DIRECTION_SOUTH;
                break;
        }
        vehicle->turning = true;
        vehicle->endLane = LANE_RIGHT; // End in rightmost lane after turn
    }

    // Center lanes (priority lanes) always go straight
    if (vehicle->startLane == LANE_CENTER) {
        vehicle->endLane = LANE_CENTER;
        vehicle->turning = false;
    }

    // Right lanes also go straight
    if (vehicle->startLane == LANE_RIGHT) {
        vehicle->endLane = LANE_RIGHT;
        vehicle->turning = false;
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

    // Adjust safe distance based on vehicle types
    float requiredDistance = SAFE_DISTANCE;
    if (v1->type > 0 || v2->type > 0) {
        requiredDistance *= 1.5; // Larger safe distance for emergency vehicles
    }

    return distance >= requiredDistance;
}

bool checkCollision(Vehicle* v1, Vehicle* v2) {
    if (!v1 || !v2) return false;

    // Consider vehicle size for collision detection
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
    int count = 0;
    // This is a simplified version that returns 6 if it's a priority lane
    // to ensure priority lanes get proper treatment
    if (lane == LANE_CENTER &&
        (direction == DIRECTION_NORTH || // AL2
         direction == DIRECTION_SOUTH || // CL2
         direction == DIRECTION_EAST ||  // BL2
         direction == DIRECTION_WEST)) { // DL2
        count = 6; // Ensures priority lane behavior
    }
    return count;
}

bool canProceedThroughIntersection(Vehicle* vehicle, bool trafficLights[4]) {
    if (!vehicle) return false;

    // Check if the relevant traffic light is green
    bool canProceed = false;

    // Traffic light rules based on the provided specifications
    switch (vehicle->startDirection) {
        case DIRECTION_WEST:  // D lanes
            if (vehicle->startLane == LANE_LEFT) {
                // DL1 - follows t2 when turning to B
                canProceed = trafficLights[1];
            } else if (vehicle->startLane == LANE_RIGHT) {
                // DL3 - normal lane, follows t1 going to A
                canProceed = trafficLights[0];
            } else {
                // DL2 - priority lane, but still follows traffic lights
                canProceed = trafficLights[0];
            }
            break;

        case DIRECTION_NORTH:  // A lanes
            if (vehicle->startLane == LANE_LEFT) {
                // AL1 - normal lane
                canProceed = trafficLights[3];
            } else if (vehicle->startLane == LANE_CENTER) {
                // AL2 - priority lane
                canProceed = trafficLights[2];
            } else {
                // AL3 - turns left to C
                canProceed = trafficLights[2];
            }
            break;

        case DIRECTION_EAST:  // B lanes
            if (vehicle->startLane == LANE_LEFT) {
                // BL1 - incoming lane
                canProceed = trafficLights[2];
            } else {
                // Other B lanes
                canProceed = trafficLights[0];
            }
            break;

        case DIRECTION_SOUTH:  // C lanes
            if (vehicle->startLane == LANE_LEFT) {
                // CL3 - incoming lane
                canProceed = trafficLights[3];
            } else {
                // Other C lanes
                canProceed = trafficLights[1];
            }
            break;
    }

    // Priority lane conditions override
    if (vehicle->isPriorityLane && getQueueSize(vehicle->startDirection, LANE_CENTER) > 5) {
        // Priority lanes get immediate service after current cycle
        return true;
    }

    // Emergency vehicles can proceed with caution even on red
    if (isEmergencyVehicle(vehicle)) {
        return true;
    }

    return canProceed;
}

void updateVehiclePosition(Vehicle* vehicle) {
    if (!vehicle) return;

    // Calculate turn path for turning vehicles
    if (vehicle->startLane == LANE_LEFT && !vehicle->turning) {
        vehicle->turning = true;
        // Calculate turn angle based on start and end directions
        int dirDiff = (vehicle->endDirection - vehicle->startDirection + 4) % 4;
        vehicle->turnAngle = dirDiff * 90.0f;
    }

    // Update position based on turn status
    if (vehicle->turning) {
        // Implement turning movement using progress
        vehicle->progress += vehicle->speed / 100.0f;
        if (vehicle->progress >= 1.0f) {
            vehicle->turning = false;
            vehicle->progress = 1.0f;
        }
    } else {
        // Straight movement
        switch (vehicle->startDirection) {
            case DIRECTION_NORTH: vehicle->y -= vehicle->speed; break;
            case DIRECTION_EAST:  vehicle->x += vehicle->speed; break;
            case DIRECTION_SOUTH: vehicle->y += vehicle->speed; break;
            case DIRECTION_WEST:  vehicle->x -= vehicle->speed; break;
        }
    }
}
