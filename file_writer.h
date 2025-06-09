/**
 * file_writer.h
 * 文件写入模块头文件
 */

#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include "simulation_params.h"

 /**
  * 保存原始信号到文件
  *
  * @param filename 输出文件名
  * @param time_array 时间数组
  * @param signal_array 信号数组
  * @param array_size 数组大小
  * @return 成功返回0，失败返回错误码
  */
int save_signal_to_file(const char* filename, double* time_array,
    double* signal_array, int array_size);

/**
 * 保存参数回显到文件
 *
 * @param filename 输出文件名
 * @param params 仿真参数
 * @param doppler_factors 计算得到的多普勒因子，可以为NULL
 * @return 成功返回0，失败返回错误码
 */
int save_params_to_file(const char* filename, SimulationParameters* params,
    double* doppler_factors);

#endif /* FILE_WRITER_H */#pragma once
