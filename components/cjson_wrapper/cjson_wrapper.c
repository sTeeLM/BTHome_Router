#include "cjson_wrapper.h"
#include "cJSON.h"
#include "esp_heap_caps.h"
#include "logger.h"

static const char *TAG = "CJSON";

#define STATIC_POOL_SIZE  (4096)  // 根据需要定义静态内存池大小（字节）
static uint8_t s_json_pool[STATIC_POOL_SIZE];
static size_t s_pool_idx = 0;

// 1. 自定义静态的内存分配函数
void *cjson_wrapper_malloc_static(size_t size) 
{
    // 4字节对齐优化
    size_t aligned_size = (size + 3) & ~3;

    if (s_pool_idx + aligned_size > STATIC_POOL_SIZE) {
        printf("[cJSON Memory Error] need %d bytes\n", (int)aligned_size);
        return NULL; // 内存不足
    }

    void* ptr = &s_json_pool[s_pool_idx];
    s_pool_idx += aligned_size;
    return ptr;
}

void cjson_wrapper_free_static(void *ptr)
{ 
  // 静态内存池不支持释放，这里可以留空或添加日志
}

void cjson_reset_memory_pool(void) 
{
    s_pool_idx = 0;
    // 可选：memset(s_json_pool, 0, STATIC_POOL_SIZE); 
}

void cjson_wrapper_init(void) {
  cJSON_Hooks hooks = {};

  BTR_LOGI(TAG, "init");
  // 3. 组装 cJSON 钩子
  hooks.malloc_fn = cjson_wrapper_malloc_static;
  hooks.free_fn = cjson_wrapper_free_static;

  // 4. 全局注副钩子（整机生效）
  cJSON_InitHooks(&hooks);

  BTR_LOGI(TAG, "internal heap size: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
  BTR_LOGI(TAG, "     spi heap size: %d bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));  
}
