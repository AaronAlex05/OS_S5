#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

int available[MAX_RESOURCES];
int maximum[MAX_PROCESSES][MAX_RESOURCES];
int allocation[MAX_PROCESSES][MAX_RESOURCES];
int need[MAX_PROCESSES][MAX_RESOURCES];

void calculateNeed(int processes, int resources) {
    for (int i = 0; i < processes; i++) {
        for (int j = 0; j < resources; j++) {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
}

bool isSafeState(int processes, int resources, int* work, bool* finish) {
    int localWork[MAX_RESOURCES];
    bool localFinish[MAX_PROCESSES] = {false};
    
    // Use provided arrays or initialize local ones
    int* workArray = work ? work : localWork;
    bool* finishArray = finish ? finish : localFinish;
    
    // Initialize work array if not provided
    if (!work) {
        for (int i = 0; i < resources; i++) {
            workArray[i] = available[i];
        }
    }
    
    // Initialize finish array if not provided
    if (!finish) {
        for (int i = 0; i < processes; i++) {
            finishArray[i] = false;
        }
    }

    bool found;
    do {
        found = false;
        for (int p = 0; p < processes; p++) {
            if (!finishArray[p]) {
                bool canAllocate = true;
                for (int r = 0; r < resources; r++) {
                    if (need[p][r] > workArray[r]) {
                        canAllocate = false;
                        break;
                    }
                }
                if (canAllocate) {
                    for (int r = 0; r < resources; r++) {
                        workArray[r] += allocation[p][r];
                    }
                    finishArray[p] = true;
                    found = true;
                }
            }
        }
    } while (found);

    // Check if all processes finished
    for (int i = 0; i < processes; i++) {
        if (!finishArray[i]) {
            return false;
        }
    }
    return true;
}

bool requestResources(int process, int request[], int processes, int resources) {
    // Validate process number
    if (process < 0 || process >= processes) {
        printf("Error: Invalid process number\n");
        return false;
    }

    // Check if request is valid
    for (int i = 0; i < resources; i++) {
        if (request[i] < 0) {
            printf("Error: Cannot request negative resources\n");
            return false;
        }
        if (request[i] > need[process][i]) {
            printf("Error: Process %d is requesting more resources than it needs\n", process);
            return false;
        }
        if (request[i] > available[i]) {
            printf("Error: Not enough resources available\n");
            return false;
        }
    }

    // Save current state
    int tempAvailable[MAX_RESOURCES];
    int tempAllocation[MAX_PROCESSES][MAX_RESOURCES];
    int tempNeed[MAX_PROCESSES][MAX_RESOURCES];
    
    for (int i = 0; i < resources; i++) {
        tempAvailable[i] = available[i];
        for (int j = 0; j < processes; j++) {
            tempAllocation[j][i] = allocation[j][i];
            tempNeed[j][i] = need[j][i];
        }
    }

    // Try to allocate resources
    for (int i = 0; i < resources; i++) {
        available[i] -= request[i];
        allocation[process][i] += request[i];
        need[process][i] -= request[i];
    }

    // Check if resulting state is safe
    if (isSafeState(processes, resources, NULL, NULL)) {
        printf("Request granted - Safe state\n");
        return true;
    } else {
        // Restore previous state
        for (int i = 0; i < resources; i++) {
            available[i] = tempAvailable[i];
            for (int j = 0; j < processes; j++) {
                allocation[j][i] = tempAllocation[j][i];
                need[j][i] = tempNeed[j][i];
            }
        }
        printf("Request denied - Would result in unsafe state\n");
        return false;
    }
}

void printSystemState(int processes, int resources) {
    printf("\nCurrent System State:\n");
    
    printf("\nAvailable Resources:\n");
    for (int i = 0; i < resources; i++) {
        printf("R%d: %d  ", i, available[i]);
    }
    
    printf("\n\nMaximum Matrix:\n");
    for (int i = 0; i < processes; i++) {
        printf("P%d |", i);
        for (int j = 0; j < resources; j++) {
            printf(" %d ", maximum[i][j]);
        }
        printf("|\n");
    }
    
    printf("\nAllocation Matrix:\n");
    for (int i = 0; i < processes; i++) {
        printf("P%d |", i);
        for (int j = 0; j < resources; j++) {
            printf(" %d ", allocation[i][j]);
        }
        printf("|\n");
    }
    
    printf("\nNeed Matrix:\n");
    for (int i = 0; i < processes; i++) {
        printf("P%d |", i);
        for (int j = 0; j < resources; j++) {
            printf(" %d ", need[i][j]);
        }
        printf("|\n");
    }
    printf("\n");
}

int main() {
    int processes, resources;
    char buffer[100];  // Buffer for input cleaning

    // Get number of processes
    do {
        printf("Enter number of processes: ");
        if (scanf("%d", &processes) != 1) {
            scanf("%s", buffer);  // Clear invalid input
            printf("Please enter a valid number\n");
            continue;
        }
    } while (processes <= 0 || processes > MAX_PROCESSES);

    // Get number of resources
    do {
        printf("Enter number of resources: ");
        if (scanf("%d", &resources) != 1) {
            scanf("%s", buffer);  // Clear invalid input
            printf("Please enter a valid number\n");
            continue;
        }
    } while (resources <= 0 || resources > MAX_RESOURCES);

    // Input available resources
    printf("\nEnter the available resources:\n");
    for (int i = 0; i < resources; i++) {
        do {
            printf("Available Resource %d: ", i);
            if (scanf("%d", &available[i]) != 1 || available[i] < 0) {
                scanf("%s", buffer);  // Clear invalid input
                printf("Please enter a valid non-negative number\n");
                continue;
            }
            break;
        } while (1);
    }

    // Input maximum resource matrix
    printf("\nEnter the maximum resource matrix:\n");
    for (int i = 0; i < processes; i++) {
        printf("For Process %d:\n", i);
        for (int j = 0; j < resources; j++) {
            do {
                printf("Maximum of Resource %d: ", j);
                if (scanf("%d", &maximum[i][j]) != 1 || maximum[i][j] < 0) {
                    scanf("%s", buffer);  // Clear invalid input
                    printf("Please enter a valid non-negative number\n");
                    continue;
                }
                break;
            } while (1);
        }
    }

    // Input allocation matrix
    printf("\nEnter the allocation matrix:\n");
    for (int i = 0; i < processes; i++) {
        printf("For Process %d:\n", i);
        for (int j = 0; j < resources; j++) {
            do {
                printf("Currently Allocated Resource %d: ", j);
                if (scanf("%d", &allocation[i][j]) != 1 || allocation[i][j] < 0) {
                    scanf("%s", buffer);  // Clear invalid input
                    printf("Please enter a valid non-negative number\n");
                    continue;
                }
                if (allocation[i][j] > maximum[i][j]) {
                    printf("Error: Allocation cannot exceed maximum (%d)\n", maximum[i][j]);
                    continue;
                }
                break;
            } while (1);
        }
    }

    // Calculate need matrix
    calculateNeed(processes, resources);

    // Check initial state
    if (isSafeState(processes, resources, NULL, NULL)) {
        printf("\nInitial state is safe. Starting resource manager...\n");
        
        while (1) {
            printf("\nBanker's Algorithm Resource Manager\n");
            printf("1. Request resources\n");
            printf("2. Print current state\n");
            printf("3. Exit\n");
            printf("Enter choice: ");
            
            int choice;
            if (scanf("%d", &choice) != 1) {
                scanf("%s", buffer);  // Clear invalid input
                printf("Please enter a valid choice\n");
                continue;
            }

            switch (choice) {
                case 1: {
                    int process;
                    int request[MAX_RESOURCES];
                    
                    printf("Enter process number (0-%d): ", processes - 1);
                    if (scanf("%d", &process) != 1) {
                        scanf("%s", buffer);  // Clear invalid input
                        printf("Invalid process number\n");
                        break;
                    }
                    
                    if (process < 0 || process >= processes) {
                        printf("Invalid process number\n");
                        break;
                    }

                    printf("Enter resource request for process %d:\n", process);
                    bool validInput = true;
                    for (int i = 0; i < resources; i++) {
                        printf("Resource %d: ", i);
                        if (scanf("%d", &request[i]) != 1 || request[i] < 0) {
                            scanf("%s", buffer);  // Clear invalid input
                            printf("Invalid request\n");
                            validInput = false;
                            break;
                        }
                    }
                    
                    if (validInput) {
                        requestResources(process, request, processes, resources);
                    }
                    break;
                }
                case 2:
                    printSystemState(processes, resources);
                    break;
                case 3:
                    printf("Exiting program\n");
                    return 0;
                default:
                    printf("Invalid choice\n");
            }
        }
    } else {
        printf("\nThe initial state is not safe. Cannot continue.\n");
    }

    return 0;
}
