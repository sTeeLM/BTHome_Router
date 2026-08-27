#include "nvs_wrapper.h"
#include "logger.h"

static const char * TAG = "NVS_WRAPPER";

void nvs_wrapper_init(void)
{
  BTR_LOGI(TAG, "init");
}

bool nvs_wrapper_read_u8(const char *key, uint8_t *data)
{
  return false;
}

bool nvs_wrapper_read_u16(const char *key, uint16_t *data)
{
  return false;
}

bool nvs_wrapper_read_u32(const char *key, uint32_t *data)
{
  return false;
}

bool nvs_wrapper_read_u64(const char *key, uint64_t *data)
{
  return false;
}

bool nvs_wrapper_read_blob(const char *key, uint8_t *data, uint16_t *size)
{
  return false;
}

bool nvs_wrapper_write_u8(const char *key, uint8_t data)
{
  return false;
}

bool nvs_wrapper_write_u16(const char *key, uint16_t data)
{
  return false;
}

bool nvs_wrapper_write_u32(const char *key, uint32_t data)
{
  return false;
}

bool nvs_wrapper_write_u64(const char *key, uint64_t data)
{
  return false;
}

bool nvs_wrapper_write_blob(const char *key, const uint8_t *data, uint16_t size)
{
  return false;
}