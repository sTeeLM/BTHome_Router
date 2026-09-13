#include "bthome_parser.h"
#include "cjson_wrapper.h"
#include "logger.h"

static const char *TAG = "BTHOME_PARSER";

void bthome_parser_init(void)
{
  BTR_LOGI(TAG, "init");
}
#include <stdint.h>
#include <stddef.h>

// 数据类型枚举
typedef enum _bthome_data_type_t {
    BTHOME_DATA_TYPE_UINT8 = 0,
    BTHOME_DATA_TYPE_INT8 = 1,
    BTHOME_DATA_TYPE_UINT16 = 2,
    BTHOME_DATA_TYPE_INT16 = 3,
    BTHOME_DATA_TYPE_UINT32 = 4,
    BTHOME_DATA_TYPE_INT32 = 5,
    BTHOME_DATA_TYPE_FLOAT = 6,
    BTHOME_DATA_TYPE_BOOL = 7,
    BTHOME_DATA_TYPE_UINT24 = 8,  // 新增: 3字节无符号 (Little Endian)
    BTHOME_DATA_TYPE_INT24 = 9,   // 新增: 3字节有符号 (Little Endian)
    BTHOME_DATA_TYPE_VAR   = 10   // 不定长数据
} bthome_data_type_t;

// 查找表结构体定义
typedef struct _bthome_lookup_table_t {
    uint8_t object_id;
    const char *property;
    bthome_data_type_t data_type;
    uint8_t factor; // 0: 不缩放, 1: 0.1, 2: 0.01, 3: 0.001, 6: 0.000001
} bthome_lookup_table_t;

