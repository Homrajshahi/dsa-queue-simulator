#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

#define MAX_QUEUE_SIZE 100
#define SAFE_DISTANCE 30

typedef enum {
    LANE_LEFT = 0,    // Leftmost lane (L1)
    LANE_CENTER = 1,  // Center lane (L2)
    LANE_RIGHT = 2    // Rightmost lane (L3)
} LanePosition;

typedef enum {
    DIRECTION_SOUTH = 0,  // Road A - Southbound
    DIRECTION_WEST = 1,   // Road B - Westbound
    DIRECTION_EAST = 2,   // Road C - Eastbound
    DIRECTION_NORTH = 3   // Road D - Northbound
} Direction;

typedef struct {
    int vehicleId;
    int type;               // 0: regular, 1: ambulance, 2: police, 3: fire truck
    Direction startDirection;
    Direction endDirection;
    LanePosition startLane;
    LanePosition endLane;
    float x;
    float y;
    float speed;
    float turnAngle;
    bool turning;
    float progress;
    int waitTime;
    bool isPriorityLane;
    bool passedIntersection;
} Vehicle;

typedef struct {
    Vehicle* items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
    Direction direction;
    LanePosition lane;
    float waitingTime;
    bool isPriorityLane;
} Queue;

Queue* createQueue(Direction direction, LanePosition lane);
bool isEmpty(Queue* queue);
bool isFull(Queue* queue);
void enqueue(Queue* queue, Vehicle* vehicle);
Vehicle* dequeue(Queue* queue);
int getSize(Queue* queue);
Vehicle* peekFront(Queue* queue);

void updateVehiclePosition(Vehicle* vehicle);
bool isEmergencyVehicle(Vehicle* vehicle);
bool canProceedThroughIntersection(Vehicle* vehicle, bool trafficLights[4]);
void setVehiclePath(Vehicle* vehicle);
bool isSafeDistance(Vehicle* v1, Vehicle* v2);
bool checkCollision(Vehicle* v1, Vehicle* v2);
bool isPriorityLaneActive(Queue* queue);
int getQueueSize(Direction direction, LanePosition lane);

#endif // QUEUE_H
