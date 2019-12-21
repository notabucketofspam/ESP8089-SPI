# ESP8089-SPI

Linux kernel module to use ESP8089 / ESP8266 over SPI. Intended for use with 
Raspberry Pi Zero.

## Hardware

It is recommended to use an ESP-201 module or a barebones ESP-12F module. A 
development board such as the NodeMCU can cause unpredictable behavior. It may 
be advisable to add a 33 Ohm resistor across each pin, but this is untested at 
the moment.

#### What pins go where:

| Raspberry Pi | ESP8089     | Function         |
| ------------ | ----------- | ---------------- |
| BCM 13       | CH\_PD / EN | esp\_reset\_gpio |
| BCM 16       | GPIO15      | esp\_cs0\_pin    |
| BCM 19       | GPIO12      | MISO             |
| BCM 20       | GPIO14      | MOSI             |
| BCM 21       | GPIO14      | SCLK             |
| BCM 26       | GPIO15      | esp\_mtdo\_gpio  |

\(Yes, you do need two different pins connected to GPIO15.\)

## Software

Start with a fresh install of Raspbian.

#### Step one: prerequisites

`sudo apt-get -y update`

`sudo apt-get -y upgrade`

`sudo apt-get -y install raspberrypi-kernel-headers gcc git make`

#### Step two: install

`mkdir ~/esp`

`cd ~/esp`

`git clone https://github.com/notabucketofspam/ESP8089-SPI.git`

`cd ESP8089-SPI`

`sudo make install`

#### Step three: configure

`sudo echo "options esp8089-spi esp_reset_gpio=13 esp_mtdo_gpio=26 esp_cs0_pin=16" > /etc/modprobe.d/esp.conf`

`sudo chmod 777 /etc/modprobe.d/esp.conf`

`sudo echo "esp8089_spi" >> /etc/modprobe.d/blacklist`

`sudo echo "spi_bcm2835" >> /etc/modules`

`sudo echo "esp8089_spi" >> /etc/modules`

`sudo echo "dtparam=spi=on" >> /boot/config.txt`

`sudo echo "dtoverlay=spi1-1cs,cs0_pin=16,cs0_spidev=disabled" >> /boot/config.txt`

`sudo reboot`

## How it works

Please note: I have but a very loose grasp on the underlying mechanics of the
kernel module at base. 

The ESP8266 is basically just a rehashed ESP8089, which is a wireless chip 
commonly used in many Unix-based devices. The ESP8089 is designed to load its 
firmware over SPI / SDIO when the device in question boots, whereas the ESP8266 
is intended to load firmware off of the integrated SPI flash component. The 
pins used for this operation, though, are exposed on most varieties of the 
chip. These pins can be therefore utilized to load any custom firmware onto an 
ESP8266; in fact, this is what the eagle\_fw\#.h files are.

Upon boot of the host device, the ESP chip is power cycled using the CH\_PD 
pin \(held low\) and subsequently set to load code over SPI via the MTDO pin 
\(held high\). Optionally, GPIO0 and GPIO2 can be connected to BCM 26 to ensure 
the correct boot configuration, but these pins are held high by default by the 
internal pull-up resistors. Note that, since SPI uses an "active low" 
configuration for chip select, these pins would have to be be connected with 
moderately high-value resistors so as not to interfere during normal operation.

## References

[https://hackaday.io/project/8678-rpi-wifi](https://hackaday.io/project/8678-rpi-wifi)

[https://github.com/al177/esp8089](https://github.com/al177/esp8089)

[https://github.com/george-hopkins/esp8089-spi](https://github.com/george-hopkins/esp8089-spi)

[https://static.abstore.pl/design/accounts/soyter/img/dokumentacje/esp8089-driver-release-desc_v1-9-2_english.pdf](https://static.abstore.pl/design/accounts/soyter/img/dokumentacje/esp8089-driver-release-desc_v1-9-2_english.pdf)

[https://pinout.xyz/pinout/spi](https://pinout.xyz/pinout/spi)

[https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf](https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf)

Table 1-3. Pin Definitions of HSPI (Slave)

| Pin Name | Pin Num | IO   | Function Name |
| -------- | ------- | ---- | ------------- |
| MTMS     | 9       | IO14 | HSPICLK       |
| MTDI     | 10      | IO12 | HSPIQ/MISO    |
| MTCK     | 12      | IO13 | HSPID/MOSI    |
| MTDO     | 13      | IO15 | HSPICS        |

