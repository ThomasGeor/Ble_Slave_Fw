/* Timer Driver for ESP32 burglar alarm
 *
 *  author : Thomas Georgiadis
 *
 */
 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/timer.h"

// Defintions
#define TIMER_DIVIDER           (16)  //  Hardware timer clock divider
#define TIMER_SCALE             (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds

typedef struct {
    int timer_group;
    int timer_idx;
    int alarm_interval;
    bool auto_reload;
} example_timer_info_t;

/**
 * @brief A sample structure to pass events from the timer ISR to task
 *
 */
typedef struct {
    example_timer_info_t info;
    uint64_t timer_counter_value;
} example_timer_event_t;

xQueueHandle   s_timer_queue;

// Interrupt Callbacks Definitions
void custom_timer_init(int group,
                       int timer,
                       bool auto_reload,
                       int timer_interval_sec);

bool IRAM_ATTR timer_group_isr_callback(void *args);
