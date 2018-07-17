pushd build
cl /EHsc /Zi /Fech8.exe /I "..\include" ../main.cpp ../emulator.cpp /link SDL2main.lib SDL2.lib /SUBSYSTEM:CONSOLE
popd build
