/**
 * doppler_processor.c
 * 多普勒效应处理模块实现
 */

#include "doppler_processor.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

double calculate_doppler_factor(double velocity_mps, double sound_speed_mps) {
    // 多普勒效应频率偏移因子
    // 对于接近运动 (velocity < 0)，接收频率增大
    // 对于远离运动 (velocity > 0)，接收频率减小
    return (sound_speed_mps / (sound_speed_mps + velocity_mps));
}

// 为单个目标生成CW信号的多普勒效应
static void generate_doppler_cw(double frequency_hz, double doppler_factor,
    double target_amplitude, double* time_array,
    int array_size, double* received_signal) {
    // 计算多普勒频移后的频率
    double doppler_frequency = frequency_hz * doppler_factor;

    for (int i = 0; i < array_size; i++) {
        received_signal[i] = target_amplitude * sin(2.0 * PI * doppler_frequency * time_array[i]);
    }
}

// 为单个目标生成LFM信号的多普勒效应
static void generate_doppler_lfm(double start_frequency_hz, double bandwidth_hz,
    double pulse_duration_s, double doppler_factor,
    double target_amplitude, double* time_array,
    int array_size, double* received_signal) {
    // 多普勒效应会改变LFM信号的起始频率和带宽
    double doppler_start_freq = start_frequency_hz * doppler_factor;
    double doppler_bandwidth = bandwidth_hz * doppler_factor;
    double chirp_rate = doppler_bandwidth / pulse_duration_s;

    for (int i = 0; i < array_size; i++) {
        double phase = 2.0 * PI * (doppler_start_freq * time_array[i] +
            0.5 * chirp_rate * time_array[i] * time_array[i]);
        received_signal[i] = target_amplitude * sin(phase);
    }
}

int generate_doppler_signal_for_target(SimulationParameters* params, int target_index,
    double* time_array, double* original_signal,
    int array_size, double** received_signal) {
    if (params == NULL || time_array == NULL || original_signal == NULL ||
        received_signal == NULL || target_index < 0 || target_index >= params->num_targets) {
        return -1;  // 参数无效
    }

    // 分配内存
    *received_signal = (double*)malloc(array_size * sizeof(double));
    if (*received_signal == NULL) {
        return -2;  // 内存分配失败
    }

    // 获取目标信息
    double target_velocity = params->targets[target_index].velocity_mps;
    double target_amplitude = params->targets[target_index].initial_amplitude;

    // 计算多普勒频移因子
    double doppler_factor = calculate_doppler_factor(target_velocity, params->sound_speed_mps);

    // 生成考虑多普勒效应的接收信号
    if (params->signal_type == SIGNAL_CW) {
        generate_doppler_cw(params->frequency_hz, doppler_factor, target_amplitude,
            time_array, array_size, *received_signal);
    }
    else if (params->signal_type == SIGNAL_LFM) {
        generate_doppler_lfm(params->frequency_hz, params->lfm_bandwidth_hz,
            params->pulse_duration_s, doppler_factor, target_amplitude,
            time_array, array_size, *received_signal);
    }
    else {
        // 未知信号类型
        free(*received_signal);
        *received_signal = NULL;
        return -3;
    }

    return 0;  // 成功
}

int generate_multi_target_signal(SimulationParameters* params,
    double* time_array, double* original_signal,
    int array_size, double** clean_received_signal) {
    if (params == NULL || time_array == NULL || original_signal == NULL ||
        clean_received_signal == NULL || array_size <= 0) {
        return -1;  // 参数无效
    }

    // 分配内存
    *clean_received_signal = (double*)calloc(array_size, sizeof(double));
    if (*clean_received_signal == NULL) {
        return -2;  // 内存分配失败
    }

    // 处理每个目标的贡献并叠加
    for (int target_idx = 0; target_idx < params->num_targets; target_idx++) {
        double* target_signal = NULL;

        // 生成当前目标的多普勒信号
        int result = generate_doppler_signal_for_target(params, target_idx,
            time_array, original_signal,
            array_size, &target_signal);

        if (result != 0 || target_signal == NULL) {
            // 处理错误，释放已分配内存
            free(*clean_received_signal);
            *clean_received_signal = NULL;
            return -3;
        }

        // 叠加当前目标的信号到总接收信号
        for (int i = 0; i < array_size; i++) {
            (*clean_received_signal)[i] += target_signal[i];
        }

        // 释放当前目标的信号内存
        free(target_signal);
    }

    return 0;  // 成功
}