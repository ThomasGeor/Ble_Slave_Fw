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
#include "ble_driver.h"
#include "timer_driver.h"
#include "gpio_driver.h"

//Seconds
#define ARM_INTERVAL            10
#define DISARM_INTERVAL         20
#define MGNTC_REED_BREACH_ST    0
#define MGNTC_REED_CLOSED_ST    1
#define MG_SNS_PIN              GPIO_NUM_35

// Globals
static const char*    alarm_tag               = "BURGLAR_ALARM";
static uint8_t        mgn_reed_state;
static uint8_t        mgn_reed_last_state;
static uint8_t        timer_interval;


static void timer_int_task(void* arg)
{
    // Timer initialization
    s_timer_queue = xQueueCreate(10, sizeof(example_timer_event_t));
    // should be called in door state change
    custom_timer_init(TIMER_GROUP_0, TIMER_0, true, DISARM_INTERVAL);
    example_timer_event_t evt;
    uint8_t count = 0;
    for(;;)
    {
      if(xQueueReceive(s_timer_queue, &evt, portMAX_DELAY))
      {
          ESP_LOGE(alarm_tag,"Sent packet!");
          ble_write_door_state_char(count++);
          // stop the timer
          timer_pause(TIMER_GROUP_0, TIMER_0);
          vTaskDelete(NULL);
      }
    }
}

static void gpio_int_task(void* arg)
{
    uint32_t io_num;
    for(;;)
    {
      if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
      {
        if(io_num == MG_SNS_PIN)
        {
          mgn_reed_state = gpio_get_level(io_num);
          if(mgn_reed_state != mgn_reed_last_state)
          {
            // Check for a breach only when the alarm is armed.
            if(mgn_reed_state == MGNTC_REED_BREACH_ST)
            {
              timer_interval = DISARM_INTERVAL;
              xTaskCreate(timer_int_task, "timer_int_task", 2048, NULL, 10, NULL);

              ESP_LOGE(alarm_tag,
                       "Door state = %d on alarm arm state.\n",
                       gpio_get_level(MG_SNS_PIN));
            }
          }
          else
          {
            // Double event detected.
          }
          mgn_reed_last_state = mgn_reed_state;
        }
      }
    }
}


static void init(void)
{
  // Set the LOGS that you want to see.
  esp_log_level_set(alarm_tag,ESP_LOG_ERROR);

  /* Initialize the GPIO general config structure. */
  gpio_init();

  // Initialize client and start searching for devices of interest
  ble_client_init();
}

void app_main(void)
{
    // initialize peripherals.
    init();

    //start gpio task
    xTaskCreate(gpio_int_task, "gpio_int_task", 2048, NULL, 10, NULL);
}
