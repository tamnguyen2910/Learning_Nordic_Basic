#ifndef LED_SERVICE_H__
#define LED_SERVICE_H__

#include <stdint.h>
#include <zephyr/bluetooth/bluetooth.h>

int led_service_init(void);
void led_service_connected(struct bt_conn *conn);
void led_service_disconnected(void);
void led_service_send_lux(uint32_t lux_x100);

#endif /* LED_SERVICE_H__ */