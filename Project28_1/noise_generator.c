/**
 * noise_generator.c
 * 噪声生成模块实现
 */

#include "noise_generator.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


 // Box-Muller法生成高斯随机数
static double generate_gaussian_random() {
    static int have_next = 0;
    static double next_gaussian;

    if (have_next) {
        have_next = 0;
        return next_gaussian;
    }
    else {
        double u1, u2;
        do {
            u1 = (double)rand() / RAND_MAX;
            u2 = (double)rand() / RAND_MAX;
        } while (u1 <= 1e-10);  // 避免对数函数的零输入

        double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
        double z1 = sqrt(-2.0 * log(u1)) * sin(2.0 * M_PI * u2);

        have_next = 1;
        next_gaussian = z1;
        return z0;
    }
}

double calculate_signal_power(double* signal, int array_size) {
    if (signal == NULL || array_size <= 0) {
        return 0.0;
    }

    double sum_squared = 0.0;
    for (int i = 0; i < array_size; i++) {
        sum_squared += signal[i] * signal[i];
    }

    return sum_squared / array_size;
}

int add_noise(SimulationParameters* params, double* clean_signal,
    int array_size, double** noisy_signal) {
    if (params == NULL || clean_signal == NULL || noisy_signal == NULL || array_size <= 0) {
        return -1;  // 参数无效
    }

    // 如果噪声未启用，则直接复制原始信号
    if (!params->noise_enabled) {
        *noisy_signal = (double*)malloc(array_size * sizeof(double));
        if (*noisy_signal == NULL) {
            return -2;  // 内存分配失败
        }

        for (int i = 0; i < array_size; i++) {
            (*noisy_signal)[i] = clean_signal[i];
        }

        return 0;  // 成功
    }

    // 初始化随机数生成器
    srand((unsigned int)time(NULL));

    // 分配内存
    *noisy_signal = (double*)malloc(array_size * sizeof(double));
    if (*noisy_signal == NULL) {
        return -2;  // 内存分配失败
    }

    // 计算信号的平均功率
    double signal_power = calculate_signal_power(clean_signal, array_size);

    // 根据SNR计算噪声功率
    double snr_linear = pow(10.0, params->snr_db / 10.0);
    double noise_power = signal_power / snr_linear;

    // 生成噪声并添加到信号中
    double noise_amplitude = sqrt(noise_power);

    for (int i = 0; i < array_size; i++) {
        double noise = noise_amplitude * generate_gaussian_random();
        (*noisy_signal)[i] = clean_signal[i] + noise;
    }

    return 0;  // 成功
}