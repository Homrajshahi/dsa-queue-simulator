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

// Window and road dimensions
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define ROAD_WIDTH 120
#define LANE_WIDTH (ROAD_WIDTH/3)
#define INTERSECTION_SIZE (ROAD_WIDTH * 1.2)
#define VEHICLE_SIZE 20
// Note: SAFE_DISTANCE is now defined in queue.h

// Traffic light positions structure
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
    const int LIGHT_SIZE = 25;
    const int BOX_PADDING = 5;

    for (int i = 0; i < 4; i++) {
        // Draw black background box
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_Rect lightBox = {
            lights[i].x - BOX_PADDING,
            lights[i].y - BOX_PADDING,
            LIGHT_SIZE + 2*BOX_PADDING,
            LIGHT_SIZE + 2*BOX_PADDING
        };
        SDL_RenderFillRect(renderer, &lightBox);

        // Draw metallic border
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_Rect border = lightBox;
        SDL_RenderDrawRect(renderer, &border);

        // Draw the light with a glowing effect
        if (lights[i].state) {
            // Green light with glow
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
            SDL_Rect outerGlow = {
                lights[i].x - 2,
                lights[i].y - 2,
                LIGHT_SIZE + 4,
                LIGHT_SIZE + 4
            };
            SDL_RenderFillRect(renderer, &outerGlow);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        } else {
            // Red light with glow
            SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
            SDL_Rect outerGlow = {
                lights[i].x - 2,
                lights[i].y - 2,
                LIGHT_SIZE + 4,
                LIGHT_SIZE + 4
            };
            SDL_RenderFillRect(renderer, &outerGlow);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }

        SDL_Rect lightBulb = {
            lights[i].x,
            lights[i].y,
            LIGHT_SIZE,
            LIGHT_SIZE
        };
        SDL_RenderFillRect(renderer, &lightBulb);
    }
}

void drawVehicles(SDL_Renderer* renderer, Queue* queues[]) {
    for (int i = 0; i < 12; i++) {
        Queue* queue = queues[i];
        for (int j = 0; j < queue->size; j++) {
            Vehicle* vehicle = queue->items[(queue->front + j) % MAX_QUEUE_SIZE];
            if (!vehicle) continue;

            // Set color based on vehicle type with better visibility
            switch(vehicle->type) {
                case 0: // Regular car - Blue
                    SDL_SetRenderDrawColor(renderer, 30, 144, 255, 255);
                    break;
                case 1: // Ambulance - Red
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    break;
                case 2: // Police car - Dark Blue
                    SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255);
                    break;
                case 3: // Fire truck - Orange
                    SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255);
                    break;
            }

            SDL_Rect vehicleRect = {
                (int)vehicle->x - VEHICLE_SIZE/2,
                (int)vehicle->y - VEHICLE_SIZE/2,
                VEHICLE_SIZE,
                VEHICLE_SIZE
            };
            SDL_RenderFillRect(renderer, &vehicleRect);
        }
    }
}

