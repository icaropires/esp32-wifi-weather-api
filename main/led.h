#ifndef LED_H_
#define LED_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define LED_TAG "led"
#define BLUE_LED 2

typedef enum {
    LED_STATE_ON,
    LED_STATE_OFF,
    LED_STATE_BLINK_CONTINUOUS,
    LED_STATE_BLINK_FAST,
} LedState;

extern xQueueHandle led_states_queue;
extern xSemaphoreHandle is_led_setup_sem;

void led_setup();

void led_start();

void led_blink(bool fast);

void led_on();

void led_off();

#endif // LED_H_
