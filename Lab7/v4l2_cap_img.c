#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#include "qdbmp.h"

#define TRUE 1
#define FALSE 0

#define WIDTH 1280
#define HEIGHT 720

struct v4l2_capability cap;
struct v4l2_cropcap cropcap;
struct v4l2_fmtdesc fmtdesc;
struct v4l2_format fmt;
struct v4l2_requestbuffers reqbufs;
struct v4l2_buffer buf;

typedef struct buf_info
{
	void* start;
	size_t length;
} buf_info;
buf_info* buf_infos;

void init_v4l2(int fd_video)
{
	int i;

	printf("\n[capability]\n");
	if(ioctl(fd_video, VIDIOC_QUERYCAP, &cap) == -1)
	{
		perror("ERROR");
	}

	printf("driver: %s\n", cap.driver);
	printf("card: %s\n", cap.card);
	printf("bus_info: %s\n", cap.bus_info);
	printf("version: %u\n", cap.version);
	printf("capability flags: 0x%x\n", cap.capabilities);

	printf("\n[supported format]\n");
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while(ioctl(fd_video, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
	{
		printf("description: %s\n", fmtdesc.description);
		printf("fourcc: %c%c%c%c\n", fmtdesc.pixelformat & 0xff, fmtdesc.pixelformat >> 8 & 0xff, fmtdesc.pixelformat >> 16 & 0xff, fmtdesc.pixelformat >> 24 & 0xff);
		fmtdesc.index++;
		printf("\n");
	}

	printf("\n[cropcap]\n");
	if(ioctl(fd_video, VIDIOC_CROPCAP, &cropcap) == -1)
	{
		perror("VIDIOC_CROPCAP");
	}
	printf("bounds: (%d, %d, %u, %u)\n", cropcap.bounds.left, cropcap.bounds.top, cropcap.bounds.width, cropcap.bounds.height);
	printf("default cropping rectangle: (%d, %d, %u, %u)\n", cropcap.defrect.left, cropcap.defrect.top, cropcap.defrect.width, cropcap.defrect.height);
	printf("pixel aspect: %u/%u\n", cropcap.pixelaspect.numerator, cropcap.pixelaspect.denominator);

	printf("\n[format]\n");
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.width = WIDTH;
	fmt.fmt.pix.height = HEIGHT;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	if(ioctl(fd_video, VIDIOC_S_FMT, &fmt) == -1)
	{
	}

	if(ioctl(fd_video, VIDIOC_G_FMT, &fmt) == -1)
	{
	}

	printf("type: %u\n", fmt.type);
	printf("pixelformat: %c%c%c%c\n", fmt.fmt.pix.pixelformat & 0xFF, (fmt.fmt.pix.pixelformat >> 8) & 0xFF, (fmt.fmt.pix.pixelformat >> 16) & 0xFF, (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
	printf("width: %u\n", fmt.fmt.pix.width);
	printf("height: %u\n", fmt.fmt.pix.height);
	printf("field: %u\n", fmt.fmt.pix.field); // none
	printf("image size: %u\n", fmt.fmt.pix.sizeimage);
	printf("color space: %u\n", fmt.fmt.pix.colorspace);

	printf("\n[requested buffer]\n");
	reqbufs.count = 16;
	reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbufs.memory = V4L2_MEMORY_MMAP;
	if(ioctl(fd_video, VIDIOC_REQBUFS, &reqbufs) == -1)
	{
	}
	printf("buffer count: %u\n", reqbufs.count);

	buf.type = reqbufs.type;
	buf.memory = reqbufs.memory;

	buf_infos = calloc(reqbufs.count, sizeof(buf_info));

	for(i = 0; i < reqbufs.count; i++)
	{
		buf.index = i;
		if(ioctl(fd_video, VIDIOC_QUERYBUF, &buf) == -1)
		{
		}

		buf_infos[i].length = buf.length;
		buf_infos[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd_video, buf.m.offset);

		if(ioctl(fd_video, VIDIOC_QBUF, &buf) == -1)
		{
		}
	}

	if(ioctl(fd_video, VIDIOC_STREAMON, &reqbufs.type) == -1)
	{
	}

	fflush(stdout);
}

int main(int argc, char* argv[])
{
	int fd_video = open("/dev/video0", O_RDWR);
	if(fd_video == -1)
	{
		perror("open /dev/video0");
		exit(EXIT_FAILURE);
	}

	init_v4l2(fd_video);

	int i;
	uint8_t y1, y2, u, v;
	uint32_t r, g, b;
	uint8_t* p_img;
	uint32_t offset;

	if(ioctl(fd_video, VIDIOC_DQBUF, &buf) == -1)
	{
	}

	p_img = buf_infos[buf.index].start;
	BMP* bmp = BMP_Create(WIDTH, HEIGHT, 24);
	offset = 0;
	for(i = 0; i < fmt.fmt.pix.sizeimage; i += 4)
	{
		y1 = *(p_img + i + 0);
		u  = *(p_img + i + 1);
		y2 = *(p_img + i + 2);
		v  = *(p_img + i + 3);

		r = y1 + 1.042*(v - 128);				/*Q1_YUYV2RGB*/
		g = y1 - 0.344*(u-128) - 0.714*(v-128); /*Q1_YUYV2RGB*/
		b = y1 + 1.772*(u-128);					/*Q1_YUYV2RGB*/

		r = (r > 255)? 255: ((r < 0)? 0: r);
		b = (b > 255)? 255: ((b < 0)? 0: b);
		g = (g > 255)? 255: ((g < 0)? 0: g);

		BMP_SetPixelRGB(bmp, offset % WIDTH, offset / WIDTH, r, g, b);
		offset++;

		r = y2 + 1.042*(v-128);					/*Q1_YUYV2RGB*/
		g = y2 - 0.344*(u-128) - 0.714*(v-128); /*Q1_YUYV2RGB*/
		b = y2 + 1.772*(u-128);					/*Q1_YUYV2RGB*/

		r = (r > 255)? 255: ((r < 0)? 0: r);
		b = (b > 255)? 255: ((b < 0)? 0: b);
		g = (g > 255)? 255: ((g < 0)? 0: g);

		BMP_SetPixelRGB(bmp, offset % WIDTH, offset / WIDTH, r, g, b);
		offset++;
	}
	BMP_WriteFile(bmp, "captured_image.bmp");
	BMP_Free(bmp);

	if(ioctl(fd_video, VIDIOC_QBUF, &buf) == -1)
	{
		perror("VIDIOC_QUERYBUF ERROR");
	}

	for(i = 0; i < reqbufs.count; i++)
	{
		munmap(buf_infos[i].start, buf_infos[i].length);
	}

	if(ioctl(fd_video, VIDIOC_STREAMOFF, &reqbufs.type) == -1)
	{
	}

	free(buf_infos);

	close(fd_video);

	return 0;
}


