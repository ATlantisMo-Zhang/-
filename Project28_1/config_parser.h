/**
 * config_parser.h
 * �����ļ�����ģ��ͷ�ļ�
 * ���ڽ�����MATLAB���ɵ������ļ�
 */

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulation_params.h"

 /**
  * ���������ļ����������ṹ��
  *
  * @param filename �����ļ�·��
  * @param params �����ṹ��ָ�룬���ڴ洢������Ĳ���
  * @return �ɹ�����0��ʧ�ܷ��ش�����
  */
int parse_config_file(const char* filename, SimulationParameters* params);

/**
 * Ϊ��������Ĭ��ֵ
 * �������ļ���ĳЩ����ȱʧʱ��ʹ����ЩĬ��ֵ
 *
 * @param params �����ṹ��ָ��
 */
void set_default_parameters(SimulationParameters* params);

/**
 * �������Ϸ���
 *
 * @param params �����ṹ��ָ��
 * @return �Ϸ�����0�����Ϸ����ش�����
 */
int validate_parameters(SimulationParameters* params);

#endif /* CONFIG_PARSER_H */#pragma once
