#include <stdint.h>
#include "led_service.h"
#include "led.h"
#include "display.h"
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>

LOG_MODULE_REGISTER(led_svc, LOG_LEVEL_INF);

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
        led_toggle();
        display_set_led(true);
        LOG_INF("LED TOGGLE");
    }

    return len;
}

BT_GATT_SERVICE_DEFINE(led_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0xABCD)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0xABCE),
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, led_control_write, NULL),
);

int led_service_init(void)
{
    return 0;
}