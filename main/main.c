#include <stdio.h>

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "http_client.h"
#include "cJSON.h"

#define MAX_QUERY_SIZE 180
#define JSON_MAX_SIZE 600

#define IPSTACK_KEY CONFIG_ESP_IPSTACK_KEY
#define OPENWEATHER_KEY CONFIG_ESP_OPENWEATHER_KEY

xSemaphoreHandle connectionWifiSemaphore;

void RealizaHTTPRequest(void * params)
{
    while(true)
    {
        if(xSemaphoreTake(connectionWifiSemaphore, portMAX_DELAY))
        {
            char query[MAX_QUERY_SIZE] = {0};
            char response_json[JSON_MAX_SIZE] = {0};

            sprintf(query, "access_key=%s&fields=main.latitude,main.longitude", IPSTACK_KEY);

            http_get_json("api.ipstack.com", "/check", query, response_json);

            cJSON *location_info = cJSON_Parse(response_json);

            double latitude = cJSON_GetObjectItem(location_info, "latitude")->valuedouble;
            double longitude = cJSON_GetObjectItem(location_info, "longitude")->valuedouble;

            sprintf(query, "lat=%lf&lon=%lf&appid=%s&units=metric", latitude, longitude, OPENWEATHER_KEY);

            http_get_json("api.openweathermap.org", "/data/2.5/weather", query, response_json);

            cJSON *weather_info = cJSON_Parse(response_json);

            cJSON *main_info = cJSON_GetObjectItem(weather_info, "main");

            double temp = cJSON_GetObjectItem(main_info, "temp")->valuedouble;
            double temp_min = cJSON_GetObjectItem(main_info, "temp_min")->valuedouble;
            double temp_max = cJSON_GetObjectItem(main_info, "temp_max")->valuedouble;
            double humidity = cJSON_GetObjectItem(main_info, "humidity")->valuedouble;

            printf("\nCurrent temperature = %0.2lfº\n"
                   "Minimum temperature = %0.2lfº\n"
                   "Maximum temperature = %0.2lfº\n"
                   "Humidity = %0.2lf %%\n"
                   , temp, temp_min, temp_max, humidity);
        }
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

    connectionWifiSemaphore = xSemaphoreCreateBinary();
    wifi_start();

    xTaskCreate(&RealizaHTTPRequest,  "Processa HTTP", 4096, NULL, 1, NULL);
}
