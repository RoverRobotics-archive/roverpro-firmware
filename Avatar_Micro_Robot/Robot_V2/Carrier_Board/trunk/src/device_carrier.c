/**
 * @file device_carrier.c
 * @author J. Brinton
 * @author Robotex, Inc.
 *
 * Robotex carrier PIC firmware.
 *
 * DEVICE MAPPING
 * Hardware Revision 30 (Robotex Robot Carrier Board)
 *
 * ----------------------------------------------------------------------------
 * DIGITAL OUTPUTS
 * ----------------------------------------------------------------------------
 * NAME                 PIN PORT  DEF FUNCTION
 * ------------------------------
 * EN_SMBPWR            4   RG6   0   EN +3.3V_SMB, HW pulldown
 * CODEC_PD# (EN_AUDIO) 5   RG7   0   EN +3.3V_AUDIO and +5V_AUDIO, HW pulldown
 * 12V_ENABLE (EN_12V#) 6   RG8   1   EN +12V supply, HW pullup
 * EN_RADIOBRD          8   RG9   0   EN +3.3V_RBRD, HW pulldown
 * NCOM_WAKE0#          30  RB15  1   COMEx PCIE wake (pullup on COMEx)
 * NCOM_SYSRST#         22  RB9   Z   Rst push-button on PCB, (pullup on COMEx & PCB), don't override button
 * EN_COMXPRESS         59  RF1   0   EN 3.3V to +3.3V_C-XPRS, HW pulldown
 * NCOM_WAKE1#          58  RF0   1   COMEx AUX Wake (pull-up on COMEx)
 * PWR_OK               23  RB10  0   COMEx power good signal
 * EN_PAYLOAD2          24  RB11  0   EN raw power to payload #2, HW pulldown
 * EN_PAYLOAD1          27  RB12  0   EN raw power to payload #1, HW pulldown
 * EN_MOTORCNTL         28  RB13  0   EN 5V to Motor Controller, HW pulldown
 * 3V3_ENABLE           48  RC14  0   EN +3.3V_ALWS, HW pulldown
 * 5V_ENABLE            32  RF5   0   EN +5V, HW pulldown
 * EN_GPS               31  RF4   0   EN 3.3V to +3.3V_GPS, HW pulldown
 * PWRBTN#              42  RD8   Z   Pwr push-button on PCB, (pull-up on COMEx & PCB), don't override button
 * PWM_IRLED            51  RD3   0   Straight out to Camera Board 
 * PWM_LED              52  RD4   0   Straight out to Camera Board
 * EN_CAMERABRD         2   RE6   0   EN 5V, 3.3V and 12V to Camera Board
 *
 * ---------------------------------------------------------------------------
 * DIGITAL INPUTS
 * ---------------------------------------------------------------------------
 * PIC_WAKE0#           29  RB14  Z   wake from radio PCIE
 * PAYLOAD1_PRESENT#    21  RB8   Z   indicates payload #1 connected
 * PAYLOAD2_PRESENT#    13  RB3   Z   indicates payload #2 connected
 * COMPASS_DRDY#        14  RB2   Z   indicates compass data ready (check!)
 * PWR_BTN#             12  RB4   Z   Pwr push-button on PCB, (pull-up on COMEx & PCB), don't override button
 * SUS_S5#              11  RB5   Z   indicates system is suspended to disk
 * SMB_ALERT#           47  RC13  Z   indicates message pending on SMBus, HW pullup
 * 33V_ALWS_GOOD        49  RD1   Z   if not good probably not alive
 * 12V_GOOD             50  RD2   Z   12V main good signal
 * 5V_PGOOD             54  RD6   Z   indicates 5V is good
 * SUS_S3#              1   RE5   Z   indicates system is suspended to RAM
 * NC_THRMTRIP#         3   RE7   Z   indicates thermal problem with COMEx
 *
 * ---------------------------------------------------------------------------
 * ANALOG INPUTS
 * ---------------------------------------------------------------------------
 * EX_TEMP_SNS          16   AN0  Z   pulled to 3.3V with 100k
 * HUMIDITY_SENSOR      15   AN1  Z   humidity sensor
 *
 *
 */

#include "stdhdr.h"
#include "device_carrier.h"

#define V3V3_ON(a)               _LATC14    = a
#define V3V3_EN(a)               _TRISC14 = !a

#define V5_ON(a)                 _LATG8     = a
#define V5_EN(a)                 _TRISG8  = !a

#define V12_ON(a)                _LATE6     = !a // inverse logic
#define V12_EN(a)                _TRISE6  = !a

