set windows-shell := ["nu", "-c"]

config := "Debug"
exe := "chip8"

# run target
[no-exit-message]
@run args="":
	./build/src/{{exe}} {{args}}

# build project
@build build_type=config:
	cmake --build build
	echo
	
@test:
	ctest --test-dir build
	echo

# generate build files
@generate: _create_build
	cmake -B build -G Ninja
	echo

@clean:
	rm -rf build/

# create build folder if needed
@_create_build:
	{{ if path_exists("build") != "true" { `mkdir build` } else { '' } }}
