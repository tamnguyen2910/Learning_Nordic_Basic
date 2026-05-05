#include "shared_state.h"
#include <zephyr/kernel.h>

K_MUTEX_DEFINE(g_state_mutex);
shared_state_t g_shared_state = {0, false, false};
