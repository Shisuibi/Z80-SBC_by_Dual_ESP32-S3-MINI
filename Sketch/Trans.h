
//==============================================================================//
//	Z80 Single Board Computer "PC-84C0SD 20MHz" I/O Sketch for ESP32-S3-MINI-1	//
//				Implemented by Shisuibi ～Grand Master Sorcerian～				//
//==============================================================================//


//==============================================================================//
#ifdef		UpperDefinition
//==============================================================================//


//==============================================================================//
#define		NUL							0x00			//	ASCIIコード（NUL）
#define		LF							0x0A			//	ASCIIコード（LF）
#define		CR							0x0D			//	ASCIIコード（CR）
#define		SP							0x20			//	ASCIIコード（SP）

#define		CodeSysReset				0x01			//	ASCIIコード（Ctrl+A）
#define		CodeLcdMode					0x07			//	ASCIIコード（Ctrl+G）
#define		CodeHelpText				0x0B			//	ASCIIコード（Ctrl+K）
#define		CodeClrScrn					0x0C			//	ASCIIコード（Ctrl+L）
#define		CodeCtrlKey					0x16			//	ASCIIコード（Ctrl+V）
//------------------------------------------------------------------------------//
#define		SerialBaudRateTx			115200			//	通信端末ボーレート（送信）
#define		SerialBaudRateDx			460800			//	通信端末ボーレート（電文）

#define		SerialSegSizeTx				0x0080			//	通信端末切片文字数（送信）
#define		SerialBufSizeDx				0x0400			//	シリアル緩衝サイズ（電文）
//------------------------------------------------------------------------------//
#define		SerialBufSizeRx				0x0800			//	シリアル緩衝サイズ（受信）
#define		SerialBufSizeTx				0x0400			//	シリアル緩衝サイズ（送信）
//------------------------------------------------------------------------------//
#define		SerialBufMaskRx(iIndex)	\
					((iIndex) & (SerialBufSizeRx - 1))	//	シリアル緩衝マスク（受信）
#define		SerialBufMaskTx(iIndex)	\
					((iIndex) & (SerialBufSizeTx - 1))	//	シリアル緩衝マスク（送信）
//------------------------------------------------------------------------------//
static void RecepData(Uint08);
static void TransMessage(Cint08*);
static void TransControl(Uint08);
//==============================================================================//


//==============================================================================//
#endif
//------------------------------------------------------------------------------//
#ifdef		LowerDefinition
//==============================================================================//


