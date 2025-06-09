/**
 * file_writer.h
 * �ļ�д��ģ��ͷ�ļ�
 */

#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include "simulation_params.h"

 /**
  * ����ԭʼ�źŵ��ļ�
  *
  * @param filename ����ļ���
  * @param time_array ʱ������
  * @param signal_array �ź�����
  * @param array_size �����С
  * @return �ɹ�����0��ʧ�ܷ��ش�����
  */
int save_signal_to_file(const char* filename, double* time_array,
    double* signal_array, int array_size);

/**
 * ����������Ե��ļ�
 *
 * @param filename ����ļ���
 * @param params �������
 * @param doppler_factors ����õ��Ķ��������ӣ�����ΪNULL
 * @return �ɹ�����0��ʧ�ܷ��ش�����
 */
int save_params_to_file(const char* filename, SimulationParameters* params,
    double* doppler_factors);

#endif /* FILE_WRITER_H */#pragma once
