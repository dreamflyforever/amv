/*
** easyai core.cc
**
*/
#include "core.h"

#if defined(RV1106_1103) 
    #include "dma_alloc.cpp"
#endif

struct timeval start_time, stop_time;
int framecount = 0;
time_t lasttime;
void updatefps()
{
	framecount++;

	time_t currentTime;
	time(&currentTime);

	double deltaTime = difftime(currentTime, lasttime);

	if (deltaTime >= 1) {
		double fps = framecount / deltaTime;

		printf(">>>>>>>> FPS: %.2f\n", fps);

		framecount = 0;
		time(&lasttime);
	}
}
double __get_us(struct timeval t) { return (t.tv_sec * 1000000 + t.tv_usec); }


cv::VideoCapture g_cap;
cv::Mat g_bgr;
int camera_init(session_str * entity)
{
	int retval = 0;
	g_cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	g_cap.set(cv::CAP_PROP_FRAME_HEIGHT, 640);
	g_cap.open(0);
#if 0
	cv::VideoCapture cap;
	memcpy(&(entity->cap), &cap, sizeof(cv::VideoCapture));
	entity->cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	os_printf("hello\n");
	entity->cap.set(cv::CAP_PROP_FRAME_HEIGHT, 640);
	os_printf("hello\n");
	entity->cap.open(0);
	os_printf("hello\n");

	const int w = entity->cap.get(cv::CAP_PROP_FRAME_WIDTH);
	const int h = entity->cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	fprintf(stderr, "[w,h] %d x %d\n", w, h);
#endif
	return retval;
}
cv::Mat camera_read(session_str * entity)
{
#if 0
	entity->cap >> entity->bgr;
#endif
	g_cap >> g_bgr;
	return g_bgr;

}

int camera_deinit(session_str * entity)
{
	g_cap.release();
	return 0;
}

/* user API for AI engine */
int preprocess(session_str * entity)
{
	start_time = stop_time;
	cv::Mat img;
	cv::cvtColor(g_bgr, img, cv::COLOR_BGR2RGB);
	memcpy(entity->src_image.virt_addr, img.data, entity->src_image.size);
	gettimeofday(&stop_time, NULL);
	printf("preprocess use %f ms\n", (__get_us(stop_time) - __get_us(start_time)) / 1000);
	return 0;
}
int postprocess(session_str * entity)
{
	int retval = 0;
	char text[256];
	for (int i = 0; i < entity->od_results.count; i++) {
		object_detect_result *det_result = &(entity->od_results.results[i]);
		printf("%s @ (%d %d %d %d) %.3f\n", coco_cls_to_name(det_result->cls_id),
				det_result->box.left, det_result->box.top,
				det_result->box.right, det_result->box.bottom,
				det_result->prop);
		int x1 = det_result->box.left;
		int y1 = det_result->box.top;
		int x2 = det_result->box.right;
		int y2 = det_result->box.bottom;

		draw_rectangle(&(entity->src_image), x1, y1, x2 - x1, y2 - y1, COLOR_BLUE, 3);

		sprintf(text, "%s %.1f%%", coco_cls_to_name(det_result->cls_id), det_result->prop * 100);
			draw_text(&(entity->src_image), text, x1, y1 - 20, COLOR_RED, 10);
		}
		return retval;
}
int session_init(session_str ** entity, const char * model_name)
{
	int retval = 0;
	*entity = (session_str * )malloc(sizeof(session_str));

	memset(&((*entity)->ctx), 0, sizeof(rknn_app_context_t));

	init_post_process();

	int ret = init_yolov5_model(model_name, &((*entity)->ctx));
	if (ret != 0) {
		printf("init_yolov5_model fail! ret=%d model_path=%s\n", ret, model_name);
		while (1);
		//goto out;
	}
	(*entity)->src_image.width = 640;//img.rows;
	(*entity)->src_image.height = 640;//img.cols;
	(*entity)->src_image.format = IMAGE_FORMAT_RGB888;
	(*entity)->src_image.size = 1228800;//img.rows * img.cols * 3;
	(*entity)->src_image.virt_addr = (unsigned char*)malloc((*entity)->src_image.size);
	//RV1106 rga requires that input and output bufs are memory allocated by dma
	ret = dma_buf_alloc(RV1106_CMA_HEAP_PATH, (*entity)->src_image.size, &((*entity)->ctx).img_dma_buf.dma_buf_fd, 
			(void **) & (((*entity)->ctx).img_dma_buf.dma_buf_virt_addr));
	memcpy(((*entity)->ctx).img_dma_buf.dma_buf_virt_addr, (*entity)->src_image.virt_addr, (*entity)->src_image.size);
	dma_sync_cpu_to_device(((*entity)->ctx).img_dma_buf.dma_buf_fd);
	free((*entity)->src_image.virt_addr);
	(*entity)->src_image.virt_addr = (unsigned char *)((*entity)->ctx).img_dma_buf.dma_buf_virt_addr;
	return retval;
}

