/*
 * This is where the global hardware registers are defined between software and
 * hardware. Device variables should only be declared here and not locally.
 * Data types should be multiples of whole bytes (no bitfields). New data types
 * should be added only to the end of the list to maintain backwards
 * compatibility.
 */

#ifndef REGISTERS_H
#define REGISTERS_H

#define BUTTON_DEPRESSED 1
#define REGISTER_START()
#define REGISTER_END()

typedef struct { uint8_t data[8]; } CRYPTO_DATA;
typedef struct { int16_t left, right, flipper; } MOTOR_DATA_3EL_16BI;

typedef struct { int16_t left, right, board; } TMP_3EL_16BI;
typedef struct { int16_t left, right; } MOTOR_DATA_2EL_32BI;
typedef struct { int16_t fan1, fan2; } FAN_DATA_2EL_16BI;
typedef struct { int8_t  left, right; } MOTOR_DATA_2EL_8BI;
typedef struct { int16_t pot1, pot2; } FLIPPER_DATA_2EL_16BI; 
typedef struct { int32_t left, right, flipper; } MOTOR_DATA_3EL_32BI;
typedef struct { int8_t  left, right, flipper; } MOTOR_DATA_3EL_8BI;
typedef struct { float   data[4][3]; } MOTOR_DATA_CTRL;
typedef struct { int16_t a,b; } BATTERY_DATA_2EL_16BI;
typedef struct { uint16_t deg, min, sec; } GPS_VECT;
typedef struct { GPS_VECT lat, lon; } GPS_DATA;
typedef struct {uint8_t data[100]; } GPS_MESSAGE;
typedef struct {uint8_t data[24]; } FIRMWARE_BUILD_STRING;
typedef struct {uint8_t data[79]; } BOARD_DATA;
typedef struct { uint32_t length, magic; } UPDATE_FIRMWARE; // magic = 0x2345BCDE

#endif


// Hardware Registers
REGISTER_START()
REGISTER( REG_TEST_VAR,            DEVICE_READ,  DEVICE_GENERIC, SYNC,    float )
REGISTER( REG_TEST_VAR2,           DEVICE_WRITE, DEVICE_GENERIC, SYNC,    int16_t )
REGISTER( REG_CRYPTO,              DEVICE_READ,  DEVICE_CARRIER, NO_SYNC, CRYPTO_DATA )

REGISTER( REG_TELEMETRY_COUNT,     DEVICE_READ,  DEVICE_CARRIER, SYNC,    uint16_t )
REGISTER( REG_PAYLOAD_1_PRESENT,   DEVICE_READ,  DEVICE_CARRIER, SYNC,    uint8_t )
REGISTER( REG_PAYLOAD_2_PRESENT,   DEVICE_READ,  DEVICE_CARRIER, SYNC,    uint8_t )
REGISTER( REG_CAMERA_PRESENT,      DEVICE_READ,  DEVICE_CARRIER, SYNC,    uint8_t )
REGISTER( REG_RAW_POWER_GOOD,      DEVICE_READ,  DEVICE_CARRIER, SYNC,    uint8_t )
REGISTER( REG_V12_POWER_GOOD,      DEVICE_READ,  DEVICE_CARRIER, SYNC,    uint8_t )
REGISTER( REG_V5_POWER_GOOD,       DEVICE_READ,  DEVICE_CARRIER, SYNC,    uint8_t )
REGISTER( REG_V14_VALUE,           DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )
REGISTER( REG_ACCEL_X,             DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )
REGISTER( REG_ACCEL_Y,             DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )
REGISTER( REG_ACCEL_Z,             DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )

REGISTER( REG_TEMP_INT0,           DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )
REGISTER( REG_TEMP_INT1,           DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )
REGISTER( REG_TEMP_EXT0,           DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )

