/**
 * doppler_processor.h
 * ������ЧӦ����ģ��ͷ�ļ�
 */

#ifndef DOPPLER_PROCESSOR_H
#define DOPPLER_PROCESSOR_H

#include "simulation_params.h"

 /**
  * ����Ŀ���ٶȼ��������Ƶ��ƫ������
  *
  * @param velocity_mps Ŀ���ٶ� (m/s)����ΪԶ�룬��Ϊ�ӽ�
  * @param sound_speed_mps ���� (m/s)
  * @return ������Ƶ��ƫ������ (����Ƶ��/����Ƶ��)
  */
double calculate_doppler_factor(double velocity_mps, double sound_speed_mps);

/**
 * Ϊ����Ŀ�����ɿ��Ƕ�����ЧӦ�Ľ����ź�
 *
 * @param params �������
 * @param target_index Ŀ������ (0��1)
 * @param time_array ʱ������
 * @param original_signal ԭʼ�ź�����
 * @param array_size �����С
 * @param received_signal ��������ź�����
 * @return �ɹ�����0��ʧ�ܷ��ش�����
 */
int generate_doppler_signal_for_target(SimulationParameters* params, int target_index,
    double* time_array, double* original_signal,
    int array_size, double** received_signal);

/**
 * ���ɶ�Ŀ����ӵĽ����źţ���������
 *
 * @param params �������
 * @param time_array ʱ������
 * @param original_signal ԭʼ�ź�����
 * @param array_size �����С
 * @param clean_received_signal ��������������ź�����
 * @return �ɹ�����0��ʧ�ܷ��ش�����
 */
int generate_multi_target_signal(SimulationParameters* params,
    double* time_array, double* original_signal,
    int array_size, double** clean_received_signal);

#endif /* DOPPLER_PROCESSOR_H */#pragma once
