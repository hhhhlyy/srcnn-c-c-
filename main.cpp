#include "top.h"
#include "reshape.h"
#include "convolution2D.h"
#include "imgedit.h"
#include "sacle.h"
#include <stdio.h>  
#include <stdlib.h>  

#if DEBUG
#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;
#endif


int main(void) {

	///
	///

	ClImage* img = clLoadImage(".\\data\\SET-5\\butterfly_LR.bmp");
	int y_img[256][256];
	// work on illuminance only
	rgb2ycbcr(img, y_img);
	auto down_tmp = new int[IMG_ROW / 2][IMG_COL / 2];
	auto lr_img = new int[IMG_ROW][IMG_COL];
	downscale_2_linear(y_img, down_tmp);
	upscale_2_linear(down_tmp, lr_img);
	delete[] down_tmp;
	ClImage* lr_img_save = new ClImage;
	save2bmpgray(lr_img, lr_img_save);
	//clSaveImage(".\\data\\result\\butterfly_LR.bmp", lr_img_save);
	delete[] lr_img_save;

	//bn
	auto y_norm_img = new float[IMG_ROW][IMG_COL];
	batch_normal(lr_img, y_norm_img);

	//conv1
	auto y_img_pad4 = new float [IMG_ROW + CONV1_PATCHSIZE - 1][IMG_COL + CONV1_PATCHSIZE - 1];
	padding4(y_norm_img, y_img_pad4);
	auto fmap1 = new float [IMG_ROW][IMG_COL][CONV1_FILTER];
	conv1(y_img_pad4, fmap1);

	//conv2
	auto fmap2 = new float[IMG_ROW][IMG_COL][CONV2_FILTER];
	conv2(fmap1, fmap2);
	delete[] fmap1;

	//conv3
	auto fmap2_pad2 = new float[IMG_ROW + CONV3_PATCHSIZE - 1][IMG_ROW + CONV3_PATCHSIZE - 1][CONV2_FILTER];
	padding2(fmap2, fmap2_pad2);
	delete[] fmap2;
	auto fmap3 = new float[IMG_ROW][IMG_COL];
	conv3(fmap2_pad2, fmap3);

	auto out_map = new int[IMG_ROW][IMG_COL];
	for (int i = 0; i < IMG_ROW; i++)
	{
		for (int j = 0; j < IMG_COL; j++)
		{
			out_map[i][j] = (int)(fmap3[i][j] * 255);
		}
	}
	delete[] fmap3;

	//save result image
	ClImage* output_img = new ClImage;
	save2bmpgray(out_map, output_img);
	clSaveImage(".\\data\\result\\butterfly_srcnn.bmp", output_img);
	delete[] out_map;
	delete[] output_img;
};