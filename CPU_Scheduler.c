#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid; //process id
    int arrival_time; //arrival time
    int burst_time; //burst time
    int remaining_time; //remaining time
    int priority; //priority
    int waiting_time; //waiting time
    int turnaround_time; //turnaround time
    int start_time; //start time
    int end_time; //end time
} Process;

// Function to sort processes by arrival time
void sort_processes_by_arrival_time(Process processes[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                // Swap processes[j] and processes[j + 1]
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

void print_gantt_chart(int gantt_chart[][3], int gantt_index) {
    printf("\nGantt Chart:\n");
    for (int i = 0; i < gantt_index; i++) {
        if (gantt_chart[i][0] == -1) {
            printf("| ZZ "); // Indicate idle time
        } else {
            printf("| P%d ", gantt_chart[i][0]);
        }
    }
    printf("|\n");

    // Print the time values with proper spacing
    for (int i = 0; i < gantt_index; i++) {
        printf("%-5d", gantt_chart[i][1]); // Align time values
    }
    printf("%-5d\n", gantt_chart[gantt_index - 1][2]); // Last time value
}

void calculate_average_times(Process processes[], int n) {
    float total_waiting_time = 0, total_turnaround_time = 0;
    printf("\nProcess ID\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < n; i++) {
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, processes[i].waiting_time, processes[i].turnaround_time);
    }
    printf("\nAverage Waiting Time: %.2f", total_waiting_time / n);
    printf("\nAverage Turnaround Time: %.2f\n", total_turnaround_time / n);
}

void rr(Process processes[], int n, int time_quantum) {
    int current_time = 0, completed_processes = 0;
    int gantt_chart[100][3]; // To store Gantt chart details
    int gantt_index = 0;

    while (completed_processes < n) {
        int found_process = 0; // Flag to check if any process is executed in this round
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) {
                found_process = 1; // A process is found
                if (processes[i].start_time == -1) {
                    processes[i].start_time = current_time;
                }
                int time_slice = (processes[i].remaining_time < time_quantum) ? processes[i].remaining_time : time_quantum;
                gantt_chart[gantt_index][0] = processes[i].pid;
                gantt_chart[gantt_index][1] = current_time; // Start time for Gantt chart
                current_time += time_slice;
                processes[i].remaining_time -= time_slice;

                if (processes[i].remaining_time == 0) {
                    processes[i].end_time = current_time;
                    processes[i].turnaround_time = processes[i].end_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                    completed_processes++;
                }
                gantt_chart[gantt_index][2] = current_time; // End time for Gantt chart
                gantt_index++;
            }
        }
        if (!found_process) {
            // If no process is found, increment time and record idle time
            gantt_chart[gantt_index][0] = -1; // -1 indicates idle time
            gantt_chart[gantt_index][1] = current_time;
            current_time++;
            gantt_chart[gantt_index][2] = current_time;
            gantt_index++;
        }
    }

    print_gantt_chart(gantt_chart, gantt_index);
    calculate_average_times(processes, n);
}

