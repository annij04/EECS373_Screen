/*
 * ili9488.c
 *
 *  Created on: Dec 14, 2021
 *      Author: timagr615
 */
#include "ili9488.h"

extern SPI_HandleTypeDef hspi1;
//extern UART_HandleTypeDef huart1;

static uint8_t rotationNum=1;
static bool _cp437    = false;
static SPI_HandleTypeDef lcdSPIhandle;
//Chip Select pin
static GPIO_TypeDef  *tftCS_GPIO = GPIOF;
static uint16_t tftCS_PIN = GPIO_PIN_12;
//Data Command pin
static GPIO_TypeDef  *tftDC_GPIO = GPIOD;
static uint16_t tftDC_PIN = GPIO_PIN_14;
//Reset pin
static GPIO_TypeDef  *tftRESET_GPIO = GPIOB;
static uint16_t tftRESET_PIN = GPIO_PIN_15;
//uint8_t frm_buf[65536] = {0};
uint16_t width;
uint16_t height;

static //Text simple font array (You can your own font)
const unsigned char font1[] = {
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
	0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
	0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
	0x18, 0x3C, 0x7E, 0x3C, 0x18,
	0x1C, 0x57, 0x7D, 0x57, 0x1C,
	0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
	0x00, 0x18, 0x3C, 0x18, 0x00,
	0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
	0x00, 0x18, 0x24, 0x18, 0x00,
	0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
	0x30, 0x48, 0x3A, 0x06, 0x0E,
	0x26, 0x29, 0x79, 0x29, 0x26,
	0x40, 0x7F, 0x05, 0x05, 0x07,
	0x40, 0x7F, 0x05, 0x25, 0x3F,
	0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
	0x7F, 0x3E, 0x1C, 0x1C, 0x08,
	0x08, 0x1C, 0x1C, 0x3E, 0x7F,
	0x14, 0x22, 0x7F, 0x22, 0x14,
	0x5F, 0x5F, 0x00, 0x5F, 0x5F,
	0x06, 0x09, 0x7F, 0x01, 0x7F,
	0x00, 0x66, 0x89, 0x95, 0x6A,
	0x60, 0x60, 0x60, 0x60, 0x60,
	0x94, 0xA2, 0xFF, 0xA2, 0x94,
	0x08, 0x04, 0x7E, 0x04, 0x08,
	0x10, 0x20, 0x7E, 0x20, 0x10,
	0x08, 0x08, 0x2A, 0x1C, 0x08,
	0x08, 0x1C, 0x2A, 0x08, 0x08,
	0x1E, 0x10, 0x10, 0x10, 0x10,
	0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
	0x30, 0x38, 0x3E, 0x38, 0x30,
	0x06, 0x0E, 0x3E, 0x0E, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x5F, 0x00, 0x00,
	0x00, 0x07, 0x00, 0x07, 0x00,
	0x14, 0x7F, 0x14, 0x7F, 0x14,
	0x24, 0x2A, 0x7F, 0x2A, 0x12,
	0x23, 0x13, 0x08, 0x64, 0x62,
	0x36, 0x49, 0x56, 0x20, 0x50,
	0x00, 0x08, 0x07, 0x03, 0x00,
	0x00, 0x1C, 0x22, 0x41, 0x00,
	0x00, 0x41, 0x22, 0x1C, 0x00,
	0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
	0x08, 0x08, 0x3E, 0x08, 0x08,
	0x00, 0x80, 0x70, 0x30, 0x00,
	0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x60, 0x60, 0x00,
	0x20, 0x10, 0x08, 0x04, 0x02,
	0x3E, 0x51, 0x49, 0x45, 0x3E,
	0x00, 0x42, 0x7F, 0x40, 0x00,
	0x72, 0x49, 0x49, 0x49, 0x46,
	0x21, 0x41, 0x49, 0x4D, 0x33,
	0x18, 0x14, 0x12, 0x7F, 0x10,
	0x27, 0x45, 0x45, 0x45, 0x39,
	0x3C, 0x4A, 0x49, 0x49, 0x31,
	0x41, 0x21, 0x11, 0x09, 0x07,
	0x36, 0x49, 0x49, 0x49, 0x36,
	0x46, 0x49, 0x49, 0x29, 0x1E,
	0x00, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x40, 0x34, 0x00, 0x00,
	0x00, 0x08, 0x14, 0x22, 0x41,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x00, 0x41, 0x22, 0x14, 0x08,
	0x02, 0x01, 0x59, 0x09, 0x06,
	0x3E, 0x41, 0x5D, 0x59, 0x4E,
	0x7C, 0x12, 0x11, 0x12, 0x7C,
	0x7F, 0x49, 0x49, 0x49, 0x36,
	0x3E, 0x41, 0x41, 0x41, 0x22,
	0x7F, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x49, 0x49, 0x49, 0x41,
	0x7F, 0x09, 0x09, 0x09, 0x01,
	0x3E, 0x41, 0x41, 0x51, 0x73,
	0x7F, 0x08, 0x08, 0x08, 0x7F,
	0x00, 0x41, 0x7F, 0x41, 0x00,
	0x20, 0x40, 0x41, 0x3F, 0x01,
	0x7F, 0x08, 0x14, 0x22, 0x41,
	0x7F, 0x40, 0x40, 0x40, 0x40,
	0x7F, 0x02, 0x1C, 0x02, 0x7F,
	0x7F, 0x04, 0x08, 0x10, 0x7F,
	0x3E, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x09, 0x09, 0x09, 0x06,
	0x3E, 0x41, 0x51, 0x21, 0x5E,
	0x7F, 0x09, 0x19, 0x29, 0x46,
	0x26, 0x49, 0x49, 0x49, 0x32,
	0x03, 0x01, 0x7F, 0x01, 0x03,
	0x3F, 0x40, 0x40, 0x40, 0x3F,
	0x1F, 0x20, 0x40, 0x20, 0x1F,
	0x3F, 0x40, 0x38, 0x40, 0x3F,
	0x63, 0x14, 0x08, 0x14, 0x63,
	0x03, 0x04, 0x78, 0x04, 0x03,
	0x61, 0x59, 0x49, 0x4D, 0x43,
	0x00, 0x7F, 0x41, 0x41, 0x41,
	0x02, 0x04, 0x08, 0x10, 0x20,
	0x00, 0x41, 0x41, 0x41, 0x7F,
	0x04, 0x02, 0x01, 0x02, 0x04,
	0x40, 0x40, 0x40, 0x40, 0x40,
	0x00, 0x03, 0x07, 0x08, 0x00,
	0x20, 0x54, 0x54, 0x78, 0x40,
	0x7F, 0x28, 0x44, 0x44, 0x38,
	0x38, 0x44, 0x44, 0x44, 0x28,
	0x38, 0x44, 0x44, 0x28, 0x7F,
	0x38, 0x54, 0x54, 0x54, 0x18,
	0x00, 0x08, 0x7E, 0x09, 0x02,
	0x18, 0xA4, 0xA4, 0x9C, 0x78,
	0x7F, 0x08, 0x04, 0x04, 0x78,
	0x00, 0x44, 0x7D, 0x40, 0x00,
	0x20, 0x40, 0x40, 0x3D, 0x00,
	0x7F, 0x10, 0x28, 0x44, 0x00,
	0x00, 0x41, 0x7F, 0x40, 0x00,
	0x7C, 0x04, 0x78, 0x04, 0x78,
	0x7C, 0x08, 0x04, 0x04, 0x78,
	0x38, 0x44, 0x44, 0x44, 0x38,
	0xFC, 0x18, 0x24, 0x24, 0x18,
	0x18, 0x24, 0x24, 0x18, 0xFC,
	0x7C, 0x08, 0x04, 0x04, 0x08,
	0x48, 0x54, 0x54, 0x54, 0x24,
	0x04, 0x04, 0x3F, 0x44, 0x24,
	0x3C, 0x40, 0x40, 0x20, 0x7C,
	0x1C, 0x20, 0x40, 0x20, 0x1C,
	0x3C, 0x40, 0x30, 0x40, 0x3C,
	0x44, 0x28, 0x10, 0x28, 0x44,
	0x4C, 0x90, 0x90, 0x90, 0x7C,
	0x44, 0x64, 0x54, 0x4C, 0x44,
	0x00, 0x08, 0x36, 0x41, 0x00,
	0x00, 0x00, 0x77, 0x00, 0x00,
	0x00, 0x41, 0x36, 0x08, 0x00,
	0x02, 0x01, 0x02, 0x04, 0x02,
	0x3C, 0x26, 0x23, 0x26, 0x3C,
	0x1E, 0xA1, 0xA1, 0x61, 0x12,
	0x3A, 0x40, 0x40, 0x20, 0x7A,
	0x38, 0x54, 0x54, 0x55, 0x59,
	0x21, 0x55, 0x55, 0x79, 0x41,
	0x22, 0x54, 0x54, 0x78, 0x42,
	0x21, 0x55, 0x54, 0x78, 0x40,
	0x20, 0x54, 0x55, 0x79, 0x40,
	0x0C, 0x1E, 0x52, 0x72, 0x12,
	0x39, 0x55, 0x55, 0x55, 0x59,
	0x39, 0x54, 0x54, 0x54, 0x59,
	0x39, 0x55, 0x54, 0x54, 0x58,
	0x00, 0x00, 0x45, 0x7C, 0x41,
	0x00, 0x02, 0x45, 0x7D, 0x42,
	0x00, 0x01, 0x45, 0x7C, 0x40,
	0x7D, 0x12, 0x11, 0x12, 0x7D,
	0xF0, 0x28, 0x25, 0x28, 0xF0,
	0x7C, 0x54, 0x55, 0x45, 0x00,
	0x20, 0x54, 0x54, 0x7C, 0x54,
	0x7C, 0x0A, 0x09, 0x7F, 0x49,
	0x32, 0x49, 0x49, 0x49, 0x32,
	0x3A, 0x44, 0x44, 0x44, 0x3A,
	0x32, 0x4A, 0x48, 0x48, 0x30,
	0x3A, 0x41, 0x41, 0x21, 0x7A,
	0x3A, 0x42, 0x40, 0x20, 0x78,
	0x00, 0x9D, 0xA0, 0xA0, 0x7D,
	0x3D, 0x42, 0x42, 0x42, 0x3D,
	0x3D, 0x40, 0x40, 0x40, 0x3D,
	0x3C, 0x24, 0xFF, 0x24, 0x24,
	0x48, 0x7E, 0x49, 0x43, 0x66,
	0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
	0xFF, 0x09, 0x29, 0xF6, 0x20,
	0xC0, 0x88, 0x7E, 0x09, 0x03,
	0x20, 0x54, 0x54, 0x79, 0x41,
	0x00, 0x00, 0x44, 0x7D, 0x41,
	0x30, 0x48, 0x48, 0x4A, 0x32,
	0x38, 0x40, 0x40, 0x22, 0x7A,
	0x00, 0x7A, 0x0A, 0x0A, 0x72,
	0x7D, 0x0D, 0x19, 0x31, 0x7D,
	0x26, 0x29, 0x29, 0x2F, 0x28,
	0x26, 0x29, 0x29, 0x29, 0x26,
	0x30, 0x48, 0x4D, 0x40, 0x20,
	0x38, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x38,
	0x2F, 0x10, 0xC8, 0xAC, 0xBA,
	0x2F, 0x10, 0x28, 0x34, 0xFA,
	0x00, 0x00, 0x7B, 0x00, 0x00,
	0x08, 0x14, 0x2A, 0x14, 0x22,
	0x22, 0x14, 0x2A, 0x14, 0x08,
	0x55, 0x00, 0x55, 0x00, 0x55,
	0xAA, 0x55, 0xAA, 0x55, 0xAA,
	0xFF, 0x55, 0xFF, 0x55, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0x00,
	0x10, 0x10, 0x10, 0xFF, 0x00,
	0x14, 0x14, 0x14, 0xFF, 0x00,
	0x10, 0x10, 0xFF, 0x00, 0xFF,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x14, 0x14, 0x14, 0xFC, 0x00,
	0x14, 0x14, 0xF7, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x14, 0x14, 0xF4, 0x04, 0xFC,
	0x14, 0x14, 0x17, 0x10, 0x1F,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0x1F, 0x00,
	0x10, 0x10, 0x10, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0xF0, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0xFF, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x14,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x00, 0x1F, 0x10, 0x17,
	0x00, 0x00, 0xFC, 0x04, 0xF4,
	0x14, 0x14, 0x17, 0x10, 0x17,
	0x14, 0x14, 0xF4, 0x04, 0xF4,
	0x00, 0x00, 0xFF, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x14, 0x14, 0xF7, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x17, 0x14,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0xF4, 0x14,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x00, 0x00, 0x1F, 0x10, 0x1F,
	0x00, 0x00, 0x00, 0x1F, 0x14,
	0x00, 0x00, 0x00, 0xFC, 0x14,
	0x00, 0x00, 0xF0, 0x10, 0xF0,
	0x10, 0x10, 0xFF, 0x10, 0xFF,
	0x14, 0x14, 0x14, 0xFF, 0x14,
	0x10, 0x10, 0x10, 0x1F, 0x00,
	0x00, 0x00, 0x00, 0xF0, 0x10,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x38, 0x44, 0x44, 0x38, 0x44,
	0xFC, 0x4A, 0x4A, 0x4A, 0x34,
	0x7E, 0x02, 0x02, 0x06, 0x06,
	0x02, 0x7E, 0x02, 0x7E, 0x02,
	0x63, 0x55, 0x49, 0x41, 0x63,
	0x38, 0x44, 0x44, 0x3C, 0x04,
	0x40, 0x7E, 0x20, 0x1E, 0x20,
	0x06, 0x02, 0x7E, 0x02, 0x02,
	0x99, 0xA5, 0xE7, 0xA5, 0x99,
	0x1C, 0x2A, 0x49, 0x2A, 0x1C,
	0x4C, 0x72, 0x01, 0x72, 0x4C,
	0x30, 0x4A, 0x4D, 0x4D, 0x30,
	0x30, 0x48, 0x78, 0x48, 0x30,
	0xBC, 0x62, 0x5A, 0x46, 0x3D,
	0x3E, 0x49, 0x49, 0x49, 0x00,
	0x7E, 0x01, 0x01, 0x01, 0x7E,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x44, 0x44, 0x5F, 0x44, 0x44,
	0x40, 0x51, 0x4A, 0x44, 0x40,
	0x40, 0x44, 0x4A, 0x51, 0x40,
	0x00, 0x00, 0xFF, 0x01, 0x03,
	0xE0, 0x80, 0xFF, 0x00, 0x00,
	0x08, 0x08, 0x6B, 0x6B, 0x08,
	0x36, 0x12, 0x36, 0x24, 0x36,
	0x06, 0x0F, 0x09, 0x0F, 0x06,
	0x00, 0x00, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x00,
	0x30, 0x40, 0xFF, 0x01, 0x01,
	0x00, 0x1F, 0x01, 0x01, 0x1E,
	0x00, 0x19, 0x1D, 0x17, 0x12,
	0x00, 0x3C, 0x3C, 0x3C, 0x3C,
	0x00, 0x00, 0x00, 0x00, 0x00
};
//***** Functions prototypes *****//

