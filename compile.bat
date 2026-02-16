cd build
cmake .. -G Ninja --fresh
cmake --build . --config Release
cd ..
cd bin
DecDialogueEditor.exe
cd ..