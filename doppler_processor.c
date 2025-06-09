/**
 * doppler_processor.c
 * ������ЧӦ����ģ��ʵ��
 */

#include "doppler_processor.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

double calculate_doppler_factor(double velocity_mps, double sound_speed_mps) {
    // ������ЧӦƵ��ƫ������
    // ���ڽӽ��˶� (velocity < 0)������Ƶ������
    // ����Զ���˶� (velocity > 0)������Ƶ�ʼ�С
    return (sound_speed_mps / (sound_speed_mps + velocity_mps));
}

// Ϊ����Ŀ������CW�źŵĶ�����ЧӦ
static void generate_doppler_cw(double frequency_hz, double doppler_factor,
    double target_amplitude, double* time_array,
    int array_size, double* received_signal) {
    // ���������Ƶ�ƺ��Ƶ��
    double doppler_frequency = frequency_hz * doppler_factor;

    for (int i = 0; i < array_size; i++) {
        received_signal[i] = target_amplitude * sin(2.0 * PI * doppler_frequency * time_array[i]);
    }
}

// Ϊ����Ŀ������LFM�źŵĶ�����ЧӦ
static void generate_doppler_lfm(double start_frequency_hz, double bandwidth_hz,
    double pulse_duration_s, double doppler_factor,
    double target_amplitude, double* time_array,
    int array_size, double* received_signal) {
    // ������ЧӦ��ı�LFM�źŵ���ʼƵ�ʺʹ���
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
        return -1;  // ������Ч
    }

    // �����ڴ�
    *received_signal = (double*)malloc(array_size * sizeof(double));
    if (*received_signal == NULL) {
        return -2;  // �ڴ����ʧ��
    }

    // ��ȡĿ����Ϣ
    double target_velocity = params->targets[target_index].velocity_mps;
    double target_amplitude = params->targets[target_index].initial_amplitude;

    // ���������Ƶ������
    double doppler_factor = calculate_doppler_factor(target_velocity, params->sound_speed_mps);

    // ���ɿ��Ƕ�����ЧӦ�Ľ����ź�
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
        // δ֪�ź�����
        free(*received_signal);
        *received_signal = NULL;
        return -3;
    }

    return 0;  // �ɹ�
}

int generate_multi_target_signal(SimulationParameters* params,
    double* time_array, double* original_signal,
    int array_size, double** clean_received_signal) {
    if (params == NULL || time_array == NULL || original_signal == NULL ||
        clean_received_signal == NULL || array_size <= 0) {
        return -1;  // ������Ч
    }

    // �����ڴ�
    *clean_received_signal = (double*)calloc(array_size, sizeof(double));
    if (*clean_received_signal == NULL) {
        return -2;  // �ڴ����ʧ��
    }

    // ����ÿ��Ŀ��Ĺ��ײ�����
    for (int target_idx = 0; target_idx < params->num_targets; target_idx++) {
        double* target_signal = NULL;

        // ���ɵ�ǰĿ��Ķ������ź�
        int result = generate_doppler_signal_for_target(params, target_idx,
            time_array, original_signal,
            array_size, &target_signal);

        if (result != 0 || target_signal == NULL) {
            // ��������ͷ��ѷ����ڴ�
            free(*clean_received_signal);
            *clean_received_signal = NULL;
            return -3;
        }

        // ���ӵ�ǰĿ����źŵ��ܽ����ź�
        for (int i = 0; i < array_size; i++) {
            (*clean_received_signal)[i] += target_signal[i];
        }

        // �ͷŵ�ǰĿ����ź��ڴ�
        free(target_signal);
    }

    return 0;  // �ɹ�
}