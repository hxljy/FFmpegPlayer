// TODO 核心的中转，是在此类，例如：解封装，解封装完成后，进行抽离 视频数据，和，音频数据，分别交给Video/Audio两个通道 进行解码操作

#include <pthread.h>
#include "KevinPlayer.h"

// TODO 异步 函数指针 - 准备工作prepare
void * customTaskPrepareThread(void * pVoid) {
    KevinPlayer * kevinPlayer = static_cast<KevinPlayer *>(pVoid);
    kevinPlayer->prepare_();
    return 0; // 坑：一定要记得return
}

// TODO 异步 函数指针 - 开始播放工作start
void * customTaskStartThread(void * pVoid) {
    KevinPlayer * kevinPlayer = static_cast<KevinPlayer *>(pVoid);
    kevinPlayer->start_();
    return 0; // 坑：一定要记得return
}

KevinPlayer::KevinPlayer() {}

KevinPlayer::KevinPlayer(const char * data_source, JNICallback *pCallback) {
    // 这里有坑，这里赋值之后，不能给其他地方用，因为被释放了，变成了悬空指针
    // this->data_source = data_source;

    // 解决上面的坑，自己Copy才行
    // [strlen(data_source)] 这段代码有坑：因为（hello  而在C++中是 hello\n），所以需要加1
    this->data_source = new char[strlen(data_source) + 1];
    strcpy(this->data_source, data_source);

    this->pCallback = pCallback;
}

KevinPlayer::~KevinPlayer() {
    if (this->data_source) {
        delete this->data_source;
        this->data_source = 0;
    }
}

/**
 * 准备-其实就是解封装
 */
void KevinPlayer::prepare() {
    // 解析媒体流，通过ffmpeg API 来解析 dataSource

    // 思考：可以直接解析吗？
    // 答：1.可能是文件，考虑到io流
    //     2.可能是直播，考虑到网络
    // 所以需要异步

    // 是由MainActivity中方法调用下来的，所以是属于在main线程，不能再main线程操作，所以需要异步
    // 创建异步线程
    pthread_create(&pid_prepare, 0, customTaskPrepareThread, this);

}

/**
 * 注意：此函数是 真正的（解封装） 是在子线程,属于子线程
 */
