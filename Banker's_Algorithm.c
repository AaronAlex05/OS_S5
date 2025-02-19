#include <stdio.h>
#include <stdbool.h>

#define MAX_PROCESSES 5
#define MAX_RESOURCES 3

int available[MAX_RESOURCES];
int maximum[MAX_PROCESSES][MAX_RESOURCES];
int allocation[MAX_PROCESSES][MAX_RESOURCES];
int need[MAX_PROCESSES][MAX_RESOURCES];

bool isSafeState(int processes, int resources) {
    int work[MAX_RESOURCES];
    bool finish[MAX_PROCESSES] = false;

    // Initialize work with available resources
    for (int i = 0; i < resources; i++) {
        work[i] = available[i];
    }

    while (true) {
        bool found = false;
        for (int p = 0; p < processes; p++) {
            if (!finish[p]) {
                bool canAllocate = true;
                for (int r = 0; r < resources; r++) {
                    if (need[p][r] > work[r]) {
                        canAllocate = false;
                        break;
                    }
                }
                if (canAllocate) {
                    // Simulate allocation
                    for (int r = 0; r < resources; r++) {
                        work[r] += allocation[p][r];
                    }
                    finish[p] = true;
                    found = true;
                }
            }
        }
        if (!found) {
            break;
        }
    }

    // Check if all processes finished
    for (int i = 0; i < processes; i++) {
        if (!finish[i]) {
            return false;
        }
    }
    return true;
}

void calculateNeed(int processes, int resources) {
    for (int i = 0; i < processes; i++) {
        for (int j = 0; j < resources; j++) {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
}

int main() {
    int processes = MAX_PROCESSES;
    int resources = MAX_RESOURCES;

    // Input available resources
    printf("Enter the available resources: ");
    for (int i = 0; i < resources; i++) {
        scanf("%d", &available[i]);
    }

    // Input maximum resource matrix
    printf("Enter the maximum resource matrix: \n");
    for (int i = 0; i < processes; i++) {
        for (int j = 0; j < resources; j++) {
            scanf("%d", &maximum[i][j]);
        }
    }

    // Input allocation matrix
    printf("Enter the allocation matrix: \n");
    for (int i = 0; i < processes; i++) {
        for (int j = 0; j < resources; j++) {
            scanf("%d", &allocation[i][j]);
            // Validate allocation does not exceed maximum
            if (allocation[i][j] > maximum[i][j]) {
                printf("Error: Allocation cannot exceed maximum for process %d resource %d.\n", i, j);
                return 1; // Exit with error
            }
        }
    }

    // Calculate need matrix
    calculateNeed(processes, resources);

    // Check if the system is in a safe state
    if (isSafeState(processes, resources)) {
        printf("The system is in a safe state.\n");
    } else {
        printf("The system is not in a safe state.\n");
    }

    return 0;
}