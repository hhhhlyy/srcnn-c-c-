#ifndef __SCALE__
#define __SCALE__

void downscale_2_linear(int data[IMG_ROW][IMG_COL], int scale_res[IMG_ROW/2][IMG_COL/2]) 
{
	for (int i = 0; i < IMG_ROW/2; i++)
	{
		for (int j = 0; j < IMG_COL/2; j++)
		{
			scale_res[i][j] = (data[2 * i][2 * j] + data[2 * i + 1][2 * j]
				+ data[2 * i + 1][2 * j + 1] + data[2 * i + 1][2 * j + 1]) / 4;
		}
	}
}
void upscale_2_linear(int data[IMG_ROW / 2][IMG_COL / 2], int scale_res[IMG_ROW][IMG_COL])
{
	for (int i = 0; i < IMG_ROW / 2; i++)
	{
		for (int j = 0; j < IMG_COL / 2; j++)
		{
			scale_res[2 * i][2 * j] = data[i][j];
			scale_res[2 * i + 1][2 * j] = data[i][j];
			scale_res[2 * i][2 * j + 1] = data[i][j];
			scale_res[2 * i + 1][2 * j + 1] = data[i][j];
		}
	}
}
#endif // !__SCALE__

