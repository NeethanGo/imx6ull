#ifndef _BUTTON_DRV_H
#define _BUTTON_DRV_H

struct button_operations {
    int count;
    void (*init)(int which);
    int (*read)(int which);
};

struct button_operations *get_board_button_opr(void);

#endif

