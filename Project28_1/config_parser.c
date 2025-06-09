/**
 * config_parser.c
 * �����ļ�����ģ��ʵ��
 */

#define _CRT_SECURE_NO_WARNINGS
#include "config_parser.h"
#include <ctype.h>

 // ��������С
#define MAX_LINE_LENGTH 512


// ȥ���ַ���ǰ��հ�
static void trim(char* str) {
    if (str == NULL) return;

    // ȥ��ǰ���հ�
    char* start = str;
    while (isspace((unsigned char)*start)) start++;

    if (*start == '\0') {  // �ַ���ֻ�����հ�
        *str = '\0';
        return;
    }

    // ȥ��β���հ�
    char* end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';

    // �����ǰ���հף���Ҫ�ƶ��ַ���
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

void set_default_parameters(SimulationParameters* params) {
    if (params == NULL) return;

    // ��Դ����Ĭ��ֵ
    params->signal_type = SIGNAL_CW;
    params->frequency_hz = 1000.0;       // 1kHz
    params->pulse_duration_s = 1.0;      // 1��
    params->sampling_rate_hz = 10000.0;  // 10kHz
    params->lfm_bandwidth_hz = 500.0;    // 500Hz (����LFMģʽ��ʹ��)

    // ��������Ĭ��ֵ
    params->sound_speed_mps = 1500.0;    // 1500 m/s

    // Ŀ�����Ĭ��ֵ
    params->num_targets = 1;
    params->targets[0].velocity_mps = 10.0;      // 10 m/s
    params->targets[0].initial_amplitude = 1.0;  // ����Ϊ1
    params->targets[1].velocity_mps = -15.0;     // -15 m/s
    params->targets[1].initial_amplitude = 0.8;  // ����Ϊ0.8

    // ��������Ĭ��ֵ
    params->noise_enabled = 0;           // Ĭ�Ϲر�
    params->snr_db = 10.0;               // 10 dB

    // ����ļ�·��Ĭ��ֵ
    strcpy(params->output_original_signal_file, "original_signal.txt");
    strcpy(params->output_received_signal_file, "received_signal.txt");
    strcpy(params->output_clean_received_signal_file, "clean_received_signal.txt");
    strcpy(params->output_params_echo_file, "params_echo.txt");
}

int validate_parameters(SimulationParameters* params) {
    if (params == NULL) return -1;

    // ����������
    if (params->frequency_hz <= 0) return -2;
    if (params->pulse_duration_s <= 0) return -3;
    if (params->sampling_rate_hz <= 0) return -4;
    if (params->signal_type == SIGNAL_LFM && params->lfm_bandwidth_hz <= 0) return -5;

    // ��黷������
    if (params->sound_speed_mps <= 0) return -6;

    // ���Ŀ�����
    if (params->num_targets < 1 || params->num_targets > 2) return -7;

    // Ŀ�����ӦΪ��ֵ
    for (int i = 0; i < params->num_targets; i++) {
        if (params->targets[i].initial_amplitude <= 0) return -8;
    }

    // ���������������������
    if (params->noise_enabled && params->snr_db <= 0) return -9;

    return 0;  // ������Ч
}

int parse_config_file(const char* filename, SimulationParameters* params) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open config file '%s'\n", filename);
        return -1;
    }

    // ����Ĭ�ϲ���ֵ
    set_default_parameters(params);

    char line[MAX_LINE_LENGTH];
    char key[MAX_LINE_LENGTH];
    char value[MAX_LINE_LENGTH];
    char* comment_pos;

    // ���ж�ȡ������
    while (fgets(line, sizeof(line), file)) {
        // ȥ��ע�Ͳ���
        comment_pos = strchr(line, '#');
        if (comment_pos) *comment_pos = '\0';

        // ��������
        if (strlen(line) <= 1) continue;

        // ������ȡ��ֵ��
        if (sscanf(line, "%[^=]=%[^\n]", key, value) == 2) {
            // ȥ����ֵǰ��Ŀհ�
            trim(key);
            trim(value);

            // ���ݼ���������Ӧ�Ĳ���
            if (strcmp(key, "SIGNAL_TYPE") == 0) {
                if (strcmp(value, "CW") == 0) {
                    params->signal_type = SIGNAL_CW;
                }
                else if (strcmp(value, "LFM") == 0) {
                    params->signal_type = SIGNAL_LFM;
                }
                else {
                    printf("Warning: Unknown signal type '%s', using default (CW)\n", value);
                }
            }
            else if (strcmp(key, "FREQUENCY_HZ") == 0) {
                params->frequency_hz = atof(value);
            }
            else if (strcmp(key, "PULSE_DURATION_S") == 0) {
                params->pulse_duration_s = atof(value);
            }
            else if (strcmp(key, "SAMPLING_RATE_HZ") == 0) {
                params->sampling_rate_hz = atof(value);
            }
            else if (strcmp(key, "LFM_BANDWIDTH_HZ") == 0) {
                params->lfm_bandwidth_hz = atof(value);
            }
            else if (strcmp(key, "SOUND_SPEED_MPS") == 0) {
                params->sound_speed_mps = atof(value);
            }
            else if (strcmp(key, "NUM_TARGETS") == 0) {
                params->num_targets = atoi(value);
                // ȷ��Ŀ�������ں���Χ��
                if (params->num_targets < 1) params->num_targets = 1;
                if (params->num_targets > 2) params->num_targets = 2;
            }
            else if (strcmp(key, "TARGET1_VELOCITY_MPS") == 0) {
                params->targets[0].velocity_mps = atof(value);
            }
            else if (strcmp(key, "TARGET1_INITIAL_AMPLITUDE") == 0) {
                params->targets[0].initial_amplitude = atof(value);
            }
            else if (strcmp(key, "TARGET2_VELOCITY_MPS") == 0) {
                params->targets[1].velocity_mps = atof(value);
            }
            else if (strcmp(key, "TARGET2_INITIAL_AMPLITUDE") == 0) {
                params->targets[1].initial_amplitude = atof(value);
            }
            else if (strcmp(key, "NOISE_ENABLED") == 0) {
                params->noise_enabled = atoi(value);
            }
            else if (strcmp(key, "SNR_DB") == 0) {
                params->snr_db = atof(value);
            }
            else if (strcmp(key, "OUTPUT_ORIGINAL_SIGNAL_FILE") == 0) {
                strncpy(params->output_original_signal_file, value, sizeof(params->output_original_signal_file) - 1);
                params->output_original_signal_file[sizeof(params->output_original_signal_file) - 1] = '\0';
            }
            else if (strcmp(key, "OUTPUT_RECEIVED_SIGNAL_FILE") == 0) {
                strncpy(params->output_received_signal_file, value, sizeof(params->output_received_signal_file) - 1);
                params->output_received_signal_file[sizeof(params->output_received_signal_file) - 1] = '\0';
            }
            else if (strcmp(key, "OUTPUT_CLEAN_RECEIVED_SIGNAL_FILE") == 0) {
                strncpy(params->output_clean_received_signal_file, value, sizeof(params->output_clean_received_signal_file) - 1);
                params->output_clean_received_signal_file[sizeof(params->output_clean_received_signal_file) - 1] = '\0';
            }
            else if (strcmp(key, "OUTPUT_PARAMS_ECHO_FILE") == 0) {
                strncpy(params->output_params_echo_file, value, sizeof(params->output_params_echo_file) - 1);
                params->output_params_echo_file[sizeof(params->output_params_echo_file) - 1] = '\0';
            }
            else {
                printf("Warning: Unknown parameter '%s' in config file\n", key);
            }
        }
    }

    fclose(file);

    // ��֤�����Ϸ���
    int validation_result = validate_parameters(params);
    if (validation_result != 0) {
        printf("Error: Parameter validation failed with code %d\n", validation_result);
        return validation_result;
    }

    return 0;  // �����ɹ�
}