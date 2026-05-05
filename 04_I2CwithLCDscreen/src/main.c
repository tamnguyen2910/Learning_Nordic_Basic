/* High level includes */
/* Include standard Zephyr headers compatible with older SDKs */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>
/* Runtime arch: two-task approach uses sensor_task/ui_task */
/* Expose task entry points */
extern void sensor_task(void *a, void *b, void *c);
extern void ui_task(void *a, void *b, void *c);

LOG_MODULE_REGISTER(main, CONFIG_DISPLAY_LOG_LEVEL);

/* Thread stacks and priority */
K_THREAD_STACK_DEFINE(sensor_stack, 1024);
K_THREAD_STACK_DEFINE(ui_stack, 1024);
#define SENSOR_THREAD_PRIORITY 7
#define UI_THREAD_PRIORITY 6
static struct k_thread sensor_thread;
static struct k_thread ui_thread;

int main(void)
{
    LOG_INF("========================================");
    LOG_INF("SSD1309 OLED Display Demo");
    LOG_INF("Pro Micro nRF52840");
    LOG_INF("========================================");

    k_tid_t sensor_tid = k_thread_create(&sensor_thread, sensor_stack, 1024,
                                       sensor_task, NULL, NULL, NULL,
                                       SENSOR_THREAD_PRIORITY, 0, K_NO_WAIT);
    if (!sensor_tid) {
        LOG_ERR("Failed to create sensor thread");
        return -1;
    }
    LOG_INF("Sensor thread started (tid: %p)", sensor_tid);

    k_tid_t ui_tid = k_thread_create(&ui_thread, ui_stack, 1024,
                                      ui_task, NULL, NULL, NULL,
                                      UI_THREAD_PRIORITY, 0, K_NO_WAIT);
    if (!ui_tid) {
        LOG_ERR("Failed to create UI thread");
        return -1;
    }
    LOG_INF("UI thread started (tid: %p)", ui_tid);

    while (1) {
        k_sleep(K_SECONDS(1));
    }
    return 0;
}
