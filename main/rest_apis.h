#ifndef REST_APIS_H_
#define REST_APIS_H_

#include <stdio.h>

#include "nvs_flash.h"

#include "http_client.h"
#include "cJSON.h"

#define MAX_QUERY_SIZE 180
#define JSON_MAX_SIZE 600

#define IPSTACK_KEY CONFIG_ESP_IPSTACK_KEY
#define OPENWEATHER_KEY CONFIG_ESP_OPENWEATHER_KEY

void fetch_coordinates(double *latitude, double *longitude);

void fetch_weather(double latitude, double longitude, double *temp, double *temp_min, double *temp_max, double *humidity);

#endif // REST_APIS_H_
