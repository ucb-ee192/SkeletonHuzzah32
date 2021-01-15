# SkeletonHuzzah32
Basic skeleton code for EE192 Spring 2021

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
In order to write, debug, compile, and flash the Huzzah32, we will be making use of the [PlatformIO plugin](https://platformio.org/platformio-ide) for [Visual Studio Code](https://code.visualstudio.com/). Depending on your choice of operating system, you may also want to download a terminal emulator (such as [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/)) for streamlining the process of establishing serial communication with your microcontroller. While the source code used on our embedded system will be written in C, we will be using Python for several communication-based tasks as well.

# Getting Started!

The first step to getting set up is to get your software installed. We will be using Microsoft's Visual Studio Code as the integrated development environment (IDE) for our embedded system code. You can find [installation links and instructions for MacOS, Windows, and Linux here](https://code.visualstudio.com/).

Once you've installed Code, we can move to the next step of installing the PlatformIO plugin. First, you will need to navigate to the VS Code Package Manager, also found under the Extensions tab on the left of the VS Code window. Once there, search for "platform ide" in the search bar. The first result should be **PlatformIO IDE 2.2.1***. Install this extension. Reload the VS Code window when prompted.

At this point, the PlatformIO plugin has been installed, but we still need to download the proper framework to compile and flash our specific microcontroller. To do this, we will start a new project. In the PlatformIO home screen, select "New Project". Enter a name into the project name field. **Make sure that your project name has no spaces. If you do not use a default location, your file path should not have spaces either.** For board, select from the dropdown menu "Adafruit ESP32 Feather". Finally, select "Espressif IoT Development Framework" for Framework. Select Finish and wait for the framework to be installed.

After the installation finishes, you should now see your project with a default file structure on the left of your VS Code window.
