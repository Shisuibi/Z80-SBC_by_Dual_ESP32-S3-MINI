
//==============================================================================//
//	Z80 Single Board Computer "PC-84C0SD 20MHz" I/O Sketch for ESP32-S3-MINI-1	//
//				Implemented by Shisuibi ～Grand Master Sorcerian～				//
//==============================================================================//


//==============================================================================//
#ifdef		UpperDefinition
//==============================================================================//


//==============================================================================//
#define		MatrixStackMax				0x10			//	行列演算スタック上限
//------------------------------------------------------------------------------//
#define		DegToRad(fAng)	(2.0 * PI * ((fAng) / 360.0))	//	度数弧度変換
//==============================================================================//


//==============================================================================//
#endif
//------------------------------------------------------------------------------//
#ifdef		LowerDefinition
//==============================================================================//


//==============================================================================//
static Sint08 iCurrMatrix;								//	現行行列演算
//------------------------------------------------------------------------------//
static Sflt32 afMatrixStack[MatrixStackMax][XYZW][XYZW];	//	行列演算スタック
//==============================================================================//


//==============================================================================//
static void MatrixPush(void) {
	if(iCurrMatrix < (MatrixStackMax - 1)) iCurrMatrix++;
}
//------------------------------------------------------------------------------//
static void MatrixPop(void) {
	if(iCurrMatrix > 0) iCurrMatrix--;
}
//------------------------------------------------------------------------------//
static void MatrixUnit(void) {
	Sint08 i, j;
	Sflt32 (* pMat)[XYZW] = afMatrixStack[iCurrMatrix];

	for(i = 0;i < XYZW;i++) {
		for(j = 0;j < XYZW;j++) {
			if(i == j)	pMat[i][j] = 1.0;
			else		pMat[i][j] = 0.0;
		}
	}
}
//------------------------------------------------------------------------------//
static void MatrixPers(Sflt32 fRad, Sflt32 fNea, Sflt32 fFar) {
	Sflt32 fPer = 1.0 / tan(fRad / 2.0);
	Sflt32 fDis = fFar - fNea;
	Sflt32 (* pMat)[XYZW] = afMatrixStack[iCurrMatrix];

	pMat[X][X] = fPer;	pMat[X][Y] =		pMat[X][Z] =								pMat[X][W] = 0.0;
	pMat[Y][X] = 0.0;	pMat[Y][Y] = fPer;	pMat[Y][Z] =								pMat[Y][W] = 0.0;
	pMat[Z][X] =		pMat[Z][Y] = 0.0;	pMat[Z][Z] =  (      fFar + fNea) / fDis;	pMat[Z][W] = 1.0;
	pMat[W][X] =		pMat[W][Y] = 0.0;	pMat[W][Z] = -(2.0 * fFar * fNea) / fDis;	pMat[W][W] = 0.0;
}
//------------------------------------------------------------------------------//
static void MatrixCopy(void) {
	Sint08 i, j;
	Sflt32 (* pSrc)[XYZW], (* pDst)[XYZW];

	if(iCurrMatrix < 1) return;

	pSrc = afMatrixStack[iCurrMatrix - 1];
	pDst = afMatrixStack[iCurrMatrix    ];

	for(i = 0;i < XYZW;i++) {
		for(j = 0;j < XYZW;j++) {
			pDst[i][j] = pSrc[i][j];
		}
	}
}
//------------------------------------------------------------------------------//
static void MatrixMulti(void) {
	Sint08 i, j;
	Sflt32 (* pSrc)[XYZW], (* pDst)[XYZW];
	Sflt32 (* pMat)[XYZW] = afMatrixStack[iCurrMatrix];

	if(iCurrMatrix < 2) return;

	pSrc = afMatrixStack[iCurrMatrix - 2];
	pDst = afMatrixStack[iCurrMatrix - 1];

	for(i = 0;i < XYZW;i++) {
		for(j = 0;j < XYZW;j++) {
			pMat[i][j] =	pDst[i][X] * pSrc[X][j] + pDst[i][Y] * pSrc[Y][j] +
							pDst[i][Z] * pSrc[Z][j] + pDst[i][W] * pSrc[W][j];
		}
	}
}
//------------------------------------------------------------------------------//
static void MatrixTrans(Sflt32* pVec) {
	Sint08 i;
	Sflt32 (* pMat)[XYZW] = afMatrixStack[iCurrMatrix];

	for(i = 0;i < XYZW;i++) {
		pMat[W][i] =	pVec[X] * pMat[X][i] + pVec[Y] * pMat[Y][i] +
						pVec[Z] * pMat[Z][i] + pVec[W] * pMat[W][i];
	}
}
//------------------------------------------------------------------------------//
static void MatrixScale(Sflt32* pVec) {
	Sint08 i, j;
	Sflt32 (* pMat)[XYZW] = afMatrixStack[iCurrMatrix];

	for(i = 0;i < XYZW;i++) {
		for(j = 0;j < XYZW;j++) {
			pMat[i][j] =  pVec[i] * pMat[i][j];
		}
	}
}
//------------------------------------------------------------------------------//
static void MatrixRotateX(Sflt32 fRad) {
	Sint08 i;
	Sflt32 (* pMat)[XYZW] = afMatrixStack[iCurrMatrix];
	Sflt32 fTmp, fSin = sin(fRad), fCos = cos(fRad);

	for(i = 0;i < XYZW;i++) {
		fTmp = pMat[Y][i];	pMat[Y][i] = fCos * fTmp - fSin * pMat[Z][i];
							pMat[Z][i] = fSin * fTmp + fCos * pMat[Z][i];
	}
}
//------------------------------------------------------------------------------//
static void MatrixRotateY(Sflt32 fRad) {
	Sint08 i;
	Sflt32 (* pMat)[XYZW] = afMatrixStack[iCurrMatrix];
	Sflt32 fTmp, fSin = sin(fRad), fCos = cos(fRad);

	for(i = 0;i < XYZW;i++) {
		fTmp = pMat[Z][i];	pMat[Z][i] = fCos * fTmp - fSin * pMat[X][i];
							pMat[X][i] = fSin * fTmp + fCos * pMat[X][i];
	}
}
//------------------------------------------------------------------------------//
static void MatrixRotateZ(Sflt32 fRad) {
	Sint08 i;
	Sflt32 (* pMat)[XYZW] = afMatrixStack[iCurrMatrix];
	Sflt32 fTmp, fSin = sin(fRad), fCos = cos(fRad);

	for(i = 0;i < XYZW;i++) {
		fTmp = pMat[X][i];	pMat[X][i] = fCos * fTmp - fSin * pMat[Y][i];
							pMat[Y][i] = fSin * fTmp + fCos * pMat[Y][i];
	}
}
//==============================================================================//


