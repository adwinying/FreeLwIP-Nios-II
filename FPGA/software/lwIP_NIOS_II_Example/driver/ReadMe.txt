
This examples includes:

The lwIP 1.3.2 Altera TSE driver for lwIP 1.3.2.  This uses the Altera altera_avalon_tse library unmodified.
An example program.
This has been tested with NIOS II IDE 8.1 SP0.01. Please report any changes or differences for later versions of the NIOS IDE.

To create a NIOS II program utilizing lwIP 1.3.2 and the example
program, you must create an Eclipse project.  Use the "Hello World"
template as it's the easiest to start with.  Here are the complete steps:

1.  Download lwIP 1.3.2: http://nongnu.askapache.com/lwip/lwip-1.3.2.zip
2.  Unzip this into a folder on your PC
3.  Use File -> New -> NIOS II C/C++ Application
4.  Select "Hello World" from the template
5.  Change the name to lwIP_NIOS_II_Example
6.  Select the PTF file and CPU for your hardware configuration
7.  Click Finish
8. Expand folder lwip_NIOS_II_Example and delete Hello_World.c
9. Use Import -> General - File System
10. Click "From Directory" Browse and find the lwip folder from step 2
11. Check the square box for this folder
12. Click "Into Folder" Browse
13. Select lwIP_NIOS_II_Example
14. Click Finish
15. Expand lwIP_NIOS_II_Example
16. Right click test and select Delete
17. Expand lwip/src/core
18. Right click ipv6 and select Delete
19. Use Import -> General - File System
20. Click "From Directory" Browse and find the folder that contains *this* download
21. Check the square box for this folder
22. Click "Into Folder" Browse
23. Select lwIP_NIOS_II_Example
24. Click Finish
25. Right-click project lwIP_NIOS_II_Example and select Properties
26. Select C/C++ Build
27. Select General
28. Add Include Path ${workspace_loc:/altera.components/Nios II Software Packages/altera_iniche/UCOSII/inc}
29. Add Include Path ${workspace_loc:/lwIP_NIOS_II_Example/lwIP/src/include}
30. Add Include Path ${workspace_loc:/lwIP_NIOS_II_Example/lwIP/src/include/ipv4}
31. Select Configuration Release from the pull-down
32. Select Preprocessor
33. Add Undefined Symbols: LWIP_NOASSERT
34. Add Undefined Symbols: LWIP_ERROR
35. Select General
36. Add Include Path ${workspace_loc:/altera.components/Nios II Software Packages/altera_iniche/UCOSII/inc}
37. Add Include Path ${workspace_loc:/lwIP_NIOS_II_Example/lwIP/src/include}
38. Add Include Path ${workspace_loc:/lwIP_NIOS_II_Example/lwIP/src/include/ipv4}
39. Select Configuration Debug from the pull-down
40. Click OK
41. Double click main.c
42. Edit alteraTseNetif.hwaddr[0-5] lines 78-83 with your board's MAC address

Build and run the program.
Something similar to the follow should appear if you have DHCP enabled on the connection:

Running...
INFO    : TSE MAC 0 found at address 0x04001c00
INFO    : PHY Marvell 88E1119 found at PHY address 0x01 of MAC Group[0]
INFO    : PHY[0.0] - Automatically mapped to tse_mac_device[0]
INFO    : PHY[0.0] - Restart Auto-Negotiation, checking PHY link...
INFO    : PHY[0.0] - Auto-Negotiation PASSED
INFO    : PHY[0.0] - Checking link...
INFO    : PHY[0.0] - Link established
INFO    : PHY[0.0] - Speed = 1000, Duplex = Full
Waiting for link...OK
Waiting for DHCP IP address...IP address: 192.168.123.136

Notes:

The Marvel PHY 88E1119 is not supported in the default Altera library.

You must have a System Clock Timer in your System Library.

Change the #defines for USE_DHCP and BUILD_HTTPD as needed.  If
USE_DHCP is 0, STATIC_IP must be defined to a static IP address
which is an IP address on your computer's subnet.

lwIP configuration items in lwipopts.h have been set to be as
close to as shipped as possible.  They are not optimum for memory
usage or bandwidth.  The intent of this example is to provide a
working TSE driver and lwIP example program.

The project should build and run if the instructions above are followed correctly.
Good luck with lwIP and your Altera TSE based project.

Bill
