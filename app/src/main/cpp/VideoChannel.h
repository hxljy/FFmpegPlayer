// TODO 专门单独处理视频，解码来播放

#ifndef KEVINPLAYER_VIDEOCHANNEL_H
#define KEVINPLAYER_VIDEOCHANNEL_H


#include "BaseChannel.h"
#include "AudioChannel.h"

extern "C" {
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

typedef void (*RenderCallback) (uint8_t * , int , int, int);

class VideoChannel : public BaseChannel
{
public:
    VideoChannel(int stream_index, AVCodecContext *pContext, AVRational rational, int fpsValue);

    ~VideoChannel();

    void start();

    void stop();

    void video_decode();

    void video_player();

    void setRenderCallback(RenderCallback renderCallback);

    void setAudioChannel(AudioChannel *audioChannel);

private:
    pthread_t pid_video_decode;
    pthread_t pid_video_player;

    // native-lib.cpp prepareNative函数执行的时候，会把"具体函数"传递到此处
    RenderCallback  renderCallback;

    int fpsValue; // fps 是视频独有的东西
    AudioChannel *  audioChannel = 0;
};


#endif //KEVINPLAYER_VIDEOCHANNEL_H
