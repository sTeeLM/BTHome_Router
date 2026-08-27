#include "config.h"
#include "nvs_wrapper.h"
#include "logger.h"

#include <string.h>

static const char * TAG = "CONFIG";

#define MAX_BLOB_SIZE 64

// 配置项列表，按照顺序存储在ROM里
static const config_slot_t config_slot[] = {
/*  
  // 是否以12小时制显示时间，0表示24小时制，1表示12小时制
  {"time_12", CONFIG_TYPE_UINT8,   {.val8 = 0}},
  // 是否打开网络对时，0表示关闭，1表示打开
  {"clock_ntp_en", CONFIG_TYPE_UINT8,   {.val8 = 1}},
  // 世纪数，例如19表示19xx年
  {"century", CONFIG_TYPE_UINT8,   {.val8 = 20}},
  // 是否打开运动检测
  {"motion_en",CONFIG_TYPE_UINT8,  {.val8 = 1}},
  // 是否打开按键声音和timer声音
  {"bp_en",CONFIG_TYPE_UINT8,      {.val8 = 1}},
  // 温度显示为摄氏度，还是华氏度？0表示摄氏度，1表示华氏度
  {"temp_unit", CONFIG_TYPE_UINT8,  {.val8 = 0}}, 
  // 气压显示单位，0表示hpa，1表示Hgmm，2表示Atm
  {"press_unit", CONFIG_TYPE_UINT8, {.val8 = 0}},
  // 定时关闭IV18? 0:关闭 1:10s，2:30s，3:1分钟
  {"iv18_ps_sec", CONFIG_TYPE_UINT8,  {.val8 = 3}},
  // IV18亮度，0为自动根据环境光线调整，1～100为对应亮度数值
  {"iv18_brightness", CONFIG_TYPE_UINT8,  {.val8 = 0}},
  // 播放器音量，0~10, 0表示静音，10表示最大声
  {"ply_vol", CONFIG_TYPE_UINT8, {.val8 = 10}},
  // 传感器数据上报间隔，0: 1min, 1: 5min, 2: 10min, 3: 15min, 4: 30min, 5: 45min, 6: 60min 
  {"reporter_interval", CONFIG_TYPE_UINT8, {.val8 = 0}},
  // 传感器pms策略：0: 常开，1: 每30分钟打开10分钟，2:每1小时打开10分钟
  {"reporter_pms_policy", CONFIG_TYPE_UINT8, {.val8 = 0}},
  // timer音效
  {"timer_snd", CONFIG_TYPE_UINT8, {.val8 = 0}},
  // oled白底黑字or黑底白字，0: 黑底白字, 1: 白底黑字
  {"oled_invert", CONFIG_TYPE_UINT8, {.val8 = 0}},
  // oled对比度：0～255
  {"oled_contrast", CONFIG_TYPE_UINT8, {.val8 = 0xFF}}, 
  // wifi ssid
  {"device_id", CONFIG_TYPE_BLOB, {.valblob.len = sizeof(nm_default_device_id), 
    .valblob.body = nm_default_device_id}},
  // wifi ssid
  {"wifi_ssid", CONFIG_TYPE_BLOB, {.valblob.len = sizeof(nm_default_ssid), 
    .valblob.body = nm_default_ssid}},
  // wifi password
  {"wifi_pass", CONFIG_TYPE_BLOB, {.valblob.len = sizeof(nm_default_pass), 
    .valblob.body = nm_default_pass}}, 
  // ntp server
  {"ntp_server", CONFIG_TYPE_BLOB, {.valblob.len = sizeof(nm_default_ntp_server), 
    .valblob.body = nm_default_ntp_server}}, 
  // report server
  {"report_url", CONFIG_TYPE_BLOB, {.valblob.len =  sizeof(nm_default_report_url), 
    .valblob.body = nm_default_report_url}}, 
  {"report_user", CONFIG_TYPE_BLOB, {.valblob.len =  sizeof(nm_default_report_user), 
    .valblob.body = nm_default_report_user}}, 
  {"report_pass", CONFIG_TYPE_BLOB, {.valblob.len =  sizeof(nm_default_report_pass), 
    .valblob.body = nm_default_report_pass}}, 
  // 是否打开BTHome蓝牙广播，0表示关闭，1表示打开
  {"bthome_en", CONFIG_TYPE_UINT8, {.val8 = 1}},
  // 是否打开整点报时？
  {"hourly_chime", CONFIG_TYPE_BLOB, {.valblob.len = sizeof(alarm0_t),
    .valblob.body = (const uint8_t * )&alarm0_blob0}},   
  // 闹钟配置，直接存成blob  
  {"alm00_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm1_t),
    .valblob.body = (const uint8_t * )&alarm1_blob0}}, 
  {"alm01_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm1_t),
    .valblob.body = (const uint8_t * )&alarm1_blob0}},
  {"alm02_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm1_t),
    .valblob.body = (const uint8_t * )&alarm1_blob0}}, 
  {"alm03_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm1_t),
    .valblob.body = (const uint8_t * )&alarm1_blob0}},
  {"alm04_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm1_t),
    .valblob.body = (const uint8_t * )&alarm1_blob0}}, 
  {"alm05_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm1_t),
    .valblob.body = (const uint8_t * )&alarm1_blob0}},
  {"alm06_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm1_t),
    .valblob.body = (const uint8_t * )&alarm1_blob0}}, 
  {"alm07_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm1_t),
    .valblob.body = (const uint8_t * )&alarm1_blob0}},
  {"alm08_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm1_t),
    .valblob.body = (const uint8_t * )&alarm1_blob0}}, 
  {"alm09_cfg", CONFIG_TYPE_BLOB,  {.valblob.len = sizeof(alarm1_t),
    .valblob.body = (const uint8_t * )&alarm1_blob0}},  
*/
};



