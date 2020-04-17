package com.kevin.kevinplayer;

// Java 层的 标记
public interface Flags {

    //打不开媒体数据源
    // #define FFMPEG_CAN_NOT_OPEN_URL (ERROR_CODE_FFMPEG_PREPARE - 1)
    int FFMPEG_CAN_NOT_OPEN_URL = -1;

    //找不到媒体流信息
    // #define FFMPEG_CAN_NOT_FIND_STREAMS (ERROR_CODE_FFMPEG_PREPARE - 2)
    int FFMPEG_CAN_NOT_FIND_STREAMS = -2;

    //找不到解码器
    // #define FFMPEG_FIND_DECODER_FAIL (ERROR_CODE_FFMPEG_PREPARE - 3)
    int FFMPEG_FIND_DECODER_FAIL = -3;

    //无法根据解码器创建上下文
    // #define FFMPEG_ALLOC_CODEC_CONTEXT_FAIL (ERROR_CODE_FFMPEG_PREPARE - 4)
    int FFMPEG_ALLOC_CODEC_CONTEXT_FAIL = -4;

    //根据流信息 配置上下文参数失败
    // #define FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL (ERROR_CODE_FFMPEG_PREPARE - 5)
    int FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL = -5;

    //打开解码器失败
    // #define FFMPEG_OPEN_DECODER_FAIL (ERROR_CODE_FFMPEG_PREPARE - 6)
    int FFMPEG_OPEN_DECODER_FAIL = -6;

    //没有音视频
    // #define FFMPEG_NOMEDIA (ERROR_CODE_FFMPEG_PREPARE - 7)
    int FFMPEG_NOMEDIA = -7;

    //读取媒体数据包失败
    // #define FFMPEG_READ_PACKETS_FAIL (ERROR_CODE_FFMPEG_PLAY - 8)
    int FFMPEG_READ_PACKETS_FAIL = -8;

}
