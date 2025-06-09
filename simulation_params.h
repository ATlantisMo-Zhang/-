/**
 * simulation_params.h
 * ����������ݽṹ����
 */

#ifndef SIMULATION_PARAMS_H
#define SIMULATION_PARAMS_H

 // �ź�����ö��
typedef enum {
    SIGNAL_CW,  // �������ź�
    SIGNAL_LFM  // ���Ե�Ƶ�ź�
} SignalType;

// ����Ŀ����Ϣ�ṹ��
typedef struct {
    double velocity_mps;      // Ŀ���ٶ� (m/s)����ΪԶ�룬��Ϊ�ӽ�
    double initial_amplitude; // Ŀ���źų�ʼ����
} TargetInfo;

// ��������ṹ��
typedef struct {
    // ��Դ����
    SignalType signal_type;       // �ź�����
    double frequency_hz;          // ��ʼƵ�� (Hz)
    double pulse_duration_s;      // �������ʱ�� (s)
    double sampling_rate_hz;      // ������ (Hz)
    double lfm_bandwidth_hz;      // LFM���� (Hz)����������LFM�ź�

    // ��������
    double sound_speed_mps;       // ˮ������ (m/s)

    // Ŀ�����
    int num_targets;              // Ŀ������
    TargetInfo targets[2];        // ���֧��2��Ŀ��

    // ��������
    int noise_enabled;            // �Ƿ���������
    double snr_db;                // ����� (dB)

    // ����ļ�·��
    char output_original_signal_file[256];    // ԭʼ�ź�����ļ�
    char output_received_signal_file[256];    // �����ź�����ļ�
    char output_clean_received_signal_file[256]; // �����������ź�����ļ�
    char output_params_echo_file[256];        // ���������ļ�
} SimulationParameters;

#endif /* SIMULATION_PARAMS_H */#pragma once
