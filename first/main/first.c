#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

#define LED 2

static const int SLOW_TIME = 2300;
static const int FAST_TIME = 1100;

static unsigned int toggle = 0;

void toggleLED(void *parameters) {
    while(true) {
        toggle = !toggle;
        ESP_LOGI(pcTaskGetName(NULL), "%s",(toggle)?"HIGH":"LOW");
        ESP_LOGI(pcTaskGetName(NULL), "Using %d bytes", uxTaskGetStackHighWaterMark(NULL));
        gpio_set_level(LED, toggle);
        vTaskDelay((*(int*)parameters)/portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    char *ourTaskName = pcTaskGetName(NULL);
    ESP_LOGI(ourTaskName, "Hello Starting up");

    //Initialize Output pins
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, toggle);

    //Define pointers
    const int* slow = &SLOW_TIME;
    const int* fast = &FAST_TIME;

    //Define Tasks
    xTaskCreatePinnedToCore(
        toggleLED,
        "Led Toggle SLOW",
        2048,
        (void*)slow,
        1,
        NULL,
        app_cpu
        );

    xTaskCreatePinnedToCore(
        toggleLED,
        "Led Toggle FAST",
        2048,
        (void*)fast,
        1,
        NULL,
        app_cpu
    );

    while(1) {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
