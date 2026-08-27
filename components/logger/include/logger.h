#ifndef BTHOME_ROUTER_LOGGER_H
#define BTHOME_ROUTER_LOGGER_H

#include "esp_log.h"

void logger_init(void);

#define BTR_LOGI(TAG, fmt, ...) ESP_LOGI(TAG, fmt, ##__VA_ARGS__)
#define BTR_LOGE(TAG, fmt, ...) ESP_LOGE(TAG, fmt, ##__VA_ARGS__)
#define BTR_LOGW(TAG, fmt, ...) ESP_LOGW(TAG, fmt, ##__VA_ARGS__)
#define BTR_LOGD(TAG, fmt, ...) ESP_LOGD(TAG, fmt, ##__VA_ARGS__)
#define BTR_LOGV(TAG, fmt, ...) ESP_LOGV(TAG, fmt, ##__VA_ARGS__)

#define BTR_EARLY_LOGI(TAG, fmt, ...) ESP_EARLY_LOGI(TAG, fmt, ##__VA_ARGS__)
#define BTR_EARLY_LOGE(TAG, fmt, ...) ESP_EARLY_LOGE(TAG, fmt, ##__VA_ARGS__)
#define BTR_EARLY_LOGW(TAG, fmt, ...) ESP_EARLY_LOGW(TAG, fmt, ##__VA_ARGS__)
#define BTR_EARLY_LOGD(TAG, fmt, ...) ESP_EARLY_LOGD(TAG, fmt, ##__VA_ARGS__)
#define BTR_EARLY_LOGV(TAG, fmt, ...) ESP_EARLY_LOGV(TAG, fmt, ##__VA_ARGS__)

#define BTR_LOGI_HEX(TAG, buffer, size) ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, size, ESP_LOG_INFO)
#define BTR_LOGE_HEX(TAG, buffer, size) ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, size, ESP_LOG_ERROR)
#define BTR_LOGW_HEX(TAG, buffer, size) ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, size, ESP_LOG_WARN)
#define BTR_LOGD_HEX(TAG, buffer, size) ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, size, ESP_LOG_DEBUG)
#define BTR_LOGV_HEX(TAG, buffer, size) ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, size, ESP_LOG_VERBOSE)

#endif // BTHOME_ROUTER_LOGGER_H
