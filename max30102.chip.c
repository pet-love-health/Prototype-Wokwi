#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int heartRate;  
    int spo2;       
} pulse_state_t;

pulse_state_t pulse;

void pulse_init() {
    pulse.heartRate = 80;
    pulse.spo2 = 98;
    printf("MAX30102 inicializado.\n");
}

void pulse_update() {
    pulse.heartRate += (rand() % 5) - 2; 
    if (pulse.heartRate < 60) pulse.heartRate = 60;
    if (pulse.heartRate > 160) pulse.heartRate = 160;

    pulse.spo2 += (rand() % 3) - 1; 
    if (pulse.spo2 < 90) pulse.spo2 = 90;
    if (pulse.spo2 > 100) pulse.spo2 = 100;

    printf("PULSE:%d,%d\n", pulse.heartRate, pulse.spo2);

    usleep(10000 * 1000); 
}

int main() {
    srand(time(NULL));
    pulse_init();

    while (1) {
        pulse_update();
    }

    return 0;
}
