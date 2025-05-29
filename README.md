[![EN](https://user-images.githubusercontent.com/9499881/33184537-7be87e86-d096-11e7-89bb-f3286f752bc6.png)](https://github.com/r57zone/TheLastHostESP32/) 
[![RU](https://user-images.githubusercontent.com/9499881/27683795-5b0fbac6-5cd8-11e7-929c-057833e01fb1.png)](https://github.com/r57zone/TheLastHostESP32/blob/master/README.RU.md)
← Choose language | Выберите язык

# The Last Host (ESP32 C3 Mini + SD Adapter)
Wi-Fi access point with a static site web server, http/https page emulation, and file operations on the SD card. Host static websites, emulate basic authentication, various services, and simple APIs, and share files across different devices within a dedicated Wi-Fi network.

## Features
* Wi-Fi access point mode; SSID and password can be changed in the file `Setup.ini` on the SD card;
* Static site web server with support for any domains. Domains and folders are listed in the file `Hosts.txt`. To open the domain as a website rather than a folder of files, place the `index.html` file in the root directory;
* Emulation of individual http/https web pages, sites. Specify in the file `Emulation.txt` the list of pages, URLs, http status codes, response types, and the response itself as a file or string. To get the required values and addresses, use [Wireshark](https://www.wireshark.org/) on Windows and [PCAPdroid](https://github.com/emanuele-f/PCAPdroid) on Android (with Wireshark). Don’t forget to share popular emulations;
* File management on the SD card: create folders, upload files, and delete them;
* Logging device requests to the file Logs.txt. To enable it, set the parameter `logs` to `1` in the `Setup.ini` file;

## Plans
* Connection to external Wi-Fi networks;
* Security certificates for HTTPS on the SD card;
* Fixing bugs with names (probably URL encoding issues);

## Wiring Diagram
![](https://github.com/user-attachments/assets/8c268a58-0b21-450a-8693-86e3dcd92cc3)

## Firmware
1. Install [Arduino IDE](https://www.arduino.cc/en/software).
2. Go to `Tools` → `Manage Libraries...` and install the `esp32 library by Espressif System`, **version 2.0.6** (this older version is important for HTTPS support). If a newer version is installed, remove it and install the old one.
3. Extract the [ESP32 HTTPS Server](https://github.com/stooged/esp32_https_server) into the folder: `%userprofile%\AppData\Local\Arduino15\libraries`.
4. Go to `Tools` → `Board` → `esp32` → `ESP32C3 Dev Module` and select the board's `COM port`.
5. Flash the [sketch](https://github.com/r57zone/TheLastHostESP32/archive/refs/heads/master.zip).
6. Format the SD card to FAT32 using [Rufus](https://github.com/pbatard/rufus/releases/) or another utility, and extract the contents of the `SDCard` folder to SD card.
7. Connect to the Wi-Fi network `TheLastHost` with the password `12345678` and go to `http://i.me` or `http://192.168.4.1`.  
8. Edit the configuration files: `Setup.ini`, `Hosts.txt`, `Emulation.txt`.  

## Web interface
![WebUI](https://github.com/user-attachments/assets/29f5a7c1-a3f6-4d6f-bcdb-e06a9b94ddca)

## Feedback
`r57zone[at]gmail.com`