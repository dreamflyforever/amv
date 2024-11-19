#include "core.h"

#if 1
int main(int argc, char **argv)
{
	if (argc != 2) {
		printf("%s <model_path>\n", argv[0]);
		return -1;
	}
	session_str * entity;
	session_init(&entity, argv[1]);
	camera_init(entity);
	while (1) {
		camera_read(entity);
		preprocess(entity);
		inference(entity);
		postprocess(entity);
		updatefps();

/*easy to debug, keep picture*/
#if 1
		char *str = (char *)malloc(20);
		memset(str, 0, 20);
		static int t = 0;
		sprintf(str, "%d_out.jpeg", t);
		write_image(str, &(entity->src_image));
		free(str);
		t++;
		/* because the flash is only 24MB*/
		if (t == 50) {
			os_printf("reach the amount of picutre, exit...\n");
			break;
		}
#endif
	}
	camera_deinit(entity);
	session_deinit(entity);
}
#endif
