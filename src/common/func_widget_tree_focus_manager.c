#include "func_widget_tree_focus_manager.h"
#include "base/window_base.h"
#include "base/widget.h"
#include "tkc/mem.h"
#include <stdbool.h>


static ret_t widget_print_all_focusable_widgets(darray_t *all_focusable)
{
	return_value_if_fail(all_focusable, RET_FAIL);
	for (int i = 0; i < all_focusable->size; i++)
	{
		if (all_focusable->elms[i])
		{
			printf("widget_print_all_focusable_widgets>find widget name: %s at %p\r\n", WIDGET(all_focusable->elms[i])->name, WIDGET(all_focusable->elms[i]));
		}
	}
	return RET_OK;
}


/**
 * @brief 查找以widget为根节点下的所有可聚焦子控件，记录在all_focusable数组上 
 * 
 * @param [in] widget 
 * @param [out] all_focusable 
 * @return ret_t 
 */
static ret_t widget_get_all_focusable_widgets(widget_t *widget, darray_t *all_focusable)
{
	widget_foreach(widget, widget_on_visit_focusable, all_focusable);
	return_value_if_fail(widget != NULL, RET_FAIL);
	return_value_if_fail(all_focusable != NULL && all_focusable->size > 0, RET_FAIL);
	return RET_OK;
}

static ret_t widget_set_all_focusable(darray_t *all_focusable, bool flag)
{
	return_value_if_fail(all_focusable, RET_FAIL);
	for (int i = 0; i < all_focusable->size; i++)
	{
		if (all_focusable->elms[i])
		{
			widget_set_focusable(WIDGET(all_focusable->elms[i]), flag);
		}
	}
	return RET_OK;
}

static ret_t widget_set_all_focused(darray_t *all_focusable, bool flag)
{
	return_value_if_fail(all_focusable, RET_FAIL);
	for (int i = 0; i < all_focusable->size; i++)
	{
		if (all_focusable->elms[i])
		{
			widget_set_prop_bool(WIDGET(all_focusable->elms[i]), WIDGET_PROP_FOCUSED, flag);
		}
	}
	return RET_OK;
}

static ret_t on_no_focusable_children_default(void *ctx, event_t *e)
{
    printf("no focusable child widget\r\n");
    return RET_OK;
}

static ret_t on_no_focusable_parent_default(void *ctx, event_t *e)
{
    printf("no focusable parent widget\r\n");
    return RET_OK;
}

ret_t widget_tree_focus_set_cb(widget_t *win, enWidgetTreeFocusEvent treefocusEvent, widget_tree_focus_cb_t cb, void *ctx, event_t *e)
{   	
	widget_tree_focus_manager_t *manager = widget_get_prop_pointer(win, "window_tree_focus_manager");
    return_value_if_fail(manager != NULL, RET_BAD_PARAMS);
    switch(treefocusEvent){
        case kNoChildrenFocusable:
            manager->on_no_children_focusable.cb = cb;
            manager->on_no_children_focusable.ctx = ctx;
            manager->on_no_children_focusable.e = e;
            break;
        case kNoParentsFocusable:
            manager->on_no_parents_focusable.cb = cb;
            manager->on_no_parents_focusable.ctx = ctx;
            manager->on_no_parents_focusable.e = e;
            break;
        default:
            break;
    }
    return RET_OK;
}

widget_tree_focus_manager_t* widget_tree_focus_manager_create(widget_t *win){
    return_value_if_fail(win != NULL, NULL);
    widget_tree_focus_manager_t *manager = TKMEM_ZALLOC(widget_tree_focus_manager_t);
    manager->win = win;
    widget_tree_focus_set_cb(win, kNoChildrenFocusable, on_no_focusable_children_default, NULL, NULL);
    widget_tree_focus_set_cb(win, kNoParentsFocusable, on_no_focusable_parent_default, NULL, NULL);
    manager->g_focus_widget_list_stack = darray_create(10, NULL, NULL);
    manager->g_focus_widget_stack = darray_create(10, NULL, NULL);
    return manager;
}

ret_t widget_tree_focus_move_parent(widget_tree_focus_manager_t* manager, widget_t *focused_widget){
    return_value_if_fail(manager != NULL, RET_BAD_PARAMS);
    darray_t *g_focus_widget_list_stack = manager->g_focus_widget_list_stack;
    darray_t *g_focus_widget_stack = manager->g_focus_widget_stack;
    widget_t *win = manager->win;
    ret_t ret = RET_OK;
    if (g_focus_widget_list_stack->size == 0)
    {
        widget_tree_focus_cb_info_t *info = &manager->on_no_parents_focusable;
        if(info->cb){
            info->cb(info->ctx, info->e);
        }
        return RET_STOP;
    }

    /* 恢复父层控件的可聚焦性 */
    darray_t *parent_focusable_widget_list = darray_pop(g_focus_widget_list_stack);
    focused_widget = darray_pop(g_focus_widget_stack);
    ret = widget_set_all_focusable(parent_focusable_widget_list, TRUE);
    ret = widget_set_all_focused(parent_focusable_widget_list, FALSE);
    ret = widget_set_focused(focused_widget, TRUE);

    ret = darray_destroy(parent_focusable_widget_list);
    return ret;
}

