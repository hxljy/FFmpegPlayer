//
// Created by Administrator on 2019/12/31.
//
// TODO 专门定义宏


#ifndef KEVINPLAYER_MACRO_H
#define KEVINPLAYER_MACRO_H

#define THREAD_MAIN 1   // 此宏代表 主线程的意思
#define THREAD_CHILD 2  // 此宏代表 子线程的意思

//打不开媒体数据源
#define FFMPEG_CAN_NOT_OPEN_URL -1

//找不到媒体流信息
#define FFMPEG_CAN_NOT_FIND_STREAMS -2

//找不到解码器
#define FFMPEG_FIND_DECODER_FAIL -3

//无法根据解码器创建上下文
#define FFMPEG_ALLOC_CODEC_CONTEXT_FAIL -4

//根据流信息 配置上下文参数失败
#define FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL -5

//打开解码器失败
#define FFMPEG_OPEN_DECODER_FAIL -6

//没有音视频
#define FFMPEG_NOMEDIA -7

//读取媒体数据包失败
#define FFMPEG_READ_PACKETS_FAIL (ERROR_CODE_FFMPEG_PLAY - 8)

#endif //KEVINPLAYER_MACRO_H
