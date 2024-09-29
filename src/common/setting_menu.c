#include "awtk.h"
#include "../common/navigator.h"
#include "common/func_widget_tree_focus_manager.h"
#include <stdbool.h>



/**
 * 初始化窗口的子控件
 */
static ret_t visit_init_child(void *ctx, const void *iter)
{
	widget_t *win = WIDGET(ctx);
	widget_t *widget = WIDGET(iter);
	const char *name = widget->name;

	// 初始化指定名称的控件（设置属性或注册事件），请保证控件名称在窗口上唯一
	if (name != NULL && *name != '\0')
	{
		if (tk_str_eq(name, "setting_menu"))
		{
	
		}
	}

	return RET_OK;
}

ret_t setting_menu_on_no_children_focusable(void *ctx, event_t *e){
	printf("setting menu on no children focusable\r\n");
	return RET_OK;
}
 
ret_t setting_menu_on_no_parents_focusable(void *ctx, event_t *e){
	printf("setting menu on no parents focusable\r\n");
	return RET_OK;
}

 
/**
 * 初始化窗口
 */
ret_t setting_menu_init(widget_t *win, void *ctx)
{
	return_value_if_fail(win != NULL, RET_BAD_PARAMS);
	widget_foreach(win, visit_init_child, win);
	printf("setting_menu_init\r\n");
	return RET_OK;
}


ret_t func_setting_menu_init(fscript_t* fscript, fscript_args_t* args, value_t* result)
{
	widget_t* self = WIDGET(tk_object_get_prop_pointer(fscript->obj, STR_PROP_SELF));
	widget_t* curr_win = widget_get_window(self);
	printf("func_setting_menu_init\r\n");
	widget_tree_focus_init(curr_win);
	widget_tree_focus_set_cb(curr_win, kNoChildrenFocusable, setting_menu_on_no_children_focusable, NULL, NULL);
	widget_tree_focus_set_cb(curr_win, kNoParentsFocusable, setting_menu_on_no_parents_focusable, NULL, NULL);
	return setting_menu_init(curr_win, NULL);
}