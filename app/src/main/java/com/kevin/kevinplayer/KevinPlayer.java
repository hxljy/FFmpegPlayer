package com.kevin.kevinplayer;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.File;

/**
 * 上层核心播放
 */
public class KevinPlayer implements SurfaceHolder.Callback {

    static {
        // 加载 libnative-lib.so 的时候，就会调用JNI_OnLoad
        System.loadLibrary("native-lib");
    }

    private OnPreparedListener onPreparedListener; // 消息告诉MainActivity

    private SurfaceHolder surfaceHolder; // 为了 渲染 屏幕 帧数据

    public KevinPlayer() {

    }

    // 媒体流的源 (文件路径 、 直播地址（要集成X264才行）)
    private String dataSource;

    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }

    /**
     * 准备工作（解封装-其实就是把.mp4给解出来）
     * MainActivity打开后，最先调用的 函数
     */
    public void prepare() {
        prepareNative(dataSource);
    }

    /**
     * 开始播放
     */
    public void start() {
        startNative();
    }

    /**
     * 停止播放
     */
    public void stop() {
        stopNative();
    }

    /**
     * 资源释放
     */
    public void release() {
        releaseNative();
    }

    /**
     * 给JNI 方式调用的方法，准备成功
     */
    public void onPrepared() {
        if (null != onPreparedListener) {
            this.onPreparedListener.onPrepared();
        }
    }

    /**
     * 给JNI 方式回调的方法（进行错误的回调）
     */
    public void onError(int errorCode) {
        if (null != onPreparedListener) {
            String errorText = null;

            switch (errorCode) {
                case Flags.FFMPEG_ALLOC_CODEC_CONTEXT_FAIL:
                    errorText = "无法根据解码器创建上下文";
                    break;
                case Flags.FFMPEG_CAN_NOT_FIND_STREAMS:
                    errorText = "找不到媒体流信息";
                    break;
                case Flags.FFMPEG_CAN_NOT_OPEN_URL:
                    errorText = "打不开媒体数据源";
                    break;
                case Flags.FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL:
                    errorText = "根据流信息 配置上下文参数失败";
                    break;
                case Flags.FFMPEG_FIND_DECODER_FAIL:
                    errorText = "找不到解码器";
                    break;
                case Flags.FFMPEG_NOMEDIA:
                    errorText = "没有音视频";
                    break;
                case Flags.FFMPEG_READ_PACKETS_FAIL:
                    errorText = "读取媒体数据包失败";
                    break;
                default:
                    errorText = "未知错误，自己去检测你的垃圾代码...";
                    break;
            }
            this.onPreparedListener.onError(errorText);
        }
    }

    /**
     * MainActivity设置监听，就可以回调到MainActivity的方法，进行UI的操作了
     * @param onPreparedListener
     */
    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
    }

    /**
     * 消息告诉MainActivity
     */
    interface OnPreparedListener {
        void onPrepared();
        void onError(String errorText);
    }

    /**
     * 全部都是 native 函数
     * @param dataSource
     */
    public native void prepareNative(String dataSource);
    public native void startNative();
    public native void stopNative();
    public native void releaseNative();
    public native String getFFmpegVersion();

    // ##############  SurfaceView 相关的

    public void setSurfaceView(SurfaceView surfaceView) {
        if (null != this.surfaceHolder) {
            this.surfaceHolder.removeCallback(this);
        }
        this.surfaceHolder = surfaceView.getHolder();
        this.surfaceHolder.addCallback(this);
    }

    // Surface创建 回调
    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) { }

    // Surface发生改变
    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        // SufaceView 的 surfaceHolder Surface对象 给Native（Native好去渲染屏幕）
        Surface surface = surfaceHolder.getSurface();
        setSurfaceNative(surface);
    }

    // Surface销毁
    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) { }

    private native void setSurfaceNative(Surface surface);

}
