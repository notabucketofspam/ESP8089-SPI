# ESP8089-SPI

Linux kernel module to use ESP8089 / ESP8266 over SPI. Intended for use with 
Raspberry Pi Zero.

## Hardware

If using an ESP8089, no changes are required to be made to the chip. If using 
an ESP8266, the SPI flash must first be desoldered. This is the 8-pin SOIC 
component near the 32-pin QFN die. The ESP-201 variation works well for this. 
Alternatively, one can use an off-brand NodeMCU or WeMos D1 Mini, albeit a 
version that doesn't use the ESP-12. 


#### What pins go where:

| Raspberry Pi | ESP8266 / ESP8089      | Function         |
| ------------ | ---------------------- | ---------------- |
| BCM 13       | CHIP_EN                | esp\_reset\_gpio |
| BCM 16       | GPIO10 / SDIO\_DATA\_3 | esp\_cs0\_pin    |
| BCM 19       | GPIO7 / SDIO\_DATA\_0  | MISO             |
| BCM 20       | GPIO11 / SDIO\_CMD     | MOSI             |
| BCM 21       | GPIO6 / SDIO\_CLK      | SCLK             |
| BCM 26       | GPIO8 / SDIO\_DATA\_1  | esp\_interrupt   |
| 3.3V         | GPIO15 / MTDO          | boot select      |
| 3.3V         | GPIO0                  | boot select      |
| GND          | GPIO2                  | boot select      |

It may be advisable to add a resistor across each I/O pin (33 ~ 330 Ohm).

## Software

Start with a fresh install of Raspbian.

#### Step one: prerequisites

`sudo apt-get -y update`

`sudo apt-get -y upgrade`

`sudo apt-get -y dist-upgrade`

`sudo apt-get -y install raspberrypi-kernel-headers gcc git make`

#### Step two: install

`mkdir ~/esp`

`cd ~/esp`

`git clone https://github.com/notabucketofspam/ESP8089-SPI.git`

`cd ESP8089-SPI`

`sudo make install`

#### Step three: configure

`sudo su`

`echo "options esp8089-spi esp_reset_gpio=13 esp_cs0_pin=16 esp_interrupt=26" > /etc/modprobe.d/esp.conf`

`echo "esp8089_spi" >> /etc/modprobe.d/blacklist`

`echo "spi_bcm2835" >> /etc/modules`

`echo "spi_bcm2835aux" >> /etc/modules`

`echo "esp8089_spi" >> /etc/modules`

`echo "dtoverlay=spi1-1cs,cs0_pin=16,cs0_spidev=disabled" >> /boot/config.txt`

`reboot`

## How it works

The ESP8266 is basically just a rehashed ESP8089, which is a wireless chip 
commonly used in many Unix-based devices. The ESP8089 is designed to load its 
firmware over SPI / SDIO when the device in question boots, whereas the ESP8266 
is intended to load firmware off of the integrated SPI flash component. The 
pins used for this operation, though, are exposed on most varieties of the 
chip. These pins can be therefore utilized to load any custom firmware onto an 
ESP8266; in fact, this is what the eagle\_fw\#.h files are. The SPI 
flash must first be removed to do this.

Upon boot of the host device, the ESP chip is power cycled using the CHIP\_EN 
pin \(held low\) and subsequently set to load code over SPI via GPIO15 
\(held high\). During this time, GPIO0 must be held high and GPIO2 must be held 
low to select the correct boot mode.

## References

[https://pinout.xyz/pinout/spi](https://pinout.xyz/pinout/spi)

[https://hackaday.io/project/8678-rpi-wifi](https://hackaday.io/project/8678-rpi-wifi)

[https://github.com/al177/esp8089](https://github.com/al177/esp8089)

[https://github.com/george-hopkins/esp8089-spi](https://github.com/george-hopkins/esp8089-spi)

[https://github.com/linux-rockchip/linux-rockchip/tree/mirror/rk3188-rbox-kk/drivers/net/wireless/esp8089](https://github.com/linux-rockchip/linux-rockchip/tree/mirror/rk3188-rbox-kk/drivers/net/wireless/esp8089)

[https://static.abstore.pl/design/accounts/soyter/img/dokumentacje/esp8089-driver-release-desc_v1-9-2_english.pdf](https://static.abstore.pl/design/accounts/soyter/img/dokumentacje/esp8089-driver-release-desc_v1-9-2_english.pdf)

[https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf](https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf)

[https://www.espressif.com/sites/default/files/documentation/esp8266_hardware_design_guidelines_en.pdf](https://www.espressif.com/sites/default/files/documentation/esp8266_hardware_design_guidelines_en.pdf)

[http://gamma.spb.ru/images/pdf/esp8089_datasheet_en.pdf](http://gamma.spb.ru/images/pdf/esp8089_datasheet_en.pdf)

[https://www.terraelectronica.ru/pdf/show?pdf_file=%252Fds%252Fpdf%252FE%252FEspressif_FAQ_EN.pdf](https://www.terraelectronica.ru/pdf/show?pdf_file=%252Fds%252Fpdf%252FE%252FEspressif_FAQ_EN.pdf)

[https://www.signal.com.tr/pdf/cat/8n-esp8266_spi_reference_en_v1.0.pdf](https://www.signal.com.tr/pdf/cat/8n-esp8266_spi_reference_en_v1.0.pdf)

[https://www.espressif.com/sites/default/files/documentation/ESP8266_Pin_List_0.xls](https://www.espressif.com/sites/default/files/documentation/ESP8266_Pin_List_0.xls)

