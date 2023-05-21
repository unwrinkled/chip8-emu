set windows-shell := ["nu", "-c"]

config := "Debug"
exe := "chip8"

# run target
run args: (build config)
	@./build/{{config}}/{{exe}}.exe {{args}}

# build project
build build_type=config: _generate
	@cmake --build build/ --config {{build_type}}
	@echo
	
# generate build files
_generate: _create_build
	@cmake -S . -B build/
	@echo

# create build folder if needed
_create_build:
	@{{ if path_exists("build") != "true" { `mkdir build` } else { '' } }}
