mkdir build_qjs
cd build_qjs
cmake -DCMAKE_CXX_COMPILER=g++ -DJS_ENGINE=quickjs -DLIB_NAME=quickjs -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -G "CodeBlocks - Unix Makefiles" ..
cd ..
cmake --build build_qjs --config Release
ls build_qjs
ls build_qjs/Release
