#include "bthome_parser.h"
#include "cjson_wrapper.h"
#include "logger.h"

static const char *TAG = "BTHOME_PARSER";

void bthome_parser_init(void)
{
  BTR_LOGI(TAG, "init");
}

// Parse the BTHome data and return a JSON string. 
// The caller is responsible for freeing the returned string.
char * bthome_parser_parse(const uint8_t *data, size_t len)
{
  BTR_LOGD(TAG, "parse");
  return NULL;
}