void prr(Process processes[], int n, int time_quantum) {
    int current_time = 0, completed_processes = 0;
    int gantt_chart[100][3];
    int gantt_index = 0;
    int current_quantum[100] = {0}; // Track remaining quantum for each process
    
    // Initialize all processes
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].start_time = -1;
    }

    while (completed_processes < n) {
        int selected_process = -1;
        
        // Find the earliest arrived process that still needs processing
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) {
                if (selected_process == -1 || current_quantum[i] > current_quantum[selected_process]) {
                    selected_process = i;
                }
            }
        }

        if (selected_process == -1) {
            // No process available, record idle time
            if (gantt_index == 0 || gantt_chart[gantt_index-1][0] != -1) {
                gantt_chart[gantt_index][0] = -1; // Idle time
                gantt_chart[gantt_index][1] = current_time;
                gantt_index++;
            }
            current_time++;
            gantt_chart[gantt_index-1][2] = current_time;
        } else {
            // Record start time for the process if it's starting for the first time
            if (processes[selected_process].start_time == -1) {
                processes[selected_process].start_time = current_time;
            }

            // Start new Gantt chart entry if switching to a different process
            if (gantt_index == 0 || gantt_chart[gantt_index-1][0] != processes[selected_process].pid) {
                gantt_chart[gantt_index][0] = processes[selected_process].pid;
                gantt_chart[gantt_index][1] = current_time;
                gantt_index++;
            }

            // Execute for one time unit
            current_time++;
            processes[selected_process].remaining_time--;
            
            // Update quantum counter
            if (current_quantum[selected_process] == 0) {
                current_quantum[selected_process] = time_quantum;
            }
            current_quantum[selected_process]--;

            // Update Gantt chart end time
            gantt_chart[gantt_index-1][2] = current_time;

            // Check if process is completed
            if (processes[selected_process].remaining_time == 0) {
                processes[selected_process].end_time = current_time;
                processes[selected_process].turnaround_time = 
                    processes[selected_process].end_time - processes[selected_process].arrival_time;
                processes[selected_process].waiting_time = 
                    processes[selected_process].turnaround_time - processes[selected_process].burst_time;
                completed_processes++;
                current_quantum[selected_process] = 0;
            }
            // Reset quantum if expired
            else if (current_quantum[selected_process] == 0) {
                // Let other processes with remaining quantum execute first
                for (int i = 0; i < n; i++) {
                    if (i != selected_process && 
                        processes[i].remaining_time > 0 && 
                        processes[i].arrival_time <= current_time) {
                        current_quantum[selected_process] = 0;
                        break;
                    }
                }
            }
        }
    }

    print_gantt_chart(gantt_chart, gantt_index);
    calculate_average_times(processes, n);
}

void sjf(Process processes[], int n) {
    int current_time = 0, completed_processes = 0;
    int gantt_chart[100][3]; // To store Gantt chart details
    int gantt_index = 0;

    while (completed_processes < n) {
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) {
                if (idx == -1 || processes[i].burst_time < processes[idx].burst_time) {
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            if (processes[idx].start_time == -1) {
                processes[idx].start_time = current_time;
            }
            gantt_chart[gantt_index][0] = processes[idx].pid;
            gantt_chart[gantt_index][1] = current_time; // Start time for Gantt chart
            current_time += processes[idx].burst_time;
            processes[idx].remaining_time = 0;
            processes[idx].end_time = current_time;
            processes[idx].turnaround_time = processes[idx].end_time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
            gantt_chart[gantt_index][2] = current_time; // End time for Gantt chart
            gantt_index++;
            completed_processes++;
        } else {
            // If no process is found, increment time and record idle time
            gantt_chart[gantt_index][0] = -1; // -1 indicates idle time
            gantt_chart[gantt_index][1] = current_time;
            current_time++;
            gantt_chart[gantt_index][2] = current_time;
            gantt_index++;
        }
    }

    print_gantt_chart(gantt_chart, gantt_index);
    calculate_average_times(processes, n);
}

void psjf(Process processes[], int n) {
    int current_time = 0, completed_processes = 0;
    int gantt_chart[100][3];
    int gantt_index = 0;
    int prev_process = -1;
    // Initialize remaining time for all processes
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].start_time = -1;
    }
    while (completed_processes < n) {
        int idx = -1;
        // Find process with shortest remaining time that has arrived
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) {
                if (idx == -1 || processes[i].remaining_time < processes[idx].remaining_time) {
                    idx = i;
                }
                // If remaining times are equal, choose the one that arrived earlier
                else if (processes[i].remaining_time == processes[idx].remaining_time && 
                         processes[i].arrival_time < processes[idx].arrival_time) {
                    idx = i;
                }
            }
        }
        if (idx != -1) {
            // Record start time for the process if it's starting for the first time
            if (processes[idx].start_time == -1) {
                processes[idx].start_time = current_time;
            }
            // If we're switching to a different process, record in Gantt chart
            if (prev_process != idx) {
                if (gantt_index > 0) {
                    gantt_chart[gantt_index - 1][2] = current_time;
                }
                gantt_chart[gantt_index][0] = processes[idx].pid;
                gantt_chart[gantt_index][1] = current_time;
                gantt_index++;
                prev_process = idx;
            }
            // Execute for one time unit
            processes[idx].remaining_time--;
            current_time++;
            // If process completes
            if(processes[idx].remaining_time == 0){
                completed_processes++;
                processes[idx].end_time = current_time;
                processes[idx].turnaround_time = processes[idx].end_time - processes[idx].arrival_time;
                processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
                gantt_chart[gantt_index - 1][2] = current_time;
                prev_process = -1;
            }
        }
        else{
            // If no process is available, record idle time
            if (prev_process != -1) {
                gantt_chart[gantt_index - 1][2] = current_time;
            }
            gantt_chart[gantt_index][0] = -1; // -1 indicates idle time
            gantt_chart[gantt_index][1] = current_time;
            gantt_chart[gantt_index][2] = current_time + 1;
            gantt_index++;
            current_time++;
            prev_process = -1;
        }
    }
    print_gantt_chart(gantt_chart, gantt_index);
    calculate_average_times(processes, n);
}

