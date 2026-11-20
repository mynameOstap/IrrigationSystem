#pragma once

#include "esp_adc/adc_oneshot.h"

struct moisture_sensor_config {
    adc_channel_t channel;
};


void config_all_adc_moisture(void);

int moisture_check(int i);
