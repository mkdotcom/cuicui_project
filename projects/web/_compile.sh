#!/bin/sh
# Builds the web version of the app into the "build" folder.
# usage: ./_compile.sh debug   or   ./_compile.sh release
# Needs emscripten (https://emscripten.org): run "source <emsdk>/emsdk_env.sh" first.
set -e
cd "$(dirname "$0")"

if [ "$1" != "debug" ] && [ "$1" != "release" ]; then
	echo "usage: ./_compile.sh debug   or   ./_compile.sh release"
	exit 1
fi

rm -rf build
mkdir -p build/mkw_assets

emcc -I../../ -std=c++14 -DCUICUI_WEB $(cat your_app_cpp_files.txt) $(cat cuicui_core_cpp_files.txt) $(cat "$1"_conf.txt) \
	-s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s FETCH=1 -s NO_EXIT_RUNTIME=1 \
	-s EXPORTED_FUNCTIONS="['_main', '_resize_event', '_back_button_event', '_malloc', '_free']" \
	-s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'print', 'HEAPU8']" \
	-o build/a.out.js

cp index.html fav_icon_png.png build/
cp ../../cuicui_core/web/cuicui_core.css ../../cuicui_core/web/cuicui_core.js build/
cp -R ../../app/assets/. build/mkw_assets/

echo 'done: serve the "build" folder with a local web server, e.g. ./_serve.sh'
