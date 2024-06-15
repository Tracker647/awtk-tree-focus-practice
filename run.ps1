./src/view_models/gen.ps1
python ./scripts/update_res.py all
scons AWTK_ROOT='d:/devtools/awtk'
bin/demo.exe