#include "display_manager.h"
#include "display.h"
#include <stdio.h>

int display_manager_init(void)
{
    return display_init();
}

int display_manager_render(int32_t lux_fixed, bool led_on, bool sensor_ok)
{
    char buf[32];

    if (!sensor_ok) {
        oled_clear();
        display_write_text(0, 1, "  SENSOR ERROR  ");
        display_write_text(0, 2, " BH1750 Offline ");
        display_write_text(0, 3, "Check wiring! ");
        return display_update();
    }

    int32_t lux_int = lux_fixed / 1000000;
    int32_t lux_frac = lux_fixed % 1000000;
    if (lux_frac < 0) lux_frac = -lux_frac;

    oled_clear();

    display_write_text(0, 0, "AMBIENT LIGHT");

    snprintf(buf, sizeof(buf), "%d.%02u lux", (int)lux_int, (unsigned)(lux_frac / 10000));
    display_write_text(0, 2, buf);

    if (led_on) {
        display_write_text(0, 3, "[ON ] LED");
    } else {
        display_write_text(0, 3, "[OFF] LED");
    }

    int ret = display_update();
    return ret;
}
