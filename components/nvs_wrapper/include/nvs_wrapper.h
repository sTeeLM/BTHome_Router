#ifndef BTHOME_ROUTER_NVS_WRAPPER_H
#define BTHOME_ROUTER_NVS_WRAPPER_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void nvs_wrapper_init(void);
bool nvs_wrapper_read_u8(const char *key, uint8_t *data);
bool nvs_wrapper_read_u16(const char *key, uint16_t *data);
bool nvs_wrapper_read_u32(const char *key, uint32_t *data);
bool nvs_wrapper_read_u64(const char *key, uint64_t *data);
bool nvs_wrapper_read_blob(const char *key, uint8_t *data, uint16_t *size);
bool nvs_wrapper_write_u8(const char *key, uint8_t data);
bool nvs_wrapper_write_u16(const char *key, uint16_t data);
bool nvs_wrapper_write_u32(const char *key, uint32_t data);
bool nvs_wrapper_write_u64(const char *key, uint64_t data);
bool nvs_wrapper_write_blob(const char *key, const uint8_t *data, uint16_t size);

#endif //BTHOME_ROUTER_NVS_WRAPPER_H
