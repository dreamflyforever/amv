/*
	test unittest for camera API
*/

#include "core.h"

typedef int (*_cb) (void * p_arg);

typedef struct cap_api_str {
	char name[10];
	_cb;
} cap_api_str;

cap_api_str cap[3] = { ("camera_init", camera_init),
			("camera_read", camera_read),
			("camera_deinit", camera_deinit),
}

int check_camera(char * camera_api, int expect)
{
	int retvalue, out;
	printf("------------------------------------------------------------------------------\n");
	printf("TEST: ");
	printf("\tInput\t camera API %s\: ", camera_api);
	printf("\tExpect\t[e =%2d]: ", expect);
	for (int i = 0; i < sizeof(cap)/sizeof(cap[0]); i++) {
		if (0 == strcmp(camera_api, cap[i].name)) {
			out = cap[i]._cb(NULL);
			break;
		}
	}
	printf("\tOutput\t[o =%2d]: ", out);
	if (expect == out) {
		printf("PASS\n");
		retvalue = 1;
	} else {
		printf("FAIL\n");
		retvalue = 0;
	}
	printf("\n");
	return retvalue;
}

#define CHECK_CAMERA(camera_api, expect, caculate, pass) \
	do { \
		caculate++; \
		pass += check_camera(camera_api, expect); \
	} while (0);


int main()
{
	int calc = 0, pass = 0;
	CHECK_CAMERA("camera_init", cal, pass);
	CHECK_CAMERA("camera_read", cal, pass);
	CHECK_CAMERA("camera_deinit", cal, pass);
	return (calc != pass)? 1:-1;
}
