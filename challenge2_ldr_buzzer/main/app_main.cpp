// โจทย์ท้าทายที่ 2: LDR-based Buzzer Alarm
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"

#define LDR_CHANNEL   ADC1_CHANNEL_7  // GPIO35
#define BUZZER_PIN    GPIO_NUM_4      // GPIO4
#define DARK_THRESHOLD 1000           // ค่า ADC ที่ถือว่ามืด

extern "C" void app_main(void)
{
    // --- ADC Configuration ---
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(LDR_CHANNEL, ADC_ATTEN_DB_11);

    // --- GPIO (Buzzer) Configuration ---
    gpio_reset_pin(BUZZER_PIN);
    gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);

    while(1) {
        int adc_raw = adc1_get_raw(LDR_CHANNEL);

        if (adc_raw < DARK_THRESHOLD) {
            // สภาพมืด: เปิด Buzzer
            gpio_set_level(BUZZER_PIN, 1);
            printf("ADC: %d -> Dark! Sounding alarm.\n", adc_raw);
        } else {
            // สภาพสว่าง: ปิด Buzzer
            gpio_set_level(BUZZER_PIN, 0);
            printf("ADC: %d -> Bright. Alarm off.\n", adc_raw);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