void fcfs(Process processes[], int n) {
    int current_time = 0;
    int gantt_chart[100][3]; // To store Gantt chart details
    int gantt_index = 0;

    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time) {
            // If the CPU is idle, record idle time
            gantt_chart[gantt_index][0] = -1; // -1 indicates idle time
            gantt_chart[gantt_index][1] = current_time;
            current_time = processes[i].arrival_time; // Jump to the next process arrival
            gantt_chart[gantt_index][2] = current_time;
            gantt_index++;
        }
        processes[i].start_time = current_time;
        gantt_chart[gantt_index][0] = processes[i].pid;
        gantt_chart[gantt_index][1] = current_time; // Start time for Gantt chart
        current_time += processes[i].burst_time;
        processes[i].end_time = current_time;
        processes[i].turnaround_time = processes[i].end_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
        gantt_chart[gantt_index][2] = current_time; // End time for Gantt chart
        gantt_index++;
    }

    print_gantt_chart(gantt_chart, gantt_index);
    calculate_average_times(processes, n);
}

void srtf(Process processes[], int n) {
    int current_time = 0;
    int completed = 0;
    int gantt_chart[100][3];
    int gantt_index = 0;
    int prev_process = -1;
    
    // Create a copy of burst times since we'll be modifying them
    int remaining_time[n];
    for(int i = 0; i < n; i++) {
        remaining_time[i] = processes[i].burst_time;
        processes[i].start_time = -1;  // Initialize start time
    }
    
    while(completed != n) {
        int shortest = -1;
        int min_burst = INT_MAX;
        
        // Find process with shortest remaining time among arrived processes
        for(int i = 0; i < n; i++) {
            if(processes[i].arrival_time <= current_time && 
               remaining_time[i] > 0 && 
               remaining_time[i] < min_burst) {
                min_burst = remaining_time[i];
                shortest = i;
            }
        }
        
        // If no process is found, add idle time to Gantt chart
        if(shortest == -1) {
            if(prev_process != -1) {
                gantt_chart[gantt_index][0] = -1;  // -1 indicates idle
                gantt_chart[gantt_index][1] = current_time;
                // Find next arrival time
                int next_arrival = INT_MAX;
                for(int i = 0; i < n; i++) {
                    if(processes[i].arrival_time > current_time &&
                       processes[i].arrival_time < next_arrival) {
                        next_arrival = processes[i].arrival_time;
                    }
                }
                current_time = next_arrival;
                gantt_chart[gantt_index][2] = current_time;
                gantt_index++;
                prev_process = -1;
            } else {
                current_time++;
            }
            continue;
        }
        
        // If process changed, update Gantt chart
        if(prev_process != shortest) {
            if(prev_process != -1) {
                gantt_chart[gantt_index][0] = processes[prev_process].pid;
                gantt_chart[gantt_index][1] = gantt_chart[gantt_index-1][2];
                gantt_chart[gantt_index][2] = current_time;
                gantt_index++;
            }
            if(processes[shortest].start_time == -1) {
                processes[shortest].start_time = current_time;
            }
            prev_process = shortest;
        }
        
        // Execute process for 1 time unit
        remaining_time[shortest]--;
        current_time++;
        
        // If process completes
        if(remaining_time[shortest] == 0) {
            completed++;
            processes[shortest].end_time = current_time;
            processes[shortest].turnaround_time = 
                processes[shortest].end_time - processes[shortest].arrival_time;
            processes[shortest].waiting_time = 
                processes[shortest].turnaround_time - processes[shortest].burst_time;
            
            // Add final entry to Gantt chart
            gantt_chart[gantt_index][0] = processes[shortest].pid;
            gantt_chart[gantt_index][1] = gantt_chart[gantt_index-1][2];
            gantt_chart[gantt_index][2] = current_time;
            gantt_index++;
            prev_process = -1;
        }
    }
    
    print_gantt_chart(gantt_chart, gantt_index);
    calculate_average_times(processes, n);
}

