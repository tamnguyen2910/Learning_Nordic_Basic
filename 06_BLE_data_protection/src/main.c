/**
 * ============================================================
 *  BLE UART Terminal — nRF52840 (Zephyr / nRF Connect SDK v3.2.4)
 * ============================================================
 *
 *  PURPOSE:
 *    - Phone sends text commands: "LED_ON" or "LED_OFF"
 *    - nRF52840 receives command => controls onboard LED
 *    - nRF52840 sends back "LED ON" / "LED OFF" via BLE Notify
 *
 *  PROTOCOL:
 *    Nordic UART Service (NUS)
 *      Service: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
 *      TX Char (nRF->Phone, Notify): 6E400003-B5A3-F393-E0A9-E50E24DCCA9E
 *      RX Char (Phone->nRF, Write):  6E400002-B5A3-F393-E0A9-E50E24DCCA9E
 *
 *  HARDWARE:
 *    Board: Pro Micro nRF52840
 *    LED:   GPIO P0.15 (onboard, active HIGH)
 *
 *  RECOMMENDED PHONE APPS:
 *    - "nRF Connect" (Nordic Semiconductor) -> UART tab
 *    - "Serial Bluetooth Terminal" (Android)
 * ============================================================
 */

// ========================= HEADERS =========================

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

/*
 * NOTE: NUS library is located in the NRF SDK (not Zephyr core).
 * Path: <bluetooth/services/nus.h> (under nrf/include/)
 * Do NOT use <zephyr/bluetooth/services/nus.h> — that is the newer
 * multi-instance version, incompatible with the NRF NUS implementation.
 */
#include <bluetooth/services/nus.h>

#include <hal/nrf_gpio.h>
#include <string.h>

// ===================== LOG CONFIG =====================

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

// ======================== CONSTANTS ========================

#define LED_PIN   15    /* LED onboard GPIO P0.15 */

// ==================== FORWARD DECLARATIONS ======================

static void on_connected(struct bt_conn *conn, uint8_t err);
static void on_disconnected(struct bt_conn *conn, uint8_t reason);
static void on_advertising_start(void);
static void on_nus_received(struct bt_conn *conn,
                            const uint8_t *const data, uint16_t len);

/* Delayable work item to restart advertising after disconnection */
static struct k_work_delayable adv_work;

// ==================== BLE CALLBACKS ======================

static struct bt_conn_cb conn_callbacks = {
    .connected    = on_connected,
    .disconnected = on_disconnected,
};

/*
 * Handler for the delayable work item.
 * Called 500ms after disconnection to let the BLE stack fully
 * tear down the connection before restarting advertising.
 */
static void adv_work_handler(struct k_work *work)
{
    on_advertising_start();
}

// ==================== LED SETUP ======================

static void led_init(void)
{
    nrf_gpio_pin_dir_set(LED_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_clear(LED_PIN);
}

static void led_on(void)
{
    nrf_gpio_pin_set(LED_PIN);
}

static void led_off(void)
{
    nrf_gpio_pin_clear(LED_PIN);
}

// ================== NUS CALLBACKS ======================

/**
 * on_nus_received() - Callback when phone sends data via NUS RX.
 *
 * Description:
 *   Receives text data from the phone, uses strstr() to detect
 *   "LED_ON" or "LED_OFF" commands regardless of position in buffer
 *   (to handle trailing \r\n or other special characters from apps).
 *
 *   After processing the command, sends a text response back to the
 *   phone via bt_nus_send().
 *
 * Parameters:
 *   conn  - Current BLE connection
 *   data  - Pointer to received data from phone
 *   len   - Data length (bytes)
 */
static void on_nus_received(struct bt_conn *conn,
                            const uint8_t *const data, uint16_t len)
{
    char cmd[32];

    /* Copy data to local buffer and add null terminator */
    if (len >= sizeof(cmd)) {
        len = sizeof(cmd) - 1;
    }
    memcpy(cmd, data, len);
    cmd[len] = '\0';

    LOG_INF("Received: %s", cmd);

    /* Check for "LED_ON" command */
    if (strstr(cmd, "LED_ON") != NULL) {
        led_on();
        LOG_INF("LED ON");

        /* Send "LED ON" response back to phone via BLE Notify */
        bt_nus_send(conn, (const uint8_t *)"LED ON\r\n", 9);

    /* Check for "LED_OFF" command */
    } else if (strstr(cmd, "LED_OFF") != NULL) {
        led_off();
        LOG_INF("LED OFF");

        bt_nus_send(conn, (const uint8_t *)"LED OFF\r\n", 10);

    } else {
        LOG_WRN("Unknown: %s", cmd);
        bt_nus_send(conn, (const uint8_t *)"Unknown\r\n", 10);
    }
}

// ================== CONNECTION CALLBACKS ===================

static void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err != 0) {
        LOG_ERR("Connection failed: %d", err);
        k_work_schedule(&adv_work, K_SECONDS(1));
        return;
    }

    LOG_INF("Connected!");

    /* Blink LED 3 times to indicate connection established */
    for (int i = 0; i < 3; i++) {
        led_on();
        k_msleep(100);
        led_off();
        k_msleep(100);
    }
}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("Disconnected, reason=%d", reason);

    /*
     * DO NOT call on_advertising_start() directly from the disconnect callback!
     * This callback runs in the BLE stack context (BT RX thread).
     * Calling bt_le_adv_start() immediately would fail because the stack has
     * not yet fully processed the disconnection.
     *
     * Solution: use k_work_schedule() with a 500ms delay,
     * allowing the stack to complete disconnection before restarting advertising.
     */
    k_work_schedule(&adv_work, K_MSEC(500));
}

