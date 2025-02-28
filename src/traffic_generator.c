#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "traffic_generator.h"
#include "queue.h"

#define VEHICLE_GENERATION_INTERVAL 1000
#define MIN_INTERVAL 500
#define MAX_INTERVAL 2000
#define EMERGENCY_VEHICLE_CHANCE 15

void writeVehicleToFile(Vehicle* vehicle, const char* filename) {
    if (!vehicle || !filename) return;
    FILE* file = fopen(filename, "a");
    if (!file) return;
    fprintf(file, "%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%d,%f,%d,%d\n",
        vehicle->vehicleId, vehicle->type, vehicle->startDirection, vehicle->endDirection,
        vehicle->startLane, vehicle->endLane, vehicle->x, vehicle->y, vehicle->speed,
        vehicle->turnAngle, vehicle->turning ? 1 : 0, vehicle->progress, vehicle->waitTime,
        vehicle->passedIntersection ? 1 : 0);
    fclose(file);
}

void generateVehicle(const char* filename) {
    Vehicle* vehicle = (Vehicle*)malloc(sizeof(Vehicle));
    if (!vehicle) return;

    vehicle->vehicleId = rand() % 1000;
    int typeRoll = rand() % 100;
    vehicle->type = (typeRoll < EMERGENCY_VEHICLE_CHANCE) ? (1 + rand() % 3) : 0;
    vehicle->startDirection = rand() % 4;
    vehicle->startLane = rand() % 2; // Only L1 or L2
    setVehiclePath(vehicle);

    float laneOffset = (vehicle->startLane + 0.5) * LANE_WIDTH;
    switch (vehicle->startDirection) {
        case DIRECTION_SOUTH: // A - from top
            vehicle->x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + laneOffset;
            vehicle->y = WINDOW_HEIGHT + VEHICLE_SIZE;
            break;
        case DIRECTION_WEST:  // B - from right
            vehicle->x = WINDOW_WIDTH + VEHICLE_SIZE;
            vehicle->y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + laneOffset;
            break;
        case DIRECTION_EAST:  // C - from left
            vehicle->x = -VEHICLE_SIZE;
            vehicle->y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + laneOffset;
            break;
        case DIRECTION_NORTH: // D - from bottom
            vehicle->x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + laneOffset;
            vehicle->y = -VEHICLE_SIZE;
            break;
    }

    vehicle->speed = (vehicle->type > 0) ? 3.0f : 2.0f;
    vehicle->turning = false;
    vehicle->turnAngle = 0.0f;
    vehicle->progress = 0.0f;
    vehicle->waitTime = 0;
    vehicle->isPriorityLane = false;
    vehicle->passedIntersection = false;

    writeVehicleToFile(vehicle, filename);
    free(vehicle);
}

void startVehicleGeneration(const char* filename) {
    srand(time(NULL));
    FILE* file = fopen(filename, "w");
    if (file) fclose(file);

    while (1) {
        generateVehicle(filename);
        time_t now = time(NULL);
        struct tm* timeinfo = localtime(&now);
        int hour = timeinfo->tm_hour;
        int interval = VEHICLE_GENERATION_INTERVAL;
        if ((hour >= 7 && hour <= 9) || (hour >= 16 && hour <= 18)) interval = MIN_INTERVAL;
        else if (hour >= 22 || hour <= 5) interval = MAX_INTERVAL;
        usleep(interval * 1000);
    }
}