//==============================================================================//
static Cint08* pEscSeqClrScrn = "\e[H\e[J";				//	ESC順序（テキスト画面消去）
//------------------------------------------------------------------------------//
static Cint08* apAssignHelpText[] = {					//	ヘルプ画面テキスト
	"+-----------------------------------------------------------+",
	"|                Ctrl Key Assign (C = Ctrl)                 |",
	"+-----+-----------------------+-----+-----------------------+",
	"| C+A | System Reset          | C+Q | CPU Clock   4[ Hz]    |",
	"| C+S | Message Display       | C+W | CPU Clock  32[ Hz]    |",
	"| C+D | CPU RD/WR History     | C+E | CPU Clock 256[ Hz]    |",
	"| C+F | Execution Cycle       | C+R | CPU Clock   2[KHz]    |",
	"+-----+-----------------------+ C+T | CPU Clock  16[KHz]    |",
	"| C+G | LCD (Segment/Graphic) | C+Y | CPU Clock 128[KHz]    |",
	"| C+V | Disable Ctrl Key      | C+U | CPU Clock   1[MHz]    |",
	"+-----+-----------------------+ C+I | CPU Clock   8[MHz]    |",
	"| C+K | Help (I/O Port/Ctrl)  | C+O | CPU Clock  20[MHz]    |",
	"| C+L | Clear Screen          | C+P | CPU Clock   0[_n_]    |",
	"+-----+-----------------------+-----+-----------------------+",
	"| C+3 | CPU Clock 2.5[MHz]    | C+5 | CPU Clock  10[MHz]    |",
	"| C+4 | CPU Clock   4[MHz]    | C+6 | CPU Clock   6[MHz]    |",
	"+-----+-----------------------+-----+-----------------------+",
	"|                Peripheral I/O Port Assign                 |",
	"+-----+-----------------------+-----+-----------------------+",
	"| 00H | ROM Image/Make Disk   | 10H | Buzzer Melody Note    |",
	"| 01H | System Reset (IN/OUT) | 11H | Buzzer Melody Volume  |",
	"| 02H | BIOS PL/ROM Adrs Low  | 12H | RAM Low Bank Page     |",
	"| 03H | BIOS PH/ROM Adrs High | 13H | CPU INT Request       |",
	"| 04H | Help Text (IN/OUT)    | 14H | CPU NMI Request       |",
	"| 05H | Clear Screen (IN/OUT) | 15H | CPU Wait Enable       |",
	"| 06H | Message Display       | 16H | RAM Chip Enable       |",
	"+-----+-----------------------+-----+-----------------------+",
	"| 08H | Star Logo (IN/OUT)    | 18H | Timer Centis          |",
	"| 09H | Screen Shot (IN/OUT)  | 19H | Timer Second          |",
	"| 0AH | Disable Ctrl Key      | 1AH | Timer Minute          |",
	"| 0FH | Reserved (Slave Ctrl) | 1BH | Timer Hour24          |",
	"+-----+-----------------------+-----+-----------------------+",
	"| 20H | Any Number 0 (LED R)  | 30H | UART RX/Control       |",
	"| 21H | Any Number 1 (LED G)  | 31H | UART TX/RX Data       |",
	"| 22H | Any Number 2 (LED B)  | 32H | BLE UART (IN Only)    |",
	"| 23H | Any Number 3 (LED:C0) | 33H | SDC Busy (IN Only)    |",
	"| 24H | CPU RD/WR History     | 34H | LCD (Segment/Graphic) |",
	"| 25H | Execution Cycle       | 35H | LCD LED Brightness    |",
	"| 26H | CPU Clock Mode        | 36H | SDC Read BASIC File   |",
	"+-----+-----------------------+-----+-----------------------+",
	"| 40H | LCD Rotation          | 48H | LCD Draw Pixel        |",
	"| 41H | LCD Clear Screen      | 49H | LCD Draw Line         |",
	"| 42H | LCD Text Size         | 4AH | LCD Draw Triangle     |",
	"| 43H | LCD Draw Char         | 4BH | LCD Fill Triangle     |",
	"| 44H | LCD Color RGB C0 Low  | 4CH | LCD Draw Rectangle    |",
	"| 45H | LCD Color RGB C0 High | 4DH | LCD Fill Rectangle    |",
	"| 46H | LCD Color RGB C1 Low  | 4EH | LCD Draw Circle       |",
	"| 47H | LCD Color RGB C1 High | 4FH | LCD Fill Circle       |",
	"+-----+-----------------------+-----+-----------------------+",
	"| 50H | LCD Position X0 Low   | 58H | LCD Pos/Disp X2 Low   |",
	"| 51H | LCD Position X0 High  | 59H | LCD Pos/Disp X2 High  |",
	"| 52H | LCD Position Y0 Low   | 5AH | LCD Pos/Disp Y2 Low   |",
	"| 53H | LCD Position Y0 High  | 5BH | LCD Pos/Disp Y2 High  |",
	"| 54H | LCD Position X1 Low   | 5CH | LCD Pos/Radi X3 Low   |",
	"| 55H | LCD Position X1 High  | 5DH | LCD Pos/Radi X3 High  |",
	"| 56H | LCD Position Y1 Low   | 5EH | LCD Pos/Radi Y3 Low   |",
	"| 57H | LCD Position Y1 High  | 5FH | LCD Pos/Radi Y3 High  |",
	"+-----+-----------------------+-----+-----------------------+",
	"| 80H | CP/M BIOS WBoot       | 88H | CP/M BIOS SelDsk      |",
	"| 81H | CP/M BIOS ConSt       | 89H | CP/M BIOS SetTrk      |",
	"| 82H | CP/M BIOS ConIn       | 8AH | CP/M BIOS SetSec      |",
	"| 83H | CP/M BIOS ConOut      | 8BH | CP/M BIOS SetDma      |",
	"| 84H | CP/M BIOS List        | 8CH | CP/M BIOS Read        |",
	"| 85H | CP/M BIOS Punch       | 8DH | CP/M BIOS Write       |",
	"| 86H | CP/M BIOS Reader      | 8EH | CP/M BIOS ListSt      |",
	"| 87H | CP/M BIOS Home        | 8FH | CP/M BIOS SecTran     |",
	"+-----+-----------------------+-----+-----------------------+",
	NULL,
};
//==============================================================================//


//==============================================================================//
static Cint08* pTransSysReset   = "CB[RST]";			//	システム再起動

