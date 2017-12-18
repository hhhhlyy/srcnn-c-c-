#ifndef __IMGEDIT_H__
#define __IMGEDIT_H__

#include <stdio.h>  
#include <stdlib.h>  
using namespace std;

typedef struct
{
	//unsigned short    bfType;  
	unsigned long    bfSize;
	unsigned short    bfReserved1;
	unsigned short    bfReserved2;
	unsigned long    bfOffBits;
} ClBitMapFileHeader;

typedef struct
{
	unsigned long  biSize;
	long   biWidth;
	long   biHeight;
	unsigned short   biPlanes;
	unsigned short   biBitCount;
	unsigned long  biCompression;
	unsigned long  biSizeImage;
	long   biXPelsPerMeter;
	long   biYPelsPerMeter;
	unsigned long   biClrUsed;
	unsigned long   biClrImportant;
} ClBitMapInfoHeader;

typedef struct
{
	unsigned char rgbBlue; 
	unsigned char rgbGreen; 
	unsigned char rgbRed;  
	unsigned char rgbReserved;  
} ClRgbQuad;

typedef struct
{
	int width;
	int height;
	int channels;
	unsigned char* imageData;
}ClImage;

ClImage* clLoadImage(char* path)
{
	ClImage* bmpImg;
	FILE* pFile;
	unsigned short fileType;
	ClBitMapFileHeader bmpFileHeader;
	ClBitMapInfoHeader bmpInfoHeader;
	int channels = 1;
	int width = 0;
	int height = 0;
	int step = 0;
	int offset = 0;
	unsigned char pixVal;
	int i, j, k;

	bmpImg = (ClImage*)malloc(sizeof(ClImage));
	fopen_s(&pFile, path, "rb");
	if (!pFile)
	{
		free(bmpImg);
		return NULL;
	}

	fread(&fileType, sizeof(unsigned short), 1, pFile);
	if (fileType == 0x4D42)
	{
		//printf("bmp file! \n");  

		fread(&bmpFileHeader, sizeof(ClBitMapFileHeader), 1, pFile);

		fread(&bmpInfoHeader, sizeof(ClBitMapInfoHeader), 1, pFile);

		//unsigned char quadtmp[68];
		unsigned char * quadtmp = new unsigned char[bmpFileHeader.bfOffBits - 54];
		fread(quadtmp, sizeof(unsigned char), bmpFileHeader.bfOffBits - 54, pFile);

		if (bmpInfoHeader.biBitCount == 8)
		{
			channels = 1;
			width = bmpInfoHeader.biWidth;
			height = bmpInfoHeader.biHeight;
			offset = (channels*width) % 4;
			if (offset != 0)
			{
				offset = 4 - offset;
			}
			//bmpImg->mat = kzCreateMat(height, width, 1, 0);  
			bmpImg->width = width;
			bmpImg->height = height;
			bmpImg->channels = 1;
			bmpImg->imageData = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
			step = channels*width;


			for (i = 0; i<height; i++)
			{
				for (j = 0; j<width; j++)
				{
					fread(&pixVal, sizeof(unsigned char), 1, pFile);
					bmpImg->imageData[(height - 1 - i)*step + j] = pixVal;
				}
				if (offset != 0)
				{
					for (j = 0; j<offset; j++)
					{
						fread(&pixVal, sizeof(unsigned char), 1, pFile);
					}
				}
			}
		}
		else if (bmpInfoHeader.biBitCount == 24)
		{
			channels = 3;
			width = bmpInfoHeader.biWidth;
			height = bmpInfoHeader.biHeight;

			bmpImg->width = width;
			bmpImg->height = height;
			bmpImg->channels = 3;
			bmpImg->imageData = (unsigned char*)malloc(sizeof(unsigned char)*width * 3 * height);
			step = channels*width;

			offset = (channels*width) % 4;
			if (offset != 0)
			{
				offset = 4 - offset;
			}

			for (i = 0; i<height; i++)
			{
				for (j = 0; j<width; j++)
				{
					for (k = 0; k<3; k++)
					{
						fread(&pixVal, sizeof(unsigned char), 1, pFile);
						bmpImg->imageData[(height - 1 - i)*step + j * 3 + k] = pixVal;
					}
					//kzSetMat(bmpImg->mat, height-1-i, j, kzScalar(pixVal[0], pixVal[1], pixVal[2]));  
				}
				if (offset != 0)
				{
					for (j = 0; j<offset; j++)
					{
						fread(&pixVal, sizeof(unsigned char), 1, pFile);
					}
				}
			}
		}
	}
	fclose(pFile);
	return bmpImg;
}

