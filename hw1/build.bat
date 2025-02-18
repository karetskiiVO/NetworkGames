cmake -S . -B build
C:\"Program Files"\"Microsoft Visual Studio"\2022\Community\MSBuild\Current\Bin\MSBuild.exe .\build\ALL_BUILD.vcxproj
rm .\server.exe
cp .\build\Debug\server.exe .\server.exe
cp .\build\Debug\client.exe .\client.exe