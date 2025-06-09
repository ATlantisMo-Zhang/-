/**
 * signal_generator.c
 * 信号生成模块实现
 */

#include "signal_generator.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

 /**
  * 生成连续波 (CW) 信号
  *
  * @param frequency_hz 信号频率 (Hz)
  * @param time_array 时间数组
  * @param array_size 数组大小
  * @param signal_array 输出信号数组
  */
static void generate_cw_signal(double frequency_hz, double* time_array, int array_size, double* signal_array) {
    for (int i = 0; i < array_size; i++) {
        signal_array[i] = sin(2.0 * PI * frequency_hz * time_array[i]);
    }
}

/**
 * 生成线性调频 (LFM) 信号
 *
 * @param start_frequency_hz 起始频率 (Hz)
 * @param bandwidth_hz 带宽 (Hz)
 * @param pulse_duration_s 脉冲持续时间 (s)
 * @param time_array 时间数组
 * @param array_size 数组大小
 * @param signal_array 输出信号数组
 */
static void generate_lfm_signal(double start_frequency_hz, double bandwidth_hz, double pulse_duration_s,
    double* time_array, int array_size, double* signal_array) {
    // 计算调频率 (Hz/s)
    double chirp_rate = bandwidth_hz / pulse_duration_s;

    for (int i = 0; i < array_size; i++) {
        // LFM信号的相位随时间的平方变化
        double instantaneous_freq = start_frequency_hz + chirp_rate * time_array[i] / 2.0;
        double phase = 2.0 * PI * (start_frequency_hz * time_array[i] + 0.5 * chirp_rate * time_array[i] * time_array[i]);
        signal_array[i] = sin(phase);
    }
}

int generate_signal(SimulationParameters* params, double** time_array, double** signal_array, int* array_size) {
    if (params == NULL || time_array == NULL || signal_array == NULL || array_size == NULL) {
        return -1;  // 无效参数
    }

    // 计算采样点数
    *array_size = (int)(params->pulse_duration_s * params->sampling_rate_hz);
    if (*array_size <= 0) {
        return -2;  // 采样点数无效
    }

    // 分配内存
    *time_array = (double*)malloc(*array_size * sizeof(double));
    *signal_array = (double*)malloc(*array_size * sizeof(double));

    if (*time_array == NULL || *signal_array == NULL) {
        // 内存分配失败，释放已分配的内存
        if (*time_array != NULL) free(*time_array);
        if (*signal_array != NULL) free(*signal_array);
        *time_array = NULL;
        *signal_array = NULL;
        return -3;  // 内存分配失败
    }

    // 生成时间数组
    double time_step = 1.0 / params->sampling_rate_hz;
    for (int i = 0; i < *array_size; i++) {
        (*time_array)[i] = i * time_step;
    }

    // 根据信号类型生成信号
    if (params->signal_type == SIGNAL_CW) {
        generate_cw_signal(params->frequency_hz, *time_array, *array_size, *signal_array);
    }
    else if (params->signal_type == SIGNAL_LFM) {
        generate_lfm_signal(params->frequency_hz, params->lfm_bandwidth_hz,
            params->pulse_duration_s, *time_array, *array_size, *signal_array);
    }
    else {
        // 未知信号类型，释放内存并返回错误
        free(*time_array);
        free(*signal_array);
        *time_array = NULL;
        *signal_array = NULL;
        return -4;  // 未知信号类型
    }

    return 0;  // 成功
}

void free_signal_arrays(double* time_array, double* signal_array) {
    if (time_array != NULL) free(time_array);
    if (signal_array != NULL) free(signal_array);
}