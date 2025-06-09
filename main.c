/**
 * main.c
 * 水下目标多普勒效应模拟与分析软件的主程序
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_parser.h"
#include "signal_generator.h"
#include "doppler_processor.h"
#include "noise_generator.h"
#include "file_writer.h"


int main(int argc, char* argv[]) {
    printf("水下目标多普勒效应模拟与分析软件 (C后端)\n");
    printf("--------------------------------------\n\n");

    // 默认配置文件路径
    const char* config_file = "config.txt";

    // 如果有命令行参数，则使用指定的配置文件路径
    if (argc > 1) {
        config_file = argv[1];
    }

    printf("使用配置文件: %s\n", config_file);

    // 步骤1: 加载和解析配置文件
    SimulationParameters params;
    memset(&params, 0, sizeof(SimulationParameters));

    printf("正在加载配置文件...\n");
    int result = parse_config_file(config_file, &params);
    if (result != 0) {
        printf("配置文件解析失败，错误码: %d\n", result);
        return 1;
    }
    printf("配置文件加载成功\n");

    // 步骤2: 生成原始信号
    double* time_array = NULL;
    double* original_signal = NULL;
    int array_size = 0;

    printf("正在生成原始信号...\n");
    result = generate_signal(&params, &time_array, &original_signal, &array_size);
    if (result != 0 || time_array == NULL || original_signal == NULL || array_size <= 0) {
        printf("信号生成失败，错误码: %d\n", result);
        return 2;
    }
    printf("原始信号生成成功，采样点数: %d\n", array_size);

    // 步骤3: 保存原始信号到文件
    printf("正在保存原始信号到文件...\n");
    result = save_signal_to_file(params.output_original_signal_file, time_array, original_signal, array_size);
    if (result != 0) {
        printf("原始信号保存失败，错误码: %d\n", result);
        free_signal_arrays(time_array, original_signal);
        return 3;
    }
    printf("原始信号已保存到: %s\n", params.output_original_signal_file);

    // 步骤4: 生成多目标多普勒接收信号（无噪声）
    double* clean_received_signal = NULL;

    printf("正在计算多普勒效应...\n");
    result = generate_multi_target_signal(&params, time_array, original_signal,
        array_size, &clean_received_signal);
    if (result != 0 || clean_received_signal == NULL) {
        printf("多普勒信号生成失败，错误码: %d\n", result);
        free_signal_arrays(time_array, original_signal);
        return 4;
    }
    printf("多普勒信号生成成功\n");

    // 保存无噪声接收信号
    printf("正在保存无噪声接收信号到文件...\n");
    result = save_signal_to_file(params.output_clean_received_signal_file,
        time_array, clean_received_signal, array_size);
    if (result != 0) {
        printf("无噪声接收信号保存失败，错误码: %d\n", result);
        free_signal_arrays(time_array, original_signal);
        free(clean_received_signal);
        return 5;
    }
    printf("无噪声接收信号已保存到: %s\n", params.output_clean_received_signal_file);

    // 步骤5: 如果启用噪声，添加噪声到接收信号
    double* noisy_received_signal = NULL;

    printf("正在添加噪声（SNR = %.2f dB）...\n", params.snr_db);
    result = add_noise(&params, clean_received_signal, array_size, &noisy_received_signal);
    if (result != 0 || noisy_received_signal == NULL) {
        printf("噪声添加失败，错误码: %d\n", result);
        free_signal_arrays(time_array, original_signal);
        free(clean_received_signal);
        return 6;
    }

    // 步骤6: 保存最终接收信号到文件
    printf("正在保存最终接收信号到文件...\n");
    result = save_signal_to_file(params.output_received_signal_file,
        time_array, noisy_received_signal, array_size);
    if (result != 0) {
        printf("最终接收信号保存失败，错误码: %d\n", result);
        free_signal_arrays(time_array, original_signal);
        free(clean_received_signal);
        free(noisy_received_signal);
        return 7;
    }
    printf("最终接收信号已保存到: %s\n", params.output_received_signal_file);

    // 步骤7: 计算并保存多普勒因子和参数回显
    double doppler_factors[2] = { 0.0, 0.0 };
    for (int i = 0; i < params.num_targets; i++) {
        doppler_factors[i] = calculate_doppler_factor(params.targets[i].velocity_mps, params.sound_speed_mps);
    }

    printf("正在保存参数回显到文件...\n");
    result = save_params_to_file(params.output_params_echo_file, &params, doppler_factors);
    if (result != 0) {
        printf("参数回显保存失败，错误码: %d\n", result);
    }
    else {
        printf("参数回显已保存到: %s\n", params.output_params_echo_file);
    }

    // 打印多普勒计算结果
    printf("\n多普勒计算结果:\n");
    for (int i = 0; i < params.num_targets; i++) {
        double rx_freq = params.frequency_hz * doppler_factors[i];
        double freq_shift = rx_freq - params.frequency_hz;

        printf("目标 %d (速度: %.2f m/s):\n", i + 1, params.targets[i].velocity_mps);
        printf("  多普勒因子: %.10f\n", doppler_factors[i]);
        printf("  接收频率: %.2f Hz (原始: %.2f Hz)\n", rx_freq, params.frequency_hz);
        printf("  频移量: %.2f Hz\n", freq_shift);
    }

    // 释放所有分配的内存
    free_signal_arrays(time_array, original_signal);
    free(clean_received_signal);
    free(noisy_received_signal);

    printf("\n仿真计算完成!\n");

    return 0;
}