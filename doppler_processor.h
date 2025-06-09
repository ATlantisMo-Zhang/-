/**
 * doppler_processor.h
 * 多普勒效应处理模块头文件
 */

#ifndef DOPPLER_PROCESSOR_H
#define DOPPLER_PROCESSOR_H

#include "simulation_params.h"

 /**
  * 根据目标速度计算多普勒频率偏移因子
  *
  * @param velocity_mps 目标速度 (m/s)，正为远离，负为接近
  * @param sound_speed_mps 声速 (m/s)
  * @return 多普勒频率偏移因子 (接收频率/发射频率)
  */
double calculate_doppler_factor(double velocity_mps, double sound_speed_mps);

/**
 * 为单个目标生成考虑多普勒效应的接收信号
 *
 * @param params 仿真参数
 * @param target_index 目标索引 (0或1)
 * @param time_array 时间数组
 * @param original_signal 原始信号数组
 * @param array_size 数组大小
 * @param received_signal 输出接收信号数组
 * @return 成功返回0，失败返回错误码
 */
int generate_doppler_signal_for_target(SimulationParameters* params, int target_index,
    double* time_array, double* original_signal,
    int array_size, double** received_signal);

/**
 * 生成多目标叠加的接收信号（无噪声）
 *
 * @param params 仿真参数
 * @param time_array 时间数组
 * @param original_signal 原始信号数组
 * @param array_size 数组大小
 * @param clean_received_signal 输出无噪声接收信号数组
 * @return 成功返回0，失败返回错误码
 */
int generate_multi_target_signal(SimulationParameters* params,
    double* time_array, double* original_signal,
    int array_size, double** clean_received_signal);

#endif /* DOPPLER_PROCESSOR_H */#pragma once
