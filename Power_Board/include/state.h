/// @file
/// State of the rover.

#ifndef STATE_H
#define STATE_H

#include "motor.h"
#include "battery.h"
#include "stdbool.h"
#include "stdint.h"

/// Transient state of the rover at any point in time.
/// State is grouped according to the functional area that *sets* the value.
/// Values may be used/affect others e.g. "overcurrent" affects the "drive" functional area and is
/// informed by data from the "analog" functional area, but lives in the "power" functional area
/// because it is only set and cleared in power.h
typedef struct State {
    struct DriveState {
        uint32_t motor_encoder_count[2];
        uint16_t motor_encoder_period[2];
        uint16_t flipper_angle;
        MotorStatusFlag motor_status[MOTOR_CHANNEL_COUNT];
    } drive;
    struct AnalogState {
        /// Voltage from each battery, according to an analog sensor
        uint16_t battery_voltage[BATTERY_COUNT];
        /// Reading of each flipper position potentiometer
        uint16_t flipper_sensors[2];
        /// Current from each battery, according to an analog sensor
        uint16_t battery_current[BATTERY_COUNT];
        /// Current to each motor, according to an analog sensor
        uint16_t motor_current[MOTOR_CHANNEL_COUNT];
    } analog;
    struct PowerState {
        /// If true, we are drawing too much battery power and are at risk of triggering a hardware
        /// overcurrent condition if we continue at current consumption.
        bool overcurrent;
    } power;
    struct I2CState {
        /// Digital temperature values, as reported by fan controller
        uint16_t temperature_sensor[2];
        /// Whether @ref temperature_sensor values were successfully polled
        bool temperature_sensor_valid[2];
        /// Whether an external voltage supply is present. (0xdada if true, 0 otherwise)
        uint16_t charger_state;
        /// Digital state of charge (0-100)
        uint16_t smartbattery_soc[BATTERY_COUNT];
        uint16_t smartbattery_status[BATTERY_COUNT];
        uint16_t smartbattery_mode[BATTERY_COUNT];
        uint16_t smartbattery_temperature[BATTERY_COUNT];
        uint16_t smartbattery_current[BATTERY_COUNT];
        uint16_t smartbattery_voltage[BATTERY_COUNT];
    } i2c;
    struct CommunicationState {
        bool use_manual_fan_speed;
        /// value ranges from 0 (off) to 240 (100%)
        uint8_t fan_speed;
        /// values from -1000 to 1000
        int16_t motor_effort[MOTOR_CHANNEL_COUNT];
    } communication;
} State;

#endif