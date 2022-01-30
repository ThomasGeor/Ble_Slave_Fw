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
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "ble_driver.h"
#include "timer_driver.h"

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

static void gpio_int_task(void* arg)
{
    for(;;)
    {
      /* Enter sleep mode */
      esp_sleep_enable_gpio_wakeup();
      esp_light_sleep_start();

      /* Execution continues here after wakeup */
      ESP_LOGE(alarm_tag,"Woke up!");
      mgn_reed_state = gpio_get_level(MG_SNS_PIN);
      if(mgn_reed_state != mgn_reed_last_state)
      {
          // Initialize client and start searching for devices of interest
          ble_client_init();
      }
      else
      {
        // Double event detected.
      }
      mgn_reed_last_state = mgn_reed_state;
    }
}

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
          ble_write_door_state_char(count++);
      }
    }
}

static void init(void)
{
  // Set the LOGS that you want to see.
  esp_log_level_set(alarm_tag,ESP_LOG_ERROR);

  // Initialize all gpios
  const int wakeup_level = 0;
  /* Initialize the GPIO general config structure. */
  gpio_config_t io_conf  = {};
  // Register the magnetic sensor's gpio.
  io_conf.intr_type      = GPIO_INTR_NEGEDGE ;
  io_conf.pull_down_en   = 0;
  io_conf.mode           = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask   = (1ULL<<MG_SNS_PIN);
  gpio_config(&io_conf);

  gpio_wakeup_enable(MG_SNS_PIN,
                     wakeup_level == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
}

void app_main(void)
{
    // initialize peripherals.
    // init();
    // Initialize client and start searching for devices of interest
    ble_client_init();
    xTaskCreate(timer_int_task, "timer_int_task", 2048, NULL, 10, NULL);

    //start gpio task
    // xTaskCreate(gpio_int_task, "gpio_int_task", 2048, NULL, 10, NULL);
}
