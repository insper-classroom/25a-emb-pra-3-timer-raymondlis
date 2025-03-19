#ifndef SENSOR_H
#define SENSOR_H

#include "pico/stdlib.h"

void sensor_init(uint trigger_pin, uint echo_pin);

void enviar_pulso_trigger(uint trigger_pin);

float obter_distancia(bool *nova_medicao, bool *timeout_ocorrido);

void sensor_start_timeout(uint32_t timeout_ms);

#endif
