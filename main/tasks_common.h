/*
 * tasks_common.h
 *
 *  Created on: Oct 21, 2022
 *      Author: mykolaluniv
 */

#ifndef MAIN_TASKS_COMMON_H_
#define MAIN_TASKS_COMMON_H_

// WiFi application task
#define WIFI_APP_TASK_STACK_SIZE			4096
#define WIFI_APP_TASK_PRIORITY				5
#define WIFI_APP_TASK_CORE_ID				0

// HTTP Server task
#define HTTP_SERVER_TASK_STACK_SIZE			8192
#define HTTP_SERVER_TASK_PRIORITY			4
#define HTTP_SERVER_TASK_CORE_ID			0

#endif /* MAIN_TASKS_COMMON_H_ */
