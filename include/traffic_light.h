#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H
#include "queue.h"
#include "vehicle.h"

#define TRAFFIC_LIGHT_CHANGE_INTERVAL 5000 // Time interval (in milliseconds) to change traffic lights

typedef struct {
    int x, y; // Position on the screen
    bool state; // Current state (true for green, false for red)
    int timer; // Timer for the current state
} TrafficLight;

// Traffic light operations
void initializeTrafficLights(TrafficLight lights[], int lightCount);
void updateTrafficLights(TrafficLight lights[], int lightCount, Queue* queues[], int queueCount);
bool canVehicleProceed(TrafficLight lights[], int lightIndex, Vehicle* vehicle, Direction direction);

#endif // TRAFFIC_LIGHT_H
