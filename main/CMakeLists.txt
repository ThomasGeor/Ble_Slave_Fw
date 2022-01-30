idf_component_register(SRCS "app_main.c" "gpio_driver.c" "ble_driver.c" "timer_driver.c"
                       REQUIRES "bt" # required to be able to #include esp_bt.h
                       INCLUDE_DIRS ".")
