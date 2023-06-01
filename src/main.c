#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_drv_common.h"
#include "nrf_drv_clock.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_gpio.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"

#define LED_PIN1 17
#define LED_PIN2 18

volatile TimerHandle_t xOneSec_Timer;

void blink_task(void *p){
    
    while(1){
        NRF_LOG_INFO("Blink");
        nrf_gpio_pin_toggle(LED_PIN2);
        vTaskDelay(500);
    }
};

void vOneSecTimer( TimerHandle_t xTimer ){
	configASSERT( xTimer );
    nrf_gpio_pin_toggle(LED_PIN1);
    NRF_LOG_INFO("Timer");
}


static void clock_init(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
    while (!nrf_drv_clock_lfclk_is_running()){__NOP();}

    nrf_drv_clock_hfclk_request(NULL);
    while (!nrf_drv_clock_hfclk_is_running()){__NOP();}
}

int main(void)
{
    ret_code_t err_code;

    // Initialize.
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    clock_init();

    nrf_gpio_cfg_output(LED_PIN1);
    nrf_gpio_cfg_output(LED_PIN2);

    NRF_LOG_INFO("Start");

    xOneSec_Timer = xTimerCreate( "1STimer", 5000 , pdTRUE, NULL, vOneSecTimer);
    xTimerStart(xOneSec_Timer,0);

    xTaskCreate(blink_task, "Blink", 256, NULL, 2, NULL);
    vTaskStartScheduler();

    while(1);
}

