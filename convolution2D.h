#ifndef __CONVOLUTION2D_H__
#define __CONVOLUTION2D_H__
#include "activation.h"
#include <assert.h>

#define DEBUG 0
#if DEBUG
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;
#endif

void conv1(float img[IMG_ROW+ CONV1_PATCHSIZE-1][IMG_COL+ CONV1_PATCHSIZE-1], float fmap[IMG_ROW][IMG_COL][CONV1_FILTER])
{
	auto weights_conv1 = new float[CONV1_PATCHSIZE][CONV1_PATCHSIZE][CONV1_FILTER];

	//init weight
	for (int i = 0; i < CONV1_PATCHSIZE; i++)
	{
		for (int j = 0; j < CONV1_PATCHSIZE; j++)
		{
			for (int f = 0; f < CONV1_FILTER; f++)
			{
				weights_conv1[i][j][f] = WEIGHT_CONV1[CONV1_PATCHSIZE*j + i][f];
			}
		}
	}

	float tmp = 0;
	for (int t = 0; t < CONV1_FILTER; t++)//64
	{
		for (int i = 0; i < IMG_ROW; i++)//256
		{
			for (int j = 0; j < IMG_COL; j++)//256
			{
				for (int k = 0; k < CONV1_PATCHSIZE; k++)//9
				{
					for (int l = 0; l < CONV1_PATCHSIZE; l++)//9
					{
						tmp += img[i + k][j + l] * weights_conv1[k][l][t];
					}
				}
				fmap[i][j][t] = (tmp + BIASES_CONV1[t]) > 0 ? (tmp + BIASES_CONV1[t]) : 0;//ReLU & biases
				tmp = 0.0;
			}
		}
	}
	delete[] weights_conv1;
}
void conv2(float img[IMG_ROW + CONV2_PATCHSIZE -1][IMG_COL + CONV2_PATCHSIZE - 1][CONV1_FILTER], float fmap[IMG_ROW][IMG_COL][CONV2_FILTER])
{
	float subfilter = 0.0;
	float tmpsum = 0.0;
	for (int f = 0; f < CONV2_FILTER; f++)//32
	{
		subfilter = WEIGHT_CONV2[CONV2_CHANNEL][CONV2_FILTER];
		for (int i = 0; i < IMG_ROW; i++)//256
		{
			for (int j = 0; j < IMG_COL; j++)//256
			{
				//fmap[i][j][f] = (img[i][j] + biases_conv2[f] > 0) ? img[i][j] + biases_conv2[f] : 0;
				for (int c = 0; c < CONV2_CHANNEL; c++)//64
				{
					tmpsum += img[i][j][c] * WEIGHT_CONV2[c][f];
				}
				fmap[i][j][f] = (tmpsum + BIASES_CONV2[f] > 0) ? tmpsum + BIASES_CONV2[f] : 0;
				tmpsum = 0.0;
			}
		}
	}
}
void conv3(float img[IMG_ROW + CONV3_PATCHSIZE - 1][IMG_COL + CONV3_PATCHSIZE - 1][CONV2_FILTER], float fmap[IMG_ROW][IMG_COL])
{
	auto weights_conv3 = new float[CONV3_CHANNEL][CONV3_PATCHSIZE][CONV3_PATCHSIZE];
	//init weight
	for (int c = 0; c < CONV3_CHANNEL; c++)
	{
		for (int i = 0; i < CONV3_PATCHSIZE; i++)
		{
			for (int j = 0; j < CONV3_PATCHSIZE; j++)
			{
				weights_conv3[c][i][j] = WEIGHT_CONV3[c][CONV3_PATCHSIZE*j + i];
			}
		}
	}
//#if DEBUG
//	for (int i = 0; i < CONV3_PATCHSIZE; i++)
//	{
//		for (int j = 0; j < CONV3_PATCHSIZE; j++)
//		{
//			cout << weights_conv3[13][i][j] << ",";
//		}
//		cout << endl;
//	}
//#endif
	float tmpsum = 0.0;
	for (int i = 0; i < IMG_ROW; i++)
	{
		for (int j = 0; j < IMG_COL; j++)
		{
			for (int c = 0; c < CONV3_CHANNEL; c++)
			{
				for (int k = 0; k < CONV3_PATCHSIZE; k++)
				{
					for (int l = 0; l < CONV3_PATCHSIZE; l++)
					{
						tmpsum += img[i + k][j + l][c] * weights_conv3[c][k][l];
					}
				}
			}
			fmap[i][j] = (tmpsum + BIASES_CONV3 > 0.0) ? tmpsum + BIASES_CONV3 : 0.0;
			tmpsum = 0.0;
		}
	}
}
#endif