// ==================== BLE ADVERTISING =====================

static void on_advertising_start(void)
{
    int err;

    static const uint8_t adv_flags[] = { 0x02 };

    static const struct bt_data ad[] = {
        BT_DATA(BT_DATA_FLAGS, adv_flags, sizeof(adv_flags)),
        BT_DATA(BT_DATA_NAME_COMPLETE,
                CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
    };

    static const struct bt_data sd[] = {};

    /*
     * BT_LE_ADV_CONN_FAST_1: connectable advertising with fast interval.
     * No need to call bt_le_adv_stop() first — advertising stops on disconnect.
     */
    err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1,
                          ad, ARRAY_SIZE(ad),
                          sd, ARRAY_SIZE(sd));
    if (err < 0) {
        LOG_ERR("Advertising start failed: %d", err);
    } else {
        LOG_INF("Scanning");
    }
}

// ========================= MAIN =========================

/**
 * main() - Program entry point.
 *
 * Initialization sequence:
 *   1. Initialize GPIO LED
 *   2. Initialize NUS (Nordic UART Service) with callback
 *   3. Register BLE connection callbacks
 *   4. Initialize BLE stack (bt_enable)
 *   5. Start advertising
 *   6. Infinite loop — all processing via callbacks
 */
int main(void)
{
    int err;

    LOG_INF("=== BLE UART Terminal ===");

    /* ===== Step 1: Initialize LED and work item ===== */
    led_init();
    k_work_init_delayable(&adv_work, adv_work_handler);

    /* ===== Step 2: Initialize NUS =====
     *
     * bt_nus_init() registers GATT NUS service + 2 characteristics (TX, RX).
     * Pass a bt_nus_cb struct to receive callbacks when data arrives from phone.
     *
     * Note:
     *   - Use <bluetooth/services/nus.h> (NRF SDK) -> bt_nus_init()
     *   - Do NOT use <zephyr/bluetooth/services/nus.h> (Zephyr core)
     *   - Callback .received has 3 parameters: (conn, data, len)
     */
    static struct bt_nus_cb nus_cb = {
        .received = on_nus_received,
    };
    err = bt_nus_init(&nus_cb);
    if (err < 0) {
        LOG_ERR("NUS init failed: %d", err);
        return err;
    }
    LOG_INF("NUS initialized");

    /* ===== Step 3: Register BLE connection callbacks ===== */
    bt_conn_cb_register(&conn_callbacks);

    /* ===== Step 4: Initialize BLE stack ===== */
    err = bt_enable(NULL);
    if (err < 0) {
        LOG_ERR("BLE init failed: %d", err);
        return err;
    }
    LOG_INF("BLE initialized");

    /* ===== Step 5: Start advertising ===== */
    on_advertising_start();

    /* ===== Step 6: Main loop ===== */
    while (1) {
        k_sleep(K_SECONDS(1));
    }

    return 0;
}
