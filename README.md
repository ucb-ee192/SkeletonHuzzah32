# Branch UDPCommands

### For advanced details see the wiki page:
https://github.com/ucb-ee192/SkeletonHuzzah32/wiki

## Updated skeleton code for EE192 Spring 2021 January 30, 2021

This branch has significant changes compared to the basic Skeleton code on the main branch.

* Remote python client is now log-input-client-udp.python
* Python client gets keyboard input of form COMMAND VALUE, where COMMAND is a string upto 8 char long, and VALUE is an integer (32 bits)
* A single entry queue cmd_queue holds the command and value received in a UDP packet. Command processing needs to be addedto 
added to udp_cmd() in usertask.c for extra commands. 
* a rudimentary error handler is added in usertask.c called car_error_handle(). This would be a good place
to shutdown the car (e.g. set PWM for zero speed), and perhaps alert the user (e.g. by blinking LED) that an errorhas occured.

-----------------------------------------

Using Espressif IoT Development Framework, for PlatformIO plugin for Visual Studio Code.
https://docs.platformio.org/en/latest/integration/ide/vscode.html#quick-start
based in part on examples taken from:
https://github.com/platformio/platform-espressif32/blob/master/examples/

INSERT IMAGE OF ASSEMBLED VEHICLE!!!

The purpose of this repository is to provide skeleton code for implementing embedded real-time control of a NitroRCX RC car using an ESP32-based microcontroller and additional peripherals. Usage of this repository requires compilation and uploading using the PlatformIO plugin for Visual Studio Code.

### Included in this repository:
1) Resources for acquiring and configuring RC car hardware and embedded systems.
2) Utilities for ESP-IDF FreeRTOS task and timer management.
3) Utilities for streaming log and debug data over WiFi and/or UART connections.
4) Basic control structure and PWM control implementations.

## Hardware Requirements
This project makes use of easily-accessible and low-cost 1:16 scale RC cars from NitroRCX as the hardware platform. While it is possible to use other products, the examples and resources in this repository will refer specifically to the following two models:
* Exceed RC Racing Desert Short Course - [Available at NitroRCX](https://www.nitrorcx.com/51c858-16-deaertsct-aagreen-24g.html)
* Exceed RC Magnet EP Electric RTR Off Road Truck - [Available at NitroRCX](https://www.nitrorcx.com/51c853-stripered-24-ghz.html)

In addition to the base RC car hardware (and included components such as the NiMH charger), we will also be making use of the following additional electronic hardware components:
* [Adafruit Huzzah32 Feather ESP32 Microcontroller Board](https://www.adafruit.com/product/3619)
* [USB Rechargeable Battery Cell (Any small alternative will do)](https://www.amazon.com/Poweradd-Slim-Ultra-compact-Portable-External/dp/B00MWU1GGI/ref=sr_1_4?dchild=1&keywords=usb+battery+mini&qid=1608613916&sr=8-4)
* [Assorted Jumper Wires](https://www.amazon.com/KABUDA-Multicolored-Dupont-Breadboard-Arduino/dp/B07QXXMWRZ/)
* [Linescan Camera](https://community.nxp.com/t5/University-Programs-Knowledge/Line-Scan-Camera-Use/ta-p/1105313)
* [Sparkfun Line Sensor Board](https://www.sparkfun.com/products/9454)

INSERT A FEW COMMENTS ABOUT CHASSIS ADAPTER DESIGN AND ACCESS TO THE DESIGN FILES!!!

## Software Requirements
In order to write, debug, compile, and flash the Huzzah32, we will be making use of the [PlatformIO plugin](https://platformio.org/platformio-ide) for [Visual Studio Code](https://code.visualstudio.com/). Depending on your choice of operating system, you may also want to download a terminal emulator (such as [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/)) for streamlining the process of establishing serial communication with your microcontroller. While the source code used on our embedded system will be written in C, we will be using Python for several communication-based tasks as well. Please make sure you have a recent Python3 distribution installed.

# Getting Started!

The first step to getting set up is to get your software installed. We will be using Microsoft's Visual Studio Code as the integrated development environment (IDE) for our embedded system code. You can find [installation links and instructions for MacOS, Windows, and Linux here](https://code.visualstudio.com/).

Once you've installed Code, we can move to the next step of installing the PlatformIO plugin. First, you will need to navigate to the VS Code Package Manager, also found under the Extensions tab on the left of the VS Code window. Once there, search for "platform ide" in the search bar. The first result should be **PlatformIO IDE 2.2.1**. Install this extension. Reload the VS Code window when prompted.

At this point, the PlatformIO plugin has been installed, but we still need to download the proper framework to compile and flash our specific microcontroller. To do this, we will start a new project. In the PlatformIO home screen, select "New Project". Enter a name into the project name field. **Make sure that your project name has no spaces. If you do not use a default location, your file path should not have spaces either.** For board, select from the dropdown menu "Adafruit ESP32 Feather". Finally, select "Espressif IoT Development Framework" for Framework. Select Finish and wait for the framework to be installed.

After the installation finishes, you should now see your project with the default file structure on the left of your VS Code window. From this point, your software environment should be configured properly! Let's test it out with a simple Hello World example. You can go ahead and close the example project you created by going to File->"Close Folder". We won't be using it moving forward.

Navigate to the PlatformIO home page by selecting the PlatformIO tab on the left (looks like an alien head) and selecting Open. From here, press the "Project Examples" option under "Quick Access". This will bring up a new window where you can select from a range of examples from the available frameworks you have installed. Select **"espidf-hello-world"** and then import.

Once this finishes, the example project will open, and you should see the file structure of the project on the left. Let's take a look at the source file for this example by navigating to **src->hello_world_main.c**. If you've coded in C before, the syntax should be familiar to you. The main function of this file "app_main()" will print out some ESP32 chip information to stdio and then restart after 10 seconds.

```c
/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


void app_main()
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

```

Before building this project, we need to make a few edits to get it to work with our hardware. Navigate to the **platformio.ini** file in your project. This file is the configuration file for PlatformIO and tells the plugin what hardware, framework, connection, etc. to prepare. Edit the platformio.ini file to remove the other boards and to add our own. Afterwards, the platformio.ini contents should look like the following:

```c
[env:featheresp32]
platform = espressif32
framework = espidf
board = featheresp32
monitor_speed = 115200
```

Don't forget to use the correct baud rate (115200) later on when connecting to your serial port!

Now, we are ready to build and flash! At the bottom of your VS Code window, you should see a thin blue toolbar with a bunch of icons. Select the checkmark to build. A terminal window will popup and you should see some diagnostic text being printed. This will take a few minutes usually. Once that finishes, use a micro USB cable to connect your Huzzah32 to your computer, and select the right arrow on the toolbar to begin flashing your microcontroller.

Once the uploading process has finished, you should see **[SUCCESS]** in the terminal. Congratulations, you've flashed your first program! Let's see if it works. Open your preferred terminal utility (we will be using PuTTY), and attempt to make a serial connection at 115200 to the port assigned to your microcontroller (COM3 in our case). Depending on your operating system, the name of your serial port will be different.

When we establish a connection, you should see that the Huzzah32 begins printing what we expected!

Now that we have successfully built and flashed our first program using PlatformIO and the ESP-IDF framework, we can move onto the main skeleton code for controlling your vehicle.
