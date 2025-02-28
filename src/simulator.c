#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "queue.h"
#include "traffic_generator.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define ROAD_WIDTH 120
#define LANE_WIDTH (ROAD_WIDTH/3)
#define INTERSECTION_SIZE (ROAD_WIDTH * 1.2)
#define VEHICLE_SIZE 20

typedef struct {
    int x, y;
    bool state;
    const char* label;
} TrafficLight;

void drawRoads(SDL_Renderer* renderer) {
    // Road background - Darker gray for better contrast
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);

    // Main roads
    SDL_Rect horizontalRoad = {
        0, WINDOW_HEIGHT/2 - ROAD_WIDTH/2,
        WINDOW_WIDTH, ROAD_WIDTH
    };
    SDL_RenderFillRect(renderer, &horizontalRoad);

    SDL_Rect verticalRoad = {
        WINDOW_WIDTH/2 - ROAD_WIDTH/2, 0,
        ROAD_WIDTH, WINDOW_HEIGHT
    };
    SDL_RenderFillRect(renderer, &verticalRoad);

    // Draw zebra crossings with proper alternating pattern
    int stripeWidth = 10;
    int crossingWidth = 40;

    for(int i = 0; i < ROAD_WIDTH; i += stripeWidth * 2) {
        // North zebra
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255); // Less bright white
        SDL_Rect whiteStripe = {
            WINDOW_WIDTH/2 - ROAD_WIDTH/2 + i,
            WINDOW_HEIGHT/2 - ROAD_WIDTH/2 - crossingWidth,
            stripeWidth,
            crossingWidth
        };
        SDL_RenderFillRect(renderer, &whiteStripe);

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // Dark gray for contrast
        SDL_Rect blackStripe = {
            WINDOW_WIDTH/2 - ROAD_WIDTH/2 + i + stripeWidth,
            WINDOW_HEIGHT/2 - ROAD_WIDTH/2 - crossingWidth,
            stripeWidth,
            crossingWidth
        };
        SDL_RenderFillRect(renderer, &blackStripe);

        // South zebra
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
        whiteStripe.y = WINDOW_HEIGHT/2 + ROAD_WIDTH/2;
        SDL_RenderFillRect(renderer, &whiteStripe);

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        blackStripe.y = WINDOW_HEIGHT/2 + ROAD_WIDTH/2;
        SDL_RenderFillRect(renderer, &blackStripe);
    }

    // East and West zebra crossings
    for(int i = 0; i < ROAD_WIDTH; i += stripeWidth * 2) {
        // East zebra
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
        SDL_Rect whiteStripe = {
            WINDOW_WIDTH/2 + ROAD_WIDTH/2,
            WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + i,
            crossingWidth,
            stripeWidth
        };
        SDL_RenderFillRect(renderer, &whiteStripe);

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_Rect blackStripe = {
            WINDOW_WIDTH/2 + ROAD_WIDTH/2,
            WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + i + stripeWidth,
            crossingWidth,
            stripeWidth
        };
        SDL_RenderFillRect(renderer, &blackStripe);

        // West zebra
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
        whiteStripe.x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 - crossingWidth;
        SDL_RenderFillRect(renderer, &whiteStripe);

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        blackStripe.x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 - crossingWidth;
        SDL_RenderFillRect(renderer, &blackStripe);
    }

    // Draw center lane markers (more visible)
    int dotLength = 15;
    int dotGap = 25;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);

    // Draw lanes up to zebra crossings
    for (int i = 1; i < 3; i++) {
        // North lanes
        int x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + i*LANE_WIDTH;
        for (int y = 0; y < WINDOW_HEIGHT/2 - ROAD_WIDTH/2 - crossingWidth; y += dotLength + dotGap) {
            SDL_Rect dot = {x - 2, y, 4, dotLength};
            SDL_RenderFillRect(renderer, &dot);
        }

        // South lanes
        for (int y = WINDOW_HEIGHT/2 + ROAD_WIDTH/2 + crossingWidth; y < WINDOW_HEIGHT; y += dotLength + dotGap) {
            SDL_Rect dot = {x - 2, y, 4, dotLength};
            SDL_RenderFillRect(renderer, &dot);
        }
    }

    // East and West lanes
    for (int i = 1; i < 3; i++) {
        int y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + i*LANE_WIDTH;

        // East lanes
        for (int x = WINDOW_WIDTH/2 + ROAD_WIDTH/2 + crossingWidth; x < WINDOW_WIDTH; x += dotLength + dotGap) {
            SDL_Rect dot = {x, y - 2, dotLength, 4};
            SDL_RenderFillRect(renderer, &dot);
        }

        // West lanes
        for (int x = 0; x < WINDOW_WIDTH/2 - ROAD_WIDTH/2 - crossingWidth; x += dotLength + dotGap) {
            SDL_Rect dot = {x, y - 2, dotLength, 4};
            SDL_RenderFillRect(renderer, &dot);
        }
    }
}

