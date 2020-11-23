mkdir build_qjs & pushd build_qjs
cmake -DJS_ENGINE=quickjs -DLIB_NAME=quickjs -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -G "CodeBlocks - Unix Makefiles" ..
popd
cmake --build build_qjs --config Release
pause
