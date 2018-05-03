#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/fb.h>

#include "qdbmp.h"

#define WIDTH 1280
#define HEIGHT 720

#define TRUE 1
#define FALSE 0

struct v4l2_capability cap;
struct v4l2_cropcap cropcap;
struct v4l2_fmtdesc fmtdesc;
struct v4l2_format fmt;
struct v4l2_requestbuffers reqbufs;
struct v4l2_buffer buf;

struct fb_fix_screeninfo fb_fsi;
struct fb_var_screeninfo fb_vsi;

typedef struct buf_info
{
	void* start;
	size_t length;
} buf_info;
buf_info* buf_infos;

uint8_t Bpp;
uint32_t line_length;
uint32_t screen_size;
uint32_t screen_offset;
uint8_t* p_fb;

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
	reqbufs.count = 4;
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

void get_screen_info(int fd_fb)
{
	printf("\n");

	if(ioctl(fd_fb, FBIOGET_FSCREENINFO, &fb_fsi) == -1)
	{
		perror("FBIOGET_FSCREENINFO");
	}
	printf("\n[fixed screen info]\n");
	printf("type: %u\n", fb_fsi.type); // packed pixels
	printf("visual: %u\n", fb_fsi.visual); // true color
	//printf("capablilities: %u\n", fb_fsi.capabilities);
	printf("line length: %u\n", fb_fsi.line_length);

	if(ioctl(fd_fb, FBIOGET_VSCREENINFO, &fb_vsi) == -1)
	{
		perror("FBIOGET_VSCREENINFO");
	}

	printf("\n[variable screen info]\n");
	//if(ioctl(fd_fb, FBIOPUT_VSCREENINFO, &fb_vsi) == -1)
	//{
	//	perror("FBIOPUT_VSCREENINFO");
	//}

	if(ioctl(fd_fb, FBIOGET_VSCREENINFO, &fb_vsi) == -1)
	{
		perror("FBIOGET_VSCREENINFO");
	}

	printf("visible resolution: %u x %u\n", fb_vsi.xres, fb_vsi.yres);
	printf("virtual resolution: %u x %u\n", fb_vsi.xres_virtual, fb_vsi.yres_virtual);
	printf("\n");
	printf("x offset: %u\n", fb_vsi.xoffset);
	printf("y offset: %u\n", fb_vsi.yoffset);
	printf("\n");
	printf("bits per pixel: %u\n", fb_vsi.bits_per_pixel);
	printf("\n");
	printf("gray scale: %u\n", fb_vsi.grayscale);
	printf("\n");
	printf("red offset: %u\n", fb_vsi.red.offset);
	printf("red length: %u\n", fb_vsi.red.length);
	printf("red msb_right: %u\n", fb_vsi.red.msb_right);
	printf("\n");
	printf("green offset: %u\n", fb_vsi.green.offset);
	printf("green length: %u\n", fb_vsi.green.length);
	printf("green msb_right: %u\n", fb_vsi.green.msb_right);
	printf("\n");
	printf("blue offset: %u\n", fb_vsi.blue.offset);
	printf("blue length: %u\n", fb_vsi.blue.length);
	printf("blue msb_right: %u\n", fb_vsi.blue.msb_right);
	printf("\n");
	printf("transparency offset: %u\n", fb_vsi.transp.offset);
	printf("transparency length: %u\n", fb_vsi.transp.length);
	printf("transparency msb_right: %u\n", fb_vsi.transp.msb_right);
	printf("\n");
	printf("vmode: %u\n", fb_vsi.vmode); // non interlaced
	printf("\n");

	fflush(stdout);
}

void fill_screen(uint8_t* p_fb, uint32_t screen_size, uint8_t r, uint8_t g, uint8_t b)
{
	int i;
	for(i = 0; i < screen_size; i += 4)
	{
		*((uint32_t*)(p_fb + i)) = (((uint32_t)r) << 16) + (((uint32_t)g) << 8) + (((uint32_t)b))/*Q2_fb_var_screeninfo*/;//value of a pixel
	}
}

void get_pic(int fd_video, int fd_fb)
{
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

    if(ioctl(fd_video, VIDIOC_QBUF, &buf) == -1)
	{
		perror("VIDIOC_QBUF ERROR");
	}

    // printf("qbuf !!\n");
    // fflush(stdout);
    // for(i = 0; i < reqbufs.count; i++)
	// {
	// 	munmap(buf_infos[i].start, buf_infos[i].length);
	// }


    BMP_CHECK_ERROR(stdout, -1);

	// uint8_t Bpp = fb_vsi.bits_per_pixel / 8;
	// uint32_t line_length = fb_vsi.xres_virtual * Bpp;
	// uint32_t screen_size = line_length * fb_vsi.yres_virtual;
	// uint32_t screen_offset = fb_vsi.xoffset * fb_vsi.yoffset * Bpp;
    // uint8_t* p_fb = mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);

	//uint8_t* p_org_fb = malloc(screen_size);
	//memcpy(p_org_fb, p_fb, screen_size);
	//fill_screen(p_fb, screen_size, 255, 255, 255);

	p_fb += screen_offset;
	uint32_t y_padding = 0;
	p_fb += y_padding;
	uint32_t x_padding = 0;

	int j;
	uint8_t r8, g8, b8;
	//uint32_t offset;
	for(i = 0; i < HEIGHT; i++)
	{
		for(j = 0; j < WIDTH; j++)
		{
			BMP_GetPixelRGB(bmp, j, i, &r8, &g8, &b8);
			offset = i * line_length + x_padding + j * Bpp;
			*((uint32_t*)(p_fb + offset)) = (r8 << 16) + (g8 << 8) + b8/*Q2_fb_var_screeninfo*/;//value of a pixel
		}
	}

	p_fb -= y_padding;
	p_fb -= screen_offset;

	//memcpy(p_fb, p_org_fb, screen_size);

	//free(p_org_fb);
	BMP_Free(bmp);
	//munmap(p_fb, screen_size);

	return;
}

int main()
{
    int fd_video = open("/dev/video0", O_RDWR);
	if(fd_video == -1)
	{
		perror("open /dev/video0");
		exit(EXIT_FAILURE);
	}

	init_v4l2(fd_video);

    int fd_fb = open("/dev/fb0", O_RDWR);
	if(fd_fb == -1)
	{
		perror("open /dev/fb0");
		exit(EXIT_FAILURE);
	}

	get_screen_info(fd_fb);

    Bpp = fb_vsi.bits_per_pixel / 8;
	line_length = fb_vsi.xres_virtual * Bpp;
	screen_size = line_length * fb_vsi.yres_virtual;
	screen_offset = fb_vsi.xoffset * fb_vsi.yoffset * Bpp;
    p_fb = mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
    while(1)
    {
        get_pic(fd_video, fd_fb);
    }

    if(ioctl(fd_video, VIDIOC_STREAMOFF, &reqbufs.type) == -1)
	{
	}

    free(buf_infos);
	close(fd_video);

    if(ioctl(fd_fb, FBIOPUT_VSCREENINFO, &fb_vsi) == -1)
	{
		perror("FBIOPUT_VSCREENINFO");
	}

	close(fd_fb);
    return 0;
}
