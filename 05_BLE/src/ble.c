#include <stdint.h>
#include <string.h>
#include "ble.h"
#include "led.h"
#include "display.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

LOG_MODULE_REGISTER(ble, LOG_LEVEL_INF);

static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);

static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

static struct k_work_delayable adv_work;

static void adv_work_handler(struct k_work *work)
{
    ble_start_advertising();
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err != 0) {
        LOG_ERR("Connection failed: %d", err);
        k_work_schedule(&adv_work, K_MSEC(500));
        return;
    }
    LOG_INF("Connected");
    led_stop_scan_blink();
    led_blink_fast(3);
    display_set_ble_connected(true);
    display_set_led(false);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("Disconnected, reason=%d", reason);
    led_start_scan_blink();
    display_set_ble_connected(false);
    display_set_led(false);
    k_work_schedule(&adv_work, K_MSEC(500));
}

int ble_init(void)
{
    int ret;

    k_work_init_delayable(&adv_work, adv_work_handler);
    bt_conn_cb_register(&conn_callbacks);

    ret = bt_enable(NULL);
    if (ret < 0) {
        return ret;
    }

    ble_start_advertising();
    return 0;
}

static uint8_t const adv_flags[] = { 0x02 };

void ble_start_advertising(void)
{
    int ret;

    static struct bt_le_adv_param param = {
        .options = BT_LE_ADV_OPT_CONN,
        .interval_min = BT_GAP_ADV_SLOW_INT_MIN,
        .interval_max = BT_GAP_ADV_SLOW_INT_MAX,
        .peer = NULL,
    };

    static const struct bt_data ad[] = {
        BT_DATA(BT_DATA_FLAGS, adv_flags, sizeof(adv_flags)),
        BT_DATA(BT_DATA_NAME_COMPLETE,
                (uint8_t const *)CONFIG_BT_DEVICE_NAME,
                strlen(CONFIG_BT_DEVICE_NAME)),
    };

    static const struct bt_data sd[] = {};

    bt_le_adv_stop();
    ret = bt_le_adv_start(&param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (ret < 0) {
        LOG_ERR("advertising start failed: %d", ret);
    }
}