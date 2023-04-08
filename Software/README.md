# PD2Breadboard Software
The RP2040 powered USB PD negotiator is not only a powerful variable power supply, but it also comes with a suite of advanced software features that set it apart from the competition. With power monitoring, self-diagnostics, and advanced algorithms, our software takes your breadboard projects to the next level.


The power monitoring feature allows you to keep an eye on your device's power consumption in real-time, providing detailed insights into your device's power usage. This feature is especially useful for those looking to optimize their device's power usage or for those who want to monitor the power consumption of their projects.


Our self-diagnostics feature ensures that your RP2040 powered USB PD negotiator is always functioning at peak performance. If any issues are detected, the software will provide detailed feedback, making it easy to diagnose and resolve any issues.


In addition to these features, our software is designed with ease of use in mind. The graphical user interface is intuitive and easy to navigate, making it easy to adjust the voltage levels and monitor power usage. And with its lightweight design, you can take it with you anywhere you go.



## Build
### Provided binaries
The provided binaries are located in the [build](build/) folder. The binaries are in the UF2 format, which can be flashed to the Pico using one of the two methods below.

### Programming via script
Dependencies:
* [Python 3](https://www.python.org/downloads/)

To build the project, simply run the `programmer.py` either using python or simply clicking the `programmer.bat` file if you are on Windows. The script will automatically install its own dependencies and transfer the code to the Pico.

### Programming manually
To program the pico there are two manual options

Using a Serial terminal:
* Using Serial to transmit the string "reset" will automatically put it in USB boot mode
* Manually drag and drop the UF2 file into the Pico drive from the build folder
Using the BOOTSEL button:
* Remove power from the Pico
* Press and hold the BOOTSEL button
* Reapply power to the Pico
* Release the BOOTSEL button
* Drag and drop the UF2 file into the Pico drive from the build folder

### Manually build the project
Dependencies:
* [VSCode](https://code.visualstudio.com/download)
* SDK [Windows](https://github.com/raspberrypi/pico-setup-windows/releases/latest/download/pico-setup-windows-x64-standalone.exe) | [Linux](https://github.com/raspberrypi/pico-sdk)

Follow the instructions on the SDK page to install the SDK. After installing the SDK, open the project in VSCode and build the project.

## Known Errors
### Version 1.0.0
* The debounce circuit, is supposed to form an RC circuit, but the pull-up resistor is placed on the wrong side of the current limiting resistor forming a voltage divider rather than an RC circuit. This means that the debounce circuit is not working as intended.
* Shunt resistor is too low value, this means we cannot get readings below 10mA. 

## License
This software is licensed under the MIT License - see the [LICENSE](LICENSE) file for details