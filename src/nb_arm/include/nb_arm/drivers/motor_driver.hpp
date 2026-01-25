#ifndef MOTOR_DRIVER_HPP
#define MOTOR_DRIVER_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
// #include "nb_arm/communication/serial_port.hpp"
#include <math.h>
#include <array>

#define MOTOR_NUM 6

class motor_driver
{
public:
    motor_driver();
    ~motor_driver() = default;

    float current_motor_pos[MOTOR_NUM];
    float current_motor_vel[MOTOR_NUM];
    float current_motor_tor[MOTOR_NUM];
    float current_motor_acc[MOTOR_NUM];
    float desire_motor_pos[MOTOR_NUM];
    float desire_motor_vel[MOTOR_NUM];
    float desire_motor_acc[MOTOR_NUM];
    float desire_motor_tor[MOTOR_NUM];
    float min_motor_tor[MOTOR_NUM];
    float max_motor_tor[MOTOR_NUM];
    float mit_torff[MOTOR_NUM];
    float mit_kp[MOTOR_NUM] = {20.0, 20.0, 20.0, 6.0, 6.0, 6.0};
    float mit_kd[MOTOR_NUM] = {5.0, 5.0, 5.0, 1.5, 1.5, 1.5};
};

#endif // MOTOR_DRIVER_HPP
