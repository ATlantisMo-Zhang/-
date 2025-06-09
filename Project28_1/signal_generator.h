/**
 * signal_generator.h
 * �ź�����ģ��ͷ�ļ�
 */

#ifndef SIGNAL_GENERATOR_H
#define SIGNAL_GENERATOR_H

#include "simulation_params.h"

 /**
  * ����ԭʼ�ź�
  *
  * @param params �������
  * @param time_array ���ʱ������
  * @param signal_array ����ź�����
  * @param array_size �������Ĵ�С��Ԫ�ظ�����
  * @return �ɹ�����0��ʧ�ܷ��ش�����
  */
int generate_signal(SimulationParameters* params, double** time_array, double** signal_array, int* array_size);

/**
 * �ͷ����ɵ��źź�ʱ������
 *
 * @param time_array ʱ������ָ��
 * @param signal_array �ź�����ָ��
 */
void free_signal_arrays(double* time_array, double* signal_array);

#endif /* SIGNAL_GENERATOR_H */#pragma once
