/**
 * file_writer.c
 * �ļ�д��ģ��ʵ��
 */

#include "file_writer.h"
#include <stdio.h>

int save_signal_to_file(const char* filename, double* time_array,
    double* signal_array, int array_size) {
    if (filename == NULL || time_array == NULL || signal_array == NULL || array_size <= 0) {
        return -1;  // ������Ч
    }

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Cannot open file '%s' for writing\n", filename);
        return -2;  // �޷����ļ�
    }

    // д���б���
    fprintf(file, "# Time(s) Signal\n");

    // д������
    for (int i = 0; i < array_size; i++) {
        fprintf(file, "%.10e %.10e\n", time_array[i], signal_array[i]);
    }

    fclose(file);
    return 0;  // �ɹ�
}

int save_params_to_file(const char* filename, SimulationParameters* params,
    double* doppler_factors) {
    if (filename == NULL || params == NULL) {
        return -1;  // ������Ч
    }

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Cannot open file '%s' for writing\n", filename);
        return -2;  // �޷����ļ�
    }

    // д���������
    fprintf(file, "# �����������\n\n");

    // ��Դ����
    fprintf(file, "# ��Դ����\n");
    fprintf(file, "SIGNAL_TYPE = %s\n", params->signal_type == SIGNAL_CW ? "CW" : "LFM");
    fprintf(file, "FREQUENCY_HZ = %.6f\n", params->frequency_hz);
    fprintf(file, "PULSE_DURATION_S = %.6f\n", params->pulse_duration_s);
    fprintf(file, "SAMPLING_RATE_HZ = %.6f\n", params->sampling_rate_hz);
    if (params->signal_type == SIGNAL_LFM) {
        fprintf(file, "LFM_BANDWIDTH_HZ = %.6f\n", params->lfm_bandwidth_hz);
    }
    fprintf(file, "\n");

    // ��������
    fprintf(file, "# ��������\n");
    fprintf(file, "SOUND_SPEED_MPS = %.6f\n", params->sound_speed_mps);
    fprintf(file, "\n");

    // Ŀ�����
    fprintf(file, "# Ŀ�����\n");
    fprintf(file, "NUM_TARGETS = %d\n", params->num_targets);

    for (int i = 0; i < params->num_targets; i++) {
        fprintf(file, "\n# Ŀ�� %d\n", i + 1);
        fprintf(file, "TARGET%d_VELOCITY_MPS = %.6f\n", i + 1, params->targets[i].velocity_mps);
        fprintf(file, "TARGET%d_INITIAL_AMPLITUDE = %.6f\n", i + 1, params->targets[i].initial_amplitude);

        // ����ṩ�˶��������ӣ���Ҳ���
        if (doppler_factors != NULL) {
            fprintf(file, "# ����õ��Ķ�����Ƶ������ = %.10f\n", doppler_factors[i]);

            // ���㲢�������Ƶ��
            double rx_freq = params->frequency_hz * doppler_factors[i];
            fprintf(file, "# �����ź�����Ƶ�� = %.6f Hz\n", rx_freq);

            // ���㲢���Ƶ����
            double freq_shift = rx_freq - params->frequency_hz;
            fprintf(file, "# Ƶ���� = %.6f Hz\n", freq_shift);
        }
    }
    fprintf(file, "\n");

    // ��������
    fprintf(file, "# ��������\n");
    fprintf(file, "NOISE_ENABLED = %d\n", params->noise_enabled);
    if (params->noise_enabled) {
        fprintf(file, "SNR_DB = %.6f\n", params->snr_db);
    }
    fprintf(file, "\n");

    // ����ļ�·��
    fprintf(file, "# ����ļ�·��\n");
    fprintf(file, "OUTPUT_ORIGINAL_SIGNAL_FILE = %s\n", params->output_original_signal_file);
    fprintf(file, "OUTPUT_RECEIVED_SIGNAL_FILE = %s\n", params->output_received_signal_file);
    fprintf(file, "OUTPUT_CLEAN_RECEIVED_SIGNAL_FILE = %s\n", params->output_clean_received_signal_file);
    fprintf(file, "OUTPUT_PARAMS_ECHO_FILE = %s\n", params->output_params_echo_file);

    fclose(file);
    return 0;  // �ɹ�
}