#define VBAT_DIGI_EN(a)			_TRISB11 = !a
#define VBAT_DIGI_ON(a)			_LATB11 = a

#define V5_PGOOD()				_RD6
#define V12_PGOOD()				_RD2

#define V3V3_PGOOD()			_RD1	

#define AMP_PWR_EN(a)			_TRISD7 = !a
#define AMP_PWR_ON(a)			_LATD7 = a

#define COM_EXPRESS_PGOOD_EN(a)	_TRISB10 = !a
#define COM_EXPRESS_PGOOD_ON(a)	_LATB10 = a

#define MIC_PWR_EN(a)			_TRISG6 = !a
#define MIC_PWR_ON(a)			_LATG6 = a

#define CODEC_PWR_EN(a)			_TRISG6 = !a
#define CODEC_PWR_ON(a)			_LATG6 = a


#define SUS_S5()					_RB5
#define SUS_S3()					_RE5




//I haven't looked at anything below this
/*
#define COM_EXPRESS_ON(a)        _LATF1     = a
#define COM_EXPRESS_EN(a)        _TRISF1  = !a

#define COM_EXPRESS_PG_ON(a)     _LATB10    = a // PWR_OK
#define COM_EXPRESS_PG_EN(a)     _TRISB10 = !a

#define GPS_ON(a)                _LATF4     = a
#define GPS_EN(a)                _TRISF4  = !a

#define RADIO_ON(a)              _LATG9     = a
#define RADIO_EN(a)              _TRISG9  = !a

#define CAMERA_BOARD_ON(a)       _LATE6     = a
#define CAMERA_BOARD_EN(a)       _TRISE6  = !a

#define SMBUS_ON(a)              _LATG6     = a
#define SMBUS_EN(a)              _TRISG6  = !a

#define AUDIO_ON(a)              _LATG7     = !a
#define AUDIO_EN(a)              _TRISG7  = a

#define NCOM_WAKE0_ON(a)         _LATB15    = !a
#define NCOM_WAKE0_EN(a)         _TRISB15 = !a

#define NCOM_WAKE1_ON(a)         _LATF0     = !a
#define NCOM_WAKE1_EN(a)         _TRISF0  = !a

#define NCOM_SYSRST_ON(a)        _LATB9     = !a
#define NCOM_SYSRST_EN(a)        _TRISB9  = !a

#define PWR_BTN_ON(a)             _LATD8    = !a
#define PWR_BTN_EN(a)             _TRISD8 = !a

#define PAYLOAD1_PWR_ON(a)       _LATB12    = a
#define PAYLOAD1_PWR_EN(a)       _TRISB12 = !a

#define PAYLOAD2_PWR_ON(a)       _LATB11    = a
#define PAYLOAD2_PWR_EN(a)       _TRISB11 = !a

#define MOTOR_CONTROLLER_ON(a)   _LATB13    = a
#define MOTOR_CONTROLLER_EN(a)   _TRISB13 = !a

#define PWM_IRLED_ON(a)          _LATD3    = !a // digital PWM output 100% duty cycle
#define PWM_IRLED_EN(a)          _TRISD3 = !a

#define PWM_LED_ON(a)            _LATD4    = !a
#define PWM_LED_EN(a)            _TRISD4 = !a


#define PIC_WAKE0()              (!_RB14)
#define PAYLOAD1_PRESENT()       (!_RB8)
#define PAYLOAD2_PRESENT()       (!_RB3)
#define COMPASS_DRDY()           (!_RB2)
#define PWR_BTN()                (!_RB4)
#define EXT_PWR_BUTTON()         (!_RB4)
#define SUS_S5()                 (!_RB5)
#define SMB_ALERT()              (!_RC13)
#define V3V3_GOOD()              (_RD1)
#define V12_GOOD()               (_RD2)
#define V5_GOOD()                (_RD6)
#define SUS_S3()                 (!_RE5)
#define NC_THRMTRIP()            (!_RE7)


#define EXTERNAL_TEMP_SENSOR_CH  0 // CH0 : input wth 100k pull-up (not used)
#define HUMIDITY_SENSOR_CH       1 // CH1 : analog input (0 - 3V ?)
#define ADC_REF_VOLTAGE          3.3f
#define ADC_SAMPLE_COUNT         1024

#define ADC_CH_TO_BIT(a) (2^a)

// board ids

#define GPS_TXD // pin 45 (RP12)
#define GPS_RXD // pin 46 (RP11)
#define SMB_SDA // pin 43 (SDA1)
#define SMB_SCL // pin 44 (SCLK1)
#define SECURE_PIC // pin 53


#define ADXL345_ADDRESS           0x53
#define TMP112_0_ADDRESS          0x48
#define TMP112_1_ADDRESS          0x49
#define HMC5843_ADDRESS           0x1E
#define EEPROM_ADDRESS            0x50 // to 0x57 (0x50 to 0x53 address four 256-byte blocks
*/
// STATE MACHINE