void SendByte(uint8_t data)
{
  //while((SPI1->SR & SPI_SR_TXE) == RESET);
  SPI1->DR = data;
}
//1. Write Command to LCD
void ILI9488_SendCommand(uint8_t com)
{
	//*(__IO uint8_t *)(0x60000000) = com;
	uint8_t tmpCmd = com;
	//Set DC HIGH for COMMAND mode
	//HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_RESET);
	DC_COMMAND();
	//Put CS LOW
	//HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);
	CS_A();
	//Write byte using SPI
	HAL_SPI_Transmit(&hspi1, &tmpCmd, 1, 1);
	//SendByte(tmpCmd);
	//WaitLastData();
	CS_D();
	//Bring CS HIGH
	//HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
}

//2. Write data to LCD
void ILI9488_SendData(uint8_t data)
{
	//*(__IO uint8_t *)(0x60040000) = data;
	uint8_t tmpCmd = data;
	//Set DC LOW for DATA mode
	//HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_SET);
	DC_DATA();
	//Put CS LOW
	//HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);
	CS_A();
	//Write byte using SPI
	HAL_SPI_Transmit(&hspi1, &tmpCmd, 1, 1);
	//SendByte(tmpCmd);
	//WaitLastData();
	CS_D();

	//Bring CS HIGH
	//HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
}
//2.2 Write multiple/DMA
void ILI9488_SendData_Multi(uint8_t *buff, size_t buff_size){
	DC_DATA();
	/*CS_A();
	for (uint32_t i = 0; i < buff_size; i++)
	  {
	    SendByte(*buff);
	    buff++;
	  }

	  WaitLastData();
	  CS_D();*/
	CS_A();
	while (buff_size > 0){
		uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
		HAL_SPI_Transmit(&hspi1, buff, chunk_size, HAL_MAX_DELAY);
		buff += chunk_size;
		buff_size -= chunk_size;
	}
	CS_D();
}

