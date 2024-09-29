#include "awtk.h"
#include "mvvm/mvvm.h"
#include "mvvm_app.inc"
#include "setting_menu.h"
#include "./view_models/temperature_view_model.h"
#include "tkc/fscript.h"
#include "common/func_widget_tree_focus_manager.h"

#ifndef APP_SYSTEM_BAR
#define APP_SYSTEM_BAR ""
#endif /*APP_SYSTEM_BAR*/

#ifndef APP_BOTTOM_SYSTEM_BAR
#define APP_BOTTOM_SYSTEM_BAR ""
#endif /*APP_BOTTOM_SYSTEM_BAR*/

#ifndef APP_START_PAGE
#define APP_START_PAGE "setting_menu"
#endif /*APP_START_PAGE*/

/**
 * 注册自定义控件
 */
static ret_t custom_widgets_register(void) {

  return RET_OK;
}

/**
 * 当程序初始化完成时调用，全局只触发一次。
 */
static ret_t application_on_launch(void) {

  return RET_OK;
}

/**
 * 当程序退出时调用，全局只触发一次。
 */
static ret_t application_on_exit(void) {

  return RET_OK;
}

/**
 * 初始化程序
 */
ret_t application_init(void) {
  mvvm_app_init();
  custom_widgets_register();
  application_on_launch();
  if (strlen(APP_SYSTEM_BAR) > 0) {
    navigator_to(APP_SYSTEM_BAR);
  }

  if (strlen(APP_BOTTOM_SYSTEM_BAR) > 0) {
    navigator_to(APP_BOTTOM_SYSTEM_BAR);
  }

  fscript_register_func("widget_tree_focus_init", func_widget_tree_focus_init);
  fscript_register_func("setting_menu_init", func_setting_menu_init);
  return navigator_to(APP_START_PAGE);
}

/**
 * 退出程序
 */
ret_t application_exit(void) {
  application_on_exit();
  log_debug("application_exit\n");
   mvvm_app_deinit();
  return RET_OK;
}
