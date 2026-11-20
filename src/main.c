#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"
#include "moisture.h"
#include "main.h"
#include "pump.h"


static zone_ctx_t zones[ZONE_COUNT];

static void zone_ctx_init()
{
    int64_t now = esp_timer_get_time() / 1000;
      for (uint8_t i = 0; i < ZONE_COUNT; i++) {
        zones[i] = (zone_ctx_t){
            .id = i,
            .state = ST_IDLE,
            .state_since_ms = now,
            .last_check_ms = now - CHECK_INTERVAL_MS,  
        };
    }
}

static void zone_processor(zone_ctx_t *zone)
{
    if(zone->state == ST_IDLE && zone->moisture_before > 2000)
    {
        zone->state = ST_WATERING;
    }
    else if( zone->state == ST_WATERING)
    {
        zone->state = ST_IDLE;
    }

}

// поки вєбати 2 стейта не вийобуввтись з часм бьільшк
static void zone_tick(zone_ctx_t *zone)
{
    switch(zone->state)
    {
        case ST_IDLE:
            zone->moisture_before = moisture_check(zone->id);
            zone->last_check_ms = esp_timer_get_time() / 1000;
            zone_processor(zone);
            break;
        case ST_WATERING:
            pump_on(zone->id);
            vTaskDelay(pdMS_TO_TICKS(2000));
            pump_off(zone->id);
            zone_processor(zone);
            break;
        case ST_SOAKING:
            break;
        default:
            break;
    }
}


void app_main(void)
{
    config_all_adc_moisture();
    zone_ctx_init();

    while (true) {
        for (uint8_t i = 0; i < ZONE_COUNT; i++) {
            zone_tick(&zones[i]);
        }
        vTaskDelay(pdMS_TO_TICKS(CHECK_INTERVAL_MS));
    }
}