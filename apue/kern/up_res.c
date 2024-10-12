#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>


//Function to upscale an image using SSE

void upscale_image_sse(float *src, float *dst, int src_width, int src_height, int scale_factor) {
	int dst_width = src_width * scale_factor;
	int dst_height = src_height * scale_factor;

	for(int y = 0; y < dst_height; y++) {
		for(int x = 0; x < dst_width; x += 4) {
			//Calculate source pixel coordinates
			float src_x = (float)x / scale_factor;
			float src_y = (float)y / scale_factor;

			int ix = (int)src_x;
			int iy = (int)src_y;

			//Load 4 pixels from the source image using SSE
			__m128 p1 = _mm_loadu_ps(&src[iy * src_width + ix]);
			__m128 p2 = _mm_loadu_ps(&src[iy * src_width + ix + 1]);

			//Perform interpolation
			__m128 weight_x = _mm_set1_ps(src_x - ix);
			__m128 weight_y = _mm_set1_ps(src_y - iy);

			__m128 interp_x = _mm_add_ps(_mm_mul_ps(p1, _mm_sub_ps(_mm_set1_ps(1.0f), weight_x)), _mm_mul_ps(p2, weight_x));
			__m128 interp_y = _mm_add_ps(_mm_mul_ps(p1, _mm_sub_ps(_mm_set1_ps(1.0f), weight_y)), _mm_mul_ps(p2, weight_y));

			//Store result in destination image
			//
			_mm_storeu_ps(&dst[y * dst_width + x], interp_x);
			_mm_storeu_ps(&dst[y * dst_width + x], interp_y);
		}
	}

}


int main() {
	int src_width, src_height, channels;
	//Load image using stb_image
	unsigned char *img_data = stbi_load("img.png", &src_width, &src_height, &channels, 1);
	if(!img_data) {
		fprintf(stderr, "Failed to load img\n");
		return -1;
	}

	//Convert img data to floats
	float *src_float = (float *)aligned_alloc(32, src_width * src_height * sizeof(float));

	int scale_factor = 2;
	int dst_width = src_width * scale_factor;
	int dst_height = src_height * scale_factor;

	//Alloc mem for src and dst imgs
	//float *src = (float *)aligned_alloc(32, src_width * src_height * sizeof(float));
	float *dst_float = (float *)aligned_alloc(32, dst_width * dst_height * sizeof(float));

	//Initialize the source image with some values
	for(int i = 0; i < src_width * src_height; i++) {
		src_float[i] = (float)img_data[i];
	}

	//Upscale
	upscale_image_sse(src_float, dst_float, src_width, src_height, scale_factor);

	//Convert the float image data back to unsigned char
	unsigned char *dst_data = (unsigned char *)malloc(dst_width * dst_height);
	for(int i = 0; i < dst_width * dst_height; i++) {
		dst_data[i] = (unsigned char)dst_float[i];
	}

	//Save result as new image
	stbi_write_png("upscaled_image.png", dst_width, dst_height, 1, dst_data, dst_width);

	//Free memory
	stbi_image_free(img_data);
	free(src_float);
	free(dst_float);
	free(dst_data);

	return 0;
}
