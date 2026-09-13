#ifndef BTHOME_ROUTER_BLM_H
#define BTHOME_ROUTER_BLM_H

#include <stdbool.h>

void blm_init(void);
bool blm_add_device(const char *device_id);
void blm_remove_device(const char *device_id);
void blm_clear(void);
bool blm_is_device_blacklisted(const char *device_id);

#endif // BTHOME_ROUTER_BLM_H