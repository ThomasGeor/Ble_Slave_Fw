/* GPIO Driver for ESP32 burglar alarm
 *
 *  author : Thomas Georgiadis
 *
*/

#include "gpio_driver.h"

void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void gpio_init()
{
  /* Initialize the GPIO general config structure. */
  gpio_config_t io_conf  = {};

  // Register the magnetic sensor's gpio.
  io_conf.intr_type      = GPIO_INTR_NEGEDGE ;
  io_conf.pull_down_en   = 0;
  io_conf.mode           = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask   = (1ULL<<MG_SNS_PIN);
  gpio_config(&io_conf);

  // Change gpio interrupt type for MG_SNS_PIN.
  gpio_set_intr_type(MG_SNS_PIN, GPIO_INTR_POSEDGE);

  //create a queue to handle gpio event from isr
  gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

  //install gpio isr service
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

  //hook isr handler for specific gpio pin
  gpio_isr_handler_add(MG_SNS_PIN, gpio_isr_handler, (void*) MG_SNS_PIN);

}
