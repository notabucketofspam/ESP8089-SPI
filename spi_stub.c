/*
 * Copyright (c) 2013 Espressif System.
 *
 *  sdio stub code for allwinner
 *
#include <asm/io.h>
#include <mach/irqs.h>
#include <mach/io.h>
#include <mach/iomux.h>
#include <mach/pmu.h>
#include <linux/gpio.h>
#include <asm/gpio.h>
#include <asm/mach/irq.h>

#include "../drivers/spi/rk29_spim.h"
#include "esp_sif.h"

//#define SPI_FREQ (20000000)                             //  1. 22.5Mhz     2. 45Mhz
#define SPI_FREQ (30000000)                             //  1. 22.5Mhz     2. 45Mhz

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

#endif

//0xE5 ~0xFF  30us totoal 
//

struct spi_device_id esp_spi_id[] = {
        {"esp_spi_0", 0},
        {"esp_spi_1", 1},
        {},
};


#ifdef  REGISTER_SPI_BOARD_INFO
static struct rk29xx_spi_chip spi_test_chip[] = {
{
        //.poll_mode = 1,
        .enable_dma = 1,
},
{
        //.poll_mode = 1,
        .enable_dma = 1,
},

};
static struct spi_board_info esp_board_spi_devices[] = {       
  {
    .modalias  = "esp_spi_0",
    .bus_num = 0,   //0 or 1
    .max_speed_hz  = 18*1000*1000,
    .chip_select   = 0,             
    .mode   = SPI_MODE_3,
    .controller_data = &spi_test_chip[0],
  },
};

void sif_platform_register_board_info(void) {
        spi_register_board_info(esp_board_spi_devices, ARRAY_SIZE(esp_board_spi_devices));
}
#endif  REGISTER_SPI_BOARD_INFO


#define RK30_GPIO0_BASE          RK2928_GPIO0_BASE
#define GPIO_NO                  RK30_PIN0_PA0
#define GPIO_BASE_ADDR           ((unsigned char __iomem *) RK30_GPIO0_BASE)
#define GPIO_INT_MASK_OFFSET     GPIO_INTEN
#define GPIO_INT_STAT_OFFSET     GPIO_PORTS_EOI

int sif_platform_get_irq_no(void)
{
	return gpio_to_irq(GPIO_NO);
}

int sif_platform_is_irq_occur(void)
{      
	return 1;
}

void sif_platform_irq_clear(void)
{
}

void sif_platform_irq_mask(int mask)
{
	if (mask)
		disable_irq_nosync(sif_platform_get_irq_no());
	else
		enable_irq(sif_platform_get_irq_no());
}

int sif_platform_irq_init(void)
{
	int ret;

	printk(KERN_ERR "%s enter\n", __func__);

	if ( (ret = gpio_request(GPIO_NO, "esp_spi_int")) != 0) {
		printk(KERN_ERR "request gpio error\n");
		return ret;
	}

	gpio_direction_input(GPIO_NO);

        sif_platform_irq_clear();
	sif_platform_irq_mask(1);

        udelay(1);

	return 0;
}


void sif_platform_irq_deinit(void)
{
	gpio_free(GPIO_NO);
}


void sif_platform_reset_target(void)
{
        gpio_direction_output(RK30_PIN1_PB3, GPIO_LOW);
        mdelay(200);
        gpio_direction_output(RK30_PIN1_PB3, GPIO_HIGH);
        mdelay(200);
}

void sif_platform_target_poweroff(void)
{
        gpio_direction_output(RK30_PIN1_PB3, GPIO_LOW);

}

void sif_platform_target_poweron(void)
{
        mdelay(200);
        gpio_direction_output(RK30_PIN1_PB3, GPIO_LOW);
        mdelay(200);
        gpio_direction_output(RK30_PIN1_PB3, GPIO_HIGH);
        mdelay(200);
}

void sif_platform_target_speed(int high_speed)
{
}

#ifdef ESP_ACK_INTERRUPT
void sif_platform_ack_interrupt(struct esp_pub *epub)
{
	sif_platform_irq_clear();
}
#endif //ESP_ACK_INTERRUPT


module_init(esp_spi_init);
module_exit(esp_spi_exit);
*/

