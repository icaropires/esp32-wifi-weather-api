# esp32 wifi weather api

esp32 exercise:

* Connecting to Wi-Fi
* Getting localization through [IP stack](https://ipstack.com/)
* Get weather information for the acquired localization through [Open Weather](https://openweathermap.org/)
* LED feedback for some conditions

Complete description [here](https://gitlab.com/fse_fga/projetos/projeto-3).

## Building and Flashing

_Considering the board is plugged and accessible, and esp-idf environment working_

1. Setup the wifi credentials on menu "Setup Wi-Fi" in `idf.py menuconfig`

2. Setup your credentials to the APIs on "APIs credentials" in `idf.py menuconfig`

3. Build and run the source code:

The code can be built and flashed to [ESP32-DevKitC](https://www.espressif.com/en/products/devkits/esp32-devkitc/overview) with:

``` bash
$ idf.py flash
```
