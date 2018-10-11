#include "p24FJ256GB106.h"
#include "stdhdr.h"
#include "device_robot_motor.h"
#include "debug_uart.h"
#include "i2clib.h"
#include "device_robot_motor_i2c.h"

int I2C2XmitReset = 0;
int I2C3XmitReset = 0;

int I2C2TimerExpired = 0;
int I2C3TimerExpired = 0;

// A step may be reached by either falling through into it (first try)
// Or by jumping back to it (every retry)
// Note an i2c_result of I2C_ILLEGAL probably means there is a coding error, hence the breakpoint
#define I2C_SUB(operationdef)                                                                      \
    op = operationdef;                                                                             \
    progress = I2C_PROGRESS_UNSTARTED;                                                             \
    case (__LINE__):                                                                               \
        resume_at = (__LINE__);                                                                    \
        i2c_result = i2c_tick(BUS, &op, &progress);                                                \
        BREAKPOINT_IF(i2c_result == I2C_ILLEGAL);                                                  \
        if (i2c_result == I2C_NOTYET) {                                                            \
            return;                                                                                \
        } else if (i2c_result == I2C_ILLEGAL) {                                                    \
            resume_at = -1;                                                                        \
            return;                                                                                \
        }                                                                                          \
        // fallthrough to next case

void I2C2Update(void) {
    const i2c_bus_t BUS = I2C_BUS2;

    static i2c_operationdef_t op;
    static i2c_progress_t progress;

    static int resume_at = 0;
    static uint16_t c;
    static uint8_t a;
    i2c_result_t i2c_result;

    if (I2C2XmitReset == true) {
        I2C2XmitReset = false; // clear the flag
        resume_at = 0;         // start from the beginning
    }
    switch (resume_at) {
    default:
        BREAKPOINT();
    // fallthrough
    case -1:
        re_init_i2c2();
        // fallthrough
    case 0:

        // REG_MOTOR_TEMP.left = FanControl ReadByte 0x00
        I2C_SUB(i2c_op_read_byte(FAN_CONTROLLER_ADDRESS, 0x00, &a))
        REG_MOTOR_TEMP_STATUS.left = (i2c_result == I2C_OKAY);
        REG_MOTOR_TEMP.left = a;

        // REG_MOTOR_TEMP.right = FanControl ReadByte 0x01
        I2C_SUB(i2c_op_read_byte(FAN_CONTROLLER_ADDRESS, 0x01, &a))
        REG_MOTOR_TEMP_STATUS.right = (i2c_result == I2C_OKAY);
        REG_MOTOR_TEMP.right = a;

        // REG_ROBOT_REL_SOC_A = Battery ReadWord 0x0d [="RelativeStateOfCharge"]
        I2C_SUB(i2c_op_read_word(BATTERY_ADDRESS, 0x0d, &c))
        REG_ROBOT_REL_SOC_A = c;

        // FanControl WriteByte 0x0b
        if (Xbee_SIDE_FAN_NEW) {
            a = Xbee_SIDE_FAN_SPEED;
        } else if (abs(Xbee_MOTOR_VELOCITY[0]) + abs(Xbee_MOTOR_VELOCITY[1]) > 10) {
            a = Xbee_SIDE_FAN_SPEED = 240;
        } else {
            a = Xbee_SIDE_FAN_SPEED = 0;
        }
        I2C_SUB(i2c_op_write_byte(FAN_CONTROLLER_ADDRESS, 0x0b, &a));

        // Battery ReadWord 0x16 [="BatteryStatus"]
        I2C_SUB(i2c_op_read_word(BATTERY_ADDRESS, 0x16, &c));
        if (i2c_result == I2C_OKAY) {
            REG_BATTERY_STATUS_A = c;
        }

        // Battery ReadWord 0x03 [="BatteryMode"]
        I2C_SUB(i2c_op_read_word(BATTERY_ADDRESS, 0x03, &c));
        if (i2c_result == I2C_OKAY) {
            REG_BATTERY_MODE_A = c;
        }

        // Battery ReadWord 0x08 [="Temperature"]
        I2C_SUB(i2c_op_read_word(BATTERY_ADDRESS, 0x08, &c))
        if (i2c_result == I2C_OKAY) {
            REG_BATTERY_TEMP_A = c;
        }

        I2C2TimerExpired = false; // reset the I2C2 update timer
        resume_at = -1;
    }
}

