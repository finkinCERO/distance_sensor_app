# Ausgewählte Kapitel Mobiler Anwendungen

1. Connect your nucleo board f103rb to your computer
2. Install Zephyr
3. Copy this project to `~/zephyrproject/zephyr/samples/basic/distance`
4. Copy this `nucleo_f103rb.dts` to `~/zephyrproject/zephyr/boards/arm/nucleof103rb/`
5. start console
```
minicom -D /dev/ttyACM0
```
6. connect infrared distance sensors to D2 and D7
7. build application
```
west build -p auto -b nucleo_f103rb samples/basic/distance
west flash
```
8. see available park spots on console `minicom`
9. put objects in front of sensors to see if available parking spots changes