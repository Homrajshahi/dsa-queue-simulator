#ifndef VEHICLE_H
#define VEHICLE_H

#include <stdbool.h>

#define MAX_VEHICLE_ID 10000
#define MAX_VEHICLE_SPEED 10.0f

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
} Vehicle;

// Vehicle operations
void setVehiclePath(Vehicle* vehicle);
bool isEmergencyVehicle(Vehicle* vehicle);
bool isSafeDistance(Vehicle* v1, Vehicle* v2);
bool checkCollision(Vehicle* v1, Vehicle* v2);
void updateVehiclePosition(Vehicle* vehicle);

#endif // VEHICLE_H