void I2C3Update(void) {
    const i2c_bus_t BUS = I2C_BUS3;

    static i2c_operationdef_t op;
    static i2c_progress_t progress;

    static int resume_at = 0;
    static uint16_t c;
    i2c_result_t i2c_result;

    if (I2C3XmitReset == true) {
        I2C3XmitReset = false; // clear the flag
        resume_at = 0;         //  start from the beginning
    }
    switch (resume_at) {
    default:
        BREAKPOINT();
    // fallthrough
    case -1:
        re_init_i2c3();
        // fallthrough
    case 0:
        // Battery ReadWord 0x0d [="RelativeStateOfCharge"]
        I2C_SUB(i2c_op_read_word(BATTERY_ADDRESS, 0x0d, &c))
        if (i2c_result == I2C_OKAY) {
            REG_ROBOT_REL_SOC_B = c;
        }

        // BatteryCharger ReadWord 0xca
        I2C_SUB(i2c_op_read_word(BATTERY_CHARGER_ADDRESS, 0xca, &c))
        if (i2c_result == I2C_OKAY) {
            REG_MOTOR_CHARGER_STATE = c;
        }

        // Battery ReadWord 0x16 [="BatteryStatus"]
        I2C_SUB(i2c_op_read_word(BATTERY_ADDRESS, 0x16, &c))
        if (i2c_result == I2C_OKAY) {
            REG_BATTERY_STATUS_B = c;
        }

        // Battery ReadWord 0x03 [="BatteryMode"]
        I2C_SUB(i2c_op_read_word(BATTERY_ADDRESS, 0x03, &c))
        if (i2c_result == I2C_OKAY) {
            REG_BATTERY_MODE_B = c;
        }

        // Battery ReadWord 0x08 [="Temperature"]
        I2C_SUB(i2c_op_read_word(BATTERY_ADDRESS, 0x08, &c))
        if (i2c_result == I2C_OKAY) {
            REG_BATTERY_TEMP_B = c;
        }

        I2C3TimerExpired = false; // reset the I2C3 update timer
        resume_at = -1;
    }
}

//*********************************************//

void re_init_i2c2(void) {
    I2C2CONbits.I2CEN = 0;
    I2C2CON = 0;
    I2C2STAT = 0;

    // New Bren-Tronics battery (or existing device interacting with the new
    // battery ties up the SMBus line (usually
    // seen after the robot is driven for a short time.  The only
    // way I've been able to recover from this is by either removing
    // the SMBus cable between the battery board and the power board,
    // or by changing the i2c pins to outputs when the i2c module is
    // disabled.
    _TRISF4 = 0;
    _TRISF5 = 0;
    _LATF4 = 0;
    _LATF5 = 0;

    // FCY should be 16M
    // I2C2BRG = FCY/100000-FCY/10000000-1;	//should be 157.4 (between 9D and 9E)
    I2C2BRG = 0xff;
    I2C2CONbits.I2CEN = 1;
}

void re_init_i2c3(void) {
    I2C3CONbits.I2CEN = 0;
    I2C3CON = 0;
    I2C3STAT = 0;

    _TRISE6 = 0;
    _TRISE7 = 0;
    _LATE6 = 0;
    _LATE7 = 0;

    // FCY should be 16M
    // I2C2BRG = FCY/100000-FCY/10000000-1;	//should be 157.4 (between 9D and 9E)
    I2C3BRG = 0xff;
    I2C3CONbits.I2CEN = 1;
}
