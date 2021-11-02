
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_INT_CORE 1

static void IRAM_ATTR gpio_handler(void* arg){
    gpio_num_t pin_num = (gpio_num_t)(int)arg;
    ets_printf("gpio_handler; core: %d, pin: %d\n", xPortGetCoreID(), pin_num);
}

void gpio_initialize(void * arg){
    static int isr_initialized = 0;

    gpio_num_t pin_num = (gpio_num_t)(int)arg;

    gpio_config_t gpio_conf = {
        .pin_bit_mask=(1ULL << pin_num),
        .mode=GPIO_MODE_INPUT,
        .pull_up_en=GPIO_PULLUP_ENABLE,
        .pull_down_en=GPIO_PULLDOWN_DISABLE,
        .intr_type=GPIO_INTR_ANYEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_conf));

    if (!isr_initialized)
    {
        isr_initialized = 1;
        ESP_ERROR_CHECK(gpio_install_isr_service(0)); // only once
    }

    ESP_ERROR_CHECK(gpio_set_intr_type(pin_num, GPIO_INTR_ANYEDGE));
    ESP_ERROR_CHECK(gpio_isr_handler_add(pin_num, gpio_handler, (void*)pin_num));

    ets_printf("gpio_initialize; core: %d, pin: %d\n", xPortGetCoreID(), pin_num);

    for (;;)
        vTaskDelay(1000);
}

void app_main()
{
    vTaskDelay(100/portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(gpio_initialize, "gpio_initialize_19", 2048, (void *)(int)GPIO_NUM_19, 1, NULL, GPIO_INT_CORE);

    vTaskDelay(100/portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(gpio_initialize, "gpio_initialize_34", 2048, (void *)(int)GPIO_NUM_34, 1, NULL, GPIO_INT_CORE);

    for (;;)
        vTaskDelay(1000);
}
