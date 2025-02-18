#include <string.h>

#include "http_client.h"

#include "esp_event.h"
#include "esp_http_client.h"

#include "esp_log.h"
#include "esp_tls.h"

#include "led.h"
// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#define MAX_HTTP_OUTPUT_BUFFER 512

static const char *TAG = "HTTP_CLIENT";


esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer = NULL;
    static int output_len = 0;

    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

            if (evt->user_data) {
                memcpy(evt->user_data + output_len, evt->data, evt->data_len);
            } else {
                if (output_buffer == NULL) {
                    output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                    output_len = 0;
                    if (output_buffer == NULL) {
                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                memcpy(output_buffer + output_len, evt->data, evt->data_len);
            }
            output_len += evt->data_len;

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                if (output_buffer != NULL) {
                    free(output_buffer);
                    output_buffer = NULL;
                }
                output_len = 0;
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            break;
    }
    return ESP_OK;
}

void http_get_json(char *host, char *path, char *query, char *result){
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    esp_http_client_config_t config = {
        .host = host,
        .path = path,
        .query = query,
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    led_blink(true);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Success get to %s, json size = %d", host, strlen(local_response_buffer));

        ESP_LOGD(TAG,
                 "JSON received:\n"
                 "@@@@@@@@@@@@@@@@@@@@\n"
                 "%s\n"
                 "@@@@@@@@@@@@@@@@@@@@\n",
                 local_response_buffer);

        strcpy(result, local_response_buffer);

    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}