void drawTrafficLights(SDL_Renderer* renderer, TrafficLight lights[4]) {
    const int LIGHT_SIZE = 25, BOX_PADDING = 5;
    for (int i = 0; i < 4; i++) {
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_Rect lightBox = {lights[i].x - BOX_PADDING, lights[i].y - BOX_PADDING, LIGHT_SIZE + 2*BOX_PADDING, LIGHT_SIZE + 2*BOX_PADDING};
        SDL_RenderFillRect(renderer, &lightBox);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderDrawRect(renderer, &lightBox);
        SDL_SetRenderDrawColor(renderer, lights[i].state ? 0 : 200, lights[i].state ? 200 : 0, 0, 255);
        SDL_Rect outerGlow = {lights[i].x - 2, lights[i].y - 2, LIGHT_SIZE + 4, LIGHT_SIZE + 4};
        SDL_RenderFillRect(renderer, &outerGlow);
        SDL_SetRenderDrawColor(renderer, lights[i].state ? 0 : 255, lights[i].state ? 255 : 0, 0, 255);
        SDL_Rect lightBulb = {lights[i].x, lights[i].y, LIGHT_SIZE, LIGHT_SIZE};
        SDL_RenderFillRect(renderer, &lightBulb);
    }
}

void drawVehicles(SDL_Renderer* renderer, Queue* queues[]) {
    for (int i = 0; i < 12; i++) {
        Queue* queue = queues[i];
        for (int j = 0; j < queue->size; j++) {
            Vehicle* vehicle = queue->items[(queue->front + j) % MAX_QUEUE_SIZE];
            if (!vehicle) continue;
            switch(vehicle->type) {
                case 0: SDL_SetRenderDrawColor(renderer, 30, 144, 255, 255); break;
                case 1: SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); break;
                case 2: SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255); break;
                case 3: SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255); break;
            }
            SDL_Rect vehicleRect = {(int)vehicle->x - VEHICLE_SIZE/2, (int)vehicle->y - VEHICLE_SIZE/2, VEHICLE_SIZE, VEHICLE_SIZE};
            SDL_RenderFillRect(renderer, &vehicleRect);
        }
    }
}

