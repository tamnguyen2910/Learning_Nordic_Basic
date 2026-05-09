#include <stdint.h>
#include "led_service.h"
#include "hal/led.h"
#include "drivers/display.h"
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

LOG_MODULE_REGISTER(led_svc, LOG_LEVEL_INF);

static struct bt_conn *current_conn;
static uint32_t lux_val;

static ssize_t led_control_write(struct bt_conn *conn,
                                 struct bt_gatt_attr const *attr,
                                 void const *buf, uint16_t len,
                                 uint16_t offset, uint8_t flags)
{
    uint8_t const *cmd = (uint8_t const *)buf;

    if (len < 1) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    if (cmd[0] == 0x01) {
        led_on();
        display_set_led(true);
        LOG_INF("LED ON");
    } else if (cmd[0] == 0x00) {
        led_off();
        display_set_led(false);
        LOG_INF("LED OFF");
    } else {
        LOG_WRN("Invalid LED command: 0x%02X", cmd[0]);
    }

    return len;
}

static ssize_t lux_read(struct bt_conn *conn,
                        struct bt_gatt_attr const *attr,
                        void *buf, uint16_t len, uint16_t offset)
{
    uint32_t val = lux_val;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &val, sizeof(val));
}

BT_GATT_SERVICE_DEFINE(led_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0xABCD)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0xABCE),
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, led_control_write, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0xABCF),
                           BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ,
                           lux_read, NULL, &lux_val),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

static const struct bt_gatt_attr *lux_attr;

int led_service_init(void)
{
    lux_attr = &led_svc.attrs[4];
    return 0;
}

void led_service_connected(struct bt_conn *conn)
{
    current_conn = conn;
}

void led_service_disconnected(void)
{
    current_conn = NULL;
}

void led_service_send_lux(uint32_t lux_x100)
{
    lux_val = lux_x100;
    if (current_conn) {
        bt_gatt_notify(current_conn, lux_attr, &lux_val, sizeof(lux_val));
    }
}