void WaitLastData()
{
	while(!(SPI1->SR & SPI_SR_TXE));
		while(SPI1->SR & SPI_SR_BSY);

}
/*void ILI9488_SendData_Multi(uint16_t Colordata, uint32_t size)
{
	uint8_t colorL,colorH;

	//Set DC LOW for DATA mode
	//HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_SET);
	DC_DATA();
	//Put CS LOW
	//HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);
	CS_A();
	//Write byte using SPI
	HAL_SPI_Transmit(&lcdSPIhandle, (uint8_t *)&Colordata, size, 1000);
	//Wait for end of DMA transfer
	//Bring CS HIGH
	CS_D();
	//HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
}*/

//void ILI9488_Init(SPI_HandleTypeDef *spiLcdHandle, GPIO_TypeDef *csPORT, uint16_t csPIN, GPIO_TypeDef *dcPORT, uint16_t dcPIN, GPIO_TypeDef *resetPORT, uint16_t resetPIN)
void ILI9488_Init()
{

	 /*//Copy SPI settings
	 memcpy(&lcdSPIhandle, spiLcdHandle, sizeof(*spiLcdHandle));
	 //CS pin
	 tftCS_GPIO = csPORT;
	 tftCS_PIN = csPIN;
	 //DC pin
	 tftDC_GPIO = dcPORT;
	 tftDC_PIN = dcPIN;
	 //HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
	 CS_D();
	 //RESET pin
	 tftRESET_GPIO = resetPORT;
	 tftRESET_PIN = resetPIN;
	 //HAL_GPIO_WritePin(tftRESET_GPIO, tftRESET_PIN, GPIO_PIN_RESET);  //Turn LCD ON
	 RST_A();
	 HAL_Delay(10);
	 RST_D();
	 //HAL_GPIO_WritePin(tftRESET_GPIO, tftRESET_PIN, GPIO_PIN_SET);  //Turn LCD ON*/
	//SPI1->CR1 |= SPI_CR1_SPE;
	CS_D();
	RST_A();
		 HAL_Delay(10);
		 RST_D();
	 width=ILI9488_TFTWIDTH;
	 height=ILI9488_TFTHEIGHT;
	 //CS_A();
	 ILI9488_SendCommand(0xE0);
	 ILI9488_SendData(0x00);
	 ILI9488_SendData(0x03);
	 ILI9488_SendData(0x09);
	 ILI9488_SendData(0x08);
	 ILI9488_SendData(0x16);
	 ILI9488_SendData(0x0A);
	 ILI9488_SendData(0x3F);
	 ILI9488_SendData(0x78);
	 ILI9488_SendData(0x4C);
	 ILI9488_SendData(0x09);
	 ILI9488_SendData(0x0A);
	 ILI9488_SendData(0x08);
	 ILI9488_SendData(0x16);
	 ILI9488_SendData(0x1A);
	 ILI9488_SendData(0x0F);

	 	ILI9488_SendCommand(0XE1);
	 	ILI9488_SendData(0x00);
	 	ILI9488_SendData(0x16);
	 	ILI9488_SendData(0x19);
	 	ILI9488_SendData(0x03);
	 	ILI9488_SendData(0x0F);
	 	ILI9488_SendData(0x05);
	 	ILI9488_SendData(0x32);
	 	ILI9488_SendData(0x45);
	 	ILI9488_SendData(0x46);
	 	ILI9488_SendData(0x04);
	 	ILI9488_SendData(0x0E);
	 	ILI9488_SendData(0x0D);
	 	ILI9488_SendData(0x35);
	 	ILI9488_SendData(0x37);
	 	ILI9488_SendData(0x0F);

	 	ILI9488_SendCommand(0XC0);      //Power Control 1
	 	ILI9488_SendData(0x17);    //Vreg1out
	 	ILI9488_SendData(0x15);    //Verg2out

	 	ILI9488_SendCommand(0xC1);      //Power Control 2
	 	ILI9488_SendData(0x41);    //VGH,VGL

	 	ILI9488_SendCommand(0xC5);      //Power Control 3
	 	ILI9488_SendData(0x00);
	 	ILI9488_SendData(0x12);    //Vcom
	 	ILI9488_SendData(0x80);

	 	ILI9488_SendCommand(0x36);      //Memory Access
	 	ILI9488_SendData(0x48);

	 	ILI9488_SendCommand(0x3A);      // Interface Pixel Format
	 	ILI9488_SendData(0x66); 	  //18 bit

	 	ILI9488_SendCommand(0XB0);      // Interface Mode Control
	 	ILI9488_SendData(0x80);     			 //SDO NOT USE

	 	ILI9488_SendCommand(0xB1);      //Frame rate
	 	ILI9488_SendData(0xA0);    //60Hz

	 	ILI9488_SendCommand(0xB4);      //Display Inversion Control
	 	ILI9488_SendData(0x02);    //2-dot

	 	ILI9488_SendCommand(0XB6); //Display Function Control  RGB/MCU Interface Control

	 	ILI9488_SendData(0x02);    //MCU
	 	ILI9488_SendData(0x02);    //Source,Gate scan dieection

	 	ILI9488_SendCommand(0XE9);      // Set Image Functio
	 	ILI9488_SendData(0x00);    // Disable 24 bit data

	 	ILI9488_SendCommand(0xF7);      // Adjust Control
	 	ILI9488_SendData(0xA9);
	 	ILI9488_SendData(0x51);
	 	ILI9488_SendData(0x2C);
	 	ILI9488_SendData(0x82);    // D7 stream, loose

	 	ILI9488_SendCommand(ILI9488_SLPOUT);    //Exit Sleep

	 	HAL_Delay(120);

	 	ILI9488_SendCommand(ILI9488_DISPON);    //Display on

}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	/*ILI9488_SendCommand(ILI9488_CASET); // Column addr set
	ILI9488_SendData(x0 >> 8);
	ILI9488_SendData(x0 & 0xFF);     // XSTART
	ILI9488_SendData(x1 >> 8);
	ILI9488_SendData(x1 & 0xFF);     // XEND
	ILI9488_SendCommand(ILI9488_PASET); // Row addr set
	ILI9488_SendData(y0 >> 8);
	ILI9488_SendData(y0 & 0xff);     // YSTART
	ILI9488_SendData(y1 >> 8);
	ILI9488_SendData(y1 & 0xff);     // YEND
	ILI9488_SendCommand(ILI9488_RAMWR); // write to RAM*/
	ILI9488_SendCommand(ILI9488_CASET); // Column addr set
		{
		uint8_t data[] = {(x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF};
		ILI9488_SendData_Multi(data, sizeof(data));
		}
		ILI9488_SendCommand(ILI9488_PASET);
		{
		uint8_t data[] = {(y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF};
		ILI9488_SendData_Multi(data, sizeof(data));
		}
		ILI9488_SendCommand(ILI9488_RAMWR); // write to RAM*/
}

void setScrollArea(uint16_t topFixedArea, uint16_t bottomFixedArea)
{
	ILI9488_SendCommand(0x33); // Vertical scroll definition
	ILI9488_SendData(topFixedArea >> 8);
	ILI9488_SendData(topFixedArea);
	ILI9488_SendData((height - topFixedArea - bottomFixedArea) >> 8);
	ILI9488_SendData(height - topFixedArea - bottomFixedArea);
	ILI9488_SendData(bottomFixedArea >> 8);
	ILI9488_SendData(bottomFixedArea);
}

void scroll(uint16_t pixels)
{
	ILI9488_SendCommand(0x37); // Vertical scrolling start address
	ILI9488_SendData(pixels >> 8);
	ILI9488_SendData(pixels);
}
void pushColor(uint16_t color)
{
	HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);
	write16BitColor(color);
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);

}