int session_deinit(session_str * entity)
{
	deinit_post_process();

	int ret = release_yolov5_model(&(entity->ctx));
	if (ret != 0) {
		printf("release_yolov5_model fail! ret=%d\n", ret);
	}

	if (entity->src_image.virt_addr != NULL) {
#if defined(RV1106_1103) 
		dma_buf_free(entity->ctx.img_dma_buf.size, &(entity->ctx).img_dma_buf.dma_buf_fd, 
				entity->ctx.img_dma_buf.dma_buf_virt_addr);
#else
		free(entity->src_image.virt_addr);
#endif
	}

	free(entity);
	return ret;
}
int inference(session_str * entity)
{
	int retval;
	start_time = stop_time;
	int ret = inference_yolov5_model(&(entity->ctx), &(entity->src_image), &(entity->od_results));
	if (ret != 0)
	{
		printf("init_yolov5_model fail! ret=%d\n", ret);
		while (1);
		//goto out;
	}
	gettimeofday(&stop_time, NULL);
	printf("inference use %f ms\n", (__get_us(stop_time) - __get_us(start_time)) / 1000);

	return retval;
}

/*-------------------------------------------
                  Main Function
-------------------------------------------*/
#if 0
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("%s <model_path>\n", argv[0]);
		return -1;
	}

	const char *model_path = argv[1];
	//const char *image_path = argv[2];

	int ret;
	rknn_app_context_t rknn_app_ctx;
	memset(&rknn_app_ctx, 0, sizeof(rknn_app_context_t));

	init_post_process();

	ret = init_yolov5_model(model_path, &rknn_app_ctx);
	if (ret != 0)
	{
		printf("init_yolov5_model fail! ret=%d model_path=%s\n", ret, model_path);
		while (1);
		//goto out;
	}
#if OPENCV
	//camera_init();
	cv::VideoCapture cap;
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 640);
	cap.open(0);

	const int w = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	const int h = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	fprintf(stderr, "[w,h] %d x %d\n", w, h);
#endif

#if OPENCV
	cv::Mat bgr[9];
	for (int i = 0; i < 9; i++)
	{
		cap >> bgr[i];
	}
	cv::Mat img;
	cv::cvtColor(bgr[8], img, cv::COLOR_BGR2RGB);
	cv::imwrite("out.jpg", img);
	printf(">>>> %d\n", __LINE__);
	//cap.release();
#endif
	const char *image_path = "out.jpg";
	image_buffer_t src_image;
	memset(&src_image, 0, sizeof(image_buffer_t));
#if 1
	src_image.width = 640;//img.rows;
	src_image.height = 640;//img.cols;
	src_image.format = IMAGE_FORMAT_RGB888;
	src_image.size = 1228800;//img.rows * img.cols * 3;
	src_image.virt_addr = (unsigned char*)malloc(src_image.size);
	memcpy(src_image.virt_addr, img.data, src_image.size);
#endif

#if defined(RV1106_1103) 
	//RV1106 rga requires that input and output bufs are memory allocated by dma
	ret = dma_buf_alloc(RV1106_CMA_HEAP_PATH, src_image.size, &rknn_app_ctx.img_dma_buf.dma_buf_fd, 
			(void **) & (rknn_app_ctx.img_dma_buf.dma_buf_virt_addr));
	memcpy(rknn_app_ctx.img_dma_buf.dma_buf_virt_addr, src_image.virt_addr, src_image.size);
	dma_sync_cpu_to_device(rknn_app_ctx.img_dma_buf.dma_buf_fd);
	free(src_image.virt_addr);
	src_image.virt_addr = (unsigned char *)rknn_app_ctx.img_dma_buf.dma_buf_virt_addr;