typedef enum
{
	CARRIER_INIT = 0,      ///< TRANSITIONAL when carrier board is power cycled everything is driven off
	CARRIER_WAIT = 1,      ///<              wait for ext PWR button to be pressed
	CARRIER_BOOT = 2,      ///< TRANSITIONAL boot sequence for full power
	CARRIER_RUN  = 3,      ///<              running normally with full power
	CARRIER_SHUTDOWN = 4,  ///< TRANSITIONAL go back into all off mode
	CARRIER_SUSPEND = 5,   ///< TRANSITIONAL S3 suspend to RAM with wake-on-lan or PWR button
	CARRIER_SUSPENDED = 6  ///<              running suspended
} CARRIER_STATE_ENUM;

int gRegCarrierState;

// PROTOTYPES

void DeviceCarrierGetTelemetry();
int DeviceCarrierBoot();
void initI2C( void );


// FUNCTIONS

#pragma code

void DeviceCarrierInit()
{
	int i = 0;

	AD1PCFGL = 0xffff;


	VBAT_DIGI_ON(0);
	V3V3_ON(0);
	V5_ON(0);
	V12_ON(0);
	AMP_PWR_ON(0);
	CODEC_PWR_ON(0);
	COM_EXPRESS_PGOOD_ON(0);


	VBAT_DIGI_EN(1);
	V3V3_EN(1);
	V5_EN(1);
	V12_EN(1);
	AMP_PWR_EN(1);
	MIC_PWR_EN(1);
	CODEC_PWR_EN(1);
	COM_EXPRESS_PGOOD_EN(1);




	block_ms(50);

	while (1)
	{

		if(DeviceCarrierBoot() == 0)
			block_ms(1000);
		else
			break;
	}




	VBAT_DIGI_ON(1);
	block_ms(100);
	V12_ON(1);
	CODEC_PWR_ON(1);
	MIC_PWR_ON(1);
	AMP_PWR_ON(1);

	while(1);



/*	initI2C();

	// enable data acquisition 
	T1CON=0x0000;//clear register
	_TCKPS=0b11;//timer stops,1:256 prescale,
	TMR1=0;//clear timer1 register
	PR1=625;//interrupt every 10ms
	T1CONbits.TON=1;
*/
	// enable A/D Converter
/*	_SSRC = 0x07; // auto-convert
	_SAMC = 0; // holding (enable this to sample)
	_ADON = 1;

	T1InterruptUserFunction = DeviceCarrierGetTelemetry;*/
}

void initI2C(void) // Initialize the I2C interface to the realtime clock
{
	// Configure I2C for 7 bit address mode 100kHz

	ODCGbits.ODG3 = 1; // SDA1 is set to open drain
	ODCGbits.ODG2 = 1; // SCL1 is set to open drain

	OpenI2C1(I2C_ON & I2C_IDLE_CON & I2C_CLK_HLD & I2C_IPMI_DIS & I2C_7BIT_ADD  
                & I2C_SLW_DIS & I2C_SM_DIS & I2C_GCALL_DIS & I2C_STR_DIS 
				& I2C_NACK, I2C_RATE_SETTING);

	IdleI2C1();

	// wait a little before continuing...
	block_ms(100);
} //initI2C


int DeviceCarrierReadAdxl345Register( unsigned char add, unsigned char reg )
{
	int a;

	writeI2C(add, reg);
	a = readI2C(add);

	return a;
}

int DeviceCarrierReadTmp112Register( unsigned char add, unsigned char reg )
{
	int a, b, c;

	writeI2C(add, reg);
	a = readI2C(add);

	writeI2C(add, reg + 0x01);
	b = readI2C(add);

	c = (b >> 4) | (a << 4);

	return c;
}

int DeviceCarrierReadHmc5843Register( unsigned char add, unsigned char reg )
{
	unsigned char a, b;
	int c;

	writeI2C(add, reg);
	a = readI2C(add);

	writeI2C(add, reg + 0x01);
	b = readI2C(add);

	c = b | (a << 8);

	return c;
}

