# ESP8089-SPI

Linux kernel module to use ESP8089 / ESP8266 over SPI

Intended for use with Raspberry Pi. Make sure that kernel headers are installed,
along with all other requisite tools (make, gcc, etc.).

## Hardware

It is recommended to use an ESP-201 module or a barebones ESP-12F module. A 
development board such as the NodeMCU can cause unpredictable behavior.

Table 1-3. Pin Definitions of HSPI (Slave)

| Pin Name | Pin Num | IO   | Function Name |
| -------- | ------- | ---- | ------------- |
| MTMS     | 9       | IO14 | HSPICLK       |
| MTDI     | 10      | IO12 | HSPIQ/MISO    |
| MTCK     | 12      | IO13 | HSPID/MOSI    |
| MTDO     | 13      | IO15 | HPSICS        |

What pins go where:

| Raspberry Pi | ESP8089     | Function         |
| ------------ | ----------- | ---------------- |
| BCM 13       | CH\_PD / EN | esp\_reset\_gpio |
| BCM 16       | GPIO15      | esp\_cs0\_pin    |
| BCM 19       | GPIO12      | MISO             |
| BCM 20       | GPIO14      | MOSI             |
| BCM 21       | GPIO14      | SCLK             |
| BCM 26       | GPIO15      | esp\_mtdo\_gpio  |

\(Yes, you do need two different pins connected to GPIO15\)

It may be advisable to add a 33 Ohm resistor across each pin.

## Software

`sudo apt-get update`

`sudo apt-get upgrade`

`sudo apt-get install raspberrypi-kernel-headers make gcc`

`mkdir ~/esp`

`cd ~/esp`

`git clone https://github.com/notabucketofspam/ESP8089-SPI.git`

`cd ~/esp8089-spi`

`sudo make install`

`sudo echo "options esp8089-spi esp_reset_gpio=13 esp_mtdo_gpio=26 esp_cs0_pin=16 esp_spi_bus=1" > /etc/modprobe.d/esp.conf`

`sudo echo " modules-load=esp8089-spi" >> /boot/cmdline.txt`

`sudo echo "dtparam=spi=on" >> /boot/config.txt`

`sudo echo "dtoverlay=spi1-1cs,cs0_pin=16,cs0_spidev=disabled" >> /boot/config.txt`

`sudo reboot`

## References

[https://hackaday.io/project/8678-rpi-wifi](https://hackaday.io/project/8678-rpi-wifi)

[https://github.com/al177/esp8089](https://github.com/al177/esp8089)

[https://github.com/george-hopkins/esp8089-spi](https://github.com/george-hopkins/esp8089-spi)

[https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf](https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf)

[https://static.abstore.pl/design/accounts/soyter/img/dokumentacje/esp8089-driver-release-desc_v1-9-2_english.pdf](https://static.abstore.pl/design/accounts/soyter/img/dokumentacje/esp8089-driver-release-desc_v1-9-2_english.pdf)

[https://pinout.xyz/pinout/spi](https://pinout.xyz/pinout/spi)

