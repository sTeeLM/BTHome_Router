#include "nm.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_event.h"
#include "esp_gap_ble_api.h"
#include "esp_http_client.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "logger.h"
#include "bthome_parser.h"

const static char *TAG = "NM";

static bool nm_wifi_connected = false;
static esp_event_handler_instance_t instance_any_id = NULL;
static esp_event_handler_instance_t instance_got_ip = NULL;
static esp_netif_t *default_sta_netif = NULL;

static char nm_wifi_ssid[32] = {0};
static char nm_wifi_password[64] = {0};

static char nm_report_url[256] = {0};
static char nm_report_user[64] = {0};
static char nm_report_password[64] = {0};

static uint32_t nm_http_client_content_length;
static char nm_dynamic_resp_buffer[1024];

#define NM_HTTP_CLIENT_TIMEO_MS 5000

///////////////////////HTTP部分////////////////////////

static esp_err_t nm_http_client_event_handler(esp_http_client_event_t *evt) 
{
  switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      BTR_LOGE(TAG, "HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      BTR_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
      nm_http_client_content_length = 0;
      memset(nm_dynamic_resp_buffer, 0, sizeof(nm_dynamic_resp_buffer));
      break;
    case HTTP_EVENT_HEADER_SENT:
      BTR_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      BTR_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA:
      BTR_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      if (nm_http_client_content_length + evt->data_len < sizeof(nm_dynamic_resp_buffer) - 1) {
        // 将新到达的分段数据追加到 Buffer 末尾
        memcpy(nm_dynamic_resp_buffer + nm_http_client_content_length, evt->data, evt->data_len);
        nm_http_client_content_length += evt->data_len;
          nm_dynamic_resp_buffer[nm_http_client_content_length] = '\0'; // 保持字符串以 \0 结尾
        } else {
          BTR_LOGW(TAG, "Response data exceeds buffer size, truncating");
        }
      break;
    case HTTP_EVENT_ON_FINISH:
      BTR_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      BTR_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
      break;
    case HTTP_EVENT_REDIRECT:
      BTR_LOGD(TAG, "HTTP_EVENT_REDIRECT");
      break;
  }
  return ESP_OK;
}


static void nm_send_https_post(const char *json_payload) 
{
    esp_http_client_config_t config = {
        .url = nm_report_url,
        .method = HTTP_METHOD_POST,
        .skip_cert_common_name_check = true,
        .auth_type = HTTP_AUTH_TYPE_BASIC,
        .username = nm_report_user,
        .password = nm_report_password,
        .timeout_ms = NM_HTTP_CLIENT_TIMEO_MS,
        .event_handler = nm_http_client_event_handler,
        .user_agent = "BT-NM-Agent/1.0",
    };

    esp_err_t err;
    int32_t status_code;
    int64_t length;
    esp_http_client_handle_t client = NULL;

    if (!nm_wifi_connected) {
        BTR_LOGW(TAG, "Wi-Fi Not Connected, cannot send HTTPS POST");
        return;
    }

    client = esp_http_client_init(&config);
    if (client == NULL) {
        BTR_LOGE(TAG, "Cannot initialize HTTP client");
        goto err;
    }

    if(esp_http_client_set_header(client, "Content-Type", "application/json") != ESP_OK) {
        BTR_LOGE(TAG, "Failed to set Content-Type header");
        goto err;
    }
    if(esp_http_client_set_post_field(client, json_payload, strlen(json_payload)) != ESP_OK) {
        BTR_LOGE(TAG, "Failed to set POST field");
        goto err;
    }

    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
      status_code = esp_http_client_get_status_code(client);
      length = esp_http_client_get_content_length(client);
      BTR_LOGD(TAG, "HTTP POST Status = %d, content_length = %lld content = %s", 
        status_code, length, nm_dynamic_resp_buffer);
      if(status_code == 201 || status_code == 200) {
        BTR_LOGD(TAG, "HTTPS POST Success");
      } else {
        BTR_LOGW(TAG, "HTTPS POST Success but status code failed: %d", status_code);
      }
    } else {
        BTR_LOGE(TAG, "HTTPS POST Failed: %s", esp_err_to_name(err));
    }
err:
    if(NULL != client) {
        esp_http_client_cleanup(client);
    }
}

////////////////////////WiFi部分////////////////////////

static void nm_wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        nm_wifi_connected = false;
        BTR_LOGD(TAG, "Wi-Fi disconnected, attempting to reconnect...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        nm_wifi_connected = true;
        BTR_LOGD(TAG, "Wi-Fi connected, IP address obtained");
    }
}

void nm_start_wifi(void) 
{
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  wifi_config_t wifi_config = {0};

  BTR_LOGI(TAG, "Starting Wi-Fi...");

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  default_sta_netif = esp_netif_create_default_wifi_sta();
  
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, 
    &nm_wifi_event_handler, NULL, &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, 
    &nm_wifi_event_handler, NULL, &instance_got_ip));

  strncpy((char *)wifi_config.sta.ssid, nm_wifi_ssid, sizeof(wifi_config.sta.ssid));
  strncpy((char *)wifi_config.sta.password, nm_wifi_password, sizeof(wifi_config.sta.password));
  wifi_config.sta.threshold.authmode = WIFI_AUTH_WEP;

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
}

