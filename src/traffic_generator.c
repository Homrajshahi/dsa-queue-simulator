#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "traffic_generator.h"
#include "queue.h"

#define VEHICLE_GENERATION_INTERVAL 1000 // Base interval in milliseconds
#define MIN_INTERVAL 500                 // Minimum interval between vehicles
#define MAX_INTERVAL 2000               // Maximum interval between vehicles
#define EMERGENCY_VEHICLE_CHANCE 15      // Percentage chance for emergency vehicles

void writeVehicleToFile(Vehicle* vehicle, const char* filename) {
    if (!vehicle || !filename) return;

    FILE* file = fopen(filename, "a");
    if (!file) return;

    fprintf(file, "%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%d,%f,%d\n",
        vehicle->vehicleId,
        vehicle->type,
        vehicle->startDirection,
        vehicle->endDirection,
        vehicle->startLane,
        vehicle->endLane,
        vehicle->x,
        vehicle->y,
        vehicle->speed,
        vehicle->turnAngle,
        vehicle->turning ? 1 : 0,
        vehicle->progress,
        vehicle->waitTime);

    fclose(file);
}

void generateVehicle(const char* filename) {
    Vehicle* vehicle = (Vehicle*)malloc(sizeof(Vehicle));
    if (!vehicle) return;

    // Generate random properties
    vehicle->vehicleId = rand() % 1000;

    // Determine vehicle type with weighted probability
    int typeRoll = rand() % 100;
    if (typeRoll < EMERGENCY_VEHICLE_CHANCE) {
        vehicle->type = 1 + (rand() % 3); // Emergency vehicle (1-3)
    } else {
        vehicle->type = 0; // Regular vehicle
    }

    // Set initial direction and lane
    vehicle->startDirection = rand() % 4;
    vehicle->startLane = rand() % 3;

    // Set path based on starting position using the function from queue.h
    setVehiclePath(vehicle);

    // Set initial position based on starting direction
    switch (vehicle->startDirection) {
        case DIRECTION_NORTH:
            vehicle->x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 +
                        (vehicle->startLane + 0.5) * LANE_WIDTH;
            vehicle->y = WINDOW_HEIGHT + VEHICLE_SIZE;
            break;
        case DIRECTION_EAST:
            vehicle->x = -VEHICLE_SIZE;
            vehicle->y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 +
                        (vehicle->startLane + 0.5) * LANE_WIDTH;
            break;
        case DIRECTION_SOUTH:
            vehicle->x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 +
                        (2 - vehicle->startLane + 0.5) * LANE_WIDTH;
            vehicle->y = -VEHICLE_SIZE;
            break;
        case DIRECTION_WEST:
            vehicle->x = WINDOW_WIDTH + VEHICLE_SIZE;
            vehicle->y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 +
                        (2 - vehicle->startLane + 0.5) * LANE_WIDTH;
            break;
    }

    // Set other properties
    vehicle->speed = (vehicle->type > 0) ? 3.0f : 2.0f;
    vehicle->turning = false;
    vehicle->turnAngle = 0.0f;
    vehicle->progress = 0.0f;
    vehicle->waitTime = 0;
    vehicle->isPriorityLane = false; // Will be set when added to queue

    writeVehicleToFile(vehicle, filename);
    free(vehicle);
}

void startVehicleGeneration(const char* filename) {
    srand(time(NULL));

    // Clear the file at start
    FILE* file = fopen(filename, "w");
    if (file) {
        fclose(file);
    }

    while (1) {
        generateVehicle(filename);

        // Dynamic interval based on time of day
        time_t now = time(NULL);
        struct tm* timeinfo = localtime(&now);
        int hour = timeinfo->tm_hour;

        int interval = VEHICLE_GENERATION_INTERVAL;
        if ((hour >= 7 && hour <= 9) || (hour >= 16 && hour <= 18)) {
            interval = MIN_INTERVAL; // Peak hours
        } else if (hour >= 22 || hour <= 5) {
            interval = MAX_INTERVAL; // Night hours
        }

        usleep(interval * 1000);
    }
}
