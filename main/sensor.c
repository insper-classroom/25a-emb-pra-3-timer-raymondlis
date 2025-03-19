#include "sensor.h"
#include "hardware/timer.h"
#include "pico/time.h"

static void echo_callback(uint gpio, uint32_t events);

static absolute_time_t t_inicial;
static absolute_time_t t_final;
static volatile bool timeout_ocorrido = false;
static volatile bool nova_medicao = false;
static alarm_id_t alarme = 0;

static int64_t alarme_callback(alarm_id_t id, void *user_data) {
    timeout_ocorrido = true;
    return 0;
}

void sensor_init(uint trigger_pin, uint echo_pin) {
    gpio_init(trigger_pin);
    gpio_set_dir(trigger_pin, GPIO_OUT);
    gpio_put(trigger_pin, 0);

    gpio_init(echo_pin);
    gpio_set_dir(echo_pin, GPIO_IN);
    gpio_set_irq_enabled_with_callback(echo_pin,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_callback);
}

void enviar_pulso_trigger(uint trigger_pin) {
    gpio_put(trigger_pin, 1);
    sleep_us(10);
    gpio_put(trigger_pin, 0);
}

static void echo_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_RISE) {
        t_inicial = get_absolute_time();
    } else if (events & GPIO_IRQ_EDGE_FALL) {
        t_final = get_absolute_time();
        nova_medicao = true;
        timeout_ocorrido = false;
        if (alarme) {
            cancel_alarm(alarme);
            alarme = 0;
        }
    }
}

void sensor_start_timeout(uint32_t timeout_ms) {
    alarme = add_alarm_in_ms(timeout_ms, alarme_callback, NULL, false);
}

float obter_distancia(bool *nova_medicao_flag, bool *timeout_ocorrido_flag) {
    *nova_medicao_flag = nova_medicao;
    *timeout_ocorrido_flag = timeout_ocorrido;

    if (nova_medicao) {
        int64_t tempo = absolute_time_diff_us(t_inicial, t_final);
        nova_medicao = false;
        return (tempo * 0.0343f) / 2.0f;
    }
    return -1.0f;
}
