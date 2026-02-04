#include "DEV_Config.h"
//#include "stm32f1xx_hal_spi.h"
#include "spi.h"
#include "main.h"

//extern SPI_HandleTypeDef hspi1;

void DEV_SPI_WriteByte(UBYTE value)
{
    //HAL_SPI_Transmit(&hspi1, &value, 1, 1000);
	while (!(LPC_SPI0->STAT & SPI_TXRDY));
	LPC_SPI0->TXDAT = value;
}

void DEV_SPI_Write_nByte(UBYTE *value, UDOUBLE len)
{
    //HAL_SPI_Transmit(&hspi1, value, len, 1000);
    for(int i=0; i<len; i++){
    	while (!(LPC_SPI0->STAT & SPI_TXRDY));
    	LPC_SPI0->TXDAT = value[i];
    }
}

int DEV_Module_Init(void)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
	// DEV_Digital_Write(EPD_PWR_PIN, 1);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    return 0;
}

void DEV_Module_Exit(void)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);

    //close 5V
	// DEV_Digital_Write(EPD_PWR_PIN, 0);
    DEV_Digital_Write(EPD_RST_PIN, 0);
}

void DEV_Delay_ms(uint32_t _xms){
	Delay_ms(_xms);
}

void DEV_Digital_Write(uint32_t _pin, uint32_t _value){
    Digital_Write(_pin, _value);
} 

unsigned char DEV_Digital_Read(uint32_t _pin){
return Digital_Read(_pin);
}
