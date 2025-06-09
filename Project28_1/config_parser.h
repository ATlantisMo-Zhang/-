/**
 * config_parser.h
 * 配置文件解析模块头文件
 * 用于解析由MATLAB生成的配置文件
 */

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulation_params.h"

 /**
  * 解析配置文件，填充参数结构体
  *
  * @param filename 配置文件路径
  * @param params 参数结构体指针，用于存储解析后的参数
  * @return 成功返回0，失败返回错误码
  */
int parse_config_file(const char* filename, SimulationParameters* params);

/**
 * 为参数设置默认值
 * 当配置文件中某些参数缺失时，使用这些默认值
 *
 * @param params 参数结构体指针
 */
void set_default_parameters(SimulationParameters* params);

/**
 * 检查参数合法性
 *
 * @param params 参数结构体指针
 * @return 合法返回0，不合法返回错误码
 */
int validate_parameters(SimulationParameters* params);

#endif /* CONFIG_PARSER_H */#pragma once
