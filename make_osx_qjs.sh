mkdir -p build_qjs && cd build_qjs
cmake -DJS_ENGINE=quickjs -DLIB_NAME=quickjs -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON  -GXcode ../
cd ..
cmake --build build_qjs --config Release
mkdir $GITHUB_WORKSPACE/macOS
cp build_qjs/Release/helloworld $GITHUB_WORKSPACE/macOS
