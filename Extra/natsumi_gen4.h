
//==============================================================================//
//	Z80 Single Board Computer "PC-84C0SD 20MHz" I/O Sketch for ESP32-S3-MINI-1	//
//				Implemented by Shisuibi --Grand Master Sorcerian--				//
//==============================================================================//


//==============================================================================//
#ifndef		ESP32_H
#define		ESP32_H
//==============================================================================//


//==============================================================================//
enum {
	X, Y, Z, XY = Z, W, XYZ = W, XYZW,					//	マトリクス参照
};
//------------------------------------------------------------------------------//
#define		False						0				//	論理値（偽）
#define		True						1				//	論理値（真）
//------------------------------------------------------------------------------//
#define		StringSizeS					0x0010			//	文字列幅（短）
#define		StringSizeL					0x0100			//	文字列幅（長）
//------------------------------------------------------------------------------//
typedef		const char					Cint08;			//	符号無し文字（08bit）
//------------------------------------------------------------------------------//
typedef		unsigned char				Uint08;			//	符号無し整数（08bit）
typedef		signed char					Sint08;			//	符号有り整数（08bit）

typedef		unsigned short				Uint16;			//	符号無し整数（16bit）
typedef		signed short				Sint16;			//	符号有り整数（16bit）

typedef		unsigned long				Uint32;			//	符号無し整数（32bit）
typedef		signed long					Sint32;			//	符号有り整数（32bit）
//------------------------------------------------------------------------------//
typedef		signed long					Sfix08;			//	固定小数点数（08bit）
typedef		signed long					Sfix16;			//	固定小数点数（16bit）
typedef		signed long					Sfix24;			//	固定小数点数（24bit）
//------------------------------------------------------------------------------//
typedef		float						Sflt32;			//	浮動小数点数（32bit）
typedef		double						Sflt64;			//	浮動小数点数（64bit）
//==============================================================================//


//==============================================================================//
#include	<BLEDevice.h>								//	BLEDeviceライブラリ
//------------------------------------------------------------------------------//
#define		BleUartRead()				(	(iSysCtrlBit & 0x01) >> 0	)

#define		BleUartLow()				{	iSysCtrlBit &= 0xFE;	}
#define		BleUartHigh()				{	iSysCtrlBit |= 0x01;	}
//------------------------------------------------------------------------------//
static Uint08 iSysCtrlBit;								//	システム制御ビット
//==============================================================================//


//==============================================================================//
#define		NUL							0x00			//	ASCIIコード（NUL）
//------------------------------------------------------------------------------//
#define		SerialBaudRateTx			115200			//	通信端末ボーレート（送信）
#define		SerialSegSizeTx				0x0080			//	通信端末切片文字数（送信）
//------------------------------------------------------------------------------//
#define		SerialBufSizeRx				0x0800			//	シリアル緩衝サイズ（受信）
#define		SerialBufSizeTx				0x0400			//	シリアル緩衝サイズ（送信）
//------------------------------------------------------------------------------//
#define		SerialBufMaskRx(iIndex)	\
					((iIndex) & (SerialBufSizeRx - 1))	//	シリアル緩衝マスク（受信）
#define		SerialBufMaskTx(iIndex)	\
					((iIndex) & (SerialBufSizeTx - 1))	//	シリアル緩衝マスク（送信）
//==============================================================================//


//==============================================================================//
static Cint08* pBleServiceUUID = "FFE0";				//	BLEサービスUUID
static Cint08* pBleCharaRxUUID = "FFE1";				//	BLE特性UUID（受信）
static Cint08* pBleCharaTxUUID = "FFE2";				//	BLE特性UUID（送信）
//------------------------------------------------------------------------------//
static Cint08* pSerialProfileName = "PC-84C0SD 20MHz";	//	通信端末プロファイル名
//------------------------------------------------------------------------------//
static Uint16 iSerialRxWr, iSerialRxRd;					//	シリアル参照（受信）
static Uint16 iSerialTxWr, iSerialTxRd;					//	シリアル参照（送信）

