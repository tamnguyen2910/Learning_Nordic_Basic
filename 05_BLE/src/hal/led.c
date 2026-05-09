#include <stdint.h>
#include <stdbool.h>
#include <hal/nrf_gpio.h>
#include <zephyr/kernel.h>

#define LED_PIN 15

static struct k_work_delayable scan_work;
static bool scan_phase_on;

static void scan_blink_handler(struct k_work *work)
{
    scan_phase_on = !scan_phase_on;
    if (scan_phase_on) {
        nrf_gpio_pin_set(LED_PIN);
        k_work_schedule(&scan_work, K_MSEC(100));
    } else {
        nrf_gpio_pin_clear(LED_PIN);
        k_work_schedule(&scan_work, K_MSEC(900));
    }
}

void led_init(void)
{
    nrf_gpio_pin_dir_set(LED_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_clear(LED_PIN);
    k_work_init_delayable(&scan_work, scan_blink_handler);
}

void led_on(void)
{
    nrf_gpio_pin_set(LED_PIN);
}

void led_off(void)
{
    nrf_gpio_pin_clear(LED_PIN);
}

void led_toggle(void)
{
    nrf_gpio_pin_toggle(LED_PIN);
}

void led_blink_fast(uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        led_on();
        k_msleep(150);
        led_off();
        k_msleep(150);
    }
}

void led_start_scan_blink(void)
{
    scan_phase_on = false;
    k_work_schedule(&scan_work, K_MSEC(100));
}

void led_stop_scan_blink(void)
{
    k_work_cancel_delayable(&scan_work);
    nrf_gpio_pin_clear(LED_PIN);
}