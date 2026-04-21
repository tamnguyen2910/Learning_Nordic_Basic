/*
 * Copyright (c) 2016 Intel Corporation
 * SPDX-License-Identifier: Apache-2.0
 *
 * Project : BH1750 Light Sensor with LED Indicator
 * Board   : Pro Micro nRF52840 (nRF52840 SoC)
 *
 * Description:
 *   - Read BH1750 ambient light sensor via I2C0
 *   - Control LED0 based on lux threshold
 *   - lux < 20 → LED ON (dark environment)
 *   - lux >= 20 → LED OFF (bright environment)
 *
 * Hardware Connection:
 *   BH1750 VCC  -> 3.3V
 *   BH1750 GND  -> GND
 *   BH1750 SDA  -> P1.00 (I2C0 SDA)
 *   BH1750 SCL  -> P0.11 (I2C0 SCL)
 *   BH1750 ADDR -> GND (Address = 0x23)
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>

/* ================================================================== */
/* CONFIGURATION                                                       */
/* ================================================================== */

/* I2C0 device node from device tree */
#define I2C0_NODE       DT_NODELABEL(i2c0)

/* BH1750 I2C address (ADDR pin connected to GND) */
#define BH1750_ADDR     0x23

/* Light threshold for LED control (in lux) */
#define LUX_THRESHOLD   20

/* Sensor reading interval (in milliseconds) */
#define READ_INTERVAL   1000

/* ================================================================== */
/* BH1750 COMMANDS                                                     */
/* ================================================================== */

#define BH1750_POWER_ON     0x01    /* Power on the sensor */
#define BH1750_RESET        0x07    /* Reset sensor registers */
#define BH1750_CONT_HIRES1  0x10    /* Continuous High Resolution Mode (1 lux) */

/* ================================================================== */
/* LED CONFIGURATION                                                   */
/* ================================================================== */

/* LED0 device from device tree (on-board red LED at P0.15) */
#define LED0_NODE DT_ALIAS(led0)

/* ================================================================== */
/* DEVICE HANDLES                                                      */
/* ================================================================== */

/* I2C0 device handle - will be initialized at runtime */
static const struct device *i2c0_dev;

/* LED0 GPIO specification from device tree */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* ================================================================== */
/* BH1750 FUNCTIONS                                                    */
/* ================================================================== */

/**
 * @brief Write a single command byte to BH1750
 * 
 * @param cmd Command byte to send
 * @return 0 on success, negative error code on failure
 */
static int bh1750_write_cmd(uint8_t cmd)
{
    return i2c_write(i2c0_dev, &cmd, 1, BH1750_ADDR);
}

/**
 * @brief Read light intensity from BH1750
 * 
 * @param lux Pointer to store the raw lux value (0-65535)
 * @return 0 on success, negative error code on failure
 * 
 * @note The sensor returns 2 bytes (MSB first) representing
 *       the light intensity in raw format. Convert to lux: lux = raw / 1.2
 */
static int bh1750_read_lux(uint16_t *lux)
{
    uint8_t data[2];
    int ret;
    
    /* Read 2 bytes from BH1750 */
    ret = i2c_read(i2c0_dev, data, 2, BH1750_ADDR);
    if (ret < 0) {
        return ret;
    }
    
    /* Combine MSB and LSB (sensor sends MSB first) */
    *lux = ((uint16_t)data[0] << 8) | data[1];
    
    return 0;
}

/**
 * @brief Initialize BH1750 sensor
 * 
 * @return 0 on success, negative error code on failure
 * 
 * @note Initialization sequence:
 *       1. Power on the sensor
 *       2. Reset sensor registers
 *       3. Set measurement mode (Continuous High Resolution)
 */
static int bh1750_init(void)
{
    /* Power on the sensor */
    if (bh1750_write_cmd(BH1750_POWER_ON) < 0) {
        return -1;
    }
    
    /* Reset sensor to default state */
    if (bh1750_write_cmd(BH1750_RESET) < 0) {
        return -1;
    }
    
    /* Start continuous high resolution mode (1 lux resolution) */
    if (bh1750_write_cmd(BH1750_CONT_HIRES1) < 0) {
        return -1;
    }
    
    return 0;
}

/* ================================================================== */
/* MAIN FUNCTION                                                       */
/* ================================================================== */

int main(void)
{
    uint16_t raw_lux;   /* Raw sensor value (0-65535) */
    int lux;            /* Converted lux value */
    
    /* -------------------------------------------------------------- */
    /* STEP 1: Initialize LED0 (on-board red LED)                    */
    /* -------------------------------------------------------------- */
    if (!gpio_is_ready_dt(&led0)) {
        printf("ERROR: LED0 not ready!\n");
        return 0;
    }
    gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
    printf("LED0: OK (P0.15)\n");
    
    /* -------------------------------------------------------------- */
    /* STEP 2: Initialize I2C0 bus                                    */
    /* -------------------------------------------------------------- */
    i2c0_dev = DEVICE_DT_GET(I2C0_NODE);
    if (!device_is_ready(i2c0_dev)) {
        printf("ERROR: I2C0 not ready!\n");
        return 0;
    }
    printf("I2C0: OK (SDA=P1.00, SCL=P0.11)\n");
    
    /* -------------------------------------------------------------- */
    /* STEP 3: Initialize BH1750 light sensor                         */
    /* -------------------------------------------------------------- */
    if (bh1750_init() < 0) {
        printf("ERROR: BH1750 not found at 0x%02X!\n", BH1750_ADDR);
        return 0;
    }
    printf("BH1750: OK (Address: 0x%02X)\n", BH1750_ADDR);
    printf("Threshold: %d lux\n", LUX_THRESHOLD);
    printf("========================================\n");
    
    /* Wait for first measurement to complete (max 120ms for high-res mode) */
    k_msleep(120);
    
    /* -------------------------------------------------------------- */
    /* STEP 4: Main loop - Read sensor and control LED                */
    /* -------------------------------------------------------------- */
    while (1) {
        /* Read light intensity from BH1750 */
        if (bh1750_read_lux(&raw_lux) == 0) {
            /* Convert raw value to lux: lux = raw / 1.2 */
            lux = (int)((uint32_t)raw_lux * 10U / 12U);
            
            /* Control LED based on light threshold */
            if (lux < LUX_THRESHOLD) {
                /* Dark environment - turn LED ON */
                gpio_pin_set_dt(&led0, 1);
                printf("Lux: %d (DARK) -> LED ON\n", lux);
            } else {
                /* Bright environment - turn LED OFF */
                gpio_pin_set_dt(&led0, 0);
                printf("Lux: %d (BRIGHT) -> LED OFF\n", lux);
            }
        }
        
        /* Wait before next reading */
        k_msleep(READ_INTERVAL);
    }
    
    return 0;
}