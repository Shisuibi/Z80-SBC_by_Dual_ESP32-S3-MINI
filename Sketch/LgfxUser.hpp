
//==============================================================================//
//	Z80 Single Board Computer "PC-84C0SD 20MHz" LGFX User Setting Instance		//
//				Implemented by Shisuibi --Grand Master Sorcerian--				//
//==============================================================================//


//==============================================================================//
#ifdef		UpperDefinition
//==============================================================================//


//==============================================================================//
class LGFX: public lgfx::LGFX_Device {
//------------------------------------------------------------------------------//
	lgfx::Panel_ST7796		_panel_instance;
	lgfx::Bus_SPI			_spibs_instance;

	lgfx::Light_PWM			_light_instance;
	lgfx::Touch_FT5x06		_touch_instance;
//------------------------------------------------------------------------------//
public:
//------------------------------------------------------------------------------//
	LGFX(void) {
		LgfxUserPanel();
		LgfxUserSpiBs();

		LgfxUserLight();
		LgfxUserTouch();

		setPanel(&_panel_instance);
	}
//------------------------------------------------------------------------------//
private:
//------------------------------------------------------------------------------//
	inline void LgfxUserPanel(void);
	inline void LgfxUserSpiBs(void);

	inline void LgfxUserLight(void);
	inline void LgfxUserTouch(void);
//------------------------------------------------------------------------------//
};
//==============================================================================//


//==============================================================================//
#endif
//------------------------------------------------------------------------------//
#ifdef		LowerDefinition
//==============================================================================//


//==============================================================================//
inline void LGFX::LgfxUserPanel(void) {
	auto cfg				=	_panel_instance.config();

	cfg.pin_cs				=	GpioLcdScs;
	cfg.pin_rst				=	GpioLcdRst;
	cfg.pin_busy			=	-1;

	cfg.readable			=	true;
	cfg.bus_shared			=	true;
	cfg.invert				=	true;

	_panel_instance.config(cfg);
}
//------------------------------------------------------------------------------//
inline void LGFX::LgfxUserSpiBs(void) {
	auto cfg				=	_spibs_instance.config();

	cfg.spi_host			=	SPI2_HOST;
	cfg.freq_write			=	40000000;
	cfg.freq_read			=	20000000;

	cfg.pin_mosi			=	GpioLcdSdi;
	cfg.pin_sclk			=	GpioLcdSck;
	cfg.pin_miso			=	GpioLcdSdo;

	cfg.pin_dc				=	GpioLcdDcr;
	cfg.spi_3wire			=	false;
	cfg.use_lock			=	true;
	cfg.dma_channel			=	SPI_DMA_CH_AUTO;

	_spibs_instance.config(cfg);
	_panel_instance.setBus(&_spibs_instance);
}
//------------------------------------------------------------------------------//
inline void LGFX::LgfxUserLight(void) {
	auto cfg				=	_light_instance.config();

	cfg.pin_bl				=	GpioLcdLed;
	cfg.freq				=	44100;

	_light_instance.config(cfg);
	_panel_instance.setLight(&_light_instance);
}
//------------------------------------------------------------------------------//
inline void LGFX::LgfxUserTouch(void) {
	auto cfg				=	_touch_instance.config();

	cfg.i2c_port			=	1;
	cfg.i2c_addr			=	0x38;
	cfg.freq				=	400000;

	cfg.pin_sda				=	GpioCtpSda;
	cfg.pin_scl				=	GpioCtpScl;
	cfg.pin_int				=	GpioCtpInt;

	_touch_instance.config(cfg);
	_panel_instance.setTouch(&_touch_instance);
}
//==============================================================================//


//==============================================================================//
#endif
//==============================================================================//