static Uint08 aiSerialBufRx[SerialBufSizeRx];			//	シリアル緩衝（受信）
static Uint08 aiSerialBufTx[SerialBufSizeTx];			//	シリアル緩衝（送信）
//------------------------------------------------------------------------------//
static Sint08 iBleAdvtReq;								//	BLE接続広告要求
static Uint32 iSerialMicros;							//	通信端末マイクロ秒
//------------------------------------------------------------------------------//
static BLEServer* pBleServer;							//	BLEサーバー識別子
static BLEService* pBleService;							//	BLEサービス識別子

static BLECharacteristic* pBleCharaRx;					//	BLE特性識別子（受信）
static BLECharacteristic* pBleCharaTx;					//	BLE特性識別子（送信）
//==============================================================================//


//==============================================================================//
static void RecepData(Uint08 iData) {
	aiSerialBufRx[iSerialRxWr = SerialBufMaskRx(iSerialRxWr + 1)] = iData;
}
//------------------------------------------------------------------------------//
static void TransData(Uint08 iData) {
	aiSerialBufTx[iSerialTxWr = SerialBufMaskTx(iSerialTxWr + 1)] = iData;
}
//------------------------------------------------------------------------------//
static Uint08 SliceData(Sint08 iMode) {
	if(iSerialRxWr != iSerialRxRd) {
		if(iMode == False) {
			return(aiSerialBufRx[              SerialBufMaskRx(iSerialRxRd + 1)]);
		} else {
			return(aiSerialBufRx[iSerialRxRd = SerialBufMaskRx(iSerialRxRd + 1)]);
		}
	} else return(NUL);
}
//------------------------------------------------------------------------------//
static void TransString(Cint08* pString) {
	while(*pString != NUL) TransData(*pString++);
}
//==============================================================================//


//==============================================================================//
class BleServerCB: public BLEServerCallbacks {
	void onConnect(BLEServer* pBleServer) {
		BleUartHigh();
		iSerialTxRd = iSerialTxWr;
	}

	void onDisconnect(BLEServer* pBleServer) {
		BleUartLow();
		iBleAdvtReq = True;
	}
};
//------------------------------------------------------------------------------//
class BleCharaCB: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic* pBleChara) {
		String sValue = pBleChara->getValue();
		Sint16 i;

		for(i = 0;i < sValue.length();i++) {
			RecepData(sValue[i]);
		}
	}
};
//==============================================================================//


//==============================================================================//
static void TransClear(void) {
	iSerialRxWr = iSerialRxRd = 0;
	iSerialTxWr = iSerialTxRd = 0;
}
//------------------------------------------------------------------------------//
static void TransBleStart(void) {
	BLEDevice::init(pSerialProfileName);
	pBleServer = BLEDevice::createServer();

	pBleServer->setCallbacks(new BleServerCB());
	pBleService = pBleServer->createService(pBleServiceUUID);

	pBleCharaRx = pBleService->createCharacteristic(pBleCharaRxUUID, BLECharacteristic::PROPERTY_WRITE );
	pBleCharaTx = pBleService->createCharacteristic(pBleCharaTxUUID, BLECharacteristic::PROPERTY_NOTIFY);

	pBleCharaRx->setCallbacks(new BleCharaCB());
	pBleService->start();
}
//------------------------------------------------------------------------------//
static Sint08 TransInterval(void) {
	if(		(micros() - iSerialMicros) >
			((1000000 * SerialSegSizeTx) / (SerialBaudRateTx >> 3))		) return(True);

	return(False);
}
//------------------------------------------------------------------------------//
static void TransWrite(void) {
	Uint08* pData;
	Uint16 iSize;

	if((iSerialTxWr != iSerialTxRd)&&(TransInterval() != False)) {
		if((iSize = SerialBufMaskTx(iSerialTxWr - iSerialTxRd)) > SerialSegSizeTx)
			iSize = SerialSegSizeTx;
		if((iSerialTxRd < (SerialBufSizeTx - 1))&&((iSerialTxRd + iSize) >= SerialBufSizeTx))
			iSize = SerialBufSizeTx - iSerialTxRd - 1;

		pData = &(aiSerialBufTx[SerialBufMaskTx(iSerialTxRd + 1)]);
		Serial.write(pData, iSize);

		if(BleUartRead() != False) {
			pBleCharaTx->setValue(pData, iSize);
			pBleCharaTx->notify();
		}

		iSerialTxRd = SerialBufMaskTx(iSerialTxRd + iSize);
		iSerialMicros = micros();
	}
}
//------------------------------------------------------------------------------//
static void TransRead(void) {
	Sint16 i;

	for(i = 0;i < SerialSegSizeTx;i++) {
		if(Serial.available() == 0) break;
		RecepData(Serial.read());
	}
}
//==============================================================================//


