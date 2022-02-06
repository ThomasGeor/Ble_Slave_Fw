idf_component_register(SRCS "timer_driver.c" "app_main.c" "ble_driver.c"
                       REQUIRES "bt" # required to be able to #include esp_bt.h
                       INCLUDE_DIRS ".")
