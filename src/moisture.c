#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "moisture.h"


#define RAW_DRY 3280   // приблизно: сухий грунт
#define RAW_WET 1140   // приблизно: мокрий грунт


static adc_oneshot_unit_handle_t adc;

static const adc_channel_t moisture_pins[] = {ADC_CHANNEL_0, ADC_CHANNEL_2};

static int read_avg(adc_channel_t ch)
{
    int sum = 0, v = 0;
    for(int i = 0; i < 16; i ++){
        adc_oneshot_read(adc, ch, &v);
        sum += v;
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    return sum / 16;
}

static int to_pct(int raw)
{
    int pct = (RAW_DRY - raw) * 100 / (RAW_DRY - RAW_WET);
    if(pct < 0) pct = 0;
    if(pct > 100) pct = 100;
    return pct;
}

void config_all_adc_moisture(void)
{
    adc_oneshot_unit_init_cfg_t unit = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit, &adc));

    adc_oneshot_chan_cfg_t cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    for(int i = 0; i < sizeof(moisture_pins)/sizeof(*moisture_pins); i++)
    {
         ESP_ERROR_CHECK(adc_oneshot_config_channel(adc,moisture_pins[i] , &cfg));
    }
   
}

int moisture_check(int i)
{
    
    // int moisture_percent = to_pct(moisture_avg);
    return read_avg(moisture_pins[i]);
} 

