
@echo off

rc WndMenu.rc
cl main.cpp WndMenu.res /W4 /std:c++17 /EHsc /link /subsystem:windows
mt -manifest main.exe.manifest -outputresource:main.exe;1

del WndMenu.res
del main.exe.manifest

