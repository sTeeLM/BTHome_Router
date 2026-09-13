#ifndef BTHOME_ROUTER_BTHOME_PARSER_H
#define BTHOME_ROUTER_BTHOME_PARSER_H

#include <stdint.h>
#include <stddef.h>

void bthome_parser_init(void);

// Parse the BTHome data and return a JSON string. 
// The caller is responsible for freeing the returned string.
const char * bthome_parser_parse(const uint8_t *data, size_t len);
void bthome_parser_free(const char *str);

#endif // BTHOME_ROUTER_BTHOME_PARSER_H