void DeviceCarrierGetTelemetry()
{
/*	unsigned char a, b;
	signed int c;
	int d;

 	IFS0bits.T1IF = 0;	//clear interrupt flag ??

	REG_V5_POWER_GOOD = V5_GOOD();
	REG_V12_POWER_GOOD = V12_GOOD();
	REG_PAYLOAD_1_PRESENT = PAYLOAD1_PRESENT();
	REG_PAYLOAD_2_PRESENT = PAYLOAD2_PRESENT();

	a = DeviceCarrierReadAdxl345Register( ADXL345_ADDRESS, 0x32 ); // Accel X-Axis Data 0
	b = DeviceCarrierReadAdxl345Register( ADXL345_ADDRESS, 0x33 ); // Accel X-Axis Data 1
	c = a | (b << 8);
	REG_ACCEL_X = (float)c * 16.0 / 512.0;

	a = DeviceCarrierReadAdxl345Register( ADXL345_ADDRESS, 0x34 ); // Accel Y-Axis Data 0
	b = DeviceCarrierReadAdxl345Register( ADXL345_ADDRESS, 0x35 ); // Accel Y-Axis Data 1
	c = a | (b << 8);
	REG_ACCEL_Y = (float)c * 16.0 / 512.0;

	a = DeviceCarrierReadAdxl345Register( ADXL345_ADDRESS, 0x36 ); // Accel Z-Axis Data 0
	b = DeviceCarrierReadAdxl345Register( ADXL345_ADDRESS, 0x37 ); // Accel Z-Axis Data 1
	c = a | (b << 8);
	REG_ACCEL_Z = -(float)c * 16.0 / 512.0;

	d = DeviceCarrierReadTmp112Register( TMP112_0_ADDRESS, 0x00 ); // Temp
	REG_TEMP_INT0 = (float)d / 16.0; // TODO: don't forget to scale

	d = DeviceCarrierReadTmp112Register( TMP112_1_ADDRESS, 0x00 ); // Temp
	REG_TEMP_INT1 = (float)d / 16.0; // TODO: don't forget to scale

	d = DeviceCarrierReadHmc5843Register( HMC5843_ADDRESS, 0x03 ); // Magnet X-Axis Data
	REG_MAGNETIC_X = (float)d;

	d = DeviceCarrierReadHmc5843Register( HMC5843_ADDRESS, 0x05 ); // Magnet Y-Axis Data 0
	REG_MAGNETIC_Y = (float)d;

	d = DeviceCarrierReadHmc5843Register( HMC5843_ADDRESS, 0x07 ); // Magnet Z-Axis Data 0
	REG_MAGNETIC_Z = (float)d;

	// Sample Analog input devices
	_CH0SA = EXTERNAL_TEMP_SENSOR_CH;
	_SAMP = 1;

	while (!_DONE) { }
	_DONE = 0;
	REG_TEMP_EXT0 = ADC1BUF0 * ADC_REF_VOLTAGE / ADC_SAMPLE_COUNT;
	
	_CH0SA = HUMIDITY_SENSOR_CH;
	_SAMP = 1;

	while (!_DONE) { }
	_DONE = 0;
	REG_ROBOT_HUMIDITY = ADC1BUF0 * ADC_REF_VOLTAGE / ADC_SAMPLE_COUNT;

	REG_TELEMETRY_COUNT++;*/
}




int DeviceCarrierBoot()
{
	unsigned char a;
	unsigned int i = 0;

	V3V3_ON(1);

	//while(!V3V3_PGOOD());

	block_ms(100);


	

	V5_ON(1);

	while(SUS_S5() | SUS_S3())
	{
		i++;
		if(i > 5) return 0;
		block_ms(100);
	}
	i=0;

	block_ms(100);

	//while(!V5_PGOOD());

	while(!SUS_S5())
	{
		i++;
		if(i > 20) return 0;
		block_ms(100);

	}
	i=0;

	COM_EXPRESS_PGOOD_ON(1);

	while(!SUS_S3())
	{
		i++;
		if(i > 20) return 0;
		block_ms(100);
	}
	
	return 1;


}

void DeviceCarrierRun()
{

}

void DeviceCarrierShutdown()
{

}

void DeviceCarrierSuspend()
{

}

void DeviceCarrierSuspended()
{

}

void DeviceCarrierProcessIO()
{
/*	switch (gRegCarrierState)
	{
		case CARRIER_WAIT:
			DeviceCarrierWait();
			break;
		case CARRIER_BOOT:
			DeviceCarrierBoot();
			break;
		case CARRIER_RUN:
			DeviceCarrierRun();
			break;
		case CARRIER_SHUTDOWN:
			DeviceCarrierShutdown();
			break;
		case CARRIER_SUSPEND:
			DeviceCarrierSuspend();
			break;
		case CARRIER_SUSPENDED:
			DeviceCarrierSuspended();
			break;
		case CARRIER_INIT:
		default:
			DeviceCarrierInit();
			break;
	}	*/
}
