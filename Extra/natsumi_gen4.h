#ifndef ESP32_H
#define ESP32_H

// SPI_DRIVER_SELECT must be set to 0 in SdFat/SdFatConfig.h (default is 0)

SdFat SD;
#define SPIINIT 12,13,11,9 // sck, miso, mosi, cs
#define SDMHZ 20
#define SDINIT 9, SD_SCK_MHZ(SDMHZ)
#define LED 0
#define LEDinv 0
#define BOARD "ESP32-S3-DevKitM/Zero"
#define board_esp32
#define board_digital_io

uint8 esp32bdos(uint16 dmaaddr) {
	return(0x00);
}

void NeoPixWrite(uint8_t iLedPin, uint8_t iLedBit) {
	static uint8_t iNeoPix = 0;

	if(iNeoPix == 0) {
		iNeoPix = 48;	pinMode(iNeoPix, INPUT_PULLDOWN);	//	ESP32-S3-DevKitM (Slave)
		if(digitalRead(iNeoPix) == LOW) iNeoPix = 21;		//	ESP32-S3-Zero
	}

	if(iLedBit == LOW)	neopixelWrite(iNeoPix, 0x00, 0x00, 0x00);
	else				neopixelWrite(iNeoPix, 0x40, 0x00, 0x80);
}

#endif
