#ifndef SETTING_MENU_H
#define SETTING_MENU_H
#include "base/widget.h"
#include "tkc/fscript.h"
ret_t setting_menu_init(widget_t *win, void *ctx);
ret_t func_setting_menu_init(fscript_t* fscript, fscript_args_t* args, value_t* result);
#endif