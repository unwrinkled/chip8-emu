set windows-shell := ["nu", "-c"]

config := "Debug"
toolchain := env_var_or_default('TOOLCHAIN_FILE', '')
generator := "Ninja Multi-Config"

exe := "chip8"

# run target
@run args="": (build config)
	./build/{{config}}/{{exe}}.exe {{args}}

# build project
@build build_type=config:
	cmake --build build/ --config {{build_type}}
	echo
	
# install project
@install install_type="Release":
	cmake --build build/ --target install --config {{install_type}}
	echo

@clean:
	rm -rf build/

# generate build files
@generate: _create_build
	cmake -S . -B build/ -G '{{generator}}' {{ if toolchain != '' { '--toolchain ' + toolchain } else {''} }}
	echo

# create build folder if needed
@_create_build:
	{{ if path_exists("build") != "true" { `mkdir build` } else { '' } }}
