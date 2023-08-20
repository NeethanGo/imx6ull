
#ifndef _LED_DRV2APP_H
#define _LED_DRV2APP_H

#include "led_opr.h"

void led_device_create(int minor);
void led_device_destory(int minor);
void register_led_operations(struct led_operations *opr);

#endif /* _LED_DRV2APP_H */

