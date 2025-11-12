#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#define ALERT_PIN 5  

typedef struct {
    float temperature;  
    float humidity;     
    int heartRate;      
    float activity;      
    float locationLat;   
    float locationLon;  
    int alert;          
} chip_state_t;

chip_state_t *chip;

// Inicializa el chip
void chip_init() {
    chip = malloc(sizeof(chip_state_t));
    if (!chip) {
        fprintf(stderr, "Error al reservar memoria para el chip.\n");
        exit(1);
    }

    chip->temperature = 37.0;
    chip->humidity = 50.0;
    chip->heartRate = 80;
    chip->activity = 0.5;
    chip->locationLat = -34.6037;
    chip->locationLon = -58.3816;
    chip->alert = 0;

    printf("Smart Pet BioChip inicializado.\n");
}

// Función para actualizar métricas cada 10 segundos
void chip_update() {
    if (!chip) return;

    // Fluctuaciones aleatorias
    chip->temperature += ((rand() % 21) - 10) / 100.0; 
    chip->humidity += ((rand() % 11) - 5) / 10.0;    
    chip->heartRate += (rand() % 5) - 2;              
    chip->activity = rand() % 101 / 100.0;           
    chip->locationLat += ((rand() % 11) - 5) / 10000.0;
    chip->locationLon += ((rand() % 11) - 5) / 10000.0;

    // Limitar rangos
    if (chip->temperature < 36.0) chip->temperature = 36.0;
    if (chip->temperature > 40.0) chip->temperature = 40.0;
    if (chip->humidity < 30.0) chip->humidity = 30.0;
    if (chip->humidity > 80.0) chip->humidity = 80.0;
    if (chip->heartRate < 60) chip->heartRate = 60;
    if (chip->heartRate > 160) chip->heartRate = 160;

    // Estado de alerta
    chip->alert = (chip->temperature > 39.0 || chip->heartRate > 150) ? 1 : 0;

    // Imprimir métricas
    printf("\n-----------------------------\n");
    printf("Temperatura corporal: %.1f °C\n", chip->temperature);
    printf("Humedad corporal: %.1f %%\n", chip->humidity);
    printf("Ritmo cardíaco: %d bpm\n", chip->heartRate);
    printf("Nivel de actividad: %s\n", chip->activity > 0.6 ? "Alta" : (chip->activity > 0.3 ? "Moderada" : "Baja"));
    printf("Ubicación: %.5f, %.5f\n", chip->locationLat, chip->locationLon);
    printf("Estado alerta: %s\n", chip->alert ? "¡En peligro!" : "Todo bien");
    printf("-----------------------------\n");

    if (chip->alert) {
        printf("ALERT LED ON (pin %d)\n", ALERT_PIN);
    } else {
        printf("ALERT LED OFF (pin %d)\n", ALERT_PIN);
    }
    usleep(10000 * 1000); // 10.000 ms = 10 s
}

int main() {
    srand(time(NULL));
    chip_init();

    while (1) {
        chip_update();
    }

    free(chip);
    return 0;
}
