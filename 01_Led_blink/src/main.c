/*
 * Copyright (c) 2016 Intel Corporation
 * SPDX-License-Identifier: Apache-2.0
 *
 * Project : Multi-function GPIO Demo
 * Board   : Pro Micro nRF52840 (nRF52840 SoC)
 *
 * Description:
 *   - LED0 blink thread: Toggles on-board red LED (P0.15) every 1000ms
 *   - GPIO toggle thread: Toggles all GPIO pins every 500ms
 *   - Uses Zephyr RTOS multi-threading with k_thread
 *
 * Pin mapping (Pro Micro nRF52840):
 *   LED0 = P0.15 (on-board red LED)
 *   Port 0: P0.02, P0.06, P0.08, P0.09, P0.10, P0.11, P0.12, P0.17, P0.20, P0.22, P0.24, P0.29, P0.31
 *   Port 1: P1.00, P1.04, P1.06, P1.11, P1.13, P1.15
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* ================================================================== */
/* Configuration Defines                                               */
/* ================================================================== */
#define LED_PERIOD_MS          1000    /* LED0 toggle period (1 second) */
#define GPIO_TOGGLE_PERIOD_MS  500     /* GPIO pins toggle period (500ms) */
#define STACK_SIZE             512     /* Thread stack size */

/* ================================================================== */
/* GPIO Pin Definitions (Pro Micro nRF52840)                          */
/* ================================================================== */
/* Port 0 pins */
#define PIN_02    2
#define PIN_06    6
#define PIN_08    8
#define PIN_09    9   /* UART0 disabled - now available as GPIO */
#define PIN_10   10   /* UART0 disabled - now available as GPIO */
#define PIN_11   11
#define PIN_12   12
#define PIN_17   17
#define PIN_20   20
#define PIN_22   22
#define PIN_24   24
#define PIN_29   29
#define PIN_31   31

/* Port 1 pins */
#define PIN1_00   0
#define PIN1_04   4
#define PIN1_06   6
#define PIN1_11  11
#define PIN1_13  13
#define PIN1_15  15

/* ================================================================== */
/* LED0 Configuration (Device Tree)                                   */
/* ================================================================== */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* GPIO device handles */
static const struct device *gpio0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
static const struct device *gpio1 = DEVICE_DT_GET(DT_NODELABEL(gpio1));

