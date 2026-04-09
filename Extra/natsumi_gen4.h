#ifndef ESP32_H
#define ESP32_H

// SPI_DRIVER_SELECT must be set to 0 in SdFat/SdFatConfig.h (default is 0)

SdFat SD;

#define		LedDevKitM	48
#define		LedXiaoC6	15

#define		SPIINIT		((iNeoPix == LedDevKitM)?12:19),\
						((iNeoPix == LedDevKitM)?13:17),\
						((iNeoPix == LedDevKitM)?11:20),\
						((iNeoPix == LedDevKitM)? 9:18)		// sck, miso, mosi, cs

#define		SDMHZ		20
#define		SDINIT		((iNeoPix == LedDevKitM)? 9:18), SD_SCK_MHZ(SDMHZ)

#define		LED			0
#define		LEDinv		0

#define		BOARD		"ESP32-S3-DevKitM (Slave) / XIAO ESP32C6"

#define		board_esp32
#define		board_digital_io

uint8 esp32bdos(uint16 dmaaddr) {
	return(0x00);
}

static uint8_t iNeoPix = LED;

void NeoPixWrite(uint8_t iLedPin, uint8_t iLedBit) {
	if(iNeoPix == LED) {
		pinMode(1, INPUT_PULLDOWN);

		if(digitalRead(1) == HIGH)	iNeoPix = LedDevKitM;	// ESP32-S3-DevKitM (Slave)
		else						iNeoPix = LedXiaoC6;	// XIAO ESP32C6

		pinMode(iNeoPix, OUTPUT);
	}

	if(iNeoPix == LedDevKitM) {
		if(iLedBit == LOW)	neopixelWrite(iNeoPix, 0x00, 0x00, 0x00);
		else				neopixelWrite(iNeoPix, 0x40, 0x00, 0x80);
	} else digitalWrite(iNeoPix, iLedBit ^ 1);
}

#endif