REGISTER( REG_JOYSTICK1_X,         DEVICE_READ,  DEVICE_OCU,     SYNC,    uint16_t )
REGISTER( REG_JOYSTICK1_Y,         DEVICE_READ,  DEVICE_OCU,     SYNC,    uint16_t )
REGISTER( REG_JOYSTICK2_X,         DEVICE_READ,  DEVICE_OCU,     SYNC,    uint16_t )
REGISTER( REG_JOYSTICK2_Y,         DEVICE_READ,  DEVICE_OCU,     SYNC,    uint16_t )
REGISTER( REG_SWITCH1_UP,          DEVICE_READ,  DEVICE_OCU,     SYNC,    uint8_t )
REGISTER( REG_SWITCH1_DOWN,        DEVICE_READ,  DEVICE_OCU,     SYNC,    uint8_t )
REGISTER( REG_SWITCH2_UP,          DEVICE_READ,  DEVICE_OCU,     SYNC,    uint8_t )
REGISTER( REG_SWITCH2_DOWN,        DEVICE_READ,  DEVICE_OCU,     SYNC,    uint8_t )
REGISTER( REG_VOLUME_UP,           DEVICE_READ,  DEVICE_OCU,     SYNC,    uint8_t )
REGISTER( REG_VOLUME_DOWN,         DEVICE_READ,  DEVICE_OCU,     SYNC,    uint8_t )
REGISTER( REG_POWER_DOWN,          DEVICE_READ,  DEVICE_OCU,     SYNC,    uint8_t )
REGISTER( REG_HOME_BUTTON,         DEVICE_READ,  DEVICE_OCU,     SYNC,    uint8_t )
REGISTER( REG_AUX_BUTTON1,         DEVICE_READ,  DEVICE_OCU,     SYNC,    uint8_t )
REGISTER( REG_AUX_BUTTON2,         DEVICE_READ,  DEVICE_OCU,     SYNC,    uint8_t )

REGISTER( REG_MOTOR_VELOCITY,      DEVICE_WRITE, DEVICE_MOTOR,   SYNC,    MOTOR_DATA_3EL_16BI)
REGISTER( REG_FAN_VELOCITY,        DEVICE_WRITE, DEVICE_MOTOR,   SYNC,    FAN_DATA_2EL_16BI)
REGISTER( REG_MOTOR_FB_RPM,        DEVICE_READ,  DEVICE_MOTOR,   SYNC,    MOTOR_DATA_2EL_32BI)
REGISTER( REG_FLIPPER_FB_POSITION, DEVICE_READ,  DEVICE_MOTOR,   SYNC,    FLIPPER_DATA_2EL_16BI)
REGISTER( REG_MOTOR_FB_CURRENT,    DEVICE_READ,  DEVICE_MOTOR,   SYNC,    MOTOR_DATA_3EL_16BI)
REGISTER( REG_MOTOR_ENCODER_COUNT, DEVICE_READ,  DEVICE_MOTOR,   SYNC,    MOTOR_DATA_2EL_32BI)
REGISTER( REG_MOTOR_KP,            DEVICE_WRITE, DEVICE_MOTOR,   SYNC,    MOTOR_DATA_CTRL)
REGISTER( REG_MOTOR_KI,            DEVICE_WRITE, DEVICE_MOTOR,   SYNC,    MOTOR_DATA_CTRL)
REGISTER( REG_MOTOR_KD,            DEVICE_WRITE, DEVICE_MOTOR,   SYNC,    MOTOR_DATA_CTRL)
REGISTER( REG_MOTOR_CTRL_MODE,     DEVICE_WRITE, DEVICE_MOTOR,   SYNC,    MOTOR_DATA_3EL_16BI)
REGISTER( REG_MOTOR_FAULT_FLAG,    DEVICE_READ,  DEVICE_MOTOR,   SYNC,    MOTOR_DATA_2EL_8BI)
REGISTER( REG_MOTOR_TEMP,          DEVICE_READ,  DEVICE_MOTOR,   SYNC,    TMP_3EL_16BI)

REGISTER( REG_MOTOR_TEMP_STATUS,   DEVICE_READ,  DEVICE_MOTOR,   SYNC,    TMP_3EL_16BI)//0-data is bad, 1-data is good
REGISTER( REG_PWR_BAT_VOLTAGE,     DEVICE_READ,  DEVICE_MOTOR,   SYNC,    BATTERY_DATA_2EL_16BI)
REGISTER( REG_PWR_TOTAL_CURRENT,   DEVICE_READ,  DEVICE_MOTOR,   SYNC,    uint16_t)



REGISTER( REG_CARRIER_STATE,       DEVICE_WRITE, DEVICE_CARRIER, SYNC,    uint8_t )
REGISTER( REG_MAGNETIC_X,          DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )
REGISTER( REG_MAGNETIC_Y,          DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )
REGISTER( REG_MAGNETIC_Z,          DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )
REGISTER( REG_ROBOT_HUMIDITY,      DEVICE_READ,  DEVICE_CARRIER, SYNC,    float )
REGISTER( REG_WHITE_LED,           DEVICE_WRITE, DEVICE_CARRIER, SYNC,    uint8_t )
   