void updateVehiclePositions(Queue* queues[], bool lightStates[]) {
    for (int i = 0; i < 12; i++) {
        Queue* queue = queues[i];
        if (isEmpty(queue)) continue;

        int direction = i / 3;  // 0=North, 1=East, 2=South, 3=West
        int lane = i % 3;      // 0=Left, 1=Middle, 2=Right

        for (int j = 0; j < queue->size; j++) {
            Vehicle* current = queue->items[(queue->front + j) % MAX_QUEUE_SIZE];
            if (!current) continue;

            // Calculate lane offset
            float laneOffset = (lane + 0.5) * LANE_WIDTH;

            // Keep vehicle aligned to lane
            switch(direction) {
                case 0: // North
                    current->x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + laneOffset;
                    break;
                case 1: // East
                    current->y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + laneOffset;
                    break;
                case 2: // South
                    current->x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + laneOffset;
                    break;
                case 3: // West
                    current->y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + laneOffset;
                    break;
            }

            // Get vehicle ahead if any
            Vehicle* ahead = NULL;
            if (j > 0) {
                ahead = queue->items[(queue->front + j - 1) % MAX_QUEUE_SIZE];
            }

            // Check if at zebra crossing
            bool atZebraCrossing = false;
            switch(direction) {
                case 0:
                    atZebraCrossing = (current->y >= WINDOW_HEIGHT/2 - ROAD_WIDTH/2 - 40 &&
                                     current->y <= WINDOW_HEIGHT/2 - ROAD_WIDTH/2);
                    break;
                case 1:
                    atZebraCrossing = (current->x <= WINDOW_WIDTH/2 + ROAD_WIDTH/2 &&
                                     current->x >= WINDOW_WIDTH/2 + ROAD_WIDTH/2 - 40);
                    break;
                case 2:
                    atZebraCrossing = (current->y <= WINDOW_HEIGHT/2 + ROAD_WIDTH/2 + 40 &&
                                     current->y >= WINDOW_HEIGHT/2 + ROAD_WIDTH/2);
                    break;
                case 3:
                    atZebraCrossing = (current->x >= WINDOW_WIDTH/2 - ROAD_WIDTH/2 - 40 &&
                                     current->x <= WINDOW_WIDTH/2 - ROAD_WIDTH/2);
                    break;
            }

            // Determine if vehicle can move
            bool canMove = true;

            // Stop at red light or if too close to vehicle ahead
            if (atZebraCrossing && !lightStates[direction]) {
                canMove = false;
                current->waitTime++;
            }

            if (ahead && !isSafeDistance(current, ahead)) {
                canMove = false;
                current->waitTime++;
            }

            if (canMove) {
                float movement = current->speed;

                // Slow down near intersection or when following
                if (atZebraCrossing || (ahead && ahead->type > 0)) {
                    movement *= 0.5;
                }

                // Handle turning vehicles
                if (current->turning && lightStates[direction]) {
                    current->progress += movement / 100.0f;

                    if (current->progress >= 1.0f) {
                        current->turning = false;
                        // Set final position after turn
                        switch(current->endDirection) {
                            case 0: current->y -= VEHICLE_SIZE; break;
                            case 1: current->x += VEHICLE_SIZE; break;
                            case 2: current->y += VEHICLE_SIZE; break;
                            case 3: current->x -= VEHICLE_SIZE; break;
                        }
                    } else {
                        // Calculate turning arc
                        float angle = current->progress * M_PI / 2;
                        float radius = ROAD_WIDTH * 0.8;
                        float centerX = WINDOW_WIDTH/2;
                        float centerY = WINDOW_HEIGHT/2;

                        switch(direction) {
                            case 0: // North to West
                                current->x = centerX - radius * (1 - cos(angle));
                                current->y = centerY - radius * sin(angle);
                                break;
                            case 1: // East to North
                                current->x = centerX + radius * sin(angle);
                                current->y = centerY - radius * (1 - cos(angle));
                                break;
                            case 2: // South to East
                                current->x = centerX + radius * (1 - cos(angle));
                                current->y = centerY + radius * sin(angle);
                                break;
                            case 3: // West to South
                                current->x = centerX - radius * sin(angle);
                                current->y = centerY + radius * (1 - cos(angle));
                                break;
                        }
                    }
                } else {
                    // Straight movement with boundary checking
                    float newX = current->x;
                    float newY = current->y;

                    switch(direction) {
                        case 0: newY += movement; break;
                        case 1: newX -= movement; break;
                        case 2: newY -= movement; break;
                        case 3: newX += movement; break;
                    }

                    // Only update if within bounds
                    if (newX >= 0 && newX <= WINDOW_WIDTH &&
                        newY >= 0 && newY <= WINDOW_HEIGHT) {
                        current->x = newX;
                        current->y = newY;
                    }
                }

                // Start turning if at intersection and in left lane
                if (!current->turning && current->startLane == 0 && lightStates[direction]) {
                    bool atIntersection = false;
                    switch(direction) {
                        case 0:
                            atIntersection = current->y >= WINDOW_HEIGHT/2 - ROAD_WIDTH/2;
                            break;
                        case 1:
                            atIntersection = current->x <= WINDOW_WIDTH/2 + ROAD_WIDTH/2;
                            break;
                        case 2:
                            atIntersection = current->y <= WINDOW_HEIGHT/2 + ROAD_WIDTH/2;
                            break;
                        case 3:
                            atIntersection = current->x >= WINDOW_WIDTH/2 - ROAD_WIDTH/2;
                            break;
                    }

                    if (atIntersection) {
                        current->turning = true;
                        current->progress = 0.0f;
                    }
                }

                // Check if vehicle has passed through junction
                bool passed = false;
                if (!current->turning) {
                    switch(direction) {
                        case 0:
                            passed = current->y > WINDOW_HEIGHT/2 + ROAD_WIDTH/2;
                            break;
                        case 1:
                            passed = current->x < WINDOW_WIDTH/2 - ROAD_WIDTH/2;
                            break;
                        case 2:
                            passed = current->y < WINDOW_HEIGHT/2 - ROAD_WIDTH/2;
                            break;
                        case 3:
                            passed = current->x > WINDOW_WIDTH/2 + ROAD_WIDTH/2;
                            break;
                    }
                }

                if (passed) {
                    dequeue(queue);
                    free(current);
                }
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

        int direction, lane;
        sscanf(line, "%d,%d,%d,%d,%f,%f,%f",
            &vehicle->vehicleId,
            &vehicle->type,
            &direction,
            &lane,
            &vehicle->x,
            &vehicle->y,
            &vehicle->speed);

        vehicle->startDirection = (Direction)direction;
        vehicle->startLane = (LanePosition)lane;

        // Set default values
        vehicle->turning = false;
        vehicle->turnAngle = 0.0f;
        vehicle->progress = 0.0f;
        vehicle->waitTime = 0;

        // Calculate end direction and lane based on vehicle path
        setVehiclePath(vehicle);

        int queueIndex = direction * 3 + lane;
        if (queueIndex >= 0 && queueIndex < 12) {
            enqueue(queues[queueIndex], vehicle);
        } else {
            free(vehicle);
        }
    }

    fclose(file);
    truncate(filename, 0);
}

int main() {
    // Initialize SDL with error checking
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Traffic Junction Simulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    printf("SDL initialization successful\n");

    // Initialize traffic lights
    TrafficLight lights[4] = {
        {WINDOW_WIDTH/2 - ROAD_WIDTH/2 - 30, WINDOW_HEIGHT/2 - ROAD_WIDTH/2 - 30, false, "t1"},
        {WINDOW_WIDTH/2 + ROAD_WIDTH/2 + 10, WINDOW_HEIGHT/2 - ROAD_WIDTH/2 - 30, false, "t2"},
        {WINDOW_WIDTH/2 - ROAD_WIDTH/2 - 30, WINDOW_HEIGHT/2 + ROAD_WIDTH/2 + 10, false, "t3"},
        {WINDOW_WIDTH/2 + ROAD_WIDTH/2 + 10, WINDOW_HEIGHT/2 + ROAD_WIDTH/2 + 10, false, "t4"}
    };

    // Create queues
    Queue* queues[12];
    for (int dir = 0; dir < 4; dir++) {
        for (int lane = 0; lane < 3; lane++) {
            queues[dir * 3 + lane] = createQueue(dir, lane);
        }
    }

    // Seed the random number generator
    srand(time(NULL));

    // Traffic light states and timing
    bool lightStates[4] = {true, false, false, false};
    int currentLight = 0;
    int lightTimer = 0;
    const int BASE_CYCLE_TIME = 150;
    const int MIN_CYCLE_TIME = 100;
    const int MAX_CYCLE_TIME = 200;

    // Vehicle generation timing
    int vehicleGenTimer = 0;
    const int VEHICLE_GEN_INTERVAL = 20;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Update queue lengths for traffic management
        int queueLengths[4] = {0};
        for (int i = 0; i < 4; i++) {
            for(int j = 0; j < 3; j++){
                queueLengths[i] += getSize(queues[i*3 + j]);
            }
        }

        // Process new vehicles from file
        processVehiclesFromFile(queues, "vehicles.txt");

        // Generate new vehicles periodically
        vehicleGenTimer++;
        if (vehicleGenTimer >= VEHICLE_GEN_INTERVAL) {
            vehicleGenTimer = 0;

            // Generate vehicles for each direction
            for (int dir = 0; dir < 4; dir++) {
                // Only generate if queue isn't too long
                if (queueLengths[dir] < 8) {
                    // Generate vehicles with higher probability for middle lane
                    int lane = rand() % 100;
                    if (lane < 40) {
                        lane = 1;  // 40% chance for middle lane
                    } else if (lane < 70) {
                        lane = 0;  // 30% chance for left lane
                    } else {
                        lane = 2;  // 30% chance for right lane
                    }

                    Vehicle* newVehicle = malloc(sizeof(Vehicle));
                    if (newVehicle) {
                        newVehicle->vehicleId = rand();
                        // More regular cars, fewer special vehicles
                        newVehicle->type = (rand() % 100) < 90 ? 0 : (1 + rand() % 3);
                        newVehicle->startDirection = dir;
                        newVehicle->startLane = lane;
                        newVehicle->speed = 1.5f + (rand() % 15) / 10.0f;

                        // Set initial position based on direction with proper lane alignment
                        float laneOffset = (lane + 0.5) * LANE_WIDTH;
                        switch(dir) {
                            case 0: // North
                                newVehicle->x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + laneOffset;
                                newVehicle->y = 0;
                                break;
                            case 1: // East
                                newVehicle->x = WINDOW_WIDTH;
                                newVehicle->y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + laneOffset;
                                break;
                            case 2: // South
                                newVehicle->x = WINDOW_WIDTH/2 - ROAD_WIDTH/2 + laneOffset;
                                newVehicle->y = WINDOW_HEIGHT;
                                break;
                            case 3: // West
                                newVehicle->x = 0;
                                newVehicle->y = WINDOW_HEIGHT/2 - ROAD_WIDTH/2 + laneOffset;
                                break;
                        }

                        // Initialize other vehicle properties
                        newVehicle->turning = false;
                        newVehicle->turnAngle = 0.0f;
                        newVehicle->progress = 0.0f;
                        newVehicle->waitTime = 0;

                        setVehiclePath(newVehicle);
                        enqueue(queues[dir * 3 + lane], newVehicle);
                    }
                }
            }
        }

        // Update vehicle positions
        updateVehiclePositions(queues, lightStates);

        // Dynamic traffic light timing based on queue lengths
        lightTimer++;
        int threshold = BASE_CYCLE_TIME;

        // Adjust timing based on queue length
        if (queueLengths[currentLight] > 6) {
            threshold = MAX_CYCLE_TIME;  // Longer green for busy lanes
        } else if (queueLengths[currentLight] < 2) {
            threshold = MIN_CYCLE_TIME;  // Shorter green for empty lanes
        }

        // Change traffic lights with smooth transition
        if (lightTimer >= threshold) {
            lightTimer = 0;
            lightStates[currentLight] = false;
            currentLight = (currentLight + 1) % 4;
            lightStates[currentLight] = true;

            // Update traffic light display
            for (int i = 0; i < 4; i++) {
                lights[i].state = lightStates[i];
            }
        }

        // Render everything
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        drawRoads(renderer);
        drawTrafficLights(renderer, lights);
        drawVehicles(renderer, queues);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // Cap at roughly 60 FPS
    }

    // Cleanup
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
