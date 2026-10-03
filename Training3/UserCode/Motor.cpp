#include "Motor.hpp"

Motor::Motor(float ratio) : ratio_(ratio) {
    // 初始化：received_ 已经是 false，tx_data_ 已经是全0
}

void Motor::canRxMsgCallback(const uint8_t rx_data[8]) {
    ecd_angle_ = static_cast<float>(rx_data[0]<<8|rx_data[1])/8192.0f*360.0f;
    speedRpm_ = static_cast<float>(static_cast<int16_t>((rx_data[2] << 8) | rx_data[3]));
    currentA_ = static_cast<float>(static_cast<int16_t>((rx_data[4] << 8) | rx_data[5]))/16384.0f*20.0f;
    tempC_ = static_cast<float>(rx_data[6]);
    // TODO: 拆字节、换算、环绕修正、减速比
    float delta;
    if (received_==false)
    {
        last_ecd_ = ecd_angle_;
        received_=true;
    }
    else
    {
        delta=ecd_angle_-last_ecd_;
        if (delta<-180.0f) delta+=360.0f;
        if (delta>180.0f) delta-=360.0f;
        last_ecd_ = ecd_angle_;
        angle_+=delta/ratio_;
    }
}
float Motor::angle() const { return angle_; }
float Motor::speedRpm() const { return speedRpm_; }
float Motor::currentAmps() const { return currentA_; }
float Motor::temperature() const { return tempC_; }
bool Motor::hasFeedback() const { return received_; }

void Motor::setTxCurrent(float amperes, uint8_t motor_id) {
    if (amperes<-2) amperes=-2;
    if (amperes>2) amperes=2;
    int16_t original_current=static_cast<int16_t>(currentA_/20.0f*16384.0f);
    // TODO: 限幅 ±20A，换算 ×16384/20，大端打包进 tx_data_ 对应两个字节
    tx_data_[2 * (motor_id - 1)]     = static_cast<uint8_t>((original_current >> 8) & 0xFF);  // 高8位
    tx_data_[2 * (motor_id - 1) + 1] = static_cast<uint8_t>(original_current & 0xFF);         // 低8
}

uint8_t* Motor::getTxData() {
    return tx_data_;
}