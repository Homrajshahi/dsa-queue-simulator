#ifndef TRAFFIC_GENERATOR_H
#define TRAFFIC_GENERATOR_H

#include "queue.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define ROAD_WIDTH 120
#define LANE_WIDTH (ROAD_WIDTH/3)
#define VEHICLE_SIZE 20

void generateVehicle(const char* filename);
void startVehicleGeneration(const char* filename);
void writeVehicleToFile(Vehicle* vehicle, const char* filename);

#endif
