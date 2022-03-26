
@echo off

rc WndMenu.rc

if not exist builds\ mkdir builds

cl main.cpp WndMenu.res /W4 /std:c++17 /EHsc /Fe:builds\main.exe /link /subsystem:windows

cd builds

mt -manifest main.exe.manifest -outputresource:main.exe;1
del main.exe.manifest

cd ..

del WndMenu.res

