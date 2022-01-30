/* BLE Driver for ESP32 burglar alarm master-server
 *
 *  author : Thomas Georgiadis
 *
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "freertos/event_groups.h"
 #include "esp_system.h"
 #include "esp_log.h"
 #include "nvs_flash.h"

 #include "esp_bt.h"
 #include "esp_gap_ble_api.h"
 #include "esp_gattc_api.h"
 #include "esp_bt_defs.h"
 #include "esp_bt_main.h"
 #include "esp_gatt_common_api.h"
 #include "gpio_driver.h"

 #include "sdkconfig.h"

 /* Defintions */
 #define GATTC_TAG                 "GATT_CLIENT"
 #define REMOTE_SERVICE_UUID        0x0012
 #define REMOTE_NOTIFY_CHAR_UUID    0x0001
 #define PROFILE_NUM                1
 #define PROFILE_A_APP_ID           0
 #define INVALID_HANDLE             0


/* Struct declarations */
struct gattc_profile_inst
{
  esp_gattc_cb_t  gattc_cb;
  uint16_t        gattc_if;
  uint16_t        app_id;
  uint16_t        conn_id;
  uint16_t        service_start_handle;
  uint16_t        service_end_handle;
  uint16_t        char_handle;
  esp_bd_addr_t   remote_bda;
};

/* Function declarations */
void ble_client_init();
void ble_write_door_state_char(uint8_t dr_st);