/* ================================================================== */
/* Thread Stacks and Structures                                        */
/* ================================================================== */
K_THREAD_STACK_DEFINE(led_blink_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(gpio_toggle_stack, STACK_SIZE);

static struct k_thread led_blink_thread_data;
static struct k_thread gpio_toggle_thread_data;

/* State tracking */
static bool led_state = true;
static bool gpio_state = false;

/* ================================================================== */
/* Port 0 Pins Array (excluding LED0 pin P0.15)                       */
/* ================================================================== */
static const uint8_t port0_pins[] = {
    PIN_02, PIN_06, PIN_08, PIN_09, PIN_10, PIN_11, PIN_12,
    PIN_17, PIN_20, PIN_22, PIN_24, PIN_29, PIN_31
};
#define PORT0_PIN_COUNT (sizeof(port0_pins) / sizeof(port0_pins[0]))

/* Port 1 Pins Array */
static const uint8_t port1_pins[] = {
    PIN1_00, PIN1_04, PIN1_06, PIN1_11, PIN1_13, PIN1_15
};
#define PORT1_PIN_COUNT (sizeof(port1_pins) / sizeof(port1_pins[0]))

/* ================================================================== */
/* LED Blink Thread - Toggle LED0 every LED_PERIOD_MS                 */
/* ================================================================== */
static void led_blink_thread(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    printf("[LED Thread] Started - Period: %d ms\n", LED_PERIOD_MS);

    while (1) {
        /* Print current state */
        printf("[LED] %s\n", led_state ? "ON" : "OFF");

        /* Toggle LED state */
        led_state = !led_state;
        gpio_pin_set_dt(&led0, led_state);

        /* Wait for next toggle */
        k_msleep(LED_PERIOD_MS);
    }
}

/* ================================================================== */
/* GPIO Toggle Thread - Toggle all GPIO pins every GPIO_TOGGLE_PERIOD_MS */
/* ================================================================== */
static void gpio_toggle_thread(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    printf("[GPIO Thread] Started - Period: %d ms\n", GPIO_TOGGLE_PERIOD_MS);
    printf("[GPIO Thread] Port 0 pins: %zu, Port 1 pins: %zu\n",
           PORT0_PIN_COUNT, PORT1_PIN_COUNT);

    while (1) {
        /* Toggle all Port 0 pins */
        for (size_t i = 0; i < PORT0_PIN_COUNT; i++) {
            gpio_pin_set(gpio0, port0_pins[i], gpio_state);
        }

        /* Toggle all Port 1 pins */
        for (size_t i = 0; i < PORT1_PIN_COUNT; i++) {
            gpio_pin_set(gpio1, port1_pins[i], gpio_state);
        }

        /* Print state */
        printf("[GPIO] All pins %s\n", gpio_state ? "HIGH" : "LOW");

        /* Toggle state for next cycle */
        gpio_state = !gpio_state;

        /* Wait for next toggle */
        k_msleep(GPIO_TOGGLE_PERIOD_MS);
    }
}

/* ================================================================== */
/* Initialize GPIO Pins as Output                                      */
/* ================================================================== */
static int init_gpio_pins(void)
{
    int ret;

    /* Initialize LED0 */
    if (!gpio_is_ready_dt(&led0)) {
        printf("LED0 device not ready!\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
    if (ret != 0) {
        printf("Failed to configure LED0: %d\n", ret);
        return ret;
    }

    /* Initialize Port 0 pins (excluding P0.15 which is LED0) */
    if (!device_is_ready(gpio0)) {
        printf("GPIO0 device not ready!\n");
        return -1;
    }

    for (size_t i = 0; i < PORT0_PIN_COUNT; i++) {
        ret = gpio_pin_configure(gpio0, port0_pins[i], GPIO_OUTPUT_INACTIVE);
        if (ret != 0) {
            printf("Failed to configure P0.%02d: %d\n", port0_pins[i], ret);
            return ret;
        }
    }

    /* Initialize Port 1 pins */
    if (!device_is_ready(gpio1)) {
        printf("GPIO1 device not ready!\n");
        return -1;
    }

    for (size_t i = 0; i < PORT1_PIN_COUNT; i++) {
        ret = gpio_pin_configure(gpio1, port1_pins[i], GPIO_OUTPUT_INACTIVE);
        if (ret != 0) {
            printf("Failed to configure P1.%02d: %d\n", port1_pins[i], ret);
            return ret;
        }
    }

    printf("GPIO pins initialized successfully\n");
    return 0;
}

/* ================================================================== */
/* Main Function                                                       */
/* ================================================================== */
int main(void)
{
    int ret;

    printf("========================================\n");
    printf("Pro Micro nRF52840 GPIO Demo\n");
    printf("LED Period: %d ms, GPIO Period: %d ms\n", LED_PERIOD_MS, GPIO_TOGGLE_PERIOD_MS);
    printf("========================================\n");

    /* Initialize all GPIO pins */
    ret = init_gpio_pins();
    if (ret != 0) {
        printf("GPIO initialization failed!\n");
        return 0;
    }

    /* Start LED blink thread */
    k_thread_create(&led_blink_thread_data, led_blink_stack,
                    K_THREAD_STACK_SIZEOF(led_blink_stack),
                    led_blink_thread, NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    /* Start GPIO toggle thread */
    k_thread_create(&gpio_toggle_thread_data, gpio_toggle_stack,
                    K_THREAD_STACK_SIZEOF(gpio_toggle_stack),
                    gpio_toggle_thread, NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    printf("Threads started successfully\n");

    /* Main thread sleeps - work done by child threads */
    while (1) {
        k_msleep(1000);
    }

    return 0;
}
