Firmware
========

## Release files

The latest release may be acquired from https://github.com/RoverRobotics/OpenRoverFirmware-dan/releases/latest

The following files are included.

* `bootypic.hex` - Bootloader. This is responsible for bringing up the device and installing new firmware
* `PowerBoard.hex` - Power Board firmware. This is responsible for driving the motors, providing hardware feedback, and more. It is expected to run on top of the bootloader, but can handle the device on its own.
* `docs` - Documentation generated from the source code. This is mostly useful only for developers.

## Installing the bootloader

The bootloader image is called `bootypic.hex`. This must be installed with a developer tool like the PICKit 3. To quick install with the `PK3CMD` command line utility:

```cmd
set pk3cmd="C:\opt\Microchip\MPLAB IDE\Programmer Utilities\PICkit3\PK3CMD.exe"
set part=24fj256gb106
set oldhex="%USERPROFILE%\Downloads\backup.hex"
set newhex="%USERPROFILE%\Documents\OpenRoverFirmware\build\clion\bootypic\bootypic.hex"
%pk3cmd% -P%part% -GF%oldhex% -R%newhex%
```

## Installing firmware on top of the bootloader

Assuming the bootloader is installed, you don't need the PICKit to install the firmware. With a header board attached to the computer via USB:

```cmd
set %newhex%="%USERPROFILE%\Documents\OpenRoverFirmware\build\clion\PowerBoard\PowerBoard.hex"
pip3 install --upgrade openrover
pitstop -f %newhex%
```

### Troubleshooting bootloader

 * If booty says "device not responding", it means that either the rover has no booted or the rover is no longer in bootloader mode:
   * If the green serial board LED is **off**, the rover has not yet run its boot sequence. Make sure the battery has charge.
   * If the green serial board LED is **on**, the has run its boot sequence. The rover only remains in bootloader mode for 10 seconds after being powered on - if that time is elapsed, you need to reboot the robot. Hold down the red side power button for 5 seconds to restart the rover into bootloader mode then retry (note the light may or may not turn off immediately upon reboot).
 * The location of your hexfile may vary. If not found, booty will report "no such file or directory"

## Development

### IDE and build tools

I recommend using a CMake-aware IDE like CLion for development.

For debugging, use MPLAB 8. MPLAB X (5.30) has numerous bugs. Even its bugs have bugs.

### Building with CMake

1. Install XC16 the [Microchip XC16 Toolsuite](https://www.microchip.com/mplab/compilers). This toolsuite contains a compiler/linker/assembler and also standard libraries for the PIC24F MCU's. I recommend installing this to the path "C:/opt/Microchip/xc16", since the default (in "Program Files (x86)" contains spaces, which can cause some build tools to complain)
2. Generate build files with CMake
3. Build it!

```
cd OpenRoverFirmware
mkdir build; cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="..\cmake\PowerBoard_toolchain.cmake"
cmake --build .
```

Note that ABI detection can fail due to [a bug in the XC16 compiler](https://www.microchip.com/forums/m1126857.aspx). I have not found this to cause a problem beyond some scary-looking warnings and failure of code autocomplete in CLion.

### IDE iteration

You can build in the MPLAB IDE using the project files in the MPLAB subfolder, though there may be some gotchas. Due to difficulties wtih XC16 and how MPLAB handles command line arguments, it is likely that the projects will build with the legacy libc and pic32 libraries. (in the `*.map` file this looks like `lega-pic30-elf` and `lega-c-elf` instead of `pic30-elf` and `c-elf`).

### Code style tools

To tidy up code, I like using **[clang-format](https://clang.llvm.org/docs/ClangFormat.html)**, and have provided a .clang-format file.

### Debugging

Load the .elf file into Microchip MPLAB X. (using the elf file instead of the hex will allow you to see debugging info)

#### Ubuntu installation of clang-format

```bash
sudo apt install clang-format
```
#### Windows installation of clang-format and git

```batch
choco install git llvm
```

Before committing, please run the following to clang-format all uncommitted C files:

```bash
git diff HEAD --name-only --relative --diff-filter=d -- **.h **.c | xargs clang-format -style=file -i
```
```batch
set PATH=C:\Program Files\Git\usr\bin;%PATH%
git diff HEAD --name-only --relative --diff-filter=d -- **.h **.c | xargs clang-format -style=file -i
```

This will tidy up all locally modified files. You can make this a precommit hook if you like.

Code tips for debuggability as of MPLAB v8.92:

* You can build the executable as either an ELF or COF (Project -> Build Options ... -> Project -> XC16 ASM/C Suite -> Output-File Format). ELF has the benefit that you can isolate each function in a section and remove unused sections to get a smaller build size. COF has the benefit that the debugger works more reliably and you can use the "Locate Headers" tool to find headers you've included not mentioned in the mcp file. If you cannot view local variables, try switching to COF.

* Typedef'd types should have a name anyway. It looks redundant, but the debugger will display the value as an enum instead of an int without that first "my_enum".

  ```C
  typedef my_enum1 {\*...*\} my_enum1; //< Don't do this
  typedef enum my_enum2 {\*...*\} my_enum2; //< Do this instead
  ```

* Similarly, if you have a magic size array, declare the size as a defined value, not a constant. Otherwise, the debugger will show the array as an opaque pointer instead of as an array.

  ```c
  static const SIZE = 10; //< Don't do this
  #define SIZE 10; //< Do this instead
  unsigned int my_array[SIZE];
  ```

### Deployment instructions with MPLAB

Given a released hex file, you can deploy to the robot power board with MPLAB instead of the PICKit3 standalone tool. Note this will erase the bootloader, so make sure to flash the "bootypic.hex" hex file after development is done.

1. File -> Import... -> (choose hex file)
2. Programmer -> Select Programmer -> PICKit 3
3. (Only needed if the target rover has no battery. If it has a power source, this step will fail) Programmer -> Settings -> Power tab -> Power Target circuit from PICKit3 -> OK
4. Programmer -> Program

## Power Board Firmware Development

The main robot firmware code is the Power Board. This is responsible for communicating with the motors / batteries / fans / serial port.

firmware.mcp = main project file. Open this with MPLab IDE v8, not MPLAB X
