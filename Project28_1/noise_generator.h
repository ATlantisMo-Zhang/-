/**
 * noise_generator.h
 * 噪声生成模块头文件
 */

#ifndef NOISE_GENERATOR_H
#define NOISE_GENERATOR_H

#include "simulation_params.h"

 /**
  * 生成高斯白噪声并添加到信号中
  *
  * @param params 仿真参数
  * @param clean_signal 无噪声信号数组
  * @param array_size 数组大小
  * @param noisy_signal 输出添加噪声后的信号数组
  * @return 成功返回0，失败返回错误码
  */
int add_noise(SimulationParameters* params, double* clean_signal,
    int array_size, double** noisy_signal);

/**
 * 计算信号的平均功率
 *
 * @param signal 信号数组
 * @param array_size 数组大小
 * @return 信号的平均功率
 */
double calculate_signal_power(double* signal, int array_size);

#endif /* NOISE_GENERATOR_H */