// 合并并按 object_id 升序排列的查找表
// 1. 0x58 官方协议中该类型的因子是 0.35
// 2. 0x53/0x54 (Text/Raw): 这两个是变长数据。
static const bthome_lookup_table_t bthome_lookup_table[] = {
    // --- Object ID: 0x01 - 0x0F ---
    {0x01, "battery",           BTHOME_DATA_TYPE_UINT8,  0}, // Unit: %
    {0x02, "temperature_0",     BTHOME_DATA_TYPE_INT16,  2}, // Unit: °C
    {0x03, "humidity_0",        BTHOME_DATA_TYPE_UINT16, 2}, // Unit: %
    {0x04, "pressure",          BTHOME_DATA_TYPE_UINT24, 2}, // Unit: hPa
    {0x05, "illuminance",       BTHOME_DATA_TYPE_UINT24, 2}, // Unit: lx
    {0x06, "mass_kg",           BTHOME_DATA_TYPE_UINT16, 2}, // Unit: kg
    {0x07, "mass_lb",           BTHOME_DATA_TYPE_UINT16, 2}, // Unit: lb
    {0x08, "dewpoint",          BTHOME_DATA_TYPE_INT16,  2}, // Unit: °C
    {0x09, "count_0",           BTHOME_DATA_TYPE_UINT8,  0}, // Unit: -
    {0x0A, "energy_0",          BTHOME_DATA_TYPE_UINT24, 3}, // Unit: kWh
    {0x0B, "power_0",           BTHOME_DATA_TYPE_UINT24, 2}, // Unit: W
    {0x0C, "voltage_0",         BTHOME_DATA_TYPE_UINT16, 3}, // Unit: V
    {0x0D, "pm2.5",             BTHOME_DATA_TYPE_UINT16, 0}, // Unit: ug/m3
    {0x0E, "pm10",              BTHOME_DATA_TYPE_UINT16, 0}, // Unit: ug/m3
    {0x0F, "generic_boolean",   BTHOME_DATA_TYPE_BOOL,  0}, // Boolean

    // --- Object ID: 0x10 - 0x1F ---
    {0x10, "power_1",           BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x11, "opening",           BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x12, "co2",               BTHOME_DATA_TYPE_UINT16, 0}, // Unit: ppm
    {0x13, "tvoc",              BTHOME_DATA_TYPE_UINT16, 0}, // Unit: ug/m3
    {0x14, "moisture_0",        BTHOME_DATA_TYPE_UINT16, 2}, // Unit: %
    {0x15, "battery_1",         BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x16, "battery_charging",  BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x17, "carbon_monoxide",   BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x18, "cold",              BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x19, "connectivity",      BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x1A, "door",              BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x1B, "garage_door",       BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x1C, "gas_0",             BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x1D, "heat",              BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x1E, "light_0",           BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x1F, "lock",              BTHOME_DATA_TYPE_BOOL,  0}, // Boolean

    // --- Object ID: 0x20 - 0x2F ---
    {0x20, "moisture_1",        BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x21, "motion",            BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x22, "moving",            BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x23, "occupancy",         BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x24, "plug",              BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x25, "presence",          BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x26, "problem",           BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x27, "running",           BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x28, "safety",            BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x29, "smoke",             BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x2A, "sound",             BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x2B, "tamper",            BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x2C, "vibration",         BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x2D, "window",            BTHOME_DATA_TYPE_BOOL,  0}, // Boolean
    {0x2E, "humidity_1",        BTHOME_DATA_TYPE_UINT8,  0}, // Unit: %
    {0x2F, "moisture_2",        BTHOME_DATA_TYPE_UINT8,  0}, // Unit: %

    // --- Object ID: 0x3D - 0x4F ---
    {0x3D, "count_1",           BTHOME_DATA_TYPE_UINT16, 0}, // Unit: -
    {0x3E, "count_2",           BTHOME_DATA_TYPE_UINT32, 0}, // Unit: -
    {0x3F, "rotation",          BTHOME_DATA_TYPE_INT16,  1}, // Unit: °
    {0x40, "distance_mm",       BTHOME_DATA_TYPE_UINT16, 0}, // Unit: mm
    {0x41, "distance_m",        BTHOME_DATA_TYPE_UINT16, 1}, // Unit: m
    {0x42, "duration",          BTHOME_DATA_TYPE_UINT24, 3}, // Unit: s
    {0x43, "current_0",         BTHOME_DATA_TYPE_UINT16, 3}, // Unit: A
    {0x44, "speed_0",           BTHOME_DATA_TYPE_UINT16, 2}, // Unit: m/s
    {0x45, "temperature_1",     BTHOME_DATA_TYPE_INT16,  1}, // Unit: °C
    {0x46, "uv_index",          BTHOME_DATA_TYPE_UINT8,  1}, // Unit: -
    {0x47, "volume_0",          BTHOME_DATA_TYPE_UINT16, 1}, // Unit: L
    {0x48, "volume_1",          BTHOME_DATA_TYPE_UINT16, 0}, // Unit: mL
    {0x49, "volume_flow_rate",  BTHOME_DATA_TYPE_UINT16, 3}, // Unit: m3/hr
    {0x4A, "voltage_1",         BTHOME_DATA_TYPE_UINT16, 1}, // Unit: V
    {0x4B, "gas_1",             BTHOME_DATA_TYPE_UINT24, 3}, // Unit: m3
    {0x4C, "gas_2",             BTHOME_DATA_TYPE_UINT32, 3}, // Unit: m3
    {0x4D, "energy_1",          BTHOME_DATA_TYPE_UINT32, 3}, // Unit: kWh
    {0x4E, "volume_2",          BTHOME_DATA_TYPE_UINT32, 3}, // Unit: L
    {0x4F, "water",             BTHOME_DATA_TYPE_UINT32, 3}, // Unit: L

    // --- Object ID: 0x50 - 0x5F ---
    {0x50, "timestamp",         BTHOME_DATA_TYPE_UINT32, 0}, // Unit: s (Unix timestamp)
    {0x51, "acceleration_0",    BTHOME_DATA_TYPE_UINT16, 3}, // Unit: m/s²
    {0x52, "gyroscope",         BTHOME_DATA_TYPE_UINT16, 3}, // Unit: °/s
    {0x53, "text",              BTHOME_DATA_TYPE_VAR,    0}, // Unit: String (Variable length)
    {0x54, "raw",               BTHOME_DATA_TYPE_VAR,    0}, // Unit: Bytes (Variable length)
    {0x55, "volume_storage",    BTHOME_DATA_TYPE_UINT32, 3}, // Unit: L
    {0x56, "conductivity",      BTHOME_DATA_TYPE_UINT16, 0}, // Unit: µS/cm
    {0x57, "temperature_2",     BTHOME_DATA_TYPE_INT8,   0}, // Unit: °C
    {0x58, "temperature_3",     BTHOME_DATA_TYPE_INT8,   1}, // Unit: °C (Special: factor 0.35, approximated as 0.1 here)
    {0x59, "count_3",           BTHOME_DATA_TYPE_INT8,   0}, // Unit: -
    {0x5A, "count_4",           BTHOME_DATA_TYPE_INT16,  0}, // Unit: -
    {0x5B, "count_5",           BTHOME_DATA_TYPE_INT32,  0}, // Unit: -
    {0x5C, "power_2",           BTHOME_DATA_TYPE_INT32,  2}, // Unit: W
    {0x5D, "current_1",         BTHOME_DATA_TYPE_INT16,  3}, // Unit: A
    {0x5E, "direction",         BTHOME_DATA_TYPE_UINT16, 2}, // Unit: °
    {0x5F, "precipitation",     BTHOME_DATA_TYPE_UINT16, 1}, // Unit: mm

    // --- Object ID: 0x60 - 0x65 ---
    {0x60, "channel",           BTHOME_DATA_TYPE_UINT8,  0}, // Unit: -
    {0x61, "rotational_speed",  BTHOME_DATA_TYPE_UINT16, 0}, // Unit: rpm
    {0x62, "speed_signed",      BTHOME_DATA_TYPE_INT32,  6}, // Unit: m/s
    {0x63, "acceleration_1",    BTHOME_DATA_TYPE_INT32,  6}, // Unit: m/s²
    {0x64, "light_level",       BTHOME_DATA_TYPE_UINT8,  0}, // Unit: -
    {0x65, "settings_revision", BTHOME_DATA_TYPE_UINT8,  0}, // Unit: -
};

// 辅助宏：获取表的大小
#define BTHOME_LOOKUP_TABLE_SIZE (sizeof(bthome_lookup_table) / sizeof(bthome_lookup_table[0]))


// 辅助宏：获取表的大小
#define BTHOME_LOOKUP_TABLE_SIZE (sizeof(bthome_lookup_table) / sizeof(bthome_lookup_table[0]))

// Parse the BTHome data and return a JSON string. 
// The caller is responsible for freeing the returned string.
const char * bthome_parser_parse(const uint8_t *data, size_t len)
{
  const bthome_lookup_table_t * p = bthome_lookup_table;
  BTR_LOGD(TAG, "parse");
  return NULL;
}

void bthome_parser_free(const char *str)
{
  BTR_LOGD(TAG, "free");
}

