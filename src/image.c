#include "cg.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef _WIN32
#define strcasecmp stricmp
#endif

static struct cg_surface_t* cg__pixel_to_surface(stbi_uc* data,int w,int h) {
	struct cg_surface_t* surface = cg_surface_create(w, h);
	if (!surface) {
		return NULL;
	}
	for (int i = 0; i < h; ++i) {
		uint32_t* src = (uint32_t*)data + i * w;
		uint32_t* dest = (uint32_t*)(surface->pixels + i * surface->stride);
		for (int j = 0; j < w; ++j) {
			uint32_t a = (src[j] >> 24) & 0xff;
			uint32_t b = (((src[j] >> 16) & 0xff) * a) / 255;
			uint32_t g = (((src[j] >> 8) & 0xff) * a) / 255;
			uint32_t r = (((src[j] >> 0) & 0xff) * a) / 255;
			dest[j] = (a << 24) | (r << 16) | (g << 8) | b;
		}
	}
	return surface;
}


struct cg_surface_t* cg_surface_load_file(const char* path) {
	int w, h, channels_in_file;

	if (!path) {
		return NULL;
	}

	stbi_uc* data = stbi_load(path,&w,&h,&channels_in_file,4);
	if (!data) {
		return NULL;
	}

	struct cg_surface_t* surface = cg__pixel_to_surface(data, w, h);
	stbi_image_free(data);
	return surface;
}

struct cg_surface_t* cg_surface_load_file_crop(const char* path,int crop_w,int crop_h) {
	int w, h, channels, nw, nh;
	uint8_t* scaled_data = NULL;
	struct cg_surface_t* surface = NULL;

	if (!path || crop_w <= 0 || crop_h <= 0) {
		return NULL;
	}

	uint8_t* data = stbi_load(path, &w, &h, &channels, 4);
	if (!data) {
		return NULL;
	}

	//计算缩放的尺寸
	if (h * crop_w > crop_h * w) {
		//宽度铺满
		nw = crop_w;
		nh = nw * h / w;
	} else {
		nh = crop_h;
		nw = nh * w / h;
	}

	//缩放图片
	scaled_data = (uint8_t*)stbi_image_malloc(nw * nh * channels);
	if (!scaled_data) {
		goto Error;
	}

	if (!stbir_resize_uint8(data, w, h, 0, scaled_data, nw, nh, 0, channels)) {
		goto Error;
	}

	surface = cg_surface_create(crop_w, crop_h);
	if (!surface) {
		goto Error;
	}

	if (nh > crop_h) {
		//宽度铺满，高度裁剪
		int offset_y = (nh - crop_h) / 2;

		for (int i = 0; i < crop_h; ++i) {
			uint32_t* src = (uint32_t*)scaled_data + (i + offset_y) * nw;
			uint32_t* dest = (uint32_t*)(surface->pixels + i * surface->stride);
			for (int j = 0; j < crop_w; ++j) {
				uint32_t a = (src[j] >> 24) & 0xff;
				uint32_t b = (((src[j] >> 16) & 0xff) * a) / 255;
				uint32_t g = (((src[j] >> 8) & 0xff) * a) / 255;
				uint32_t r = (((src[j] >> 0) & 0xff) * a) / 255;
				dest[j] = (a << 24) | (r << 16) | (g << 8) | b;
			}
		}

	} else {
		int offset_x = (nw - crop_w) / 2;

		for (int i = 0; i < crop_h; ++i) {
			uint32_t* src = (uint32_t*)scaled_data + i * nw + offset_x;
			uint32_t* dest = (uint32_t*)(surface->pixels + i * surface->stride);
			for (int j = 0; j < crop_w; ++j) {
				uint32_t a = (src[j] >> 24) & 0xff;
				uint32_t b = (((src[j] >> 16) & 0xff) * a) / 255;
				uint32_t g = (((src[j] >> 8) & 0xff) * a) / 255;
				uint32_t r = (((src[j] >> 0) & 0xff) * a) / 255;
				dest[j] = (a << 24) | (r << 16) | (g << 8) | b;
			}
		}
	}

Error:
	if(data)
		stbi_image_free(data);
	if(scaled_data)
		stbi_image_free(scaled_data);
	return surface;
}


int cg_surface_save_file(struct cg_surface_t* surface, const char* path) {
	int rslt = 0;
	unsigned char* data = surface->pixels;
	int width = surface->width;
	int height = surface->height;
	int stride = surface->stride;
	unsigned char* image = malloc((size_t)(stride * height));
	for (int y = 0; y < height; y++)
	{
		uint32_t* src = (uint32_t*)(data + stride * y);
		uint32_t* dst = (uint32_t*)(image + stride * y);
		for (int x = 0; x < width; x++)
		{
			uint32_t a = src[x] >> 24;
			if (a != 0)
			{
				uint32_t r = (((src[x] >> 16) & 0xff) * 255) / a;
				uint32_t g = (((src[x] >> 8) & 0xff) * 255) / a;
				uint32_t b = (((src[x] >> 0) & 0xff) * 255) / a;
				dst[x] = (a << 24) | (b << 16) | (g << 8) | r;
			}
			else
			{
				dst[x] = 0;
			}
		}
	}

	const char* postfix = strrchr(path, '.');
	if (postfix) {
		if (strcasecmp(postfix, ".png") == 0) {
			rslt = stbi_write_png(path, width, height, 4, image, 0);
		}
		else if (strcasecmp(postfix, ".bmp") == 0) {
			rslt = stbi_write_bmp(path, width, height, 4, image);
		}
		else if (strcasecmp(postfix, ".jpg") == 0) {
			rslt = stbi_write_jpg(path, width, height, 4, image,80);
		}
		else if (strcasecmp(postfix, ".tga") == 0) {
			rslt = stbi_write_tga(path, width, height, 4, image, 80);
		}
	} else {
		rslt = stbi_write_png(path, width, height, 4, image, 0);
	}

	free(image);
	return rslt;
}


