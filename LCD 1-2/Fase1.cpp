
#include "Fase1.h"
#include <chrono>
#include <thread>


#define FUNCTION_SET_8		0x03
#define FUNCTION_SET_4		0x02
#define FS_4B_2L_5x8		0x38
#define DISPLAY_ON_OFF_CONTROL	0x08
#define ENTRY_MODE_SET		0x06

FT_HANDLE& deviceHandler_lcdInit(int iDevice) //Inicializacion del modo 4 bits.
{
	FT_HANDLE deviceHandler = 0;
	if (FT_Open(iDevice, &deviceHandler) == FT_OK)
	{
		UCHAR Mask = 0xFF;	//Selects all FTDI pins.
		UCHAR Mode = 1; 	// Set asynchronous bit-bang mode
		if (FT_SetBitMode(deviceHandler, Mask, Mode) == FT_OK)	
		{
			lcdWriteNibble(&deviceHandler, 0x03, IR);					//1. Enviar el nibble alto de �function set� con el modo en 8 bits
			std::this_thread::sleep_for(std::chrono::milliseconds(50)); //2. Esperar 4 ms
			lcdWriteNibble(&deviceHandler, 0x03, IR);					//3. Enviar el nibble alto de �function set� con el modo en 8 bits			
			std::this_thread::sleep_for(std::chrono::microseconds(1000));//4. Esperar 100 �s
			lcdWriteNibble(&deviceHandler, 0x03, IR);					//5. Enviar el nibble alto de �function set� con el modo en 8 bits
			lcdWriteNibble(&deviceHandler, 0x02, IR);					//6. Enviar el nibble alto de �function set� con el modo en 4 bits.
			lcdWriteByte(&deviceHandler, 0x38, IR);						//7. Enviar la instrucci�n �function set� con el modo en 4 bits, 2 l�neas y fuente de 5x8
			lcdWriteByte(&deviceHandler, 0x08, IR);						//8. Enviar la instrucci�n �display on / off control� con todo apagado
			lcdWriteByte(&deviceHandler, 0x01, IR);						//9. Enviar la instrucci�n �clear screen�
			lcdWriteByte(&deviceHandler, 0x06, IR);						//10. Enviar la instrucci�n �entry mode set�

			return deviceHandler;
		}	
		else
			printf("Couldn't configure LCD\n");
		FT_Close(deviceHandler);
	}
	printf("Couldn't open USB %d\n", iDevice);
	return deviceHandler;
}

void lcdWriteIR(FT_HANDLE * deviceHandler, BYTE valor)
{
	lcdWriteByte(deviceHandler, valor, IR);
}

void lcdWriteDR(FT_HANDLE * deviceHandler, BYTE valor)
{
	lcdWriteByte(deviceHandler, valor, DR);
}


void lcdWriteNibble(FT_HANDLE * deviceHandler, BYTE value, BYTE RS) //RS vale 0x00(lcdIR) o 0x02(lcdDR)
{
	char buffer[1];
	unsigned long BytesSent = 0;
	buffer[0] = ((value << 4) & 0xF0); //buffer[0] es enable; el menos significativo
	buffer[0] |= RS;
	if (FT_Write(*deviceHandler, buffer, 1, &BytesSent) == FT_OK) //enable en 0 || RS esta en lo que me vino || nibble en value
	{
		Sleep(1); //1ms
		buffer[0] |= LCD_E; //define LCD_E 0x01; //prendo el bit menos significativo

			if (FT_Write(*deviceHandler, buffer, 1, &BytesSent) == FT_OK) //enable en 1
			{
				Sleep(10); 
				buffer[0] &= (~LCD_E);
				FT_Write(*deviceHandler, buffer, 1, &BytesSent);
				Sleep(1); 
			}
	}
}



void lcdWriteByte(FT_HANDLE * deviceHandler, BYTE value, BYTE RS) //Llama a write nibble con parte alta y despues con la baja
{
	lcdWriteNibble(deviceHandler, ((value >> 4)&(0x0F)), RS);
	lcdWriteNibble(deviceHandler, (value & 0x0F), RS);
}

//void lcdWriteByte(FT_HANDLE * deviceHandler, BYTE value, BYTE RS) //RAMA
//{
//	//Devuelve un 0 si se produjo un error en alguna de los dos por eso el and bitwise ambas devuelven 1 si estuvo todo ok 
//	// ademas corta la ejecucion si el primero genero error por el lazy del && (cosa que no se produce bitwise)
//	lcdWriteNibble(MSN(value) | rs);
//	lcdWriteNibble(LSN(value) | rs));
//}

//AGUSTIN
//#define MSN(x) ((x)&0xF0)
//#define LSN(x) (((x)<<4)&0xF0)
//void lcdWriteByte(FT_HANDLE * deviceHandler, BYTE value, BYTE rs)
//{
//
//	lcdWriteNibble(deviceHandler, ((value&(0xF0)) | rs));
//	lcdWriteNibble(deviceHandler, ((value << 4) & 0xF0) | rs);
//}