void pushColors(uint16_t *data, uint8_t len, uint8_t first)
{
	uint16_t color;
	uint8_t buff[len * 3 + 1];
	uint16_t count = 0;
	uint8_t lencount = len;
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);

	if (first == 1) {
		HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_SET);
	}
	while (lencount--)
	{
		color = *data++;
		buff[count] = (((color & 0xF800) >> 11) * 255) / 31;
		count++;
		buff[count] = (((color & 0x07E0) >> 5) * 255) / 63;
		count++;
		buff[count] = ((color & 0x001F) * 255) / 31;
		count++;
	}
	HAL_SPI_Transmit(&lcdSPIhandle, buff, len * 3, 100);
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
}

void drawImage(const uint8_t* img, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{

	if ((x >= width) || (y >= height))
		return;
	if ((x + w - 1) >= width)
		w = width - x;
	if ((y + h - 1) >= height)
		h = height - y;
	setAddrWindow(x, y, x + w - 1, y + h - 1);
	HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);

	uint8_t linebuff[w * 3 + 1];
	uint32_t count = 0;
	for (uint16_t i = 0; i < h; i++) {
		uint16_t pixcount = 0;
		for (uint16_t o = 0; o < w; o++) {
			uint8_t b1 = img[count];
			count++;
			uint8_t b2 = img[count];
			count++;
			uint16_t color = b1 << 8 | b2;
			linebuff[pixcount] = (((color & 0xF800) >> 11) * 255)
					/ 31;
			pixcount++;
			linebuff[pixcount] = (((color & 0x07E0) >> 5) * 255)
					/ 63;
			pixcount++;
			linebuff[pixcount] = ((color & 0x001F) * 255) / 31;
			pixcount++;
		}
		HAL_SPI_Transmit(&lcdSPIhandle, linebuff, w * 3, 100);

	}

	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
}


void drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
		return;

	setAddrWindow(x, y, x + 1, y + 1);
	HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);

	write16BitColor(color);
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);

}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{

	if ((x >= width) || (y >= height))
		return;

	if ((y + h - 1) >= height)
		h = height - y;

	setAddrWindow(x, y, x, y + h - 1);
	HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);

	while (h--) {
		write16BitColor(color);
	}
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);

}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{

	if ((x >= width) || (y >= height))
		return;
	if ((x + w - 1) >= width)
		w = width - x;
	setAddrWindow(x, y, x + w - 1, y);
	HAL_GPIO_WritePin(tftDC_GPIO, tftDC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_RESET);
	while (w--)
	{
		write16BitColor(color);
	}
	HAL_GPIO_WritePin(tftCS_GPIO, tftCS_PIN, GPIO_PIN_SET);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	 if (x0 == x1) {
	    if (y0 > y1)
	      swap(y0, y1);
	    drawFastVLine(x0, y0, y1 - y0 + 1, color);
	  } else if (y0 == y1) {
	    if (x0 > x1)
	      swap(x0, x1);
	    drawFastHLine(x0, y0, x1 - x0 + 1, color);
	  } else {

	    writeLine(x0, y0, x1, y1, color);

	  }

}

void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {

  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
	  swap(x0, x1);
	  swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}
//6. Fill the entire screen with a background color

void fillScreen(uint16_t color)
{
	fillRect(0, 0,  width, height, color);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{

	uint32_t i, n, cnt, buf_size;
	if ((x >= width) || (y >= height))
		return;
	if ((x + w - 1) >= width)
		w = width - x;
	if ((y + h - 1) >= height)
		h = height - y;
	setAddrWindow(x, y, x + w - 1, y + h - 1);
	uint8_t r = (color & 0xF800) >> 11;
	uint8_t g = (color & 0x07E0) >> 5;
	uint8_t b = color & 0x001F;

	r = (r * 255) / 31;
	g = (g * 255) / 63;
	b = (b * 255) / 31;

	n = w*h*3;
	if (n <= 65535){
		cnt = 1;
		buf_size = n;
	}
	else {
		cnt = n/3;
		buf_size = 3;
		uint8_t min_cnt = n/65535+1;
		for (i=min_cnt; i < n/3; i++){
			if(n%i == 0){
				cnt = i;
				buf_size = n/i;
				break;
			}
		}
	}
	uint8_t frm_buf[buf_size];
	for (i=0; i < buf_size/3; i++)
	{
		frm_buf[i*3] = r;
		frm_buf[i*3+1] = g;
		frm_buf[i*3+2] = b;
	}
	DC_DATA();
	CS_A();
		while(cnt>0)
		{
			HAL_SPI_Transmit(&hspi1, frm_buf, buf_size, HAL_MAX_DELAY);

			cnt -= 1;
		}
		CS_D();

}


void setRotation(uint8_t r)
{

	ILI9488_SendCommand(ILI9488_MADCTL);
	uint8_t rotation = r % 4; // can't be higher than 3
	switch (rotation) {
	case 0:
		ILI9488_SendData(MADCTL_MX | MADCTL_BGR);
		width = ILI9488_TFTWIDTH;
		height = ILI9488_TFTHEIGHT;
		break;
	case 1:
		ILI9488_SendData(MADCTL_MV | MADCTL_BGR);
		width = ILI9488_TFTHEIGHT;
		height = ILI9488_TFTWIDTH;
		break;
	case 2:
		ILI9488_SendData(MADCTL_MY | MADCTL_BGR);
		width = ILI9488_TFTWIDTH;
		height = ILI9488_TFTHEIGHT;
		break;
	case 3:
		ILI9488_SendData(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		width = ILI9488_TFTHEIGHT;
		height = ILI9488_TFTWIDTH;
		break;
	}

}

void invertDisplay(uint8_t i)
{

	ILI9488_SendCommand(i ? ILI9488_INVON : ILI9488_INVOFF);

}
uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

//11. Text printing functions
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
	if(rotationNum == 1 || rotationNum ==3)
	{
		if((x >= ILI9488_TFTWIDTH)            || // Clip right
     (y >= ILI9488_TFTHEIGHT)           || // Clip bottom
     ((x + 6 * size - 1) < 0) || // Clip left
     ((y + 8 * size - 1) < 0))   // Clip top
    return;
	}
	else
	{
		if((y >= ILI9488_TFTWIDTH)            || // Clip right
     (x >= ILI9488_TFTHEIGHT)           || // Clip bottom
     ((y + 6 * size - 1) < 0) || // Clip left
     ((x + 8 * size - 1) < 0))   // Clip top
    return;
	}


  if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior

  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5)
      line = 0x0;
    else
      line = pgm_read_byte(font1+(c*5)+i);
    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
        	drawPixel(x+i, y+j, color);
        else {  // big size
        	fillRect(x+(i*size), y+(j*size), size + x+(i*size), size+1 + y+(j*size), color);
        }
      } else if (bg != color) {
        if (size == 1) // default size
        	drawPixel(x+i, y+j, bg);
        else {  // big size
        	fillRect(x+i*size, y+j*size, size + x+i*size, size+1 + y+j*size, bg);
        }
      }
      line >>= 1;
    }
  }
}
void ILI9488_printText(char text[], int16_t x, int16_t y, uint16_t color, uint16_t bg, uint8_t size)
{
	int16_t offset;
	offset = size*6;

	for(uint16_t i=0; i<40 && text[i]!=NULL; i++)
	{
		drawChar(x+(offset*i), y, text[i],color,bg,size);
	}
}
void testLines(uint8_t color)
{

	unsigned long start, t;
	int x1, y1, x2, y2, w = width, h = height;
	fillScreen(ILI9488_BLACK);

	x1 = y1 = 0;
	y2 = h - 1;

	for (x2 = 0; x2 < w; x2 += 6)
		drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for (y2 = 0; y2 < h; y2 += 6)
		drawLine(x1, y1, x2, y2, color);
	fillScreen(ILI9488_BLACK);

	x1 = w - 1;
	y1 = 0;
	y2 = h - 1;

	for (x2 = 0; x2 < w; x2 += 6)
		drawLine(x1, y1, x2, y2, color);
	x2 = 0;
	for (y2 = 0; y2 < h; y2 += 6)
		drawLine(x1, y1, x2, y2, color);

	fillScreen(ILI9488_BLACK);

	x1 = 0;
	y1 = h - 1;
	y2 = 0;

	for (x2 = 0; x2 < w; x2 += 6)
		drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for (y2 = 0; y2 < h; y2 += 6)
		drawLine(x1, y1, x2, y2, color);

	fillScreen(ILI9488_BLACK);

	x1 = w - 1;
	y1 = h - 1;
	y2 = 0;

	for (x2 = 0; x2 < w; x2 += 6)
		drawLine(x1, y1, x2, y2, color);
	x2 = 0;
	for (y2 = 0; y2 < h; y2 += 6)
		drawLine(x1, y1, x2, y2, color);
}