REGISTER( REG_OCU_TEMP_1,          DEVICE_READ,  DEVICE_OCU, SYNC,        float )
REGISTER( REG_OCU_TEMP_2,          DEVICE_READ,  DEVICE_OCU, SYNC,        float )
REGISTER( REG_OCU_ACCEL_X,         DEVICE_READ,  DEVICE_OCU, SYNC,        float )
REGISTER( REG_OCU_ACCEL_Y,         DEVICE_READ,  DEVICE_OCU, SYNC,        float )
REGISTER( REG_OCU_ACCEL_Z,         DEVICE_READ,  DEVICE_OCU, SYNC,        float)
REGISTER( REG_OCU_MAGNETIC_X,      DEVICE_READ,  DEVICE_OCU, SYNC,        float )
REGISTER( REG_OCU_MAGNETIC_Y,      DEVICE_READ,  DEVICE_OCU, SYNC,        float )
REGISTER( REG_OCU_MAGNETIC_Z,      DEVICE_READ,  DEVICE_OCU, SYNC,        float )
REGISTER( REG_OCU_HUMIDITY,        DEVICE_READ,  DEVICE_OCU, SYNC,        float )

REGISTER( REG_OCU_TOUCH_PEN,       DEVICE_READ,  DEVICE_OCU, SYNC,        uint8_t )
REGISTER( REG_OCU_TOUCH_X,         DEVICE_READ,  DEVICE_OCU, SYNC,        int16_t )
REGISTER( REG_OCU_TOUCH_Y,         DEVICE_READ,  DEVICE_OCU, SYNC,        int16_t )

REGISTER( REG_ROBOT_GPS,           DEVICE_READ,  DEVICE_CARRIER, SYNC,    GPS_DATA )
REGISTER( REG_IR_LED,              DEVICE_WRITE, DEVICE_CARRIER, SYNC,    uint8_t )

REGISTER( CMD_UPDATE_FIRMWARE,     DEVICE_WRITE, DEVICE_CARRIER, NO_SYNC, UPDATE_FIRMWARE)

REGISTER( REG_OCU_GPS_MESSAGE,        DEVICE_READ,  DEVICE_OCU, SYNC,    GPS_MESSAGE )
REGISTER( REG_OCU_BATT_CURRENT,   DEVICE_READ,  DEVICE_OCU,   SYNC,    int16_t)
REGISTER( REG_OCU_BATT_REL_SOC,   DEVICE_READ,  DEVICE_OCU,   SYNC,    uint16_t)
REGISTER( REG_OCU_BATT_ABS_SOC,   DEVICE_READ,  DEVICE_OCU,   SYNC,    uint16_t)
REGISTER( REG_OCU_BATT_REM_CAP,   DEVICE_READ,  DEVICE_OCU,   SYNC,    uint16_t)
REGISTER( REG_OCU_BATT_VOLTAGE,   DEVICE_READ,  DEVICE_OCU,   SYNC,    uint16_t)
REGISTER( REG_OCU_BATT_FULL_CHARGE_CAP,   DEVICE_READ,  DEVICE_OCU,   SYNC,    uint16_t)
REGISTER( REG_OCU_FIRMWARE_BUILD,   DEVICE_READ,  DEVICE_OCU,   SYNC,    FIRMWARE_BUILD_STRING)

REGISTER( REG_ROBOT_FIRMWARE_BUILD,   DEVICE_READ,  DEVICE_OCU,   NO_SYNC,    FIRMWARE_BUILD_STRING)
REGISTER( REG_ROBOT_GPS_MESSAGE,        DEVICE_READ,  DEVICE_OCU, NO_SYNC,    GPS_MESSAGE )
REGISTER( REG_ROBOT_BOARD_DATA,        DEVICE_READ,  DEVICE_OCU, NO_SYNC,    BOARD_DATA )

REGISTER( REG_ROBOT_REL_SOC_A, DEVICE_READ,  DEVICE_OCU, SYNC,        int16_t )
REGISTER( REG_ROBOT_REL_SOC_B, DEVICE_READ,  DEVICE_OCU, SYNC,        int16_t )

REGISTER( REG_MOTOR_BOARD_DATA,        DEVICE_READ,  DEVICE_MOTOR, NO_SYNC,    BOARD_DATA )

   
REGISTER_END()

