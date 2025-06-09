/**
 * signal_generator.h
 * 信号生成模块头文件
 */

#ifndef SIGNAL_GENERATOR_H
#define SIGNAL_GENERATOR_H

#include "simulation_params.h"

 /**
  * 生成原始信号
  *
  * @param params 仿真参数
  * @param time_array 输出时间数组
  * @param signal_array 输出信号数组
  * @param array_size 输出数组的大小（元素个数）
  * @return 成功返回0，失败返回错误码
  */
int generate_signal(SimulationParameters* params, double** time_array, double** signal_array, int* array_size);

/**
 * 释放生成的信号和时间数组
 *
 * @param time_array 时间数组指针
 * @param signal_array 信号数组指针
 */
void free_signal_arrays(double* time_array, double* signal_array);

#endif /* SIGNAL_GENERATOR_H */#pragma once
