#pragma once

#include "types.h"

#define TIMER_MS_WAIT (625) // 5*5*5*5

void timers_handle_t4();

void timers_setup();

isize sleep(u32 millis); // sleep for this amount of ms (maximum 6871947ms, >2hrs)

void shortsleep(u32 units); // sleep for approximately units * 1.6us

u32 get_time();

void timers_setup_user_timeout(u32 timeout);

