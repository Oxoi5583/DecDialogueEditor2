cd build
cmake .. -G Ninja --fresh
cmake --build . --config Debug
cd ..
cd bin
DecDialogueEditor.exe
cd ..