static void config_factory_reset(void)
{
  uint32_t i;
  for(i = 0 ; i < sizeof(config_slot) / sizeof(config_slot_t) ; i ++) { 
    switch(config_slot[i].type) {
      case CONFIG_TYPE_UINT8:
        nvs_wrapper_write_u8(config_slot[i].name, config_slot[i].default_val.val8);
        break;
      case CONFIG_TYPE_UINT16:
        nvs_wrapper_write_u16(config_slot[i].name, config_slot[i].default_val.val16);
        break;
      case CONFIG_TYPE_UINT32:
        nvs_wrapper_write_u32(config_slot[i].name, config_slot[i].default_val.val32);
        break;
      case CONFIG_TYPE_UINT64:
        nvs_wrapper_write_u64(config_slot[i].name, config_slot[i].default_val.val64);
        break;
      case CONFIG_TYPE_BLOB:
        nvs_wrapper_write_blob(config_slot[i].name, config_slot[i].default_val.valblob.body, config_slot[i].default_val.valblob.len);
        break; 
      default: ;
    }   
  } 
}

static const config_slot_t * find_config(const char * name, uint32_t * offset)
{
  uint32_t i;
  *offset = 0;
  for(i = 0 ; i < sizeof(config_slot) / sizeof(config_slot_t) ; i ++) { 
    if(strcmp(name, config_slot[i].name) == 0) {
      BTR_LOGD(TAG, "find_config %s offset at %ld", name, *offset);
      return &config_slot[i];
    }
    switch(config_slot[i].type) {
      case CONFIG_TYPE_UINT8:
        *offset += 1;
        break;
      case CONFIG_TYPE_UINT16:
        *offset += 2;
        break;
      case CONFIG_TYPE_UINT32:
        *offset += 4;
        break;
      case CONFIG_TYPE_UINT64:
        *offset += 8;
        break;
      case CONFIG_TYPE_BLOB:
        *offset += config_slot[i].default_val.valblob.len + sizeof(config_slot[i].default_val.valblob.len);
        break; 
      default: ;
    }   
  }
  return NULL;
}

static void config_dump(void)
{
  uint32_t i, j, offset = 0;
  config_val_t val;
  uint8_t buffer[MAX_BLOB_SIZE];
  BTR_LOGI(TAG, "dump all config begin ----------------------");
  for(i = 0 ; i < sizeof(config_slot) / sizeof(config_slot_t) ; i ++) { 
    switch(config_slot[i].type) {
      case CONFIG_TYPE_UINT8:
        nvs_wrapper_read_u8(config_slot[i].name, &val.val8);
        break;
      case CONFIG_TYPE_UINT16:
        nvs_wrapper_read_u16(config_slot[i].name, &val.val16);
        break;
      case CONFIG_TYPE_UINT32:
        nvs_wrapper_read_u32(config_slot[i].name, &val.val32);
        break;
      case CONFIG_TYPE_UINT64:
        nvs_wrapper_read_u64(config_slot[i].name, &val.val64);
        break;
      case CONFIG_TYPE_BLOB:
        val.valblob.len = sizeof(buffer);
        val.valblob.body = buffer;
        nvs_wrapper_read_blob(config_slot[i].name, (uint8_t *)(val.valblob.body), &val.valblob.len);
        break;      
      default: ;
    }
    if(config_slot[i].type != CONFIG_TYPE_BLOB) {
      switch(config_slot[i].type) {
        case CONFIG_TYPE_UINT8:
          BTR_LOGI(TAG, "[0x%08lx][08] %s : %u", offset, config_slot[i].name, val.val8);
          break;
        case CONFIG_TYPE_UINT16:
          BTR_LOGI(TAG, "[0x%08lx][16] %s : %u", offset, config_slot[i].name, val.val16);
          break;
        case CONFIG_TYPE_UINT32:
          BTR_LOGI(TAG, "[0x%08lx][32] %s : %u", offset, config_slot[i].name, val.val32);
          break;
        case CONFIG_TYPE_UINT64:
          BTR_LOGI(TAG, "[0x%08lx][64] %s : %llu", offset, config_slot[i].name, val.val64);
          break;
        default: ;
      }
    } else {
      BTR_LOGI(TAG, "[0x%08lx][blob] %s (blob len = %d):", offset, config_slot[i].name, val.valblob.len);
      for(j = 0 ; j < val.valblob.len / 8 ; j ++) {
        BTR_LOGI_HEX(TAG, buffer + j * 8, 8);
      }
      if(val.valblob.len % 8) {
        BTR_LOGI_HEX(TAG, buffer + (val.valblob.len / 8) * 8, val.valblob.len % 8);
      }
    }
  } 
  BTR_LOGI(TAG, "dump all config end, total %d bytes ----------------------", offset);
}

