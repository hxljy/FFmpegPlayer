cmake_minimum_required(VERSION 3.4.1)

# 定义ffmpeg总目录的变量
set(FFMPEG ${CMAKE_SOURCE_DIR}/ffmpeg)

# 定义 rtmp总目录的变量
set(RTMP ${CMAKE_SOURCE_DIR}/rtmp)

# 指定ffmpeg里面的头文件目录
include_directories(${FFMPEG}/include)

# 指定ffmpeg里面的库文件目录（静态库）
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${FFMPEG}/libs/${CMAKE_ANDROID_ARCH_ABI}")

# 指定rtmp里面的库文件目录（静态库）
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${RTMP}/libs/${CMAKE_ANDROID_ARCH_ABI}")

# 批量添加自己写的cpp文件
file(GLOB CPPFILS *.cpp) # 注意：这里有一个坑，不要把 *.h 加入进来了，例如：*.h

add_library( 
        native-lib
        SHARED
        # native-lib.cpp
        ${CPPFILS}
)

find_library( 
        log-lib
        log)

target_link_libraries( 
        native-lib

        # 此坑不想去踩了，直接可以忽略顺序，为所欲为的填写即可
        -Wl,--start-group
        avcodec avfilter avformat avutil swresample swscale
        -Wl,--end-group

        ${log-lib}
        z # 存放在NDK目录的，可以去支持FFmpeg，FFmpeg必须要依赖z
        rtmp

        android # NDK中的支持，能够去渲染屏幕
        OpenSLES # NDK的支持，能够去支持声卡
)