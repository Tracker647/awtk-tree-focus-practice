import os
import scripts.app_helper as app

CUSTOM_WIDGET_LIBS = []

DEPENDS_LIBS = CUSTOM_WIDGET_LIBS + []

ARGUMENTS['WITH_MVVM'] = 'true'
helper = app.Helper(ARGUMENTS)
APP_SRC = os.path.normpath(os.path.join(os.getcwd(), 'src'))
APP_CPPPATH = [
  os.path.join(APP_SRC, 'common'),
  os.path.join(APP_SRC, 'view_models'),
]
MVVM_SRC = os.path.normpath(os.path.join(os.getcwd(),'awtk_mvvm'))
MVVM_CPPPATH = [
  os.path.join(MVVM_SRC, '')
]

helper.set_deps(DEPENDS_LIBS).add_cpppath(APP_CPPPATH).add_cpppath(MVVM_CPPPATH).call(DefaultEnvironment)

SConscriptFiles = ['src/SConscript', 'tests/SConscript']
helper.SConscript(SConscriptFiles)
