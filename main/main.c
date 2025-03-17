#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/time.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"

const int TRIGGER_PIN = 17;
const int ECHO_PIN    = 16;

volatile absolute_time_t t_inicial;
volatile absolute_time_t t_final;
volatile alarm_id_t alarme;
volatile bool timeout_ocorrido = false;
volatile bool nova_medicao = false;
volatile float distancia_cm = 0;

int64_t alarme_callback(alarm_id_t id, void *user_data) {
    timeout_ocorrido = true;
    return 0;
}

void echo_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_RISE) {
        t_inicial = get_absolute_time();
    } else if (events & GPIO_IRQ_EDGE_FALL) {
        t_final = get_absolute_time();
        int64_t tempo = absolute_time_diff_us(t_inicial, t_final);
        distancia_cm = (tempo * 0.0343f) / 2;
        nova_medicao = true;
        cancel_alarm(alarme);
        timeout_ocorrido = false;
    }
}

void pulso_trigger() {
    gpio_put(TRIGGER_PIN, 1);
    sleep_us(10);
    gpio_put(TRIGGER_PIN, 0);
}

int main() {
    stdio_init_all();

    gpio_init(TRIGGER_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_put(TRIGGER_PIN, 0);
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ECHO_PIN,GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_callback);
    
    rtc_init();
    datetime_t t_rtc = {
        .year  = 2025,
        .month = 3,
        .day   = 17,
        .dotw  = 2,
        .hour  = 11,
        .min   = 20,
        .sec   = 0,
    };
    rtc_set_datetime(&t_rtc);
    
    while (true) {
        nova_medicao = false;
        timeout_ocorrido = false;

        pulso_trigger();
        alarme = add_alarm_in_ms(30, alarme_callback, NULL, false);
        sleep_ms(40);
        datetime_t current_time;
        rtc_get_datetime(&current_time);

        if (timeout_ocorrido) {
            printf("%02d:%02d:%02d - Falha\n", current_time.hour, current_time.min, current_time.sec);
        } else if (nova_medicao) {
            printf("%02d:%02d:%02d - %.2f cm\n", current_time.hour, current_time.min, current_time.sec, distancia_cm);
        }
        sleep_ms(960);
    }
}
