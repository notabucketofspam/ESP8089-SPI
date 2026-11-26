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

| Raspberry Pi    | ESP8266 / ESP8089      | Function         |
| --------------- | ---------------------- | ---------------- |
| BCM 13 / Pin 33 | CHIP_EN                | esp\_reset\_gpio |
| BCM 16 / Pin 36 | GPIO10 / SDIO\_DATA\_3 | esp\_cs0\_pin    |
| BCM 19 / Pin 35 | GPIO7 / SDIO\_DATA\_0  | MISO             |
| BCM 20 / Pin 38 | GPIO11 / SDIO\_CMD     | MOSI             |
| BCM 21 / Pin 40 | GPIO6 / SDIO\_CLK      | SCLK             |
| BCM 26 / Pin 37 | GPIO8 / SDIO\_DATA\_1  | esp\_interrupt   |
| 3.3V            | GPIO15 / MTDO          | boot select      |
| 3.3V            | GPIO0                  | boot select      |
| GND             | GPIO2                  | boot select      |
| GND             | GND                    | Common ground    |

It may be advisable to add a resistor across each I/O pin (33 ~ 330 Ohm).

**Note**: a Raspberry Pi cannot power the ESP chip solely via its onboard
3.3V pins; an external 3.3V power source is required (a linear regulator with
a 10uF capacitor or two is probably adequate).

#### Regarding the crystal oscillator

Most ESP8266 / ESP8089 boards use a **26 MHz** crystal oscillator, but there's a
handful that use **40 MHz** instead.

If your ESP board uses 40 MHz, compile with the `XTAL=40` flag.

## Software

Start with a fresh install of Raspberry Pi OS (tested with kernel `6.18.60+rpt-rpi-v6`).

#### Step one: prerequisites

```
sudo apt-get -y update
sudo apt-get -y upgrade
sudo apt-get -y dist-upgrade
```

Reboot before continuing.

```
sudo apt-get -y install linux-headers-rpi-v6 dkms git build-essential
```

#### Step two: build / install

```
cd ~
git clone https://github.com/notabucketofspam/ESP8089-SPI.git
cd ~/ESP8089-SPI

# if your ESP uses 40 MHz, run the line below before you compile:
# sed -i 's|^MAKE\[0\]=.*|MAKE[0]="make KVERS_UNAME=$kernelver XTAL=40"|' dkms.conf

# compile and install
sudo make dkms
```

#### Step three: configure

```
# set the wifi regulatory country code (replace US with your 2-letter code)
sudo raspi-config nonint do_wifi_country US

# set the config options
echo "options esp8089-spi esp_reset_gpio=13 esp_cs0_pin=16 esp_interrupt=26" | sudo tee /etc/modprobe.d/esp8089.conf
echo "dtoverlay=spi1-1cs,cs0_pin=16,cs0_spidev=disabled" | sudo tee -a /boot/firmware/config.txt

# load the module on boot
sudo tee /etc/systemd/system/esp8089-load.service > /dev/null <<'EOF'
[Unit]
Description=Load ESP8089 SPI Wi-Fi kernel module
After=systemd-udev-settle.service

[Service]
Type=exec
ExecStart=/sbin/modprobe esp8089-spi

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl daemon-reload
sudo systemctl enable esp8089-load.service
```

... and then `reboot`.

```
# actually enable the wifi
sudo rfkill unblock wifi
sudo nmcli radio wifi on
```

### Some extra steps

Scan nearby networks:
```
sudo iw dev wlan0 scan | grep -E "SSID:|signal:"
```

Connect to a network:
```
sudo nmcli dev wifi connect "THE_SSID" password "THE_PASSWORD" ifname wlan0
```

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