void write16BitColor(uint16_t color)
{

	  uint8_t r = (color & 0xF800) >> 11;
	  uint8_t g = (color & 0x07E0) >> 5;
	  uint8_t b = color & 0x001F;

	  r = (r * 255) / 31;
	  g = (g * 255) / 63;
	  b = (b * 255) / 31;
	  uint8_t data[3] = {r, g, b};
	  ILI9488_SendData_Multi(data, 3);
	  //HAL_SPI_Transmit(&hspi1, (uint8_t *)&r, 1, 10);
	  //HAL_SPI_Transmit(&hspi1, (uint8_t *)&g, 1, 10);
	  //HAL_SPI_Transmit(&hspi1, (uint8_t *)&b, 1, 10);

}




// ==========================================
// ==========================================
// STUFF IM ADDING ==========================
// ==========================================
// ==========================================

// Define cube colors
#define CUBE_WHITE      ILI9488_WHITE
#define CUBE_ORANGE     0xFD00      // Orange
#define CUBE_GREEN      ILI9488_GREEN
#define CUBE_RED        ILI9488_RED
#define CUBE_BLUE       ILI9488_BLUE
#define CUBE_YELLOW     ILI9488_YELLOW

// Define cube face size parameters
#define CUBE_FACE_SIZE  30          // Size of each small cube square
#define FACE_GAP        2           // Gap between squares
#define FACE_DIM        3           // 3x3 dimensions

// Screen positioning
#define START_X         20          // X position to start drawing
#define START_Y         40          // Y position to start drawing

#define FACE_WIDTH 	FACE_DIM * CUBE_FACE_SIZE + (FACE_DIM - 1) * FACE_GAP

#define LEFTX		START_X
#define LEFTY		START_Y + FACE_WIDTH
#define FRONTX		LEFTX + FACE_WIDTH + FACE_GAP
#define FRONTY		START_Y + FACE_WIDTH
#define RIGHTX		FRONTX + FACE_WIDTH + FACE_GAP
#define RIGHTY		START_Y + FACE_WIDTH
#define BACKX		LEFTX + FACE_WIDTH + FACE_GAP
#define BACKY		FRONTY + FACE_WIDTH + FACE_WIDTH + FACE_GAP
#define UPX 		FRONTX
#define UPY		START_Y
#define DOWNX		FRONTX
#define DOWNY		FRONTY + FACE_WIDTH + FACE_GAP


// ===========================================
// Draw a single square with specified color and optional label
void drawCubeSquare(int16_t x, int16_t y, uint16_t color) {
    // Draw the colored square
    fillRect(x, y, CUBE_FACE_SIZE, CUBE_FACE_SIZE, color);

    // Draw black outline
    drawLine(x, y, x + CUBE_FACE_SIZE, y, ILI9488_BLACK);
    drawLine(x, y, x, y + CUBE_FACE_SIZE, ILI9488_BLACK);
    drawLine(x + CUBE_FACE_SIZE, y, x + CUBE_FACE_SIZE, y + CUBE_FACE_SIZE, ILI9488_BLACK);
    drawLine(x, y + CUBE_FACE_SIZE, x + CUBE_FACE_SIZE, y + CUBE_FACE_SIZE, ILI9488_BLACK);
}


// SOLID COLOR
// Draw a complete 3x3 face with the specified color and label
void drawCubeFace(int16_t startX, int16_t startY, uint16_t color) {
    for (int row = 0; row < FACE_DIM; row++) {
        for (int col = 0; col < FACE_DIM; col++) {
            int16_t x = startX + col * (CUBE_FACE_SIZE + FACE_GAP);
            int16_t y = startY + row * (CUBE_FACE_SIZE + FACE_GAP);

            drawCubeSquare(x, y, color);
        }
    }
}


// SOLID COLOR
// Draw the entire unfolded cube (6 faces in a cross pattern)
void drawRubiksCube() {
	// Background Color
	fillScreen(ILI9488_WHITE);

    // Draw each face with its color and label in new positions
    drawCubeFace(UPX, UPY, CUBE_WHITE);
    drawCubeFace(LEFTX, LEFTY, CUBE_ORANGE);
    drawCubeFace(FRONTX, FRONTY, CUBE_GREEN);
    drawCubeFace(RIGHTX, RIGHTY, CUBE_RED);
    drawCubeFace(BACKX, BACKY, CUBE_BLUE);
    drawCubeFace(DOWNX, DOWNY, CUBE_YELLOW);
}