void updateVehiclePositions(Queue* queues[], bool lightStates[]) {
    for (int i = 0; i < 12; i++) {
        Queue* queue = queues[i];
        if (isEmpty(queue)) continue;
        int direction = i / 3; // 0=A, 1=B, 2=C, 3=D
        int lane = i % 3;

        for (int j = 0; j < queue->size; j++) {
            Vehicle* vehicle = queue->items[(queue->front + j) % MAX_QUEUE_SIZE];
            if (!vehicle) continue;

            // Determine lane offset
            float laneOffset;
            if (direction == 1 || direction == 3) {
                laneOffset = ((2 - lane) + 0.5) * LANE_WIDTH;
            } else {
                laneOffset = (lane + 0.5) * LANE_WIDTH;
            }

            // Set vehicle position based on direction
            switch (direction) {
                case 0: vehicle->x = WINDOW_WIDTH / 2 - ROAD_WIDTH / 2 + laneOffset; break; // A
                case 1: vehicle->y = WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2 + laneOffset; break; // B
                case 2: vehicle->y = WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2 + laneOffset; break; // C
                case 3: vehicle->x = WINDOW_WIDTH / 2 - ROAD_WIDTH / 2 + laneOffset; break; // D
            }

            // Check if vehicle is at the zebra crossing
            bool atZebra = false, pastIntersection = false;
            switch (direction) {
                case 0: // A (Southbound)
                    atZebra = vehicle->y >= WINDOW_HEIGHT / 2 + ROAD_WIDTH / 2 - 40 && vehicle->y < WINDOW_HEIGHT / 2;
                    break;
                case 1: // B (Westbound)
                    atZebra = vehicle->x >= WINDOW_WIDTH / 2 - ROAD_WIDTH / 2 - 40 && vehicle->x < WINDOW_WIDTH / 2;
                    break;
                case 2: // C (Eastbound)
                    atZebra = vehicle->x <= WINDOW_WIDTH / 2 + ROAD_WIDTH / 2 + 40 && vehicle->x > WINDOW_WIDTH / 2;
                    pastIntersection = vehicle->x > WINDOW_WIDTH / 2;
                    break;
                case 3: // D (Northbound)
                    atZebra = vehicle->y <= WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2 + 40 && vehicle->y > WINDOW_HEIGHT / 2;
                    pastIntersection = vehicle->y < WINDOW_HEIGHT / 2;
                    break;
            }

            // Determine if the vehicle can proceed through the intersection
            bool canMove = canProceedThroughIntersection(vehicle, lightStates);
            Vehicle* ahead = (j > 0) ? queue->items[(queue->front + j - 1) % MAX_QUEUE_SIZE] : NULL;
            if (ahead && !isSafeDistance(vehicle, ahead)) canMove = false;

            // Stop at zebra crossing if light is red (unless past intersection or emergency)
            if (atZebra && !pastIntersection && !canMove) {
                vehicle->waitTime++;
                switch (direction) {
                    case 0: vehicle->y = WINDOW_HEIGHT / 2 + ROAD_WIDTH / 2 + 5; break; // A
                    case 1: vehicle->x = WINDOW_WIDTH / 2 - ROAD_WIDTH / 2 - 5; break; // B
                    case 2: vehicle->x = WINDOW_WIDTH / 2 + ROAD_WIDTH / 2 + 5; break; // C
                    case 3: vehicle->y = WINDOW_HEIGHT / 2 - ROAD_WIDTH / 2 - 5; break; // D
                }
                continue; // Skip further movement logic
            }

            // Update vehicle position if allowed to move
            if (canMove) {
                updateVehiclePosition(vehicle);
            } else {
                vehicle->waitTime++;
            }

            // Check if the vehicle should be dequeued and moved to the next queue
            bool shouldDequeue = false;
            switch (direction) {
                case 0: shouldDequeue = vehicle->y < -VEHICLE_SIZE; break; // A
                case 1: shouldDequeue = vehicle->x < -VEHICLE_SIZE; break; // B
                case 2: shouldDequeue = vehicle->x > WINDOW_WIDTH + VEHICLE_SIZE; break; // C
                case 3: shouldDequeue = vehicle->y > WINDOW_HEIGHT + VEHICLE_SIZE; break; // D
            }
            if (shouldDequeue) {
                int nextQueueIndex = vehicle->endDirection * 3 + vehicle->endLane;
                if (nextQueueIndex >= 0 && nextQueueIndex < 12) enqueue(queues[nextQueueIndex], vehicle);
                else free(vehicle);
                dequeue(queue);
            }
        }
    }
}