bool clSaveImage(char* path, ClImage* bmpImg)
{
	FILE *pFile;
	unsigned short fileType;
	ClBitMapFileHeader bmpFileHeader;
	ClBitMapInfoHeader bmpInfoHeader;
	int step;
	int offset;
	unsigned char pixVal = '\0';
	int i, j;
	ClRgbQuad* quad;

	int debug = 0;

	fopen_s(&pFile, path, "wb");
	if (!pFile)
	{
		return false;
	}

	fileType = 0x4D42;
	fwrite(&fileType, sizeof(unsigned short), 1, pFile);
	debug += sizeof(unsigned short);

	if (bmpImg->channels == 3)//3channels, 24bit
	{
		step = bmpImg->channels*bmpImg->width;
		offset = step % 4;
		if (offset != 0)
		{
			step += 4 - offset;
		}
		//bmpFileHeader.bfType = 0x4D42;
		bmpFileHeader.bfSize = bmpImg->height*step + 54;
		//bmpFileHeader.bfSize = 196730;
		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;
		bmpFileHeader.bfOffBits = 54;//2+12+40
		int tmpsize = sizeof(ClBitMapFileHeader);//12
		tmpsize = sizeof(ClBitMapInfoHeader);//40
		fwrite(&bmpFileHeader, sizeof(ClBitMapFileHeader), 1, pFile);
		debug += sizeof(ClBitMapFileHeader);

		bmpInfoHeader.biSize = 40;
		bmpInfoHeader.biWidth = bmpImg->width;
		bmpInfoHeader.biHeight = bmpImg->height;
		bmpInfoHeader.biPlanes = 1;
		bmpInfoHeader.biBitCount = 24;
		bmpInfoHeader.biCompression = 0;
		bmpInfoHeader.biSizeImage = bmpImg->height*step;
		bmpInfoHeader.biXPelsPerMeter = 0;
		bmpInfoHeader.biYPelsPerMeter = 0;
		bmpInfoHeader.biClrUsed = 0;
		bmpInfoHeader.biClrImportant = 0;
		fwrite(&bmpInfoHeader, sizeof(ClBitMapInfoHeader), 1, pFile);
		debug += sizeof(ClBitMapInfoHeader);

		for (i = bmpImg->height - 1; i>-1; i--)
		{
			for (j = 0; j<bmpImg->width; j++)
			{
				pixVal = bmpImg->imageData[i*bmpImg->width * 3 + j * 3];
				fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
				pixVal = bmpImg->imageData[i*bmpImg->width * 3 + j * 3 + 1];
				fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
				pixVal = bmpImg->imageData[i*bmpImg->width * 3 + j * 3 + 2];
				fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
				debug += sizeof(unsigned char) * 3;
			}
			if (offset != 0)
			{
				for (j = 0; j<offset; j++)
				{
					pixVal = 0;
					fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
					debug += sizeof(unsigned char);
				}
			}
		}
	}
	else if (bmpImg->channels == 1)//single channel, gray img 
	{
		step = bmpImg->width;
		offset = step % 4;
		if (offset != 4)
		{
			step += 4 - offset;
		}

		bmpFileHeader.bfSize = 54 + 256 * 4 + bmpImg->width;
		//bmpFileHeader.bfSize = 20;
		bmpFileHeader.bfReserved1 = 0;
		bmpFileHeader.bfReserved2 = 0;
		bmpFileHeader.bfOffBits = 54 + 256 * 4;
		fwrite(&bmpFileHeader, sizeof(ClBitMapFileHeader), 1, pFile);

		bmpInfoHeader.biSize = 40;
		bmpInfoHeader.biWidth = bmpImg->width;
		bmpInfoHeader.biHeight = bmpImg->height;
		bmpInfoHeader.biPlanes = 1;
		bmpInfoHeader.biBitCount = 8;
		bmpInfoHeader.biCompression = 0;
		bmpInfoHeader.biSizeImage = bmpImg->height*step;
		bmpInfoHeader.biXPelsPerMeter = 0;
		bmpInfoHeader.biYPelsPerMeter = 0;
		bmpInfoHeader.biClrUsed = 256;
		bmpInfoHeader.biClrImportant = 256;
		fwrite(&bmpInfoHeader, sizeof(ClBitMapInfoHeader), 1, pFile);

		quad = (ClRgbQuad*)malloc(sizeof(ClRgbQuad) * 256);
		for (i = 0; i<256; i++)
		{
			quad[i].rgbBlue = i;
			quad[i].rgbGreen = i;
			quad[i].rgbRed = i;
			quad[i].rgbReserved = 0;
		}
		fwrite(quad, sizeof(ClRgbQuad), 256, pFile);
		free(quad);

		for (i = bmpImg->height - 1; i>-1; i--)
		{
			for (j = 0; j<bmpImg->width; j++)
			{
				pixVal = bmpImg->imageData[i*bmpImg->width + j];
				fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
			}
			if (offset != 0)
			{
				for (j = 0; j<offset; j++)
				{
					pixVal = 0;
					fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
				}
			}
		}
	}
	fclose(pFile);
	return true;
}