//==============================================================================//
static void TransInit(void) {
	TransClear();	iBleAdvtReq = True;

	iSerialMicros = micros();
	TransBleStart();
}
//------------------------------------------------------------------------------//
static void TransMove(void) {
	if(iBleAdvtReq != False) {
		pBleServer->startAdvertising();
		iBleAdvtReq = False;
	}

	TransWrite();
	TransRead();
}
//==============================================================================//


//==============================================================================//
//	SPI_DRIVER_SELECT must be set to 0 in SdFat/SdFatConfig.h (default is 0)
//------------------------------------------------------------------------------//
#define		SDMHZ						20
#define		BOARD						"XIAO ESP32C6 / ESP32-S3-DevKitM"

#define		LED							0
#define		LEDinv						0

#define		board_esp32
#define		board_digital_io
//------------------------------------------------------------------------------//
#define		LedXiaoC6					15				//	XIAO ESP32C6
#define		LedDevKitM					48				//	ESP32-S3-DevKitM
//------------------------------------------------------------------------------//
#define		SPIINIT		((iNeoPix == LedXiaoC6)?19:12),\
						((iNeoPix == LedXiaoC6)?17:13),\
						((iNeoPix == LedXiaoC6)?20:11),\
						((iNeoPix == LedXiaoC6)?18: 9)	//	sck, miso, mosi, cs
//------------------------------------------------------------------------------//
#define		SDINIT		((iNeoPix == LedXiaoC6)?18: 9), SD_SCK_MHZ(SDMHZ)
//==============================================================================//


//==============================================================================//
static SdFat SD;
//------------------------------------------------------------------------------//
static Uint08 iNeoPix = LED;
//==============================================================================//


//==============================================================================//
static Uint08 esp32bdos(Uint16 iDmaAddr) {
	return(0x00);
}
//------------------------------------------------------------------------------//
static void NeoPixWrite(Uint08 iLedPin, Uint08 iLedBit) {
	if(iNeoPix == LED) {
		pinMode(1, INPUT_PULLDOWN);

		if(digitalRead(1) == LOW) {
			iNeoPix = LedXiaoC6;

			pinMode(WIFI_ENABLE    , OUTPUT);	digitalWrite(WIFI_ENABLE    , LOW);		delay(100);
			pinMode(WIFI_ANT_CONFIG, OUTPUT);	digitalWrite(WIFI_ANT_CONFIG, LOW);
		} else iNeoPix = LedDevKitM;

		pinMode(iNeoPix, OUTPUT);
		TransInit();
	}

	if(iNeoPix == LedDevKitM) {
		if(iLedBit == LOW)	neopixelWrite(iNeoPix, 0x00, 0x00, 0x00);
		else				neopixelWrite(iNeoPix, 0x40, 0x00, 0x80);
	} else digitalWrite(iNeoPix, iLedBit ^ 1);
}
//==============================================================================//


//==============================================================================//
static Uint32 _kbhit(void) {
	TransMove();
	return(SliceData(False));
}
//------------------------------------------------------------------------------//
static Uint08 _getch(void) {
	while(!SliceData(False)) TransMove();
	return(SliceData(True));
}
//------------------------------------------------------------------------------//
static Uint08 _getche(void) {
	Uint08 iData;

	TransData(iData = _getch());
	TransMove();

	return(iData);
}
//------------------------------------------------------------------------------//
static void _putch(Uint08 iData) {
	TransData(iData);
	TransMove();
}
//------------------------------------------------------------------------------//
static void _clrscr(void) {
	TransString("\e[H\e[J");
	TransMove();
}
//==============================================================================//


//==============================================================================//
#endif
//==============================================================================//