void priority(Process processes[], int n) {
    int current_time = 0, completed_processes = 0;
    int gantt_chart[100][3]; // To store Gantt chart details
    int gantt_index = 0;

    while (completed_processes < n) {
        int idx = -1;
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) {
                if (idx == -1 || processes[i].priority < processes[idx].priority || 
                    (processes[i].priority == processes[idx].priority && processes[i].arrival_time < processes[idx].arrival_time)) {
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            if (processes[idx].start_time == -1) {
                processes[idx].start_time = current_time;
            }
            gantt_chart[gantt_index][0] = processes[idx].pid;
            gantt_chart[gantt_index][1] = current_time; // Start time for Gantt chart
            current_time += processes[idx].burst_time;
            processes[idx].remaining_time = 0;
            processes[idx].end_time = current_time;
            processes[idx].turnaround_time = processes[idx].end_time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
            gantt_chart[gantt_index][2] = current_time; // End time for Gantt chart
            gantt_index++;
            completed_processes++;
        } else {
            // If no process is found, increment time and record idle time
            gantt_chart[gantt_index][0] = -1; // -1 indicates idle time
            gantt_chart[gantt_index][1] = current_time;
            current_time++;
            gantt_chart[gantt_index][2] = current_time;
            gantt_index++;
        }
    }

    print_gantt_chart(gantt_chart, gantt_index);
    calculate_average_times(processes, n);
}

void ppriority(Process processes[], int n) {
    int current_time = 0, completed_processes = 0;
    int gantt_chart[100][3];
    int gantt_index = 0;
    int prev_process = -1;

    // Initialize remaining time for all processes
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].start_time = -1;
    }

    while (completed_processes < n) {
        int idx = -1;
        
        // Find process with highest priority (lowest priority number)
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) {
                if (idx == -1 || processes[i].priority < processes[idx].priority ||
                    (processes[i].priority == processes[idx].priority && processes[i].arrival_time < processes[idx].arrival_time)) {
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            // If this is a new process or different from previous process
            if (prev_process != processes[idx].pid) {
                if (prev_process != -1) {
                    // Record the previous process's end in Gantt chart
                    gantt_chart[gantt_index][2] = current_time;
                    gantt_index++;
                }
                
                // Start new Gantt chart entry
                gantt_chart[gantt_index][0] = processes[idx].pid;
                gantt_chart[gantt_index][1] = current_time;
                
                // Record start time if process is starting for the first time
                if (processes[idx].start_time == -1) {
                    processes[idx].start_time = current_time;
                }
            }
            
            // Execute process for one time unit
            processes[idx].remaining_time--;
            prev_process = processes[idx].pid;
            
            // If process completes
            if (processes[idx].remaining_time == 0) {
                processes[idx].end_time = current_time + 1;
                processes[idx].turnaround_time = processes[idx].end_time - processes[idx].arrival_time;
                processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
                completed_processes++;
                
                // Record completion in Gantt chart
                gantt_chart[gantt_index][2] = current_time + 1;
                gantt_index++;
                prev_process = -1;
            }
        } else {
            // If no process is available, record idle time
            if (prev_process != -1) {
                gantt_chart[gantt_index][2] = current_time;
                gantt_index++;
            }
            
            gantt_chart[gantt_index][0] = -1; // -1 indicates idle time
            gantt_chart[gantt_index][1] = current_time;
            gantt_chart[gantt_index][2] = current_time + 1;
            gantt_index++;
            prev_process = -1;
        }
        
        current_time++;
    }

    print_gantt_chart(gantt_chart, gantt_index);
    calculate_average_times(processes, n);
}

