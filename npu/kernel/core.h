#ifndef _CORE_H_
#define _CORE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "yolov5.h"
#include "image_utils.h"
#include "file_utils.h"
#include "image_drawing.h"
#define OPENCV 1
#if OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif
#include <unistd.h>

typedef int (*USER_CB) (void * p_arg);

typedef struct session_str {
	/*camera*/
	cv::VideoCapture cap;
	/*origin image*/	
	cv::Mat bgr;

	rknn_app_context_t ctx;
	image_buffer_t src_image;
	USER_CB cb;
	object_detect_result_list od_results;
} session_str;

int camera_init(session_str * entity);
cv::Mat camera_read(session_str * entity);
int camera_deinit(session_str * entity);
void updatefps();

/* user API for AI engine */
int preprocess(session_str * entity);
int postprocess(session_str * entity);
int session_init(session_str ** entity, const char * model_name);
int session_deinit(session_str * entity);
int inference(session_str * entity);
int set_user_cb(session_str * entity, USER_CB cb);

/* c++ class API*/
class session_cls {
	cv::VideoCapture cap;
	cv::Mat bgr;

	rknn_app_context_t ctx;
	image_buffer_t src_image;
	USER_CB cb;
	object_detect_result_list od_results;

	session_cls(char * model_path);
	int cap_init();
	int cap_read();
	int cap_deinit();

	int preprocess();
	int postprocess();
	int session_init(const char * model_name);
	int session_deinit();
	int inference();
	int set_user_cb(USER_CB cb);
};

/* debug printf*/
#define DEBUG 1
#if DEBUG
#define os_printf(format, ...) \
	{printf("[%s : %s : %d] ", \
	__FILE__, __func__, __LINE__); \
	printf(format, ##__VA_ARGS__);}
#else
#define os_printf(format, ...) 
#endif

#endif
