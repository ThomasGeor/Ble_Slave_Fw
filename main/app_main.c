/*  Burglar alarm system slave - BLE client !
 *
 *  author : Thomas Georgiadis
 *
 * dat
 *
 *  @brief  This is the main for a slave in a bluetooth network
 *          of 3 slaves and 1 master. Wifi is used in order to
 *          inform the owner when a breach has been detected.
 *          Bluetooth is used so that the slaves can inform the master
 *          that a breach has been detected.
 *
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "ble_driver.h"
#include "timer_driver.h"
#include "esp_system.h"

//Seconds
#define MGNTC_REED_BREACH_ST    0
#define MGNTC_REED_CLOSED_ST    1
#define MG_SNS_PIN              GPIO_NUM_35
#define BLE_SETUP_INTERVAL      2 // Seconds 

// Globals
static const char* alarm_tag = "BURGLAR_ALARM";
static uint8_t reed_state;
const gpio_config_t io_conf  = {
                                 .pull_up_en = 1,
                                 .mode = GPIO_MODE_INPUT,
                                 .pin_bit_mask   = (1ULL<<MG_SNS_PIN)
                               };

static void timer_int_task(void* arg)
{
    // Timer initialization
    s_timer_queue = xQueueCreate(10, sizeof(example_timer_event_t));
    // should be called in door state change
    custom_timer_init(TIMER_GROUP_0, TIMER_0, true, BLE_SETUP_INTERVAL);
    example_timer_event_t evt;
    for(;;)
    {
      if(xQueueReceive(s_timer_queue, &evt, portMAX_DELAY))
      {
          ESP_LOGI(alarm_tag,"Sent packet %d!",
                              reed_state == 0? MGNTC_REED_CLOSED_ST : MGNTC_REED_BREACH_ST);
          ble_write_door_state_char(reed_state == 0? MGNTC_REED_CLOSED_ST : MGNTC_REED_BREACH_ST);
          timer_pause(TIMER_GROUP_0, TIMER_0);
          vTaskDelete(NULL);
      }
    }
}

void app_main(void)
{
    // Set the LOGS that you want to see.
    esp_log_level_set(alarm_tag,ESP_LOG_INFO);

    // Woke up
    if(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0)
    {
      ESP_LOGI(alarm_tag,"Wake up from GPIO %d", MG_SNS_PIN);
      // Initialize client and start searching for devices of interest
      ble_client_init();
      // Start the timer and report to the master after 2 seconds.
      xTaskCreate(timer_int_task, "timer_int_task", 2048, NULL, 10, NULL);
      // Wait for ble to send the packet
      vTaskDelay((BLE_SETUP_INTERVAL + 1)*1000 / portTICK_PERIOD_MS);
    }

    // Configure gpio to read the state
    gpio_config(&io_conf);

    // Read the state of the magnetic reed sensor 
    reed_state = gpio_get_level(MG_SNS_PIN);
    ESP_LOGI(alarm_tag,"Start state %d", reed_state);

    // Setup the controller's sleeping
    rtc_gpio_pulldown_dis(MG_SNS_PIN);
    rtc_gpio_pullup_en(MG_SNS_PIN);
    rtc_gpio_init(MG_SNS_PIN);
    if(reed_state == MGNTC_REED_BREACH_ST)
    {
      reed_state = MGNTC_REED_CLOSED_ST;
    }
    else
    {
      reed_state = MGNTC_REED_BREACH_ST;      
    }
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(MG_SNS_PIN, reed_state));
    ESP_LOGI(alarm_tag,"Enabling GPIO wakeup on pin GPIO%d\n", MG_SNS_PIN);

    // To minimize current consumption in deep sleep
    rtc_gpio_isolate(GPIO_NUM_12);

    // Go to sleep
    esp_deep_sleep_start();
}