void nm_stop_wifi(void) 
{
  BTR_LOGI(TAG, "Stopping Wi-Fi and releasing network resources...");
  nm_wifi_connected = false;

  ESP_ERROR_CHECK(esp_wifi_stop());

  if (instance_any_id != NULL) {
      ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
      instance_any_id = NULL;
  }
  if (instance_got_ip != NULL) {
      ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
      instance_got_ip = NULL;
  }

  ESP_ERROR_CHECK(esp_wifi_deinit());

  if (default_sta_netif != NULL) {
      esp_netif_destroy_default_wifi(default_sta_netif);
      default_sta_netif = NULL;
  }

  ESP_ERROR_CHECK(esp_event_loop_delete_default());
  ESP_ERROR_CHECK(esp_netif_deinit());
  BTR_LOGI(TAG, "Wi-Fi Stopped");
}

//////////////////////BLE部分////////////////////////

#define NM_BTHOME_MAX_RAW_BUF_SIZE 62 // 31 bytes for Adv + 31 bytes for Scan Rsp
#define NM_BTHOME_MAX_CACHE_DEVICES 20 // Maximum number of devices to cache
#define NM_BTHOME_MAX_CACHE_TTL_MS 2000 // Maximum cache time-to-live for BT Home data

typedef struct _nm_bthome_raw_cache_t{
  uint8_t mac[6];
  uint32_t last_tick;
  uint8_t raw_buf[NM_BTHOME_MAX_RAW_BUF_SIZE];
  uint8_t adv_len;
  uint8_t scan_rsp_len;
  int32_t last_rssi;
} nm_bthome_raw_cache_t;

static nm_bthome_raw_cache_t nm_bthome_device_cache[NM_BTHOME_MAX_CACHE_DEVICES];

static nm_bthome_raw_cache_t *nm_find_or_create_cache(uint8_t *mac) 
{
  uint32_t now = xTaskGetTickCount();
  int oldest_idx = 0;
  uint32_t oldest_tick = 0xFFFFFFFF;

  for (int i = 0; i < NM_BTHOME_MAX_CACHE_DEVICES; i++) {
    if (memcmp(nm_bthome_device_cache[i].mac, mac, 6) == 0) {
      // 如果缓存时间超过 2 秒，认为是设备新一轮的广播，重置旧缓存
      if ((now - nm_bthome_device_cache[i].last_tick) > pdMS_TO_TICKS(NM_BTHOME_MAX_CACHE_TTL_MS)) {
        nm_bthome_device_cache[i].adv_len = 0;
        nm_bthome_device_cache[i].scan_rsp_len = 0;
      }
      nm_bthome_device_cache[i].last_tick = now;
      return &nm_bthome_device_cache[i];
    }
    if (nm_bthome_device_cache[i].last_tick < oldest_tick) {
      oldest_tick = nm_bthome_device_cache[i].last_tick;
      oldest_idx = i;
    }
  }
  // 未找到，替换最老的缓存槽
  nm_bthome_raw_cache_t *slot = &nm_bthome_device_cache[oldest_idx];
  memcpy(slot->mac, mac, 6);
  slot->last_tick = now;
  slot->adv_len = 0;
  slot->scan_rsp_len = 0;
  return slot;
}

// 解析 BT Home 广播数据的函数，并通过HTTPS协议发送
void nm_parse_bthome_payload(uint8_t *mac, int32_t rssi, uint8_t *data, uint8_t len) 
{
  const char * res = NULL;
  // 这里可以实现对 BT Home 广播数据的解析逻辑
  // 例如，提取传感器数据、设备状态等，并进行处理或上报
  BTR_LOGD(TAG, "Parsing BT Home payload from MAC: %02X:%02X:%02X:%02X:%02X:%02X, RSSI: %d, Data Length: %d",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], rssi, len);
  // 示例：打印原始数据
  BTR_LOGD_HEX(TAG, data, len);
  // 构建 JSON 数据
  res = bthome_parser_parse(data, len);
  if(res != NULL) {
    BTR_LOGD(TAG, "Parsed JSON: %s", res);
    nm_send_https_post(res);
    bthome_parser_free(res); // 释放解析器返回的字符串
  } else {
    BTR_LOGW(TAG, "Failed to parse BT Home payload");
  }
}

static bool nm_is_bthome_packet(const uint8_t *data, uint8_t len) 
{
  uint8_t i = 0;
  // 每次循环直接利用 length 字段“跳过”不关心的积木块
  while (i < len) {
    uint8_t item_len = data[i];
    if (item_len == 0)
      break; // 后面全是空数据，直接退出

    // 边界安全检查：防止恶意或残缺包导致内存越界
    if (i + item_len >= len)
      break;

    // 核心判断：Type 是否为 0x16，且后续的 16位 UUID 是否为 0xFCD2
    // 小端序中：0xFCD2 在内存里存储顺序为 data[i+2]==0xD2, data[i+3]==0xFC
    if (data[i + 1] == 0x16 && data[i + 2] == 0xD2 && data[i + 3] == 0xFC) {
      return true; // 命中目标，极速返回
    }

    // 直接加 Length+1，瞬间跳到下一个积木的起始位置
    i += item_len + 1;
  }
  return false;
}