void rgb2gray(ClImage* rgbImg, int gray[IMG_ROW][IMG_COL]) {
	int i = 0;
	int j = 0;
	for (i = 0; i < rgbImg->height; i++)
	{
		for (j = 0; j < rgbImg->width; j++)
		{
			//R*0.299 + G*0.587 + B*0.114 
			gray[i][j] = (rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 2])*0.229 +
				(rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 1])*0.587 +
				(rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 0])*0.114;
		}
	}
}
void rgb2ycbcr(ClImage* rgbImg, int img_y[IMG_ROW][IMG_COL])
{
	int i = 0;
	int j = 0;
	for (i = 0; i < rgbImg->height; i++)
	{
		for (j = 0; j < rgbImg->width; j++)
		{
			//Y   = 0.257*R+0.564*G+0.098*B+16
			img_y[i][j] = (rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 2])*0.257 +
				(rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 1])*0.564 +
				(rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 0])*0.098 + 16;
			//img_cb[i][j] = 128 - ((rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 2])*0.148 +
			//	(rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 1])*0.291 +
			//	(rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 0])*0.439);
			//img_cr[i][j] = 128 + ((rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 2])*0.439 +
			//	(rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 1])*0.368 +
			//	(rgbImg->imageData[(rgbImg->height - 1 - i)*rgbImg->width * 3 + j * 3 + 0])*0.071);
		}
	}
}
void save2bmpgray(int single_channel_img[IMG_ROW][IMG_COL], ClImage* bmp_img)
{
	bmp_img->imageData = (unsigned char*)malloc(sizeof(unsigned char) * 256 * 256);
	bmp_img->channels = 1;
	bmp_img->height = 256;
	bmp_img->width = 256;
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			bmp_img->imageData[(256 - 1 - i) * 256 + j] = single_channel_img[i][j];
		}
	}
}

//'replicate padding'  Input array values outside the bounds of the array
//                    are assumed to equal the nearest array border
//                    value.
void padding4(float data[IMG_ROW][IMG_COL], float padding_res[IMG_ROW + 8][IMG_COL + 8])
{
	for (int i = 4; i < IMG_ROW + 4; i++)
	{
		for (int j = 4; j < IMG_COL + 4; j++)
		{
			padding_res[i][j] = data[i - 4][j - 4];
		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int k = 4; k < IMG_COL + 4; k++)
		{
			padding_res[0 + i][k] = padding_res[4][k];
			padding_res[IMG_COL + 7 - i][k] = padding_res[IMG_COL + 3][k];
		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int l = 0; l < IMG_ROW + 8; l++)
		{
			padding_res[l][0 + i] = padding_res[l][4];
			padding_res[l][IMG_ROW + 7 - i] = padding_res[l][IMG_ROW + 3];
		}
	}
}
void padding2(float data[IMG_ROW][IMG_COL][32], float padding_res[IMG_ROW + 4][IMG_COL + 4][32])
{
	for (int f = 0; f < CONV2_FILTER; f++)
	{
		for (int i = 2; i < IMG_ROW + 2; i++)
		{
			for (int j = 2; j < IMG_COL + 2; j++)
			{
				padding_res[i][j][f] = data[i - 2][j - 2][f];
			}
		}
		for (int i = 0; i < 2; i++)
		{
			for (int k = 2; k < IMG_COL + 2; k++)
			{
				padding_res[0 + i][k][f] = padding_res[2][k][f];
				padding_res[IMG_COL + 3 - i][k][f] = padding_res[IMG_COL + 1][k][f];
			}
		}
		for (int i = 0; i < 2; i++)
		{
			for (int l = 0; l < IMG_ROW + 4; l++)
			{
				padding_res[l][0 + i][f] = padding_res[l][2][f];
				padding_res[l][IMG_ROW + 3 - i][f] = padding_res[l][IMG_ROW + 1][f];
			}
		}
	}
}


#endif  