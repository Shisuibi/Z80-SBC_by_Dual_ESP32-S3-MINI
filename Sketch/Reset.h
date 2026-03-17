
//==============================================================================//
//	Z80 Single Board Computer "PC-84C0SD 20MHz" I/O Sketch for ESP32-S3-MINI-1	//
//				Implemented by Shisuibi ～Grand Master Sorcerian～				//
//==============================================================================//


//==============================================================================//
#ifdef		UpperDefinition
//==============================================================================//


//==============================================================================//
enum {
	ResetModeStandBy  ,									//	再起動モード（待機）
	ResetModeSynchWait,									//	再起動モード（同期）
	ResetModeBootExec ,									//	再起動モード（実行）

	ResetModeMax,										//	再起動モード上限
};
//------------------------------------------------------------------------------//
#define		CpuCtrlHALT					0x80			//	CPU制御ビット（HALT）
#define		CpuCtrlBSAK					0x40			//	CPU制御ビット（BUSACK）

#define		CpuCtrlRFSH					0x20			//	CPU制御ビット（RFSH）
#define		CpuCtrlM1					0x10			//	CPU制御ビット（M1）

#define		CpuCtrlIORQ					0x08			//	CPU制御ビット（IORQ）
#define		CpuCtrlMREQ					0x04			//	CPU制御ビット（MREQ）

#define		CpuCtrlWR					0x02			//	CPU制御ビット（WR）
#define		CpuCtrlRD					0x01			//	CPU制御ビット（RD）
//------------------------------------------------------------------------------//
static Sint08 iResetRequest;							//	リセット要求
//==============================================================================//


//==============================================================================//
#endif
//------------------------------------------------------------------------------//
#ifdef		LowerDefinition
//==============================================================================//


//==============================================================================//
static void ResetGpio(Sint08 iReset) {
	if(Esp32Master) {
		if(iReset == False) {
			CpuClkOutput();		CpuClkLow();
			DataBusInput();		PortBusInput();

			PioReaInput();	PioWriInput();	PioMemInput();	PioReqInput();
			PioFetInput();	PioRfsInput();	PioBusInput();	PioHalInput();

			CpuIntOutput();		CpuNmiOutput();		PioWitOutput();		RamChpOutput();
			CpuWitOutput();		CpuRstOutput();		BusReqOutput();		NeoPixOutput();
		}

		CpuIntHigh();	CpuNmiHigh();	PioWitLow();	RamChpLow();
		CpuWitHigh();	CpuRstHigh();	BusReqLow();

		NeoPixWrite(0x00, 0x00, 0x00);
	} else {
		if(iReset == False) {
			CtrlBitInput();		AdrsBusInput();
			LowBnkOutput();		BuzTonOutput();		NeoPixOutput();
		}

		LowBnkWrite(0x00);	ClockMelody(0x00, 0x80);
		NeoPixWrite(0x00, 0x00, 0x00);
	}
}
//------------------------------------------------------------------------------//
static void ResetCtrl(Sint08 iReset) {
	BnkPageWrite(0x00);
	IntRqstLow();	NmiRqstLow();	WitEnblHigh();	ChpEnblHigh();

	PioHistWrite(0);	ClkModeWrite(0);	ExeCyclLow();
	iUartTxData = NUL;

	if(iReset == False) BleUartLow();

	SdcBusyLow();	LcdModeHigh();	MsgDispHigh();
	iSysCtrlBit &= 0xCF;	LcdUpdtHigh();	CtrlKeyHigh();

	iAnyNumDt00 = iAnyNumDt01 = iAnyNumDt02 = iAnyNumDt03 = 0x00;
	iCpuAdrsBus = iCpuPortBus = iCpuDataBus = iCpuCtrlBit = 0x00;
}
//------------------------------------------------------------------------------//
static void ResetBoot(void) {
	Uint16 i, j;
	EspDmaInit(True);

	for(i = 0, iCpuAdrsBus = (CpmBootBaseAdrs >> 8);i < RomBootSize;iCpuAdrsBus++) {
		EspDmaPageAdrs();

		for(j = iCpuPortBus = 0x00;j < 0x0100;j++, iCpuPortBus++) {
			iCpuDataBus = pgm_read_byte(aiRomBootImage + 0x0000 + i++);
			EspDmaWrite();
		}
	}

	for(i = 0, iCpuAdrsBus = (CpmBiosBaseAdrs >> 8);i < RomBiosSize;iCpuAdrsBus++) {
		EspDmaPageAdrs();

		for(j = iCpuPortBus = 0x00;j < 0x0100;j++, iCpuPortBus++) {
			iCpuDataBus = pgm_read_byte(aiRomBootImage + 0x0100 + i++);
			EspDmaWrite();
		}
	}

	EspDmaExit(True);
}
//------------------------------------------------------------------------------//
static void ResetExec(void) {
	ClockChange(ClockModeMax - 1);
	while(PioBusRead() != False);
	ClockChange(ClockMode000iHz);		

	CpuWitLow();
	ResetBoot();

	CpuRstLow();
	ClockManual(100);	ClockManual(100);	ClockManual(100);
	CpuRstHigh();
}
//==============================================================================//


//==============================================================================//
static void ResetInit(void) {
	ResetGpio(False);
	ResetCtrl(False);

	iResetRequest = ResetModeSynchWait;
}
//------------------------------------------------------------------------------//
static void ResetMove(void) {
	if(iResetRequest != ResetModeBootExec) return;
	ClockChange(ClockMode000iHz);		

	ResetGpio(True);
	ResetCtrl(True);

	TransClear();
	CpmEmuInit(True);

	MatrixInit();
	VportInit();
	SpiLcdInit(True);

	if(Esp32Master) ResetExec();
	TransMessage(pTransSysReset);

	iResetRequest = ResetModeStandBy;
}
//==============================================================================//


//==============================================================================//
#endif
//==============================================================================//

