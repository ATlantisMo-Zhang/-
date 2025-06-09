/**
 * simulation_params.h
 * 仿真参数数据结构定义
 */

#ifndef SIMULATION_PARAMS_H
#define SIMULATION_PARAMS_H

 // 信号类型枚举
typedef enum {
    SIGNAL_CW,  // 连续波信号
    SIGNAL_LFM  // 线性调频信号
} SignalType;

// 单个目标信息结构体
typedef struct {
    double velocity_mps;      // 目标速度 (m/s)，正为远离，负为接近
    double initial_amplitude; // 目标信号初始幅度
} TargetInfo;

// 仿真参数结构体
typedef struct {
    // 声源参数
    SignalType signal_type;       // 信号类型
    double frequency_hz;          // 初始频率 (Hz)
    double pulse_duration_s;      // 脉冲持续时间 (s)
    double sampling_rate_hz;      // 采样率 (Hz)
    double lfm_bandwidth_hz;      // LFM带宽 (Hz)，仅适用于LFM信号

    // 环境参数
    double sound_speed_mps;       // 水中声速 (m/s)

    // 目标参数
    int num_targets;              // 目标数量
    TargetInfo targets[2];        // 最多支持2个目标

    // 噪声参数
    int noise_enabled;            // 是否启用噪声
    double snr_db;                // 信噪比 (dB)

    // 输出文件路径
    char output_original_signal_file[256];    // 原始信号输出文件
    char output_received_signal_file[256];    // 接收信号输出文件
    char output_clean_received_signal_file[256]; // 无噪声接收信号输出文件
    char output_params_echo_file[256];        // 参数回显文件
} SimulationParameters;

#endif /* SIMULATION_PARAMS_H */#pragma once
