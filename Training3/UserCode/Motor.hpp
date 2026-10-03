/* UserCode/Motor.hpp */
#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <cstdint>

class Motor {
public:
    explicit Motor(float ratio);           // ratio = 减速比

    // 入口：CAN 接收中断调用
    void canRxMsgCallback(const uint8_t rx_data[8]);

    // 出口：控制逻辑读取
    float angle() const;                   // 输出轴角度（度）
    float speedRpm() const;                // 转子转速（RPM）
    float currentAmps() const;             // 转矩电流（A）
    float temperature() const;             // 温度（℃）
    bool hasFeedback() const;              // 收到过帧？

    // 命令：控制逻辑调用，内部限幅
    void setTxCurrent(float amperes, uint8_t motor_id);

    // 命令出口：TIM6 中断取用
    uint8_t* getTxData();

private:
    // ---- 配置 ----
    const float ratio_;                    // 减速比

    // ---- 这一帧解出来的 ----
    float ecd_angle_ = 0;                  // 机械角度（0~8191）
    float speedRpm_ = 0;                   // 转速（RPM）
    float currentA_ = 0;                   // 转矩电流（A）
    float tempC_ = 0;                      // 温度（℃）

    // ---- 连续角度 ----
    float angle_ = 0;                      // 输出轴累计角度
    float last_ecd_ = 0;                   // 上一帧角度，用来求差
    bool  received_ = false;               // 第一帧特殊处理

    // ---- 待发送 ----
    uint8_t tx_data_[8] = {};              // 初值全0

    // ---- TODO: 你来实现 ----
    // 1. 拆 8 字节为字段（字节序 + 符号）
    // 2. 角度换算 ×360/8192
    // 3. 增量累加 + 过零点环绕修正
    // 4. 折算到输出轴（÷ 减速比）
    static constexpr uint16_t kEncoderRange = 8192;
    // 成员变量见下一页（第34页）
};

#endif