// BLE GAP 事件回调
static void nm_ble_gap_cb(esp_gap_ble_cb_event_t event,
                          esp_ble_gap_cb_param_t *param) 
{
  esp_ble_gap_cb_param_t *scan_rst;
  uint8_t *mac, *amt_data, amt_len;
  esp_ble_evt_type_t pkt_type;
  nm_bthome_raw_cache_t *cache;
  uint8_t combined[NM_BTHOME_MAX_RAW_BUF_SIZE]; // 临时数组用于拼接 Adv + Scan Rsp
  uint8_t total_len;

  if (event == ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT) {
    esp_ble_gap_start_scanning(0);
  } else if (event == ESP_GAP_BLE_SCAN_RESULT_EVT) {
    scan_rst = (esp_ble_gap_cb_param_t *)param;
    if (scan_rst->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {

      mac = scan_rst->scan_rst.bda;
      pkt_type = scan_rst->scan_rst.ble_evt_type;
      amt_data = scan_rst->scan_rst.ble_adv;
      amt_len = scan_rst->scan_rst.adv_data_len;

      if(nm_is_bthome_packet(amt_data, amt_len) == false) {
        return; // 不是 BT Home 广播，直接丢弃
      }

      // 获取或创建对应的纯字节缓存槽
      cache = nm_find_or_create_cache(mac);
      cache->last_rssi = scan_rst->scan_rst.rssi;

      // 基于字节流位置进行直接拼接
      if (pkt_type == ESP_BLE_EVT_SCAN_RSP) {
        // 收到的是扫描响应包：存放在缓冲区的后半段
        if (amt_len > 31)
          amt_len = 31; // 安全截断防溢出
        memcpy(&cache->raw_buf[31], amt_data, amt_len);
        cache->scan_rsp_len = amt_len;
      } else {
        // 收到的是普通广告包：存放在前半段
        if (amt_len > 31)
          amt_len = 31;
        memcpy(&cache->raw_buf[0], amt_data, amt_len);
        cache->adv_len = amt_len;
      }

      // 当两部分数据块都集齐时（或者收到响应包时），交付统一解析
      if (cache->scan_rsp_len > 0 && cache->adv_len > 0) {

        // 构建用于线性连续遍历的临时紧凑数组
        memset(combined, 0, sizeof(combined));
        total_len = cache->adv_len + cache->scan_rsp_len;

        // 将前半段(Adv)和后半段(Scan Rsp)无缝首尾相连
        memcpy(combined, &cache->raw_buf[0], cache->adv_len);
        memcpy(&combined[cache->adv_len], &cache->raw_buf[31],
               cache->scan_rsp_len);

        // 交付无状态的全局解析器处理
        nm_parse_bthome_payload(cache->mac, cache->last_rssi, combined, total_len);

        // 处理完毕，清空当前槽位计数，等待下一周期
        cache->adv_len = 0;
        cache->scan_rsp_len = 0;
      }
    }
  }
}

// 初始化 BLE
void nm_start_ble(void) 
{
  // BLE 扫描参数配置
  esp_ble_scan_params_t ble_scan_params = {
      .scan_type = BLE_SCAN_TYPE_ACTIVE,
      .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
      .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
      .scan_interval = 0x50, // 50ms
      .scan_window = 0x30,   // 30ms
      .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE};
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

  BTR_LOGI(TAG, "Starting BLE...");

  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

  ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
  ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

  ESP_ERROR_CHECK(esp_bluedroid_init());
  ESP_ERROR_CHECK(esp_bluedroid_enable());

  ESP_ERROR_CHECK(esp_ble_gap_register_callback(nm_ble_gap_cb));
  ESP_ERROR_CHECK(esp_ble_gap_set_scan_params(&ble_scan_params));
}

void nm_stop_ble(void) 
{
  BTR_LOGD(TAG, "Stopping BLE...");
  ESP_ERROR_CHECK(esp_ble_gap_stop_scanning());
  ESP_ERROR_CHECK(esp_ble_gap_register_callback(NULL));
  if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_ENABLED) {
    ESP_ERROR_CHECK(esp_bluedroid_disable());
  }
  ESP_ERROR_CHECK(esp_bluedroid_deinit());
  if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) {
    ESP_ERROR_CHECK(esp_bt_controller_disable());
  }
  ESP_ERROR_CHECK(esp_bt_controller_deinit());// 彻底释放内存归还给堆
  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
  BTR_LOGD(TAG, "BLE Stopped");
}

void nm_init(void) {
  BTR_LOGI(TAG, "init");
  nm_start_wifi();
  nm_start_ble();
}
