# FreeLWIP-Nios-II

This is a forked project from EngineeringSpirit/FreeLwIP-Nios-II:
https://github.com/EngineeringSpirit/FreeLwIP-Nios-II

### File structure

- `freeRTOS_lwIP_src`: Contain files from the forked project

- `freeRTOS_lwIP_src/FreeRTOS_src`: FreeRTOS source files. Currently at v7.2.0

- `FPGA`: SOPC files for DE2-115

- `FPGA/software`: source files for various projects. Pick projects from this file and add to NIOS II SBT along with its corresponding BSP file.

- `FPGA/software/FreeLwIP`: lwIP running on top of FreeRTOS. Source files compiled from `freeRTOS_lwIP_src`. 

- `FPGA/software/lwIP_NIOS_II_Example`: lwIP running on bare metal (no OS). Source files obtained from [Altera forums](http://www.alteraforum.com/forum/showthread.php?p=93638#post93638). 

- `FPGA/software/hello_world`: Hello world sample program for NIOS II.

- `FPGA/software/sss`: Simple Socket Server(SSS) sample program for NIOS II. `lwIP_NIOS_II_example` should run if SSS can run without errors.