void processVehiclesFromFile(Queue* queues[], const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Vehicle* vehicle = (Vehicle*)malloc(sizeof(Vehicle));
        if (!vehicle) continue;
        int turning, passed;
        int startDir, endDir, startLane, endLane; // Use ints for sscanf
        sscanf(line, "%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%d,%f,%d,%d",
            &vehicle->vehicleId, &vehicle->type, &startDir, &endDir,
            &startLane, &endLane, &vehicle->x, &vehicle->y, &vehicle->speed,
            &vehicle->turnAngle, &turning, &vehicle->progress, &vehicle->waitTime, &passed);
        vehicle->startDirection = (Direction)startDir; // Cast to Direction
        vehicle->endDirection = (Direction)endDir;     // Cast to Direction
        vehicle->startLane = (LanePosition)startLane;  // Cast to LanePosition
        vehicle->endLane = (LanePosition)endLane;      // Cast to LanePosition
        vehicle->turning = turning;
        vehicle->passedIntersection = passed;
        int queueIndex = vehicle->startDirection * 3 + vehicle->startLane;
        if (queueIndex >= 0 && queueIndex < 12) enqueue(queues[queueIndex], vehicle);
        else free(vehicle);
    }
    fclose(file);
    truncate(filename, 0);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("Traffic Junction Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TrafficLight lights[4] = {
        {310, 370, false, "t1"}, // t1 (Southwest corner)
        {310, 210, false, "t2"}, // t2 (Northwest corner)
        {470, 210, false, "t3"}, // t3 (Northeast corner)
        {470, 370, false, "t4"}  // t4 (Southeast corner)
    };

    Queue* queues[12];
    for (int dir = 0; dir < 4; dir++) {
        for (int lane = 0; lane < 3; lane++) {
            queues[dir * 3 + lane] = createQueue(dir, lane);
        }
    }

    srand(time(NULL));
    bool lightStates[4] = {false, false, false, false}; // All red initially
    int currentLight = 0, lightTimer = 0;
    const int LIGHT_CYCLE_TIME = 5000; // 5 seconds

    int vehicleGenTimer = 0, VEHICLE_GEN_INTERVAL = 20;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        int queueLengths[4] = {0};
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 3; j++) queueLengths[i] += getSize(queues[i*3 + j]);
        }

        processVehiclesFromFile(queues, "vehicles.txt");

        vehicleGenTimer++;
        if (vehicleGenTimer >= VEHICLE_GEN_INTERVAL) {
            vehicleGenTimer = 0;
            for (int dir = 0; dir < 4; dir++) {
                if (queueLengths[dir] < 8) {
                    int lane = rand() % 2; // Lane 0 (Left) or Lane 1 (Center)
                    Vehicle* vehicle = malloc(sizeof(Vehicle));
                    if (vehicle) {
                        vehicle->vehicleId = rand();
                        vehicle->type = (rand() % 100 < 90) ? 0 : (1 + rand() % 3);
                        vehicle->startDirection = dir;
                        vehicle->startLane = lane;
                        vehicle->speed = 2.0f + (rand() % 15) / 10.0f; // Faster speed
                        float laneOffset;
                        if (dir == 2 || dir == 3) { 
                            laneOffset = ((2 - lane) + 0.5) * LANE_WIDTH;
                        } else {
                            laneOffset = (lane + 0.5) * LANE_WIDTH;
                        }
                        switch(dir) {
                            case 0: // A (Southbound)
                                vehicle->x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + laneOffset;
                                vehicle->y = WINDOW_HEIGHT + VEHICLE_SIZE;
                                break;
                            case 1: // B (Westbound)
                                vehicle->x = WINDOW_WIDTH + VEHICLE_SIZE;
                                vehicle->y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + laneOffset;
                                break;
                            case 2: // C (Eastbound)
                                vehicle->x = -VEHICLE_SIZE;
                                vehicle->y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + laneOffset;
                                break;
                            case 3: // D (Northbound)
                                vehicle->x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + laneOffset;
                                vehicle->y = -VEHICLE_SIZE;
                                break;
                        }
                        vehicle->turning = false;
                        vehicle->turnAngle = 0.0f;
                        vehicle->progress = 0.0f;
                        vehicle->waitTime = 0;
                        vehicle->passedIntersection = false;
                        setVehiclePath(vehicle);
                        enqueue(queues[dir * 3 + lane], vehicle);
                    }
                }
            }
        }

        updateVehiclePositions(queues, lightStates);

        lightTimer += 16; // Increment by frame delay (16ms ~ 60 FPS)
        if (lightTimer >= LIGHT_CYCLE_TIME) {
            lightTimer = 0;
            lightStates[currentLight] = false;
            currentLight = (currentLight + 1) % 4; // Cycle: t1 -> t2 -> t3 -> t4
            lightStates[currentLight] = true;
            for (int i = 0; i < 4; i++) lights[i].state = lightStates[i];
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        drawRoads(renderer);
        drawTrafficLights(renderer, lights);
        drawVehicles(renderer, queues);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    for (int i = 0; i < 12; i++) {
        while (!isEmpty(queues[i])) {
            Vehicle* vehicle = dequeue(queues[i]);
            if (vehicle) free(vehicle);
        }
        free(queues[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
