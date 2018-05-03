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
#include <linux/fb.h>

#include "qdbmp.h"

#define TRUE 1
#define FALSE 0

struct fb_fix_screeninfo fb_fsi;
struct fb_var_screeninfo fb_vsi;

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

int main(int argc, char* argv[])
{
	int fd_fb = open("/dev/fb0", O_RDWR);
	if(fd_fb == -1)
	{
		perror("open /dev/fb0");
		exit(EXIT_FAILURE);
	}

	get_screen_info(fd_fb);

	BMP* bmp = BMP_ReadFile("captured_image.bmp");
	BMP_CHECK_ERROR(stdout, -1);

	uint32_t WIDTH = BMP_GetWidth(bmp);
	uint32_t HEIGHT = BMP_GetHeight(bmp);

	uint8_t Bpp = fb_vsi.bits_per_pixel / 8;
	uint32_t line_length = fb_vsi.xres_virtual * Bpp;
	uint32_t screen_size = line_length * fb_vsi.yres_virtual;
	uint32_t screen_offset = fb_vsi.xoffset * fb_vsi.yoffset * Bpp;
	uint8_t* p_fb = mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);

	uint8_t* p_org_fb = malloc(screen_size);
	memcpy(p_org_fb, p_fb, screen_size);
	fill_screen(p_fb, screen_size, 255, 255, 255);

	p_fb += screen_offset;
	uint32_t y_padding = 0;
	p_fb += y_padding;
	uint32_t x_padding = 0;

	int i, j;
	uint8_t r, g, b;
	uint32_t offset;
	for(i = 0; i < HEIGHT; i++)
	{
		for(j = 0; j < WIDTH; j++)
		{
			BMP_GetPixelRGB(bmp, j, i, &r, &g, &b);
			offset = i * line_length + x_padding + j * Bpp;
			*((uint32_t*)(p_fb + offset)) = (((uint32_t)r) << 16) + (((uint32_t)g) << 8) + (((uint32_t)b))/*Q2_fb_var_screeninfo*/;//value of a pixel
		}
	}
	sleep(4);

	p_fb -= y_padding;
	p_fb -= screen_offset;
	memcpy(p_fb, p_org_fb, screen_size);

	free(p_org_fb);

	BMP_Free(bmp);

	munmap(p_fb, screen_size);

	if(ioctl(fd_fb, FBIOPUT_VSCREENINFO, &fb_vsi) == -1)
	{
		perror("FBIOPUT_VSCREENINFO");
	}

	close(fd_fb);

	return 0;
}
