#ifndef _LED_OPS_H_
#define _LED_OPS_H_


struct led_ops {
    int num; /* 控制led灯的数量 */
    int (*init)(int which); /* 初始化led，which-哪一个led */
    int (*ctl)(int which, char status); /* 控制led， which-哪一个led，status-1-亮，0-灭 */
};

struct led_ops *get_board_led_ops(void);

#endif /* _LED_OPS_H_ */
