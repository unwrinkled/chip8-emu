set windows-shell := ["nu", "-c"]

config := "Debug"
exe := "chip8"

# run target
[no-exit-message]
@run args="":
	./build/bin/{{config}}/{{exe}}.exe {{args}}

# build project
@build build_type=config:
	cmake --build build/ --preset {{lowercase(build_type)}}
	echo
	
# install project
@install install_type="Release":
	cmake --build build/ --target install --config {{install_type}}
	echo

@test:
	ctest --test-dir build
	echo

@clean:
	rm -rf build/

# generate build files
@generate: _create_build
	cmake -Bbuild/ --preset default -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
	echo

# create build folder if needed
@_create_build:
	{{ if path_exists("build") != "true" { `mkdir build` } else { '' } }}
