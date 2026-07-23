cd build
cmake .. -G Ninja -DCMAKE_POLICY_VERSION_MINIMUM=3.5 --fresh
cmake --build . --config Debug
cd ..