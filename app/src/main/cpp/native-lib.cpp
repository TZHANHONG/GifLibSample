#include <jni.h>
#include <string>
#include "giflib/gif_lib.h"
#include <android/bitmap.h>
#include "gif.h"

//双引号是从本地项目找，找不到再从系统找，<>是直接从系统找

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_gifndk_GifHandler_openFile(
        JNIEnv *env,
        jobject /* this */, jstring path) {
    const char *path1 = env->GetStringUTFChars(path, 0);
    int err;
    GifFileType *gif = DGifOpenFileName(path1, &err);
    err = DGifSlurp(gif);
    env->ReleaseStringUTFChars(path, path1);
    return reinterpret_cast<jlong>(gif);
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_gifndk_GifHandler_getWidthN(JNIEnv *env, jobject thiz, jlong gif_info) {
    return ((GifFileType *) gif_info)->SWidth;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_example_gifndk_GifHandler_getHeightN(JNIEnv *env, jobject thiz, jlong gif_info) {
    return ((GifFileType *) gif_info)->SHeight;

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_gifndk_GifHandler_getLengthN(JNIEnv *env, jobject thiz, jlong gif_info) {
    return ((GifFileType *) gif_info)->ImageCount;
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_gifndk_GifHandler_renderFrameN(JNIEnv *env, jobject thiz, jlong gif_info,
                                                jobject bitmap, jint index) {
    GifFileType *gifFileType = (GifFileType *) gif_info;
    //bitmap转成颜色矩阵
    void *pixels;
    AndroidBitmapInfo info;
    int err;
    if ((err = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        return -1;
    }
    //也可以不判断，这里的格式需要有透明通道
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        return -1;
    }
    //&pixels为指针的指针，即二维数组
    if ((err = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        return -1;
    }
    //渲染
    long delay_time = drawFrame(gifFileType, &info, (int *) pixels, index);
    AndroidBitmap_unlockPixels(env, bitmap);
    return delay_time;
}

#define delay(ext) (10 *((ext) -> Bytes[2] << 8 | (ext) ->Bytes[1]))


int drawFrame(GifFileType *gif, AndroidBitmapInfo *info, int *pixels, int frame_no) {
    GifColorType *color;
    //每一帧图片
    SavedImage *frame;
    //扩展快，定义一些行为
    ExtensionBlock *ext = 0;
    //描述文件
    GifImageDesc *frameInfo;
    //颜色表
    ColorMapObject *colorMap;
    int *line;
    int  x, y, j, loc;
    int *px;
    //获取这一帧
    frame = &(gif->SavedImages[frame_no]);
    //这一帧相关的描述文件，虽然描述文件已经在GifFileType里面声明了，但是没有赋值
    //所以需要用这种方式
    frameInfo = &(frame->ImageDesc);

    //这一帧的颜色列表
    if (frameInfo->ColorMap) {
        colorMap = frameInfo->ColorMap;
    } else {
        //没有的话就获取全局的颜色列表
        colorMap = gif->SColorMap;
    }

    //遍历这一帧的扩展块，找到具有GRAPHICS_EXT_FUNC_CODE标志位的，这个扩展快存放着对该帧图片的
    //处置方法，是不处理还是其他
    for (j = 0; j < frame->ExtensionBlockCount; ++j) {
        if (frame->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {
            ext = &(frame->ExtensionBlocks[j]);
            break;
        }
    }
    //这时候px是二维指针
    px = pixels;
    //frameInfo->Top：从哪个y坐标开始（距离顶部的top），乘以每一行的字节，就是我们需要开始遍历的字节
    px = (int *) ((char *) px + info->stride * frameInfo->Top);
    //遍历y
    for (y = frameInfo->Top; y < frameInfo->Top + frameInfo->Height; ++y) {
        //行
        line = px;
        //遍历x
        for (x = frameInfo->Left; x < frameInfo->Left + frameInfo->Width; ++x) {
            //当前的这一点
            loc = (y - frameInfo->Top) * frameInfo->Width + (x - frameInfo->Left);
            //判断处置方法，拿到当前帧loc位置的字节，看是否等于扩展块中索引为3的字节，并且数值为1
            if (frame->RasterBits[loc] == ext->Bytes[3] && ext->Bytes[0]) {
                continue;
            }
            color =  &colorMap->Colors[frame->RasterBits[loc]];
            line[x] = ((255 & 0xff)<< 24) | ((color->Blue & 0xff)<< 16) | ((color->Green)<< 8) | (color->Red);
        }
        px = (int *) ((char *) px + info->stride);
    }
    return delay(ext);
}