#define MHz (1000000)

#define SPI_FREQ (20000000)                             //  1. 22.5Mhz     2. 45Mhz
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

#endif

#include "esp_sif.h"
#include "linux/interrupt.h"
#include "linux/spi/spi.h"
#include <linux/init.h>

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

struct spi_device_id esp_spi_id[] = { 
  {"esp_spi_0", 0},
  {"esp_spi_1", 1},
  {},
};

static int esp_cs0_pin = 0;
module_param(esp_cs0_pin, int, 0);
MODULE_PARM_DESC(esp_cs0_pin, "SPI chip select GPIO number");

#ifdef REGISTER_SPI_BOARD_INFO

#define MAX_SPEED_HZ (80*MHz)

static struct spi_master *master;
static struct spi_device *spi;

static struct spi_board_info spi_device_info = {
  .modalias = "esp8089-spi",
  .max_speed_hz = MAX_SPEED_HZ,
  .bus_num = 1,
  .chip_select = 0,
  .mode = 2, /* <-- Magic numbers, yay */
}; /* https://www.raspberrypi.org/forums/viewtopic.php?t=245999 */

struct spi_device* sif_platform_register_board_info(void) {

  master = spi_busnum_to_master( spi_device_info.bus_num );
  if( !master ) {
      printk("esp8089_spi: FAILED to find master.\n");
  }
  
  spi = spi_new_device( master, &spi_device_info );
  if( !spi ) {
      printk("esp8089_spi: FAILED to create slave.\n");
    }
  spi->cs_gpio = esp_cs0_pin;

  return spi;
}
#endif

int sif_platform_irq_init(void) { 
  int ret;

	printk(KERN_ERR "esp8089_spi: %s enter\n", __func__);

	if ( (ret = gpio_request(esp_ack_int, "esp_spi_int")) != 0) {
		printk(KERN_ERR "esp8089_spi: request gpio error\n");
		return ret;
	}
	gpio_direction_input(esp_ack_int);

        sif_platform_irq_clear();
	sif_platform_irq_mask(1);

        udelay(1);

	return 0;
}

void sif_platform_irq_deinit(void) {
	gpio_free(esp_ack_int);
}

int sif_platform_get_irq_no(void) { 
	return gpio_to_irq(esp_ack_int);
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

static int esp_reset_gpio = 0;
module_param(esp_reset_gpio, int, 0);
MODULE_PARM_DESC(esp_reset_gpio, "ESP8089 CH_PD GPIO number");

void sif_platform_reset_target(void) {
  gpio_request(esp_cs0_pin, "esp_cs0_pin");
  gpio_request(esp_reset_gpio, "esp_reset_gpio");
  gpio_direction_output(esp_cs0_pin, 1);
  gpio_direction_output(esp_reset_gpio, 0);
  mdelay(200);
  gpio_direction_output(esp_reset_gpio, 1);
  mdelay(200);
  gpio_direction_output(esp_cs0_pin, 0);
  gpio_free(esp_reset_gpio);
  gpio_free(esp_cs0_pin);
}

void sif_platform_target_poweroff(void) {
  gpio_direction_output(esp_reset_gpio, 0);
}

void sif_platform_target_poweron(void) {
  gpio_request(esp_cs0_pin, "esp_cs0_pin");
  gpio_request(esp_reset_gpio, "esp_reset_gpio");
  gpio_direction_output(esp_cs0_pin, 1);
  mdelay(200);
  gpio_direction_output(esp_reset_gpio, 0);
  mdelay(200);
  gpio_direction_output(esp_reset_gpio, 1);
  mdelay(200);
  gpio_direction_output(esp_cs0_pin, 0);
  gpio_free(esp_reset_gpio);
  gpio_free(esp_cs0_pin);
}

#ifdef ESP_ACK_INTERRUPT
void sif_platform_ack_interrupt(struct esp_pub *epub) {
	sif_platform_irq_clear();
}
#endif

//module_init(esp_spi_init);
late_initcall(esp_spi_init);
module_exit(esp_spi_exit);

