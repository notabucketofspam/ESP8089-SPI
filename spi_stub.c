
#define MHz (1000000)

/* https://www.signal.com.tr/pdf/cat/8n-esp8266_spi_reference_en_v1.0.pdf */

#define SPI_FREQ (10000000)
//#define SPI_FREQ (20000000)                             //  1. 22.5Mhz     2. 45Mhz
//#define SPI_FREQ (30000000)                             //  1. 22.5Mhz     2. 45Mhz

//Below are for spi HZ 22.5M
#if (SPI_FREQ == 30000000)

#define CMD_RESP_SIZE   (10) //(50)    //Common respon wait time
#define DATA_RESP_SIZE_W   (142+45) // (1024*13)//   (1024*16)  //(398+400) // (1024*10)    //Only for Write bytes function, data write response.  max:(361+109) 
#define DATA_RESP_SIZE_R   (231+75) //  (340+102)  //(231+75)//(340+102)   //Only for Read bytes function, data write response    max:(340+102) 

#define BLOCK_W_DATA_RESP_SIZE_EACH          (10)           //For each data write resp size, in block write 
#define BLOCK_W_DATA_RESP_SIZE_FINAL (152) // (142+52)   //For final data write resp size, in block write ,max: 119

#define BLOCK_R_DATA_RESP_SIZE_1ST   (265)  // (231+75)    //For each data read resp size, in block read  ,max: 134
#define BLOCK_R_DATA_RESP_SIZE_EACH    (10)  // (20)   //For each data read resp size, in block read 

#elif(SPI_FREQ == 20000000)

#define CMD_RESP_SIZE (10)    //Common respon wait time
#define DATA_RESP_SIZE_W (103+40)    //Only for Write bytes function, data write response.  max: 103
#define DATA_RESP_SIZE_R (118+40)    //Only for Read bytes function, data write response  max: 118
//w: oxFF : 218 clock.     oxFE : 214 clock.  

#define BLOCK_W_DATA_RESP_SIZE_EACH          (20)           //For each data write resp size, in block write 
#define BLOCK_W_DATA_RESP_SIZE_FINAL     (112+40)    //For final data write resp size, in block write ,max :112

#define BLOCK_R_DATA_RESP_SIZE_1ST          (123+40)   //For each data read resp size, in block read ,max: 123
#define BLOCK_R_DATA_RESP_SIZE_EACH       (20)   //For each data read resp size, in block read 
//0xE5 ~0xFF  30us totoal 

#elif (SPI_FREQ == 10000000)

#define CMD_RESP_SIZE 10
#define DATA_RESP_SIZE_W 99
#define DATA_RESP_SIZE_R 10

#define BLOCK_W_DATA_RESP_SIZE_EACH  30
#define BLOCK_W_DATA_RESP_SIZE_FINAL 152

#define BLOCK_R_DATA_RESP_SIZE_1ST   61
#define BLOCK_R_DATA_RESP_SIZE_EACH  30

#else /* Per 1*MHz */

#define CMD_RESP_SIZE                 (0*(SPI_FREQ/1000000)+10)
#define DATA_RESP_SIZE_W              (4.4*(SPI_FREQ/1000000)+55)
#define DATA_RESP_SIZE_R              (14.8*(SPI_FREQ/1000000)-138)

#define BLOCK_W_DATA_RESP_SIZE_EACH   (-1*(SPI_FREQ/1000000)+40)
#define BLOCK_W_DATA_RESP_SIZE_FINAL  (0*(SPI_FREQ/1000000)+152)

#define BLOCK_R_DATA_RESP_SIZE_1ST    (10.2*(SPI_FREQ/1000000)-41)
#define BLOCK_R_DATA_RESP_SIZE_EACH   (-1*(SPI_FREQ/1000000)+40)

#endif

/*
x per 10*MHz

CMD_RESP_SIZE                 0x+10
DATA_RESP_SIZE_W              44x+55
DATA_RESP_SIZE_R              148x-138
                              
BLOCK_W_DATA_RESP_SIZE_EACH   -10x+40
BLOCK_W_DATA_RESP_SIZE_FINAL  0x+152

BLOCK_R_DATA_RESP_SIZE_1ST    102x-41
BLOCK_R_DATA_RESP_SIZE_EACH   -10x+40
*/

#include "esp_sif.h"
#include "linux/interrupt.h"
#include "linux/spi/spi.h"
#include <linux/init.h>

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

/* *** *** Board info *** *** */

struct spi_device_id esp_spi_id[] = { 
  {"ESP8089_0", 0},
  {"ESP8089_1", 1},
  {"ESP8089_2", 2},
};

static int esp_cs2_pin = 16;
module_param(esp_cs2_pin, int, 0);
MODULE_PARM_DESC(esp_cs2_pin, "SPI chip select two");

