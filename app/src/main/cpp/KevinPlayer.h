//
// Created by Administrator on 2019/12/31.
//
#include <cstring>
#include "AudioChannel.h"
#include "VideoChannel.h"
#include "JNICallback.h"
#include "macro.h"

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavutil/time.h>
}

#ifndef KEVINPLAYER_KEVINPLAYER_H
#define KEVINPLAYER_KEVINPLAYER_H


class KevinPlayer {
public:
    KevinPlayer();

    // KevinPlayer(const char *data_source);

    KevinPlayer(const char *data_source, JNICallback *pCallback);

    ~KevinPlayer(); // 这个类没有子类，所以没有添加虚函数

    void prepare();

    void prepare_();

    void start();

    void start_();

    void setRenderCallback(RenderCallback renderCallback);

private:
    char *data_source = 0;

    pthread_t pid_prepare ;

    AVFormatContext *formatContext = 0;

    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    JNICallback *pCallback;
    pthread_t pid_start;
    bool isPlaying;

    // native-lib.cpp prepareNative函数执行的时候，会把"具体函数"传递到此处
    RenderCallback  renderCallback;
};


#endif //KEVINPLAYER_KEVINPLAYER_H
