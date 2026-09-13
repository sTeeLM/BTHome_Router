#include "nvs_wrapper.h"
#include "logger.h"
#include "nvs_flash.h"

static const char * TAG = "NVS_WRAPPER";
void nvs_wrapper_init(void)
{
  esp_err_t ret ;
  BTR_LOGI(TAG, "init");
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);  
}

bool nvs_wrapper_read_u8(const char *key, uint8_t *data)
{
  nvs_handle_t nvs_handle;
  bool ret = false;
  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    ret = nvs_get_u8(nvs_handle, key, data) == ESP_OK;
    nvs_close(nvs_handle);
  }
  return ret;
}

bool nvs_wrapper_read_u16(const char *key, uint16_t *data)
{
  nvs_handle_t nvs_handle;
  bool ret = false;
  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    ret = nvs_get_u16(nvs_handle, key, data) == ESP_OK;
    nvs_close(nvs_handle);
  }
  return ret;
}

bool nvs_wrapper_read_u32(const char *key, uint32_t *data)
{
  nvs_handle_t nvs_handle;
  bool ret = false;
  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    ret = nvs_get_u32(nvs_handle, key, data) == ESP_OK;
    nvs_close(nvs_handle);
  }
  return ret;
}

bool nvs_wrapper_read_u64(const char *key, uint64_t *data)
{
  nvs_handle_t nvs_handle;
  bool ret = false;
  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    ret = nvs_get_u64(nvs_handle, key, data) == ESP_OK;
    nvs_close(nvs_handle);
  }
  return ret;
}

bool nvs_wrapper_read_blob(const char *key, uint8_t *data, uint16_t *size)
{
  nvs_handle_t nvs_handle;
  size_t required_size = *size;
  bool ret = false;
  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    ret = nvs_get_blob(nvs_handle, key, data, &required_size) == ESP_OK;
    *size = (uint16_t)required_size;
    nvs_close(nvs_handle);
  }
  return ret;
}

bool nvs_wrapper_write_u8(const char *key, uint8_t data)
{
  nvs_handle_t nvs_handle;
  bool ret = false;
  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    ret = nvs_set_u8(nvs_handle, key, data) == ESP_OK;
    nvs_close(nvs_handle);
  }
  return ret;
}

bool nvs_wrapper_write_u16(const char *key, uint16_t data)
{
  nvs_handle_t nvs_handle;
  bool ret = false;
  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    ret = nvs_set_u16(nvs_handle, key, data) == ESP_OK;
    nvs_close(nvs_handle);
  }
  return ret;
}

bool nvs_wrapper_write_u32(const char *key, uint32_t data)
{
  nvs_handle_t nvs_handle;
  bool ret = false;
  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    ret = nvs_set_u32(nvs_handle, key, data) == ESP_OK;
    nvs_close(nvs_handle);
  }
  return ret;
}

bool nvs_wrapper_write_u64(const char *key, uint64_t data)
{
  nvs_handle_t nvs_handle;
  bool ret = false;
  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    ret = nvs_set_u64(nvs_handle, key, data) == ESP_OK;
    nvs_close(nvs_handle);
  }
  return ret;
}

bool nvs_wrapper_write_blob(const char *key, const uint8_t *data, uint16_t size)
{
  nvs_handle_t nvs_handle;
  bool ret = false;
  if(nvs_open("storage", NVS_READWRITE, &nvs_handle) == ESP_OK) {
    ret = nvs_set_blob(nvs_handle, key, data, (size_t)size) == ESP_OK;
    nvs_close(nvs_handle);
  }
  return ret;
}