#endif

	if (ret != 0)
	{
		printf("read image fail! ret=%d image_path=%s\n", ret, image_path);
		while (1);
		//goto out;
	}

	while (1) {

		gettimeofday(&start_time, NULL);
		cap >> bgr[0];
		gettimeofday(&stop_time, NULL);
		printf("read camera use %f ms\n", (__get_us(stop_time) - __get_us(start_time)) / 1000);
		
		start_time = stop_time;
		cv::Mat img;
		cv::cvtColor(bgr[0], img, cv::COLOR_BGR2RGB);
		memcpy(src_image.virt_addr, img.data, src_image.size);

		gettimeofday(&stop_time, NULL);
		printf("preprocess use %f ms\n", (__get_us(stop_time) - __get_us(start_time)) / 1000);

		start_time = stop_time;
		object_detect_result_list od_results;

		ret = inference_yolov5_model(&rknn_app_ctx, &src_image, &od_results);
		if (ret != 0)
		{
			printf("init_yolov5_model fail! ret=%d\n", ret);
			while (1);
			//goto out;
		}
		gettimeofday(&stop_time, NULL);
		printf("inference use %f ms\n", (__get_us(stop_time) - __get_us(start_time)) / 1000);
#if 1
		// 画框和概率
		char text[256];
		for (int i = 0; i < od_results.count; i++)
		{
			object_detect_result *det_result = &(od_results.results[i]);
			printf("%s @ (%d %d %d %d) %.3f\n", coco_cls_to_name(det_result->cls_id),
					det_result->box.left, det_result->box.top,
					det_result->box.right, det_result->box.bottom,
					det_result->prop);
			int x1 = det_result->box.left;
			int y1 = det_result->box.top;
			int x2 = det_result->box.right;
			int y2 = det_result->box.bottom;

			draw_rectangle(&src_image, x1, y1, x2 - x1, y2 - y1, COLOR_BLUE, 3);

			sprintf(text, "%s %.1f%%", coco_cls_to_name(det_result->cls_id), det_result->prop * 100);
			draw_text(&src_image, text, x1, y1 - 20, COLOR_RED, 10);
		}
#endif
		updatefps();
#if 0
		char *str = (char *)malloc(20);
		memset(str, 0, 20);
		static int t = 0;
		sprintf(str, "%d_out.jpeg", t);
		write_image(str, &src_image);
		free(str);
		t++;
#endif
	}
#if 1
out:
	deinit_post_process();

	ret = release_yolov5_model(&rknn_app_ctx);
	if (ret != 0)
	{
		printf("release_yolov5_model fail! ret=%d\n", ret);
	}

	if (src_image.virt_addr != NULL)
	{
#if defined(RV1106_1103) 
		dma_buf_free(rknn_app_ctx.img_dma_buf.size, &rknn_app_ctx.img_dma_buf.dma_buf_fd, 
				rknn_app_ctx.img_dma_buf.dma_buf_virt_addr);
#else
		free(src_image.virt_addr);
#endif
	}
#endif

	return 0;
}
#endif

session_cls :: session_cls(char * model_name)
{
	memset(&(this->ctx), 0, sizeof(rknn_app_context_t));

	init_post_process();

	int ret = init_yolov5_model(model_name, &(this->ctx));
	if (ret != 0) {
		printf("init_yolov5_model fail! ret=%d model_path=%s\n", ret, model_name);
		while (1);
		//goto out;
	}
	this->src_image.width = 640;//img.rows;
	this->src_image.height = 640;//img.cols;
	this->src_image.format = IMAGE_FORMAT_RGB888;
	this->src_image.size = 1228800;//img.rows * img.cols * 3;
	this->src_image.virt_addr = (unsigned char*)malloc((this)->src_image.size);
	//RV1106 rga requires that input and output bufs are memory allocated by dma
	ret = dma_buf_alloc(RV1106_CMA_HEAP_PATH, (this)->src_image.size, &((this)->ctx).img_dma_buf.dma_buf_fd, 
			(void **) & (((this)->ctx).img_dma_buf.dma_buf_virt_addr));
	memcpy(((this)->ctx).img_dma_buf.dma_buf_virt_addr, (this)->src_image.virt_addr, (this)->src_image.size);
	dma_sync_cpu_to_device(((this)->ctx).img_dma_buf.dma_buf_fd);
	free((this)->src_image.virt_addr);
	(this)->src_image.virt_addr = (unsigned char *)((this)->ctx).img_dma_buf.dma_buf_virt_addr;

}

int session_cls :: cap_init()
{
	int retval;
	retval = camera_init(NULL);
	return retval;
}

int session_cls :: cap_read()
{
	int retval = 0;
	camera_read(NULL);
	return retval;
}


int session_cls :: cap_deinit()
{
	int retval;
	retval = camera_deinit(NULL);
	return retval;
}

int session_cls :: preprocess()
{
	int retval;
	start_time = stop_time;
	cv::Mat img;
	cv::cvtColor(g_bgr, img, cv::COLOR_BGR2RGB);
	memcpy(this->src_image.virt_addr, img.data, this->src_image.size);
	gettimeofday(&stop_time, NULL);
	printf("preprocess use %f ms\n", (__get_us(stop_time) - __get_us(start_time)) / 1000);
	return retval;
}
