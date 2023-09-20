# CHIP-8 Emulator

My take on the emulation of programs written in the CHIP-8 interpreted language for COSMAC VIP systems.

## Installation

### Requirements

You will need to download and install [vcpkg](https://vcpkg.io/en/) for
dependency management and [CMake](https://cmake.org/) for building the
project. Also, be sure to set the VCPKG_ROOT environment variable to the
root of your vcpkg installation.

First create the build folder and then generate the build files:

```bash
mkdir build
cmake --preset default
```

Then build the executable:

```bash
cmake --build --preset default
```

The binary file can be found at `build/bin/Release` by the name `chip8`

## Usage

You will need to find some CHIP-8 roms on the internet. Some testing roms
can be found in the roms folder. Then just run:

```bash
path/to/chip8.exe rom_path
```

## LICENSE

[MIT](https://github.com/unwrinkled/chip8-emu/blob/master/LICENSE)
