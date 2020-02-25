package com.example.gifndk;

import android.graphics.Bitmap;

public class GifHandler {
    static {
        System.loadLibrary("native-lib");
    }

    private volatile long gifInfo;

    public GifHandler(String path) {
        gifInfo = openFile(path);
    }

    public synchronized int getWidth() {
        return getWidthN(gifInfo);
    }

    public synchronized int getHeight() {
        return getHeightN(gifInfo);
    }

    public synchronized int getLength() {
        return getLengthN(gifInfo);
    }

    /**
     * @param bitmap
     * @param index  第几帧
     * @return
     */
    public long renderFrame(Bitmap bitmap, int index) {
        return renderFrameN(gifInfo, bitmap, index);
    }

    private native int getWidthN(long gifInfo);

    private native int getHeightN(long gifInfo);

    private native int getLengthN(long gifInfo);

    private native long renderFrameN(long gifInfo, Bitmap bitmap, int index);

    private native long openFile(String path);
}
