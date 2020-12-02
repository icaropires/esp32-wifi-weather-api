#include "led.h"

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

xQueueHandle led_states_queue;

extern xSemaphoreHandle is_led_setup_sem;

void led_setup(){
    gpio_reset_pin(BLUE_LED);

    gpio_set_direction(BLUE_LED, GPIO_MODE_OUTPUT);

    led_states_queue = xQueueCreate(1, sizeof(LedState));
}

void led_start() {
    led_setup();

    xSemaphoreGive(is_led_setup_sem);

    LedState last_state = LED_STATE_OFF;

    ESP_LOGI(LED_TAG, "Starting LED loop");

    while(true) {
        LedState state;

        TickType_t blink_interval = 1000 / portTICK_PERIOD_MS;

        TickType_t wait_on_queue = 50 / portTICK_PERIOD_MS;

        if(last_state == LED_STATE_BLINK_CONTINUOUS) {
            if(!xQueueReceive(led_states_queue, &state, wait_on_queue)) {
                state = last_state;
            }
        } else if (last_state == LED_STATE_BLINK_FAST) {
            if(!xQueueReceive(led_states_queue, &state, wait_on_queue)) {
                state = LED_STATE_ON;
            }
        } else {
            xQueueReceive(led_states_queue, &state, portMAX_DELAY);
        }

        switch(state) {

            case LED_STATE_ON:
                ESP_LOGD(LED_TAG, "Setting led state to ON");
                gpio_set_level(BLUE_LED, 1);

                break;

            case LED_STATE_OFF:
                ESP_LOGD(LED_TAG, "Setting led state to OFF");
                gpio_set_level(BLUE_LED, 1);

                break;

            case LED_STATE_BLINK_FAST:
                blink_interval = 200 / portTICK_PERIOD_MS;
                // fall through

            case LED_STATE_BLINK_CONTINUOUS:
                ESP_LOGD(LED_TAG, "Setting led state to BLINK");

                if (last_state == LED_STATE_ON) {
                    gpio_set_level(BLUE_LED, 0);
                    vTaskDelay(blink_interval);

                    gpio_set_level(BLUE_LED, 1);
                    vTaskDelay(blink_interval);
                } else {
                    gpio_set_level(BLUE_LED, 1);
                    vTaskDelay(blink_interval);

                    gpio_set_level(BLUE_LED, 0);
                    vTaskDelay(blink_interval);
                }

                break;

            default:
                ESP_LOGE(LED_TAG, "Invalid state received");
        }

        last_state = state;
    }
}

void led_blink(bool fast) {
    LedState state = LED_STATE_BLINK_CONTINUOUS;

    if(fast) {
        state = LED_STATE_BLINK_FAST;
    }

    xQueueOverwrite(led_states_queue, &state);
}

void led_on() {
    LedState state = LED_STATE_ON;
    xQueueOverwrite(led_states_queue, &state);
}

void led_off() {
    LedState state = LED_STATE_OFF;
    xQueueOverwrite(led_states_queue, &state);
}
