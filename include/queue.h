//queue.h code
#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

#define MAX_QUEUE_SIZE 100
#define SAFE_DISTANCE 30  // Reduced safe distance for better vehicle flow

// Lane positions (a1, a2, a3, etc.)
typedef enum {
    LANE_LEFT = 0,    // Leftmost lane (a1, b1, c1, d1)
    LANE_CENTER = 1,  // Center lane (a2, b2, c2, d2)
    LANE_RIGHT = 2    // Rightmost lane (a3, b3, c3, d3)
} LanePosition;

// Directions (N, E, S, W)
typedef enum {
    DIRECTION_NORTH = 0,  // Towards top of screen
    DIRECTION_EAST = 1,   // Towards right of screen
    DIRECTION_SOUTH = 2,  // Towards bottom of screen
    DIRECTION_WEST = 3    // Towards left of screen
} Direction;

typedef struct {
    int vehicleId;
    int type;               // 0: regular, 1: ambulance, 2: police, 3: fire truck
    Direction startDirection;  // Starting direction (N,E,S,W)
    Direction endDirection;    // Ending direction (N,E,S,W)
    LanePosition startLane;    // Starting lane position
    LanePosition endLane;      // Ending lane position
    float x;                   // Current x position
    float y;                   // Current y position
    float speed;              // Movement speed
    float turnAngle;          // Current turning angle
    bool turning;             // Whether vehicle is currently turning
    float progress;           // Progress along path (0.0 to 1.0)
    int waitTime;             // Time spent waiting at intersection
    bool isPriorityLane;      // Whether vehicle is in a priority lane
    bool hasPassed;
} Vehicle;

typedef struct {
    Vehicle* items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
    Direction direction;    // Associated direction (N,E,S,W)
    LanePosition lane;     // Associated lane (Left,Center,Right)
    float waitingTime;     // Cumulative waiting time for priority handling
    bool isPriorityLane;   // Whether this is a priority lane (e.g. AL2)
} Queue;

// Queue operations
Queue* createQueue(Direction direction, LanePosition lane);
bool isEmpty(Queue* queue);
bool isFull(Queue* queue);
void enqueue(Queue* queue, Vehicle* vehicle);
Vehicle* dequeue(Queue* queue);
int getSize(Queue* queue);
Vehicle* peekFront(Queue* queue);

// Vehicle operations
void updateVehiclePosition(Vehicle* vehicle);
bool isEmergencyVehicle(Vehicle* vehicle);
bool canProceedThroughIntersection(Vehicle* vehicle, bool trafficLights[4]);
void setVehiclePath(Vehicle* vehicle);
bool isSafeDistance(Vehicle* v1, Vehicle* v2);
bool checkCollision(Vehicle* v1, Vehicle* v2);
bool isPriorityLaneActive(Queue* queue);
int getQueueSize(Direction direction, LanePosition lane);  // Added function declaration


#endif // QUEUE_H 
