/**
 * signal_generator.c
 * �ź�����ģ��ʵ��
 */

#include "signal_generator.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

 /**
  * ���������� (CW) �ź�
  *
  * @param frequency_hz �ź�Ƶ�� (Hz)
  * @param time_array ʱ������
  * @param array_size �����С
  * @param signal_array ����ź�����
  */
static void generate_cw_signal(double frequency_hz, double* time_array, int array_size, double* signal_array) {
    for (int i = 0; i < array_size; i++) {
        signal_array[i] = sin(2.0 * PI * frequency_hz * time_array[i]);
    }
}

/**
 * �������Ե�Ƶ (LFM) �ź�
 *
 * @param start_frequency_hz ��ʼƵ�� (Hz)
 * @param bandwidth_hz ���� (Hz)
 * @param pulse_duration_s �������ʱ�� (s)
 * @param time_array ʱ������
 * @param array_size �����С
 * @param signal_array ����ź�����
 */
static void generate_lfm_signal(double start_frequency_hz, double bandwidth_hz, double pulse_duration_s,
    double* time_array, int array_size, double* signal_array) {
    // �����Ƶ�� (Hz/s)
    double chirp_rate = bandwidth_hz / pulse_duration_s;

    for (int i = 0; i < array_size; i++) {
        // LFM�źŵ���λ��ʱ���ƽ���仯
        double instantaneous_freq = start_frequency_hz + chirp_rate * time_array[i] / 2.0;
        double phase = 2.0 * PI * (start_frequency_hz * time_array[i] + 0.5 * chirp_rate * time_array[i] * time_array[i]);
        signal_array[i] = sin(phase);
    }
}

int generate_signal(SimulationParameters* params, double** time_array, double** signal_array, int* array_size) {
    if (params == NULL || time_array == NULL || signal_array == NULL || array_size == NULL) {
        return -1;  // ��Ч����
    }

    // �����������
    *array_size = (int)(params->pulse_duration_s * params->sampling_rate_hz);
    if (*array_size <= 0) {
        return -2;  // ����������Ч
    }

    // �����ڴ�
    *time_array = (double*)malloc(*array_size * sizeof(double));
    *signal_array = (double*)malloc(*array_size * sizeof(double));

    if (*time_array == NULL || *signal_array == NULL) {
        // �ڴ����ʧ�ܣ��ͷ��ѷ�����ڴ�
        if (*time_array != NULL) free(*time_array);
        if (*signal_array != NULL) free(*signal_array);
        *time_array = NULL;
        *signal_array = NULL;
        return -3;  // �ڴ����ʧ��
    }

    // ����ʱ������
    double time_step = 1.0 / params->sampling_rate_hz;
    for (int i = 0; i < *array_size; i++) {
        (*time_array)[i] = i * time_step;
    }

    // �����ź����������ź�
    if (params->signal_type == SIGNAL_CW) {
        generate_cw_signal(params->frequency_hz, *time_array, *array_size, *signal_array);
    }
    else if (params->signal_type == SIGNAL_LFM) {
        generate_lfm_signal(params->frequency_hz, params->lfm_bandwidth_hz,
            params->pulse_duration_s, *time_array, *array_size, *signal_array);
    }
    else {
        // δ֪�ź����ͣ��ͷ��ڴ沢���ش���
        free(*time_array);
        free(*signal_array);
        *time_array = NULL;
        *signal_array = NULL;
        return -4;  // δ֪�ź�����
    }

    return 0;  // �ɹ�
}

void free_signal_arrays(double* time_array, double* signal_array) {
    if (time_array != NULL) free(time_array);
    if (signal_array != NULL) free(signal_array);
}