static Cint08* pTransMsgDispOff = "MS[OFF]";			//	メッセージ表示（オフ）
static Cint08* pTransMsgDispOn  = "MS[ ON]";			//	メッセージ表示（オン）

static Cint08* pTransExeCyclOff = "EC[OFF]";			//	CPU実行サイクル（オフ）
static Cint08* pTransExeCyclOn  = "EC[ ON]";			//	CPU実行サイクル（オン）

static Cint08* pTransLcdModeOff = "LC[OFF]";			//	液晶表示モード（オフ）
static Cint08* pTransLcdModeOn  = "LC[ ON]";			//	液晶表示モード（オン）

static Cint08* pTransCtrlKeyOff = "CK[OFF]";			//	制御キー操作（オフ）
static Cint08* pTransCtrlKeyOn  = "CK[ ON]";			//	制御キー操作（オン）
//------------------------------------------------------------------------------//
static Cint08* apTransPioHist[4] = {					//	CPU入出力履歴
	"--[A/N]",											//	CPU入出力履歴（任意データ）
	"-M[MEM]",											//	CPU入出力履歴（MREQを表示）
	"P-[PIO]",											//	CPU入出力履歴（IORQを表示）
	"PM[P&M]",											//	CPU入出力履歴（MREQ・IORQ）
};
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
static Uint08 iHelpTextCount;							//	ヘルプ画面表示番号
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
			if((iSynchWait != False)&&((iSerialRxWr - iSerialRxRd) == 1)) MultiData(CodeTelSynchWait);
			return(aiSerialBufRx[iSerialRxRd = SerialBufMaskRx(iSerialRxRd + 1)]);
		}
	} else return(NUL);
}
//------------------------------------------------------------------------------//
static void TransString(Cint08* pString) {
	while(*pString != NUL) TransData(*pString++);
}
//------------------------------------------------------------------------------//
static void TransMessage(Cint08* pMessage) {
	if(MsgDispRead() != False) {
		TransString(pMessage);

		TransData(CR);
		TransData(LF);
	}
}
//------------------------------------------------------------------------------//
static void TransChangeHist(Uint08 iPioHist) {
	Uint08 iPrevHist = PioHistRead();
	Uint08 iCurrHist;	PioHistWrite(iPioHist);

	if((iCurrHist = PioHistRead()) == iPrevHist) return;
	TransMessage(apTransPioHist[iCurrHist]);

	if((iPrevHist != 0)&&(iCurrHist == 0)) {
		iPioHistD01 = iAnyNumDt01;	iPioHistD02 = iAnyNumDt02;
		iPioHistD03 = iAnyNumDt03;	return;
	}

	if((iPrevHist != 0)&&(iCurrHist == 3)) return;
	if((iPrevHist == 3)&&(iCurrHist != 0)) return;

	iPioHistD01 = iPioHistD02 = 0x00;
	iPioHistD03 &= 0x30;
}
//==============================================================================//


//==============================================================================//
static void TransSysReset(void) {
	iResetRequest = ResetModeSynchWait;
}
//------------------------------------------------------------------------------//
static void TransMsgDisp(void) {
	if(MsgDispRead() != False)	{	TransMessage(pTransMsgDispOff);		MsgDispLow();	}
	else						{	MsgDispHigh();	TransMessage(pTransMsgDispOn);	}
}
//------------------------------------------------------------------------------//
static void TransPioHist(void) {
	TransChangeHist(PioHistRead() + 1);
}
//------------------------------------------------------------------------------//
static void TransExeCycl(void) {
	if(ExeCyclRead() != False)	{	TransMessage(pTransExeCyclOff);		ExeCyclLow();	}
	else						{	ExeCyclHigh();	TransMessage(pTransExeCyclOn);	}
}
//------------------------------------------------------------------------------//
static void TransLcdMode(void) {
	if(LcdModeRead() != False)	{	TransMessage(pTransLcdModeOff);		LcdModeLow();	}
	else						{	LcdModeHigh();	TransMessage(pTransLcdModeOn);	}
}
//------------------------------------------------------------------------------//
static void TransCtrlKey(void) {
	if(CtrlKeyRead() != False)	{	TransMessage(pTransCtrlKeyOff);		CtrlKeyLow();	}
	else						{	CtrlKeyHigh();	TransMessage(pTransCtrlKeyOn);	}
}
//------------------------------------------------------------------------------//
static void TransHelpText(void) {
	if(iHelpTextCount == 0xFF) iHelpTextCount = 0x00;
}
//------------------------------------------------------------------------------//
static void TransClrScrn(void) {
	TransString(pEscSeqClrScrn);
}
//==============================================================================//


