/* sched.c  –  gcc -o sched sched.c */    
#include <stdio.h>    
#include <stdlib.h>    
#include <string.h>    
#include <limits.h>    
#define MAX 10    
   
typedef struct {    
    int pid, arr, burst, pri, rem, fin, wt, tat, done;    
} Proc;    
   
Proc p[MAX], bak[MAX];    
int n;    
   
void reset() {    
    memcpy(p, bak, sizeof(Proc) * n);    
    for (int i = 0; i < n; i++) { 
        p[i].rem = p[i].burst; 
        p[i].done = 0; 
    }    
}   
   
double print_res(const char *name) {    
    printf("\n=== %s ===\n", name);    
    printf("PID  Arr  Burst  Finish  Wait  TAT\n");    
    double tw = 0;    
    for (int i = 0; i < n; i++) {    
        printf("P%d   %3d  %5d  %6d  %4d  %3d\n",    
               p[i].pid, p[i].arr, p[i].burst, p[i].fin, p[i].wt, p[i].tat);    
        tw += p[i].wt;    
    }    
    printf("Avg Wait = %.2f\n", tw / n);    
    return tw / n;    
}   
   
double fcfs() {    
    reset();    
    /* Sort by arrival time */    
    for(int i = 0; i < n - 1; i++)    
        for(int j = i + 1; j < n; j++)    
            if(p[j].arr < p[i].arr) { Proc t = p[i]; p[i] = p[j]; p[j] = t; }    
    
    int t = 0;    
    for(int i = 0; i < n; i++){    
        if(t < p[i].arr) t = p[i].arr;    
        p[i].fin = t + p[i].burst;    
        p[i].tat = p[i].fin - p[i].arr;    
        p[i].wt  = p[i].tat - p[i].burst;    
        t = p[i].fin;    
    }    
    return print_res("FCFS");    
}   
   
double srtf() {    
    reset();    
    int done = 0, t = 0;    
    while(done < n){    
        int mn = INT_MAX, idx = -1;   
        for(int i = 0; i < n; i++)    
            if(!p[i].done && p[i].arr <= t && p[i].rem < mn) { 
                mn = p[i].rem; 
                idx = i; 
            }    
        if(idx == -1) { t++; continue; }    
        p[idx].rem--; 
        t++;    
        if(p[idx].rem == 0){    
            p[idx].fin = t;    
            p[idx].tat = t - p[idx].arr;    
            p[idx].wt  = p[idx].tat - p[idx].burst;    
            p[idx].done = 1; 
            done++;    
        }    
    }    
    return print_res("SRTF (Preemptive SJF)");    
}   
   
double priority_np() {    
    reset();    
    int done = 0, t = 0;    
    while(done < n){    
        int mx = -1, idx = -1;    
        for(int i = 0; i < n; i++)    
            if(!p[i].done && p[i].arr <= t && p[i].pri > mx) { 
                mx = p[i].pri; 
                idx = i; 
            }    
        if(idx == -1) { t++; continue; }    
        t += p[idx].burst;    
        p[idx].fin = t;    
        p[idx].tat = t - p[idx].arr;    
        p[idx].wt  = p[idx].tat - p[idx].burst;    
        p[idx].done = 1; 
        done++;    
    }    
    return print_res("Non-Preemptive Priority (higher=better)");    
}   
   
double rr() {    
    reset();    
    int Q = 3, done = 0, t = 0;    
    int q[1000], fr = 0, rr_ptr = 0;    
    int inq[MAX] = {0};    
    
    // Enqueue all processes that arrive at time 0
    for(int i = 0; i < n; i++) {
        if(p[i].arr <= t) { 
            q[rr_ptr++] = i; 
            inq[i] = 1; 
        }
    }    
   
    while(done < n){    
        if(fr == rr_ptr){ 
            t++;    
            for(int i = 0; i < n; i++)    
                if(!p[i].done && !inq[i] && p[i].arr <= t) { 
                    q[rr_ptr++] = i; 
                    inq[i] = 1; 
                }    
            continue;    
        }    
        int i = q[fr++];    
        int ex = (p[i].rem > Q) ? Q : p[i].rem;    
        p[i].rem -= ex; 
        t += ex;    
        
        /* 1. Enqueue newly arrived processes during execution */    
        for(int j = 0; j < n; j++)    
            if(!p[j].done && !inq[j] && p[j].arr <= t) { 
                q[rr_ptr++] = j; 
                inq[j] = 1; 
            }    
            
        /* 2. Re-enqueue current process if it's not finished */
        if(p[i].rem == 0){    
            p[i].fin = t; 
            p[i].tat = t - p[i].arr;    
            p[i].wt = p[i].tat - p[i].burst;    
            p[i].done = 1; 
            done++;    
        } else { 
            q[rr_ptr++] = i; 
        }    
    }    
    return print_res("Round Robin (Q=3)");    
}   
   
int main() {    
    printf("Enter number of processes: ");    
    if (scanf("%d", &n) != 1 || n > MAX) return 1;
    
    printf("Enter: Arrival Burst Priority for each process\n");    
    for(int i = 0; i < n; i++){    
        p[i].pid = i + 1;    
        printf("P%d: ", i + 1);    
        scanf("%d %d %d", &p[i].arr, &p[i].burst, &p[i].pri);    
    }    
    memcpy(bak, p, sizeof(Proc) * n);    
   
    double w[4];    
    w[0] = fcfs();  
    w[1] = srtf();  
    w[2] = priority_np(); 
    w[3] = rr();    
   
    const char *names[] = {"FCFS", "SRTF", "Priority", "Round Robin"};    
    int best = 0;    
    for(int i = 1; i < 4; i++) {
        if(w[i] < w[best]) best = i;
    }    
    printf("\nBEST: %s with avg wait = %.2f\n", names[best], w[best]);    
    return 0;    
}

