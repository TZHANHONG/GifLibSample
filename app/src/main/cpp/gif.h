//
// Created by Administrator on 2020/2/9.
//
#include "giflib/gif_lib.h"
#include <android/bitmap.h>

#ifndef GIFNDK_GIF_H
#define GIFNDK_GIF_H

#endif //GIFNDK_GIF_H
extern "C"
int drawFrame(GifFileType *gif, AndroidBitmapInfo *info, int *pixels, int frame_no);
