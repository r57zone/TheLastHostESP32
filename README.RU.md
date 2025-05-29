[![EN](https://user-images.githubusercontent.com/9499881/33184537-7be87e86-d096-11e7-89bb-f3286f752bc6.png)](https://github.com/r57zone/TheLastHostESP32/) 
[![RU](https://user-images.githubusercontent.com/9499881/27683795-5b0fbac6-5cd8-11e7-929c-057833e01fb1.png)](https://github.com/r57zone/TheLastHostESP32/blob/master/README.RU.md)

# The Last Host (ESP32 C3 Mini + SD адаптер)
Wi-Fi точка доступа, с веб-сервером статичных сайтов, эмуляцией http/https страниц, а также работой с файлами карты памяти.

## Функциональность
* Режим Wi-Fi точки доступа, название и пароль изменяются в файле `Setup.ini`, на карте памяти;
* Веб-сервер статичных сайтов, с любыми доменами. Указать домены и папки можно в файле `Hosts.txt`. Для открытия домена как сайта, а не как папки с файлами, разместите файл index.html в корне папки;
* Эмуляция отдельных http/https веб-страниц, сайтов. Укажите в файле `Emulation.txt` список страниц, адресов, http код статуса, тип ответа и сам ответ в виде файла или строки. Для получения нужных значений и адресов используйте [Wireshark](https://www.wireshark.org/) на Windows и [PCAPdroid](https://github.com/emanuele-f/PCAPdroid) на Android (вместе с Wireshark). Не забывайте делиться популярными эмуляциями;
* Работа с файлами карты памяти. Создание папок, загрузка файлов и их удаление;
* Логирование запросов устройств в файл `Logs.txt`. Для включения измените параметр `logs` на `1`, в файле `Setup.ini`;

## Планы
* Подключение к внешней Wi-Fi сети;
* Сертификаты безопасности для HTTPS на флешке;
* Исправление багов с названиями (URL кодировки вероятно);

## Схема подключения
![](https://github.com/user-attachments/assets/8c268a58-0b21-450a-8693-86e3dcd92cc3)

## Прошивка
1. Установить [Arduino IDE](https://www.arduino.cc/en/software/).
2. Нажмите `Инструменты` → `Управление библиотеками...` и установите библиотеку `esp32 от Espressif System` **версии 2.0.6** (старая версия важна для работы https). Если у вас установлена более свежая версия, то удалите и установите старую.
3. Распакуйте библиотеку [ESP32 HTTPS Server](https://github.com/stooged/esp32_https_server) в папку `%userprofile%\AppData\Local\Arduino15\libraries`.
4. Выберите `Инструменты` → `Плата` → `esp32` → `ESP32C3 Dev Module` и `COM-порт` платы.
5. Прошейте [скетч](https://github.com/r57zone/TheLastHostESP32/archive/refs/heads/master.zip).
6. Отформатируйте карту памяти в `FAT32`, с помощью [Rufus](https://github.com/pbatard/rufus/releases/) или другой утилиты и распакуйте содержимое папки `SDCard` на карту памяти.
7. Подключитесь к Wi-Fi точке `TheLastHost`, с паролем `12345678` и перейдите на `http://i.me` или `http://192.168.4.1`.
8. Измените файлы конфигурации: `Setup.ini`, `Hosts.txt`, `Emulation.txt`.

## Веб-интерфейс
![WebUI](https://github.com/user-attachments/assets/29f5a7c1-a3f6-4d6f-bcdb-e06a9b94ddca)

## Обратная связь
`r57zone[собака]gmail.com`