int main() {
    int n, c, time_quantum, k, c1, c2, c3, c4;
    printf("Enter the number of processes: ");
    scanf("%d", &n);
    Process *processes = (Process *)malloc(n * sizeof(Process));
    
    if (processes == NULL) {
        printf("Memory allocation failed.\n");
        return 1; // Exit if memory allocation fails
    }

    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        printf("Enter arrival time, burst time, and priority for process %d: ", i + 1);
        scanf("%d %d %d", &processes[i].arrival_time, &processes[i].burst_time, &processes[i].priority);
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].start_time = -1;
        processes[i].end_time = -1;
    }

    // Sort processes by arrival time
    sort_processes_by_arrival_time(processes, n);

    do {
        printf("\nSelect Scheduling Algorithm : \n1. Round Robin (RR)\n2. Shortest Job First (SJF)\n3. First Come First Serve (FCFS)\n4. Priority Scheduling\n5. Exit\n");
        scanf("%d", &c);
        // Reset remaining time for each algorithm
        for (int i = 0; i < n; i++) {
            processes[i].remaining_time = processes[i].burst_time;
            processes[i].waiting_time = 0; // Reset waiting time
            processes[i].turnaround_time = 0; // Reset turnaround time
            processes[i].start_time = -1; // Reset start time
            processes[i].end_time = -1; // Reset end time
        }

        switch (c) {
            case 1:
                printf("U have Chosen Round Robin Scheduling.\n");
                printf("Select which function to perform in Round Robin : \n1.Non-Premtive\n2.Premtive");
                scanf("%d", &c1);
                if(c1==1){
                    printf("Enter time quantum: ");
                    scanf("%d", &time_quantum);
                    rr(processes, n, time_quantum);
                    break;
                }
                else if(c1==2){
                    printf("Enter time quantum: ");
                    scanf("%d", &time_quantum);
                    prr(processes, n, time_quantum);
                    break;
                }
                else{
                    printf("Invalid choice. Please try again.\n");
                }
            case 2:
                printf("U have Chosen Shortest Job First Scheduling.\n");
                printf("Select which function to perform in Shortest Job First : \n1.Non-Premtive\n2.Premtive");
                scanf("%d", &c2);
                if(c2==1){
                        sjf(processes, n);
                        break;
                }
                else if(c2==2){
                        psjf(processes, n);
                        break;
                }
                else{
                        printf("Invalid choice. Please try again.\n");
                }
            case 3:
                printf("U have Chosen First Come First Serve Scheduling.\n");
                printf("Select which function to perform in First Come First Serve : \n1.Non-Premtive\n2.Premtive");
                scanf("%d", &c3);
                if(c3==1){
                    fcfs(processes, n);
                    break;
                }
                else if(c3==2){
                    srtf(processes, n);
                    break;
                }
                else{
                        printf("Invalid choice. Please try again.\n");
                }
            case 4:
                printf("U have Chosen Priority Scheduling.\n");
                printf("Select which function to perform in Priority : \n1.Non-Premtive\n2.Premtive");
                scanf("%d", &c4);
                if(c4==1){
                    priority(processes, n);
                    break;
                    }
                else if(c4==2){
                    ppriority(processes, n);
                    break;
                }
                else{
                    printf("Invalid choice. Please try again.\n");
                }
            case 5:
                printf("Thank u for using the CPU Scheduler.\n");
                printf("Good Bye.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (c != 5);
        
    free(processes);
    printf("Thank u for testing the CPU_Scheduler.\n");
    printf("Good Bye.\n");
    return 0;
}
