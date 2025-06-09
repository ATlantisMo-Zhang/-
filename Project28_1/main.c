/**
 * main.c
 * ˮ��Ŀ�������ЧӦģ������������������
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
    printf("ˮ��Ŀ�������ЧӦģ���������� (C���)\n");
    printf("--------------------------------------\n\n");

    // Ĭ�������ļ�·��
    const char* config_file = "config.txt";

    // ����������в�������ʹ��ָ���������ļ�·��
    if (argc > 1) {
        config_file = argv[1];
    }

    printf("ʹ�������ļ�: %s\n", config_file);

    // ����1: ���غͽ��������ļ�
    SimulationParameters params;
    memset(&params, 0, sizeof(SimulationParameters));

    printf("���ڼ��������ļ�...\n");
    int result = parse_config_file(config_file, &params);
    if (result != 0) {
        printf("�����ļ�����ʧ�ܣ�������: %d\n", result);
        return 1;
    }
    printf("�����ļ����سɹ�\n");

    // ����2: ����ԭʼ�ź�
    double* time_array = NULL;
    double* original_signal = NULL;
    int array_size = 0;

    printf("��������ԭʼ�ź�...\n");
    result = generate_signal(&params, &time_array, &original_signal, &array_size);
    if (result != 0 || time_array == NULL || original_signal == NULL || array_size <= 0) {
        printf("�ź�����ʧ�ܣ�������: %d\n", result);
        return 2;
    }
    printf("ԭʼ�ź����ɳɹ�����������: %d\n", array_size);

    // ����3: ����ԭʼ�źŵ��ļ�
    printf("���ڱ���ԭʼ�źŵ��ļ�...\n");
    result = save_signal_to_file(params.output_original_signal_file, time_array, original_signal, array_size);
    if (result != 0) {
        printf("ԭʼ�źű���ʧ�ܣ�������: %d\n", result);
        free_signal_arrays(time_array, original_signal);
        return 3;
    }
    printf("ԭʼ�ź��ѱ��浽: %s\n", params.output_original_signal_file);

    // ����4: ���ɶ�Ŀ������ս����źţ���������
    double* clean_received_signal = NULL;

    printf("���ڼ��������ЧӦ...\n");
    result = generate_multi_target_signal(&params, time_array, original_signal,
        array_size, &clean_received_signal);
    if (result != 0 || clean_received_signal == NULL) {
        printf("�������ź�����ʧ�ܣ�������: %d\n", result);
        free_signal_arrays(time_array, original_signal);
        return 4;
    }
    printf("�������ź����ɳɹ�\n");

    // ���������������ź�
    printf("���ڱ��������������źŵ��ļ�...\n");
    result = save_signal_to_file(params.output_clean_received_signal_file,
        time_array, clean_received_signal, array_size);
    if (result != 0) {
        printf("�����������źű���ʧ�ܣ�������: %d\n", result);
        free_signal_arrays(time_array, original_signal);
        free(clean_received_signal);
        return 5;
    }
    printf("�����������ź��ѱ��浽: %s\n", params.output_clean_received_signal_file);

    // ����5: ���������������������������ź�
    double* noisy_received_signal = NULL;

    printf("�������������SNR = %.2f dB��...\n", params.snr_db);
    result = add_noise(&params, clean_received_signal, array_size, &noisy_received_signal);
    if (result != 0 || noisy_received_signal == NULL) {
        printf("�������ʧ�ܣ�������: %d\n", result);
        free_signal_arrays(time_array, original_signal);
        free(clean_received_signal);
        return 6;
    }

    // ����6: �������ս����źŵ��ļ�
    printf("���ڱ������ս����źŵ��ļ�...\n");
    result = save_signal_to_file(params.output_received_signal_file,
        time_array, noisy_received_signal, array_size);
    if (result != 0) {
        printf("���ս����źű���ʧ�ܣ�������: %d\n", result);
        free_signal_arrays(time_array, original_signal);
        free(clean_received_signal);
        free(noisy_received_signal);
        return 7;
    }
    printf("���ս����ź��ѱ��浽: %s\n", params.output_received_signal_file);

    // ����7: ���㲢������������ӺͲ�������
    double doppler_factors[2] = { 0.0, 0.0 };
    for (int i = 0; i < params.num_targets; i++) {
        doppler_factors[i] = calculate_doppler_factor(params.targets[i].velocity_mps, params.sound_speed_mps);
    }

    printf("���ڱ���������Ե��ļ�...\n");
    result = save_params_to_file(params.output_params_echo_file, &params, doppler_factors);
    if (result != 0) {
        printf("�������Ա���ʧ�ܣ�������: %d\n", result);
    }
    else {
        printf("���������ѱ��浽: %s\n", params.output_params_echo_file);
    }

    // ��ӡ�����ռ�����
    printf("\n�����ռ�����:\n");
    for (int i = 0; i < params.num_targets; i++) {
        double rx_freq = params.frequency_hz * doppler_factors[i];
        double freq_shift = rx_freq - params.frequency_hz;

        printf("Ŀ�� %d (�ٶ�: %.2f m/s):\n", i + 1, params.targets[i].velocity_mps);
        printf("  ����������: %.10f\n", doppler_factors[i]);
        printf("  ����Ƶ��: %.2f Hz (ԭʼ: %.2f Hz)\n", rx_freq, params.frequency_hz);
        printf("  Ƶ����: %.2f Hz\n", freq_shift);
    }

    // �ͷ����з�����ڴ�
    free_signal_arrays(time_array, original_signal);
    free(clean_received_signal);
    free(noisy_received_signal);

    printf("\n����������!\n");

    return 0;
}