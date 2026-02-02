# CHIP-8 Emulator

My take on building a CHIP-8 emulator.

## Installation

### Requirements

You will need a C compiler, and [CMake](https://cmake.org/) to build the project.

First create the build folder and generate the build files:

```bash
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Then build the executable:

```bash
cmake --build build
```

Finally, install the binary together with some sample roms:

```bash
cmake --install build --prefix out
```

The binary file will be located at `out/bin/chip8`.

## Usage

You will need to find some CHIP-8 roms on the internet, but you can also test it
with the roms that are installed together with the binary. For example, you can
invoke the executable like so:

```bash
./out/bin/chip8 ./out/roms/IBM_Logo.ch8
```

## LICENSE

[MIT](https://github.com/unwrinkled/chip8-emu/blob/master/LICENSE)