void KevinPlayer::prepare_() {
    // TODO 【大流程是：解封装】

    // TODO 第一步：打开媒体地址（文件路径 、 直播地址）

    // 可以初始为NULL，如果初始为NULL，当执行avformat_open_input函数时，内部会自动申请avformat_alloc_context，这里干脆手动申请
    // 封装了媒体流的格式信息
    formatContext = avformat_alloc_context(); // 媒体上下文（总上下文）（打开包裹）

    // 字典：键值对的 @1
    AVDictionary *dictionary = 0;
    av_dict_set(&dictionary, "timeout", "5000000", 0); // 单位是微妙

    /**
     * 1.AVFormatContext 二级指针
     * 2.媒体文件路径，或，直播地址   注意：this->data_source; 这样是拿不到的，所以需要把this传递到pVoid
     * 3.输入的封装格式：一般是让ffmpeg自己去检测，所以给了一个0
     * 4.参数
     */
    int ret = avformat_open_input(&formatContext, data_source, 0, &dictionary); // 打开文件需要字典@1

    // @@@ 注意：字典使用过后，一定要去释放
    av_dict_free(&dictionary); // 释放字典

    if (ret) { // ret 不等于 0    ffmpeg【0==success】
        // 你的文件路径，或，你的文件损坏了，需要告诉用户
        // 把错误信息，告诉给Java层去（回调给Java）
        if (pCallback) {
            this->pCallback->onErrorAction(THREAD_CHILD, FFMPEG_CAN_NOT_OPEN_URL);
        }
        return;
    }

    // TODO 第二步：1.查找媒体中的音视频流的信息  2.给媒体上下文初始化
    ret = avformat_find_stream_info(formatContext, 0);
    if (ret < 0) {
        // 把错误信息，告诉给Java层去（回调给Java）
        if (pCallback) {
            pCallback->onErrorAction(THREAD_CHILD, FFMPEG_CAN_NOT_FIND_STREAMS);
        }
        return;
    }

    // TODO 第三步：根据流信息，流的个数，循环查找， 音频流 视频流
    // nb_streams == 流的个数
    for (int stream_index = 0; stream_index < formatContext->nb_streams; ++stream_index) {
        // TODO 第四步：获取媒体流（视频、音频）
        AVStream * stream = formatContext->streams[stream_index];

        // TODO 第五步：从stream流中获取解码这段流的参数信息，区分到底是 音频 还是 视频
        AVCodecParameters * codecParameters = stream->codecpar;

        // TODO 第六步：通过流的编解码参数中编解码ID，来获取当前流的解码器
        AVCodec * codec = avcodec_find_decoder(codecParameters->codec_id);
        // 虽然在第六步，已经拿到当前流的解码器了，但可能不支持解码这种解码方式
        if (!codec) { // 如果为空，就代表：解码器不支持
            // 把错误信息，告诉给Java层去（回调给Java）
            pCallback->onErrorAction(THREAD_CHILD, FFMPEG_FIND_DECODER_FAIL);
            return;
        }

        // TODO 第七步：通过 拿到的解码器，获取解码器上下文
        AVCodecContext *codecContext = avcodec_alloc_context3(codec);
        if (!codecContext) {
            // 把错误信息，告诉给Java层去（回调给Java）
            pCallback->onErrorAction(THREAD_CHILD, FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            return;
        }

        // TODO 第八步：给解码器上下文 设置参数 (内部会让编解码器上下文初始化)
        ret = avcodec_parameters_to_context(codecContext, codecParameters);
        if (ret < 0) {
            // 把错误信息，告诉给Java层去（回调给Java）
            pCallback->onErrorAction(THREAD_CHILD, FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            return;
        }

        // TODO 第九步：打开解码器
        ret =  avcodec_open2(codecContext, codec, 0);
        if (ret) { // 不等于0
            // 把错误信息，告诉给Java层去（回调给Java）
            pCallback->onErrorAction(THREAD_CHILD, FFMPEG_OPEN_DECODER_FAIL);
            return;
        }

        // AVStream媒体流中就可以拿到时间基 (音视频同步)
        AVRational time_base = stream->time_base;

        // TODO 第十步：从编码器参数中获取流类型codec_type
        if (codecParameters->codec_type == AVMEDIA_TYPE_AUDIO) { // 音频流
             audioChannel = new AudioChannel(stream_index, codecContext, time_base);
        } else if (codecParameters->codec_type == AVMEDIA_TYPE_VIDEO) { // 视频流（目前很多字幕流，都放在视频轨道中）
            // 获取视频相关的 fps
            // 平均帧率 == 时间基
            AVRational frame_rate = stream->avg_frame_rate;
            int fpsValue = av_q2d(frame_rate);

             videoChannel = new VideoChannel(stream_index, codecContext, time_base, fpsValue);
             videoChannel->setRenderCallback(renderCallback);
        }
    } // end for

    // TODO 第十一步：如果流中 没有音频 也 没有视频
    if (!audioChannel && !videoChannel) {
        // 把错误信息，告诉给Java层去（回调给Java）
        pCallback->onErrorAction(THREAD_CHILD, FFMPEG_NOMEDIA);
        return;
    }

    // TODO 第十二步：要么有音频，要么有视频，要么音视频都有
    // 准备完毕，通知Android上层开始播放
    if (this->pCallback) {
        pCallback->onPrepared(THREAD_CHILD); // 准备成功
    }

}

/**
 * 开始播放 -- 主线程
 */
void KevinPlayer::start() {
    isPlaying = 1;
    if (videoChannel) {
        videoChannel->setAudioChannel(audioChannel);
        videoChannel->start();
    }
    if (audioChannel) {
        audioChannel->start();
    }
    // 子线程  把压缩数据 存入到队列里面去
    pthread_create(&pid_start, 0, customTaskStartThread, this);
}

/**
 * 真正开始播放，属于子线程
 *
 * 此函数（读包（未解码 音频/视频 包） 放入队列）
 *
 * 把 音频 视频 压缩数据包 放入队列
 */
void KevinPlayer::start_() {
    // 循环 读视频包
    while (isPlaying) {

        // TODO 由于我们的操作是在异步线程，那就好办了，等待（先让消费者消费掉，然后在生产）
        // 下面解决方案：通俗易懂 让生产慢一点，消费了，在生产
        // 内存泄漏点1，解决方案：控制队列大小
        if (videoChannel && videoChannel->packages.queueSize() > 100) {
            // 休眠 等待队列中的数据被消费
            av_usleep(10 * 1000);
            continue;
        }
        // 内存泄漏点1，解决方案：控制队列大小
        if (audioChannel && audioChannel->packages.queueSize() > 100) {
            // 休眠 等待队列中的数据被消费
            av_usleep(10 * 1000);
            continue;
        }


        // AVPacket 可能是音频 可能是视频, 没有解码的（数据包） 压缩数据AVPacket
        AVPacket * packet = av_packet_alloc();
        int ret = av_read_frame(formatContext, packet); // 这行代码一执行完毕，packet就有（音视频数据）
        /*if (ret != 0) {
            // 后续处理
            return;
        }*/
        if (!ret) { // ret == 0
            // 把已经得到的packet 放入队列中
            // 先判断是视频  还是  音频， 分别对应的放入 音频队列  视频队列

            // packet->stream_index 对应之前的prepare中循环的i
            if (videoChannel && videoChannel->stream_index == packet->stream_index) {
                // 如果他们两 相等 说明是视频  视频包

                // 未解码的 视频数据包 加入到队列
                videoChannel->packages.push(packet);

            } else if (audioChannel && audioChannel->stream_index == packet->stream_index) {
                // 如果他们两 相等 说明是音频  音频包

                // 未解码的 音频数据包 加入到队列
                audioChannel->packages.push(packet);
            }
        } else if (ret == AVERROR_EOF) {  // or   end of file， 文件结尾，读完了 的意思
            // 代表读完了
            // TODO 一定是要 读完了 并且 也播完了，才做事情

        } else { // ret  != 0
            // 代表失败了，有问题
            break;
        }
    }
    // end while

    // 最后做释放的工作
    isPlaying = 0; // 标记清零
    videoChannel->stop();
    audioChannel->stop();
}

void KevinPlayer::setRenderCallback(RenderCallback renderCallback) {
    this->renderCallback = renderCallback;
}