//==============================================================================//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
static Sflt32 afModelVertex[8][XYZW] = {
	{	-1.0,	-1.0,	-1.0,	 1.0,	},
	{	 1.0,	-1.0,	-1.0,	 1.0,	},
	{	-1.0,	 1.0,	-1.0,	 1.0,	},
	{	 1.0,	 1.0,	-1.0,	 1.0,	},
	{	-1.0,	-1.0,	 1.0,	 1.0,	},
	{	 1.0,	-1.0,	 1.0,	 1.0,	},
	{	-1.0,	 1.0,	 1.0,	 1.0,	},
	{	 1.0,	 1.0,	 1.0,	 1.0,	},
};
//------------------------------------------------------------------------------//
static Uint08 aiModelPolygon[12][4] = {
	{	0,	3,	1,	0x00,	},
	{	0,	2,	3,	0x00,	},
	{	1,	7,	5,	0x00,	},
	{	1,	3,	7,	0x00,	},
	{	2,	4,	6,	0x00,	},
	{	2,	0,	4,	0x00,	},
	{	3,	6,	7,	0x00,	},
	{	3,	2,	6,	0x00,	},
	{	4,	1,	5,	0x00,	},
	{	4,	0,	1,	0x00,	},
	{	5,	6,	4,	0x00,	},
	{	5,	7,	6,	0x00,	},
};
//------------------------------------------------------------------------------//
static Uint16 aiModelMaterial[12][2] = {
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
	{	0x041F,		0x0000,		},
};
//------------------------------------------------------------------------------//
//			6----7
//		   /|   /|
//		  / |  / |
//		 /  4-/--5
//		2----3  /		Y  Z
//		| /  | /		| /
//		|/   |/			|/__X
//		0----1
//------------------------------------------------------------------------------//
static Sflt32 afCalcVertex[8][XYZW];
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//------------------------------------------------------------------------------//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//==============================================================================//