// Update a specific face with new colors (for when a face rotates)
void updateCubeFace(int face, uint16_t colors[FACE_DIM][FACE_DIM]) {
    int16_t startX, startY;

    // 0=Up, 1=Left, 2=Front, 3=Right, 4=Back, 5=Down
    int faceWidth = FACE_DIM * CUBE_FACE_SIZE + (FACE_DIM - 1) * FACE_GAP;

    switch(face) {
        case 0: // Up
            startX = UPX;
            startY = UPY;
            break;
        case 1: // Left
            startX = LEFTX;
            startY = LEFTY;
            break;
        case 2: // Front
            startX = FRONTX;
            startY = FRONTY;
            break;
        case 3: // Right
            startX = RIGHTX;
            startY = RIGHTY;
            break;
        case 4: // Back
            startX = BACKX;
            startY = BACKY;
            break;
        case 5: // Down
            startX = DOWNX;
            startY = DOWNY;
            break;
        default:
            return; // Invalid face
    }

    // Draw each square with its new color
    for (int row = 0; row < FACE_DIM; row++) {
        for (int col = 0; col < FACE_DIM; col++) {
            int16_t x = startX + col * (CUBE_FACE_SIZE + FACE_GAP);
            int16_t y = startY + row * (CUBE_FACE_SIZE + FACE_GAP);

            drawCubeSquare(x, y, colors[row][col]);
        }
    }
}


// Example function to display the motor state
void displayMotorState(uint8_t motorNumber, uint16_t steps, int8_t direction) {
    char buffer[32];

    // Clear motor status area
    fillRect(10, 10, 300, 40, ILI9488_BLACK);

    // Display motor status
    sprintf(buffer, "Motor %d: %+d steps", motorNumber, direction * steps);
    ILI9488_printText(buffer, 10, 10, ILI9488_WHITE, ILI9488_BLACK, 2);

    // Show which face is rotating
    char *faceName;
    switch(motorNumber) {
        case 0: faceName = "Up"; break;
        case 1: faceName = "Left"; break;
        case 2: faceName = "Front"; break;
        case 3: faceName = "Right"; break;
        case 4: faceName = "Back"; break;
        case 5: faceName = "Down"; break;
        default: faceName = "Unknown"; break;
    }

    sprintf(buffer, "Rotating %s face", faceName);
    ILI9488_printText(buffer, 10, 30, ILI9488_WHITE, ILI9488_BLACK, 1);
}




// Example usage in main
void displayCubeExample() {
    // Initialize LCD
    ILI9488_Init();

    // Set rotation if needed
    setRotation(1);  // Adjust based on your LCD orientation

    // Clear screen
    fillScreen(ILI9488_BLACK);

    // Draw the initial cube state
    drawRubiksCube();

    // Display some motor state (sample)
    displayMotorState(2, 90, 1);  // Motor 2 (Front face) rotating 90 steps clockwise

    // Example of updating a face after rotation
    // This would be called after a motor completes a rotation
    uint16_t newColors[3][3] = {
        {CUBE_GREEN, CUBE_GREEN, CUBE_GREEN},
        {CUBE_GREEN, CUBE_GREEN, CUBE_GREEN},
        {CUBE_GREEN, CUBE_GREEN, CUBE_GREEN}
    };

    updateCubeFace(2, newColors);
}



// ================================================
// FUNCTIONS FOR UPDATING THE STATE OF THE CUBE
// 0=Up, 1=Left, 2=Front, 3=Right, 4=Back, 5=Down

// Can input custom colors for the cube
void initCustomCube(CubeState *cube, uint16_t colors[3][3]) {
	for (int face = 0; face < 6; ++face) {
		for (int r = 0; r < 3; ++r) {
			for (int c = 0; c < 3; ++c) {
				cube->Cube[face][r][c] = colors[r][c];
			}
		}
	}

	// Draw on lcd screen
		for (int i = 0; i < 6; ++i) {
			updateCubeFace(i, cube->Cube[i]);
		}
}

// Default state of the cube, which is solved
void defaultCube(CubeState *cube) {
	uint16_t white[3][3] = {
		{ILI9488_WHITE, ILI9488_WHITE, ILI9488_WHITE},
		{ILI9488_WHITE, ILI9488_WHITE, ILI9488_WHITE},
		{ILI9488_WHITE, ILI9488_WHITE, ILI9488_WHITE}
	};

	uint16_t pink[3][3] = {
		{ILI9488_PINK, ILI9488_PINK, ILI9488_PINK},
		{ILI9488_PINK, ILI9488_PINK, ILI9488_PINK},
		{ILI9488_PINK, ILI9488_PINK, ILI9488_PINK}
	};

	uint16_t blue[3][3] = {
		{ILI9488_CYAN, ILI9488_CYAN, ILI9488_CYAN},
		{ILI9488_CYAN, ILI9488_CYAN, ILI9488_CYAN},
		{ILI9488_CYAN, ILI9488_CYAN, ILI9488_CYAN}
	};

	uint16_t orange[3][3] = {
		{ILI9488_ORANGE, ILI9488_ORANGE, ILI9488_ORANGE},
		{ILI9488_ORANGE, ILI9488_ORANGE, ILI9488_ORANGE},
		{ILI9488_ORANGE, ILI9488_ORANGE, ILI9488_ORANGE}
	};

	uint16_t yellow[3][3] = {
		{ILI9488_YELLOW, ILI9488_YELLOW, ILI9488_YELLOW},
		{ILI9488_YELLOW, ILI9488_YELLOW, ILI9488_YELLOW},
		{ILI9488_YELLOW, ILI9488_YELLOW, ILI9488_YELLOW}
	};

	uint16_t green[3][3] = {
		{ILI9488_GREEN, ILI9488_GREEN, ILI9488_GREEN},
		{ILI9488_GREEN, ILI9488_GREEN, ILI9488_GREEN},
		{ILI9488_GREEN, ILI9488_GREEN, ILI9488_GREEN}
	};

	// Fill each face
	for (int r = 0; r < 3; ++r) {
		for (int c = 0; c < 3; ++c) {
			cube->Cube[0][r][c] = white[r][c];
			cube->Cube[1][r][c] = pink[r][c];
			cube->Cube[2][r][c] = blue[r][c];
			cube->Cube[3][r][c] = orange[r][c];
			cube->Cube[4][r][c] = yellow[r][c];
			cube->Cube[5][r][c] = green[r][c];
		}
	}

	// Draw on lcd screen
	for (int i = 0; i < 6; ++i) {
		updateCubeFace(i, cube->Cube[i]);
	}
}


void rotateFaceMatrix(uint16_t face[3][3], int direction) {
    uint16_t temp[3][3];

    if (direction == 1) { // Clockwise
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                temp[c][2 - r] = face[r][c];
    }
	else {  // Counter-clockwise
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                temp[2 - c][r] = face[r][c];
    }

    // Copy back
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            face[r][c] = temp[r][c];
}



// Face indices
#define UP 0
#define LEFT 1
#define FRONT 2
#define RIGHT 3
#define BACK 4
#define DOWN 5

