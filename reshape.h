#pragma once

#ifndef __RESHAPE_H__
#define __RESHAPE_H__

#if DEBUG
#include <iostream>
using namespace std;
#endif

void batch_normal(int data[IMG_ROW][IMG_COL], float norm_res[IMG_ROW][IMG_COL])
{
	for (int i = 0; i < IMG_ROW; i++)
	{
		for (int j = 0; j < IMG_COL; j++)
		{
			norm_res[i][j] = data[i][j] / 255.0;
		}
	}
}

#endif
