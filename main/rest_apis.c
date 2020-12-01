#include "rest_apis.h"

void fetch_coordinates(double *latitude, double *longitude) {
    char query[MAX_QUERY_SIZE] = {0};
    char response_json[JSON_MAX_SIZE] = {0};

    sprintf(query, "access_key=%s&fields=main.latitude,main.longitude", IPSTACK_KEY);

    http_get_json("api.ipstack.com", "/check", query, response_json);

    cJSON *location_info = cJSON_Parse(response_json);

    *latitude = cJSON_GetObjectItem(location_info, "latitude")->valuedouble;
    *longitude = cJSON_GetObjectItem(location_info, "longitude")->valuedouble;
}

void fetch_weather(double latitude, double longitude, double *temp, double *temp_min, double *temp_max, double *humidity) {
    char query[MAX_QUERY_SIZE] = {0};
    char response_json[JSON_MAX_SIZE] = {0};

    sprintf(query, "lat=%lf&lon=%lf&appid=%s&units=metric", latitude, longitude, OPENWEATHER_KEY);

    http_get_json("api.openweathermap.org", "/data/2.5/weather", query, response_json);

    cJSON *weather_info = cJSON_Parse(response_json);

    cJSON *main_info = cJSON_GetObjectItem(weather_info, "main");

    *temp = cJSON_GetObjectItem(main_info, "temp")->valuedouble;
    *temp_min = cJSON_GetObjectItem(main_info, "temp_min")->valuedouble;
    *temp_max = cJSON_GetObjectItem(main_info, "temp_max")->valuedouble;
    *humidity = cJSON_GetObjectItem(main_info, "humidity")->valuedouble;
}
