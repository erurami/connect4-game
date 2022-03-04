
@echo off

cl main.cpp /W4 /std:c++17 /EHsc
mt -manifest main.exe.manifest -outputresource:main.exe;1