#ifdef REGISTER_SPI_BOARD_INFO

#define MAX_SPEED_HZ (20*MHz)

static struct spi_master *master;
static struct spi_device *spi;

static struct spi_board_info spi_device_info = {
  .modalias = "eagle",
  .max_speed_hz = MAX_SPEED_HZ,
  .bus_num = 1,
  .chip_select = 2,
  .mode = 0,
};
/* https://www.raspberrypi.org/forums/viewtopic.php?t=245999 */

struct spi_device* sif_platform_register_board_info(void) {

  master = spi_busnum_to_master( spi_device_info.bus_num );
  if( !master )
    printk("esp8089_spi: FAILED to find master\n");
  
  spi = spi_new_device( master, &spi_device_info );
  if( !spi )
    printk("esp8089_spi: FAILED to create slave\n");

  return spi;
}
#endif

/* *** *** Interrupt *** *** */

static int esp_interrupt = 26;
module_param(esp_interrupt, int, 0);
MODULE_PARM_DESC(esp_interrupt, "Interrupt pin");

int sif_platform_irq_init(void) { 
  int ret;

	printk(KERN_ERR "esp8089_spi: %s enter\n", __func__);

	if ( (ret = gpio_request(esp_interrupt, "esp_interrupt")) != 0) {
		printk(KERN_ERR "esp8089_spi: request gpio error\n");
		return ret;
	}
	gpio_direction_input(esp_interrupt);

  sif_platform_irq_clear();
	sif_platform_irq_mask(1);

  udelay(1);

	return 0;
}

void sif_platform_irq_deinit(void) {
	gpio_free(esp_interrupt);
}

int sif_platform_get_irq_no(void) { 
	return gpio_to_irq(esp_interrupt);
} 

int sif_platform_is_irq_occur(void) { 
  return 1;
}

void sif_platform_irq_clear(void) {

}

void sif_platform_irq_mask(int mask) {
	if (mask)
		disable_irq_nosync(sif_platform_get_irq_no());
	else
		enable_irq(sif_platform_get_irq_no());
}

void sif_platform_target_speed(int high_speed) {

}

#ifdef ESP_ACK_INTERRUPT
void sif_platform_ack_interrupt(struct esp_pub *epub) {
	sif_platform_irq_clear();
}
#endif

/* *** *** Platform power *** *** */

/* 
HSPI:
  GPIO12  HMISO
  GPIO13  HMOSI
  GPIO14  HSCLK
  GPIO15  HCS

SPI:
  GPIO6   SCLK
  GPIO7   MISO
  GPIO8   MOSI
  GPIO11  CS

SDIO:
  GPIO6   SDCLK
  GPIO7   SDD0
  GPIO8   SDD1
  GPIO9   SDD2
  GPIO10  SDD3
  GPIO11  SDCMD

https://lastminuteengineers.com/esp8266-nodemcu-arduino-tutorial/ 
*/

//#define USE_HSPI

static int esp_reset_gpio = 13;
module_param(esp_reset_gpio, int, 0);
MODULE_PARM_DESC(esp_reset_gpio, "ESP8089 CH_PD GPIO number");

void sif_platform_reset_target(void) {
#ifdef USE_HSPI
  gpio_request(esp_cs2_pin, "esp_cs2_pin");
  gpio_direction_output(esp_cs2_pin, 1);
#endif

  gpio_request(esp_reset_gpio, "esp_reset_gpio");
  gpio_direction_output(esp_reset_gpio, 0);
  mdelay(200);
  gpio_direction_output(esp_reset_gpio, 1);
  mdelay(200);
  gpio_free(esp_reset_gpio);

#ifdef USE_HSPI
  gpio_direction_output(esp_cs2_pin, 0);
  gpio_free(esp_cs2_pin);
#endif
}

void sif_platform_target_poweroff(void) {
  gpio_direction_output(esp_reset_gpio, 0);
}

void sif_platform_target_poweron(void) {
#ifdef USE_HSPI
  gpio_request(esp_cs2_pin, "esp_cs2_pin");
  gpio_direction_output(esp_cs2_pin, 1);
#endif

  gpio_request(esp_reset_gpio, "esp_reset_gpio");
  mdelay(200);
  gpio_direction_output(esp_reset_gpio, 0);
  mdelay(200);
  gpio_direction_output(esp_reset_gpio, 1);
  mdelay(200);
  gpio_free(esp_reset_gpio);

#ifdef USE_HSPI
  gpio_direction_output(esp_cs2_pin, 0);
  gpio_free(esp_cs2_pin);
#endif
}

//module_init(esp_spi_init);
late_initcall(esp_spi_init);
module_exit(esp_spi_exit);

