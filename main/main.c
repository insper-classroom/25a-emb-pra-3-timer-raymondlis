#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "sensor.h"
#include "rtc_utils.h"

const int TRIGGER_PIN = 17;
const int ECHO_PIN = 16;

int main() {
    stdio_init_all();

    rtc_init_config(2025, 3, 17, 11, 20, 0);

    sensor_init(TRIGGER_PIN, ECHO_PIN);

    printf("Iniciando medicao...\n");

    while (true) {
        bool nova_med = false;
        bool timeout_ocor = false;

        enviar_pulso_trigger(TRIGGER_PIN);
        sensor_start_timeout(30);
        sleep_ms(40);

        char time_buffer[16];
        rtc_get_formatted_time(time_buffer, sizeof(time_buffer));

        float distancia = obter_distancia(&nova_med, &timeout_ocor);

        if (timeout_ocor) {
            printf("%s - Falha\n", time_buffer);
        } else if (nova_med) {
            printf("%s - %.2f cm\n", time_buffer, distancia);
        }

        sleep_ms(960);
    }

    return 0;
}