# FreeLWIP-Nios-II

This is a forked project from EngineeringSpirit/FreeLwIP-Nios-II:
https://github.com/EngineeringSpirit/FreeLwIP-Nios-II

### File structure

- freeRTOS_lwIP_src: Contain files from the forked project
- freeRTOS_lwIP_src/FreeRTOS_src: FreeRTOS source files. Currently at v7.2.0
- FPGA: SOPC files for DE2-115
- FPGA/software/FreeLwIP: source files compiled from freeRTOS_lwIP_src. This, along with FreeLWIP_bsp should be added to NIOS II SBT then build and run.
- FPGA/software/lwIP_NIOS_II_Example: source files obtained from [Altera forums](http://www.alteraforum.com/forum/showthread.php?p=93638#post93638). This, along with lwIP_NIOS_II_Example_bsp should be added to NIOS II SBT then build and run.