ret_t widget_tree_focus_move_children(widget_tree_focus_manager_t* manager, widget_t *focused_widget){
    return_value_if_fail(manager != NULL, RET_BAD_PARAMS);
    darray_t *g_focus_widget_list_stack = manager->g_focus_widget_list_stack;
    darray_t *g_focus_widget_stack = manager->g_focus_widget_stack;
    widget_t *win = manager->win;
    ret_t ret = RET_OK;

    /* parent_focusable_widget_list的生命周期：在widget_tree_focus_move_children创建， 赋值入栈，在widget_tree_focus_move_parent出栈，销毁 */
    darray_t *parent_focusable_widget_list = darray_create(10, NULL, NULL);
    widget_get_all_focusable_widgets(win, parent_focusable_widget_list);

    /* child_focusable_widget_list的生命周期：比较简单，本函数内创建赋值，完成查找所有可聚焦子控件的任务后销毁 */
    darray_t *child_focusable_widget_list = darray_create(10, NULL, NULL);
    widget_get_all_focusable_widgets(focused_widget, child_focusable_widget_list);
    darray_remove(child_focusable_widget_list, focused_widget);

    if (child_focusable_widget_list->size > 0)
    {
        /* 屏蔽当前控件以上所有父层子控件，并聚焦当前控件的第一个子控件 */
        widget_set_all_focusable(parent_focusable_widget_list, FALSE);
        widget_set_all_focusable(child_focusable_widget_list, TRUE);
        widget_set_all_focused(parent_focusable_widget_list, FALSE);
        widget_set_focused(WIDGET(child_focusable_widget_list->elms[0]), TRUE);

        /* 保存这些取消聚焦的控件列表和前一个聚焦控件，以便在widget_tree_focus_move_parent中出栈恢复 */
        darray_push(g_focus_widget_list_stack, parent_focusable_widget_list);
        darray_push(g_focus_widget_stack, focused_widget);
    }
    else
    {
        widget_tree_focus_cb_info_t *info = &manager->on_no_children_focusable;
        if(info->cb){
            info->cb(info->ctx, info->e);
        }
    }
    ret = darray_destroy(child_focusable_widget_list);
    return ret;
}

ret_t widget_tree_focus_manager_destroy(widget_tree_focus_manager_t *manager)
{
    printf("widget_tree_focus_manager_destroy\r\n");
    if(manager == NULL)
        return RET_OK;
    darray_destroy(manager->g_focus_widget_stack);
    darray_destroy(manager->g_focus_widget_list_stack);
    TKMEM_FREE(manager);
    return RET_OK;
}

ret_t widget_tree_focus_manager_on_destroy(void* ctx, event_t* e){
    widget_tree_focus_manager_destroy(ctx);
    return RET_OK;
}

ret_t widget_tree_focus_manager_on_key_down(void* ctx, event_t* e)
{
    key_event_t* evt = (key_event_t*)e;
    widget_tree_focus_manager_t *manager = (widget_tree_focus_manager_t *)ctx;
    widget_t *focused_widget = widget_get_focused_widget(manager->win);
    return_value_if_fail(focused_widget, RET_BAD_PARAMS);

    if(evt->key == TK_KEY_RETURN){
        widget_tree_focus_move_children(manager, focused_widget);
    }
    else if(evt->key == TK_KEY_ESCAPE){
        widget_tree_focus_move_parent(manager, focused_widget);
    }
    return RET_OK;
}

ret_t widget_tree_focus_init(widget_t *win){
    printf("widget_tree_focus_init\r\n");
    widget_tree_focus_manager_t* tree_focus_manager = widget_get_prop_pointer(win, "window_tree_focus_manager");
    if(NULL == tree_focus_manager){
        printf("window_tree_focus_manager created\r\n");
        tree_focus_manager = widget_tree_focus_manager_create(win);
        return_value_if_fail(tree_focus_manager != NULL, RET_OOM);
        widget_set_prop_pointer(win, "window_tree_focus_manager", tree_focus_manager);
        widget_on(win, EVT_KEY_DOWN, widget_tree_focus_manager_on_key_down, tree_focus_manager);    
        widget_on(win, EVT_DESTROY, widget_tree_focus_manager_on_destroy, tree_focus_manager);  
    }
    return RET_OK;
}

ret_t func_widget_tree_focus_init(fscript_t* fscript, fscript_args_t* args, value_t* result){
    widget_t* widget = NULL;
    // FSCRIPT_FUNC_CHECK(args->size >= 1, RET_BAD_PARAMS);
    widget = WIDGET(tk_object_get_prop_pointer(fscript->obj, STR_PROP_SELF));
    widget_t *win = widget_get_window(widget);
    printf("func_widget_tree_focus_init\r\n");
    if (args->size == 0) {
        widget_tree_focus_manager_t* tree_focus_manager = widget_get_prop_pointer(win, "window_tree_focus_manager");
        if(NULL == tree_focus_manager){
            printf("window_tree_focus_manager created\r\n");
            tree_focus_manager = widget_tree_focus_manager_create(win);
            return_value_if_fail(tree_focus_manager != NULL, RET_OOM);
            widget_set_prop_pointer(win, "window_tree_focus_manager", tree_focus_manager);
            widget_on(win, EVT_KEY_DOWN, widget_tree_focus_manager_on_key_down, tree_focus_manager);    
            widget_on(win, EVT_DESTROY, widget_tree_focus_manager_on_destroy, tree_focus_manager);  
        }
    }
    FSCRIPT_FUNC_CHECK(win != NULL, RET_BAD_PARAMS);
    return RET_OK;
}