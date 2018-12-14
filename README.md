# Light_Distance_Pression_Gyro-sensors
This project contains an IHM using QT to receive data with CAN bus from light, distance, pression and Gyro sensors

Used technologies : Eclipse, QT, C, CAN Bus and Linux

To connect to the sensors using USB (Carte Nucleo-F103):  openocd -s /local/sdk_elec/openocd/share/openocd/scripts -f board/st_nucleo_f103rb.cfg -c init 
To read the USB output in terminal : picocom -b 115200 -r -l /dev/ttyACM0


To read the CAN Bus Output in a terminal : receivetest -f=/dev/pcanusb32

Open C workspaces on Eclipse and CANIHM on QT.

Connect the sensors between them using CAN bus and just one with USB to the computer.

PS: It is important to flash the sensors before executing the code :  openocd -s /local/sdk_elec/openocd/share/openocd/scripts -f board/st_nucleo_f103rb.cfg -f flash_board.cfg -c "myFlash template.elf"
