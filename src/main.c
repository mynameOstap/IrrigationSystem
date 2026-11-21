#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

static void ST_IDLE_processor(zone_ctx_t *zone)
{
    zone->moisture_before = moisture_check(zone->id);
    zone->last_check_ms = esp_timer_get_time() / 1000;
    zone->state_since_ms = esp_timer_get_time() / 1000;
    if(zone->moisture_before > 2000)
    {
        zone->state = ST_WATERING;
    }
   
}

static void ST_WATERING_processor(zone_ctx_t *zone)
{
    zone->state_since_ms = esp_timer_get_time() / 1000;
    pump_on(zone->id);
    vTaskDelay(pdMS_TO_TICKS(2000));
    pump_off(zone->id);
    zone->state = ST_SOAKING;
    
}

static void ST_SOAKING_processor(zone_ctx_t *zone)
{
    zone->state_since_ms = esp_timer_get_time() / 1000;
    vTaskDelay(pdMS_TO_TICKS(SOAK_MS));
    zone->state = ST_IDLE;
    ST_IDLE_processor(zone);
}


static void zone_tick(zone_ctx_t *zone)
{
    switch(zone->state)
    {
        case ST_IDLE:
            ST_IDLE_processor(zone);
            break;
        case ST_WATERING:
            ST_WATERING_processor(zone);
            break;
        case ST_SOAKING:
            ST_SOAKING_processor(zone);
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