void config_init(void)
{
  BTR_LOGI(TAG, "init"); 
  config_dump();
}

bool config_read(const char * name, config_val_t * val)
{
  bool ret = false;
  uint32_t off = 0;
  const config_slot_t * p = find_config(name, &off);
  if(p) {
    ret = true;
    switch (p->type) {
      case CONFIG_TYPE_UINT8:
        nvs_wrapper_read_u8(name, &val->val8);
        break;
      case CONFIG_TYPE_UINT16:
        nvs_wrapper_read_u16(name, &val->val16);
        break;
      case CONFIG_TYPE_UINT32:
        nvs_wrapper_read_u32(name, &val->val32);
        break;
      case CONFIG_TYPE_UINT64:
        nvs_wrapper_read_u64(name, &val->val64);
        break;
      case CONFIG_TYPE_BLOB:
        nvs_wrapper_read_blob(name, (uint8_t *)(val->valblob.body), &val->valblob.len);
      default: ;
    }
  }
  return ret;
}

uint64_t config_read_int(const char * name)
{
  uint32_t off = 0;
  uint64_t ret = 0;
  config_val_t val;
  const config_slot_t * p = find_config(name, &off);
  if(p) {
    switch (p->type) {
      case CONFIG_TYPE_UINT8:
        nvs_wrapper_read_u8(name, &val.val8);
        ret = val.val8;
        break;
      case CONFIG_TYPE_UINT16:
        nvs_wrapper_read_u16(name, &val.val16);
        ret = val.val16;
        break;
      case CONFIG_TYPE_UINT32:
        nvs_wrapper_read_u32(name, &val.val32);
        ret = val.val32;
        break;
      case CONFIG_TYPE_UINT64:
        nvs_wrapper_read_u64(name, &val.val64);
        ret = val.val64;
        break;
      default: ;
    }             
  } else {
    BTR_LOGE(TAG, "config_read: %s not exist!\n", name);
  }
  return ret;
}

void config_write_int(const char * name, uint64_t val)
{
  config_val_t val1;
  uint32_t off = 0;
  const config_slot_t * p = find_config(name, &off);

  if(p) {
    switch (p->type) {
      case CONFIG_TYPE_UINT8:
        val1.val8 = (uint8_t) val;
        nvs_wrapper_write_u8(name, val1.val8);
        break;
      case CONFIG_TYPE_UINT16:
        val1.val16 = (uint16_t) val;
        nvs_wrapper_write_u16(name, val1.val16);
        break;
      case CONFIG_TYPE_UINT32:
        val1.val32 = (uint32_t) val;
        nvs_wrapper_write_u32(name, val1.val32);
        break;
      case CONFIG_TYPE_UINT64:
        val1.val64 = val;
        nvs_wrapper_write_u64(name, val1.val64);
        break;
      default: ;
    }
  } else {
    BTR_LOGE(TAG, "config_write_int: %s not exist!\n", name);
  }
}

void config_write(const char * name, const config_val_t * val)
{
  uint32_t off = 0;
  const config_slot_t * p = find_config(name, &off);
  if(p) {
    switch (p->type) {
      case CONFIG_TYPE_UINT8:
        nvs_wrapper_write_u8(name, val->val8);
        break;
      case CONFIG_TYPE_UINT16:
        nvs_wrapper_write_u16(name, val->val16);
        break;
      case CONFIG_TYPE_UINT32:
        nvs_wrapper_write_u32(name, val->val32);
        break;
      case CONFIG_TYPE_UINT64:
        nvs_wrapper_write_u64(name, val->val64);
        break;
      case CONFIG_TYPE_BLOB:
        nvs_wrapper_write_blob(name, (uint8_t *)(val->valblob.body), val->valblob.len);
      default: ;
    }
  } else {
    BTR_LOGE(TAG, "config_write: %s not exist!\n", name);
  }
}
