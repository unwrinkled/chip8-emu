set windows-shell := ["nu", "-c"]

config := "Debug"
exe := "chip8"

# run target
[no-exit-message]
@run args="":
	./build/src/{{exe}} {{args}}

# build project
@build:
	cmake --build build
	echo
	
@test:
	ctest --test-dir build
	echo

# generate build files
@generate config="Debug": _create_build
	cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE={{ config }}
	echo

@install:
	cmake --install build --prefix out
	echo

@clean:
	rm -rf build/

# create build folder if needed
@_create_build:
	{{ if path_exists("build") != "true" { `mkdir build` } else { '' } }}
