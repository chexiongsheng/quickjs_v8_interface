mkdir build_qjs
cd build_qjs
cmake -DJS_ENGINE=quickjs -DLIB_NAME=quickjs -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -G "CodeBlocks - Unix Makefiles" ..
cd ..
cmake --build build_qjs --config Release
pause