//==============================================================================//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
static void MatrixVertex(void) {
	Sint08 i, j;
	Sflt32 (* pMat)[XYZW] = afMatrixStack[iCurrMatrix];
	Sflt32 fTmp;

	for(i = 0;i < 8;i++) {
		for(j = 0;j < XYZW;j++) {
			afCalcVertex[i][j] =	afModelVertex[i][X] * pMat[X][j] + afModelVertex[i][Y] * pMat[Y][j] +
									afModelVertex[i][Z] * pMat[Z][j] + afModelVertex[i][W] * pMat[W][j];
		}

		fTmp = 1.0 / afCalcVertex[i][W];	afCalcVertex[i][W] = 1.0;
		for(j = 0;j < XYZ;j++) afCalcVertex[i][j] *= fTmp;
	}
}
//------------------------------------------------------------------------------//
static void MatrixDraw(void) {
	Sflt32* pV0, * pV1, * pV2;
	Sint08 i;

	SpiLCD.startWrite();
	SpiLCD.fillScreen(iLcdRgbC1);

	for(i = 0;i < 12;i++) {
		pV0 = afCalcVertex[aiModelPolygon[i][0]];
		pV1 = afCalcVertex[aiModelPolygon[i][1]];
		pV2 = afCalcVertex[aiModelPolygon[i][2]];

		SpiLCD.drawTriangle(	(Sint16)pV0[X], (Sint16)pV0[Y], (Sint16)pV1[X], (Sint16)pV1[Y],
								(Sint16)pV2[X], (Sint16)pV2[Y], aiModelMaterial[i][0]	);
	}

	SpiLCD.endWrite();
}
//------------------------------------------------------------------------------//
static void MatrixPrint(void) {
	Sint08 i, j;
	Sflt32 (* pMat)[XYZW] = afMatrixStack[iCurrMatrix];

	for(i = 0;i < XYZW;i++) {
		for(j = 0;j < XYZW;j++) {
			Serial.printf("%f | ", pMat[i][j]);
		}

		Serial.println();
	}

	Serial.println();
}
//------------------------------------------------------------------------------//
static void VertexPrint(void) {
	Sint08 i, j;

	for(i = 0;i < 8;i++) {
		for(j = 0;j < XYZW;j++) {
			Serial.printf("%f | ", afCalcVertex[i][j]);
		}

		Serial.println();
	}

	Serial.println();
}
//------------------------------------------------------------------------------//
static void MatrixModel(void) {
	static Uint08 iDeg = 0;

	MatrixPers(DegToRad(45.0), 1.0, 100.0);
	MatrixPush();	MatrixMulti();

	afCalcVertex[0][X] = 0.0;	afCalcVertex[0][Y] = 0.0;	afCalcVertex[0][Z] = 8.0;	afCalcVertex[0][W] = 1.0;
	MatrixTrans(afCalcVertex[0]);

	MatrixRotateX(DegToRad(360.0 * ((Sflt32)iDeg   * 1.0 / 256.0)));
	MatrixRotateY(DegToRad(360.0 * ((Sflt32)iDeg   * 2.0 / 256.0)));
	MatrixRotateZ(DegToRad(360.0 * ((Sflt32)iDeg++ * 3.0 / 256.0)));

	MatrixVertex();
	MatrixPop();

	MatrixDraw();
};
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//------------------------------------------------------------------------------//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//==============================================================================//


//==============================================================================//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
static void MatrixGraphic(void) {
	static Uint16 aiColor[4][6] = {
		{	TFT_BLACK,	TFT_NAVY,		TFT_DARKGREEN,	TFT_DARKCYAN,	TFT_MAROON,		TFT_PURPLE,			},
		{	TFT_OLIVE,	TFT_LIGHTGREY,	TFT_DARKGREY,	TFT_BLUE,		TFT_GREEN,		TFT_CYAN,			},
		{	TFT_RED,	TFT_MAGENTA,	TFT_YELLOW,		TFT_WHITE,		TFT_ORANGE,		TFT_GREENYELLOW,	},
		{	TFT_PINK,	TFT_BROWN,		TFT_GOLD,		TFT_SILVER,		TFT_SKYBLUE,	TFT_VIOLET,			},
	};
	Sint16 i, j;

	SpiLCD.startWrite();

	for(i = 0;i < 4;i++) {
		for(j = 0;j < 6;j++) {
			Canvas.writeFillRect(j * 60, i * 60, 60, 60, aiColor[i][j]);
		}
	}

	SpiLCD.endWrite();
}
//------------------------------------------------------------------------------//
static void MatrixFrameRate(void) {
	static Uint32 iCount = 0;
	static Uint32 iFrame = 0;
	static Uint32 iMatrixMicros = 0;
	Uint32 iMicros = micros();

	if(LcdModeSegment) return;

	iFrame += 1000000 / (iMicros - iMatrixMicros);
	iMatrixMicros = iMicros;

	if(++iCount >= 100) {
		Serial.printf("[FPS %02X]", iFrame / 100);
		iCount = iFrame = 0;
	}
}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//------------------------------------------------------------------------------//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//==============================================================================//


//==============================================================================//
static void MatrixInit(void) {
	if(Esp32Master) return;
	iCurrMatrix = 0;	MatrixUnit();

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
/*@@@@
	afCalcVertex[0][X] = (Sflt32)(LcdScrnPixelX >> 1);	afCalcVertex[0][Y] = (Sflt32)(LcdScrnPixelY >> 1);
	afCalcVertex[0][Z] = afCalcVertex[0][W] = 1.0;	MatrixTrans(afCalcVertex[0]);

	afCalcVertex[0][Y] = -afCalcVertex[0][X];		MatrixScale(afCalcVertex[0]);
	MatrixPush();

	MatrixGraphic();
@@@@*/
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
}
//------------------------------------------------------------------------------//
static void MatrixMove(void) {
	if((Esp32Master)||(LcdModeSegment)) return;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
/*@@@@
	MatrixModel();

	MatrixFrameRate();
@@@@*/
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
}
//==============================================================================//


//==============================================================================//
#endif
//==============================================================================//

