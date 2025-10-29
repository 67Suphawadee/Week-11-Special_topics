// โจทย์ท้าทายที่ 1: LDR-controlled LED (PWM)
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LDR_CHANNEL ADC1_CHANNEL_7      // GPIO35
#define LED_PIN     GPIO_NUM_2          // GPIO2 (Onboard LED on many boards)
#define LEDC_TIMER  LEDC_TIMER_0
#define LEDC_MODE   LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_0 LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_10_BIT // Resolution of PWM duty cycle, 10-bit (0-1023)

extern "C" void app_main(void)
{
    // --- ADC Configuration ---
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(LDR_CHANNEL, ADC_ATTEN_DB_11);

    // --- LEDC (PWM) Timer Configuration ---
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = 5000, // 5 kHz PWM frequency
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // --- LEDC (PWM) Channel Configuration ---
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LED_PIN,
        .duty           = 0, // Initial duty 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    while(1) {
        int adc_raw = adc1_get_raw(LDR_CHANNEL);

        // Map ADC value (0-4095) to LEDC duty cycle (0-1023)
        // More light (higher adc_raw) -> higher duty cycle -> brighter LED
        uint32_t duty_cycle = (adc_raw * 1023) / 4095;

        // Set duty and update
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, duty_cycle));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0));

        printf("ADC: %d, Duty Cycle: %d\n", adc_raw, (int)duty_cycle);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
