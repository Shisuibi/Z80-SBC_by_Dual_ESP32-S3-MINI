
//==============================================================================//
//		Z80 Single Board Computer "PC-84C0xx 20MHz" HEX to ROM Converter		//
//				Implemented by Shisuibi --Grand Master Sorcerian--				//
//==============================================================================//


//==============================================================================//
#include	"Hex2Rom.h"
//------------------------------------------------------------------------------//
static Uint08* piCurRec;

static Uint08 aiRecData[RecSize];
static Uint08 aiRomData[RomSize];
//------------------------------------------------------------------------------//
static Uint32 iMinAdrs, iMaxAdrs;
//==============================================================================//


//==============================================================================//
static void RecInit(void) {
	while((piCurRec = (Uint08*)gets(aiRecData)) != NULL) {
		if(*piCurRec++ != ':') continue;
		break;
	}
}
//------------------------------------------------------------------------------//
static Uint08 RecMove(void) {
	Uint08 iNum, iData;

	if(piCurRec == NULL) return(0x00);
	if((iNum = piCurRec[0] - '0') > 0x09) iNum -= 'A' - '9' - 1;

	iData = iNum << 4;
	if((iNum = piCurRec[1] - '0') > 0x09) iNum -= 'A' - '9' - 1;

	iData |= iNum;
	piCurRec += 2;

	return(iData);
}
//==============================================================================//


//==============================================================================//
static void RomInit(void) {
	for(iMinAdrs = iMaxAdrs = 0x0000;iMinAdrs < RomSize;iMinAdrs++) {
		aiRomData[iMinAdrs] = RomDataFix;
	}
}
//------------------------------------------------------------------------------//
static void RomMove(void) {
	Sint08 i;

	while(iMinAdrs <= iMaxAdrs) {
		if((iMinAdrs & 0x00FF) == 0x0000) printf("\n");
		printf("\t");

		for(i = 0x00;i < 0x10;i++) {
			printf("0x%02X,", aiRomData[iMinAdrs++]);
		}

		printf("\t//\t%04XH\n", iMinAdrs - 0x0010);
	}
}
//==============================================================================//


//==============================================================================//
void main(void) {
	Uint08 iCount, iType;
	Uint32 iAdrs;

	RomInit();

	while(True) {
		RecInit();	iCount = RecMove();
		iAdrs = (Uint32)RecMove();	iAdrs = (iAdrs << 8) | (Uint32)RecMove();

		if((iType = RecMove()) == RecTypeEOF) break;
		if(iType != RecTypeData) continue;

		while(iCount--) {
			if(iAdrs < iMinAdrs) iMinAdrs =  iAdrs & 0xFFF0;
			if(iAdrs > iMaxAdrs) iMaxAdrs = (iAdrs & 0xFFF0) + 0x000F;

			aiRomData[iAdrs++] = RecMove();
		}
	}

	RomMove();
}
//==============================================================================//