void rotateFace(CubeState *cube, int face, int direction) {
    rotateFaceMatrix(cube->Cube[face], direction);  // Rotate the face itself
    uint16_t temp[3];
    uint16_t (*C)[3][3] = cube->Cube;

    switch(face) {
        case FRONT:
            if (direction == 1) {
                for (int i = 0; i < 3; ++i) temp[i] = C[UP][2][i];
                for (int i = 0; i < 3; ++i) C[UP][2][i] = C[LEFT][2 - i][2];
                for (int i = 0; i < 3; ++i) C[LEFT][i][2] = C[DOWN][0][i];
                for (int i = 0; i < 3; ++i) C[DOWN][0][i] = C[RIGHT][2 - i][0];
                for (int i = 0; i < 3; ++i) C[RIGHT][i][0] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = C[UP][2][i];
                for (int i = 0; i < 3; ++i) C[UP][2][i] = C[RIGHT][i][0];
                for (int i = 0; i < 3; ++i) C[RIGHT][2 - i][0] = C[DOWN][0][i];
                for (int i = 0; i < 3; ++i) C[DOWN][0][i] = C[LEFT][i][2];
                for (int i = 0; i < 3; ++i) C[LEFT][2 - i][2] = temp[i];
            }
            break;

        case BACK:
            if (direction == 1) {
                for (int i = 0; i < 3; ++i) temp[i] = C[UP][0][i];
                for (int i = 0; i < 3; ++i) C[UP][0][i] = C[RIGHT][i][2];
                for (int i = 0; i < 3; ++i) C[RIGHT][2 - i][2] = C[DOWN][2][i];
                for (int i = 0; i < 3; ++i) C[DOWN][2][i] = C[LEFT][i][0];
                for (int i = 0; i < 3; ++i) C[LEFT][2 - i][0] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = C[UP][0][i];
                for (int i = 0; i < 3; ++i) C[UP][0][i] = C[LEFT][2 - i][0];
                for (int i = 0; i < 3; ++i) C[LEFT][i][0] = C[DOWN][2][i];
                for (int i = 0; i < 3; ++i) C[DOWN][2][i] = C[RIGHT][2 - i][2];
                for (int i = 0; i < 3; ++i) C[RIGHT][i][2] = temp[i];
            }
            break;

        case LEFT:
            if (direction == 1) {
                for (int i = 0; i < 3; ++i) temp[i] = C[UP][i][0];
                for (int i = 0; i < 3; ++i) C[UP][i][0] = C[BACK][2 - i][2];
                for (int i = 0; i < 3; ++i) C[BACK][i][2] = C[DOWN][2 - i][0];
                for (int i = 0; i < 3; ++i) C[DOWN][i][0] = C[FRONT][i][0];
                for (int i = 0; i < 3; ++i) C[FRONT][i][0] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = C[UP][i][0];
                for (int i = 0; i < 3; ++i) C[UP][i][0] = C[FRONT][i][0];
                for (int i = 0; i < 3; ++i) C[FRONT][i][0] = C[DOWN][i][0];
                for (int i = 0; i < 3; ++i) C[DOWN][2 - i][0] = C[BACK][i][2];
                for (int i = 0; i < 3; ++i) C[BACK][2 - i][2] = temp[i];
            }
            break;

        case RIGHT:
            if (direction == 1) {
                for (int i = 0; i < 3; ++i) temp[i] = C[UP][i][2];
                for (int i = 0; i < 3; ++i) C[UP][i][2] = C[FRONT][i][2];
                for (int i = 0; i < 3; ++i) C[FRONT][i][2] = C[DOWN][i][2];
                for (int i = 0; i < 3; ++i) C[DOWN][2 - i][2] = C[BACK][i][0];
                for (int i = 0; i < 3; ++i) C[BACK][2 - i][0] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = C[UP][i][2];
                for (int i = 0; i < 3; ++i) C[UP][i][2] = C[BACK][2 - i][0];
                for (int i = 0; i < 3; ++i) C[BACK][i][0] = C[DOWN][2 - i][2];
                for (int i = 0; i < 3; ++i) C[DOWN][i][2] = C[FRONT][i][2];
                for (int i = 0; i < 3; ++i) C[FRONT][i][2] = temp[i];
            }
            break;

        case UP:
            if (direction == 1) {
                for (int i = 0; i < 3; ++i) temp[i] = C[BACK][0][2 - i];
                for (int i = 0; i < 3; ++i) C[BACK][0][2 - i] = C[RIGHT][0][i];
                for (int i = 0; i < 3; ++i) C[RIGHT][0][i] = C[FRONT][0][i];
                for (int i = 0; i < 3; ++i) C[FRONT][0][i] = C[LEFT][0][i];
                for (int i = 0; i < 3; ++i) C[LEFT][0][i] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = C[BACK][0][2 - i];
                for (int i = 0; i < 3; ++i) C[BACK][0][2 - i] = C[LEFT][0][i];
                for (int i = 0; i < 3; ++i) C[LEFT][0][i] = C[FRONT][0][i];
                for (int i = 0; i < 3; ++i) C[FRONT][0][i] = C[RIGHT][0][i];
                for (int i = 0; i < 3; ++i) C[RIGHT][0][i] = temp[i];
            }
            break;

        case DOWN:
            if (direction == 1) {
                for (int i = 0; i < 3; ++i) temp[i] = C[BACK][2][2 - i];
                for (int i = 0; i < 3; ++i) C[BACK][2][2 - i] = C[LEFT][2][i];
                for (int i = 0; i < 3; ++i) C[LEFT][2][i] = C[FRONT][2][i];
                for (int i = 0; i < 3; ++i) C[FRONT][2][i] = C[RIGHT][2][i];
                for (int i = 0; i < 3; ++i) C[RIGHT][2][i] = temp[i];
            } else {
                for (int i = 0; i < 3; ++i) temp[i] = C[BACK][2][2 - i];
                for (int i = 0; i < 3; ++i) C[BACK][2][2 - i] = C[RIGHT][2][i];
                for (int i = 0; i < 3; ++i) C[RIGHT][2][i] = C[FRONT][2][i];
                for (int i = 0; i < 3; ++i) C[FRONT][2][i] = C[LEFT][2][i];
                for (int i = 0; i < 3; ++i) C[LEFT][2][i] = temp[i];
            }
            break;

        default:
            break;
    }

    // Draw on lcd screen
    for (int i = 0; i < 6; ++i) {
    	updateCubeFace(i, cube->Cube[i]);
    }
}


