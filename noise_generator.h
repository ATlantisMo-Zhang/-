/**
 * noise_generator.h
 * ��������ģ��ͷ�ļ�
 */

#ifndef NOISE_GENERATOR_H
#define NOISE_GENERATOR_H

#include "simulation_params.h"

 /**
  * ���ɸ�˹����������ӵ��ź���
  *
  * @param params �������
  * @param clean_signal �������ź�����
  * @param array_size �����С
  * @param noisy_signal ��������������ź�����
  * @return �ɹ�����0��ʧ�ܷ��ش�����
  */
int add_noise(SimulationParameters* params, double* clean_signal,
    int array_size, double** noisy_signal);

/**
 * �����źŵ�ƽ������
 *
 * @param signal �ź�����
 * @param array_size �����С
 * @return �źŵ�ƽ������
 */
double calculate_signal_power(double* signal, int array_size);

#endif /* NOISE_GENERATOR_H */