#include <stdio.h>

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOSConfig.h"

#include "wifi.h"
#include "led.h"
#include "rest_apis.h"

xSemaphoreHandle connection_wifi_sem;
xSemaphoreHandle is_led_setup_sem;

void update_weather() {
    xSemaphoreTake(connection_wifi_sem, portMAX_DELAY);

    while(true) {
        double latitude, longitude;
        fetch_coordinates(&latitude, &longitude);

        double temp, temp_min, temp_max, humidity;
        fetch_weather(latitude, longitude, &temp, &temp_min, &temp_max, &humidity);

        printf("\nCurrent temperature = %0.2lfº\n"
                "Minimum temperature = %0.2lfº\n"
                "Maximum temperature = %0.2lfº\n"
                "Humidity = %0.2lf %%\n"
                , temp, temp_min, temp_max, humidity);

        TickType_t requests_interval = 5 * 60 * 1000 / portTICK_PERIOD_MS;
        vTaskDelay(requests_interval);
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    connection_wifi_sem = xSemaphoreCreateBinary();
    is_led_setup_sem = xSemaphoreCreateBinary();

    xTaskCreate(&led_start, "Controls the blue LED", 4096, NULL, 2, NULL);
    xTaskCreate(&update_weather,  "Shows weather information periodically", 4096, NULL, 1, NULL);

    wifi_start();
}
