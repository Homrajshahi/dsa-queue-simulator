#ifndef QUEUE_H
#define QUEUE_H

<<<<<<< HEAD
#include "vehicle.h"

#define MAX_QUEUE_SIZE 100
#define SAFE_DISTANCE 20
=======
#include <stdbool.h>

#define MAX_QUEUE_SIZE 100
#define SAFE_DISTANCE 30

typedef enum {
    LANE_LEFT = 0,
    LANE_CENTER = 1,
    LANE_RIGHT = 2
} LanePosition;

typedef enum {
    DIRECTION_NORTH = 0,
    DIRECTION_EAST = 1,
    DIRECTION_SOUTH = 2,
    DIRECTION_WEST = 3
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
    bool hasPassed;         // Added to track if vehicle has passed intersection
} Vehicle;
>>>>>>> b73988b (solving a problem of dequw)

typedef struct {
    Vehicle* items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
<<<<<<< HEAD
    Direction direction;    // Associated direction (N,E,S,W)
    LanePosition lane; // Associated lane (Left,Center,Right)
    float waitingTime; // Cumulative waiting time for priority handling
    bool isPriorityLane; // Whether this is a priority lane (e.g., AL2)
} Queue;

// Queue operations
=======
    Direction direction;
    LanePosition lane;
    float waitingTime;
    bool isPriorityLane;
} Queue;

>>>>>>> b73988b (solving a problem of dequw)
Queue* createQueue(Direction direction, LanePosition lane);
bool isEmpty(Queue* queue);
bool isFull(Queue* queue);
void enqueue(Queue* queue, Vehicle* vehicle);
Vehicle* dequeue(Queue* queue);
int getSize(Queue* queue);
Vehicle* peekFront(Queue* queue);

<<<<<<< HEAD
#endif // QUEUE_H
=======
void updateVehiclePosition(Vehicle* vehicle);
bool isEmergencyVehicle(Vehicle* vehicle);
bool canProceedThroughIntersection(Vehicle* vehicle, bool trafficLights[4]);
void setVehiclePath(Vehicle* vehicle);
bool isSafeDistance(Vehicle* v1, Vehicle* v2);
bool checkCollision(Vehicle* v1, Vehicle* v2);
bool isPriorityLaneActive(Queue* queue);
int getQueueSize(Direction direction, LanePosition lane);

#endif
>>>>>>> b73988b (solving a problem of dequw)
