#ifndef SETTINGS_H
#define SETTINGS_H

#include "stdint.h"
#include "stdbool.h"
#include "stddef.h"

typedef struct Settings {
    struct {
        /// How often to update the motor pwm / direction protection
        uint16_t motor_poll_ms;
        /// How often to update the electrical protection logic
        uint16_t electrical_poll_ms;
        /// How often to update internal serial communication.
        /// this is responsible for communicating the with the fan and measuring some SmartBattery
        /// info
        uint16_t i2c_poll_ms;
        /// How often to check for and process inbound UART commands.
        uint16_t communication_poll_ms;
        /// How often to read out internal analog outputs.
        uint16_t analog_readouts_poll_ms;
        /// How often to run the PID motor controller (if enabled)
        uint16_t motor_controller_poll_ms;
        /// How often to update the flipper positional feedback and command
        uint16_t flipper_poll_ms;
    } main;
    struct {
        uint16_t motor_command_timeout_ms;
        uint16_t fan_command_timeout_ms;
        uint16_t rx_bufsize_bytes;
        uint16_t tx_bufsize_bytes;
        uint16_t baud_rate;
    } communication;
    struct {
        uint16_t overcurrent_trigger_threshold_ma;
        uint16_t overcurrent_trigger_duration_ms;
        uint16_t overcurrent_reset_threshold_ma;
        uint16_t overcurrent_reset_duration_ms;
    } electrical;
    struct {
        /// Whether or not the calibration routine has been run and angle_offset can be trusted
        bool is_calibrated;
        /// Value needed to compute the actual flippera angle from onboard sensors. Computed by the
        /// flipper calibration routine
        uint16_t angle_offset;
    } flipper;
    struct {
        uint16_t step_timeout_ms;
    } i2c;
    struct {
        float pid_p_weight;
        float pid_i_weight;
        float pid_d_weight;
        float min_effort;
        float max_effort;
        float iir_alpha;
    } motor_controller;
    struct {
        uint16_t pwm_hz;
        uint16_t motor_dead_time_between_directions_ms;
    } motor;
} Settings;

Settings settings_load();
void settings_save(const Settings *settings);

#endif