//==============================================================================//
static void (* apSerialCtrl[SP])(void) = {
	NULL         ,	TransSysReset,	NULL         ,	NULL         ,
	TransPioHist ,	Clock256iHz  ,	TransExeCycl ,	TransLcdMode ,
	NULL         ,	Clock008MHz  ,	NULL         ,	TransHelpText,
	TransClrScrn ,	NULL         ,	NULL         ,	Clock020MHz  ,

	Clock000iHz  ,	Clock004iHz  ,	Clock002KHz  ,	TransMsgDisp ,
	Clock016KHz  ,	Clock001MHz  ,	TransCtrlKey ,	Clock032iHz  ,
	NULL         ,	Clock128KHz  ,	NULL         ,	Clock2p5MHz  ,
	Clock004MHz  ,	Clock010MHz  ,	Clock006MHz  ,	NULL         ,
};
//------------------------------------------------------------------------------//
static void TransControl(Uint08 iData) {
	(* apSerialCtrl[iData])();
}
//------------------------------------------------------------------------------//
static void TransCheck(Uint08 iData) {
	if((iData < SP)&&(apSerialCtrl[iData] != NULL)) {
		if((CtrlKeyRead() != False)||(iData == CodeSysReset)||(iData == CodeCtrlKey)) {
			MultiData(CodeTelSerCtrl);	MultiData(iData);
			TransControl(iData);
			return;
		}
	}

	if(Esp32Master)		RecepData(iData);
	else				MultiKeep(iData);
}
//==============================================================================//


//==============================================================================//
class BleServerCB: public BLEServerCallbacks {
	void onConnect(BLEServer* pBleServer) {
		BleUartHigh();	MultiData(CodeTelBleUart);
		iSerialTxRd = iSerialTxWr;
	}

	void onDisconnect(BLEServer* pBleServer) {
		BleUartLow();	MultiData(CodeTelBleUart);
		iBleAdvtReq = True;
	}
};
//------------------------------------------------------------------------------//
class BleCharaCB: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic* pBleChara) {
		String sValue = pBleChara->getValue();
		Sint16 i;

		for(i = 0;i < sValue.length();i++) {
			TransCheck(sValue[i]);
		}
	}
};
//==============================================================================//


//==============================================================================//
static void TransClear(void) {
	iSerialRxWr = iSerialRxRd = 0;
	iSerialTxWr = iSerialTxRd = 0;

	iHelpTextCount = 0xFF;
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
		iUartTxData = *pData;	Serial.write(pData, iSize);

		if((Esp32Master)&&(BleUartRead() != False)) {
			pBleCharaTx->setValue(pData, iSize);
			pBleCharaTx->notify();
		}

		iSerialTxRd = SerialBufMaskTx(iSerialTxRd + iSize);
		iSerialMicros = micros();
	}

	if((iHelpTextCount != 0xFF)&&(TransInterval() != False)) {
		if(apAssignHelpText[iHelpTextCount] != NULL)	TransMessage(apAssignHelpText[iHelpTextCount++]);
		else											iHelpTextCount = 0xFF;

		iSerialMicros = micros();
	}
}
//------------------------------------------------------------------------------//
static void TransRead(void) {
	Sint16 i;

	for(i = 0;i < SerialSegSizeTx;i++) {
		if(Serial.available() == 0) break;
		TransCheck(Serial.read());
	}

	if(Esp32Slave) MultiSend();
}
//==============================================================================//


//==============================================================================//
static void TransInit(void) {
	Serial.end();
	Serial.setRxBufferSize(SerialBufSizeTx);
	Serial.setTxBufferSize(SerialBufSizeTx);
	Serial.begin(SerialBaudRateTx, SERIAL_8N1, GpioUa0Rxd, GpioUa0Txd);

	delay(100);		MultiFlush();	delay(100);
	TransClear();	iBleAdvtReq = True;

	iSerialMicros = micros();
	if(Esp32Master) TransBleStart();
}
//------------------------------------------------------------------------------//
static void TransMove(void) {
	if(iBleAdvtReq != False) {
		if(Esp32Master) pBleServer->startAdvertising();
		iBleAdvtReq = False;
	}

	TransWrite();
	TransRead();
}
//==============================================================================//


//==============================================================================//
#endif
//==============================================================================//

