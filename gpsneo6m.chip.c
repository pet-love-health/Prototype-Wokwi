#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    float lat;
    float lon;
} gps_state_t;

gps_state_t gps;

void gps_init() {
    gps.lat = -34.6037;
    gps.lon = -58.3816;
    printf("GPS NEO-6M inicializado.\n");
}

void gps_update() {
    gps.lat += ((rand() % 11) - 5) / 10000.0;
    gps.lon += ((rand() % 11) - 5) / 10000.0;

    printf("GPS:%0.5f,%0.5f\n", gps.lat, gps.lon);

    usleep(10000 * 1000);
}

int main() {
    srand(time(NULL));
    gps_init();
    while (1) {
        gps_update();
    }
    return 0;
}
