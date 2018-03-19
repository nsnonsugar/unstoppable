cmake_minimum_required(VERSION 3.4.1)

#グルーコードのビルド
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue
    STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Wall")

# Export ANativeActivity_onCreate(),
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")


# ディレクトリ設定
set(DRAW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/draw)
set(SOUND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sound)
set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common)
set(PLATFORM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/platform)
set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/app)

set(LIBPNG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/libpng)

#libpngのビルド
add_subdirectory(${LIBPNG_DIR})

# インクルードパス設定
include_directories(
    ${DRAW_DIR}
    ${SOUND_DIR}
    ${COMMON_DIR}
    ${PLATFORM_DIR}
    ${APP_DIR}
    ${ANDROID_NDK}/sources/android/native_app_glue
    ${LIBPNG_DIR}/jni
)

add_library(unstoppable
    SHARED
    ${PLATFORM_DIR}/platform_dependent.cpp
    ${PLATFORM_DIR}/android_fopen.c

    ${COMMON_DIR}/Message.cpp

    ${DRAW_DIR}/draw_thread.cpp
    ${DRAW_DIR}/stoppable_renderer.cpp
    ${DRAW_DIR}/unstoppable_renderer.cpp
    ${DRAW_DIR}/util.c
    ${DRAW_DIR}//PngPixelImage.c

    ${SOUND_DIR}/sound_thread.cpp
    ${SOUND_DIR}/sound_player.cpp

    ${APP_DIR}/main.cpp
)

# add lib dependencies
target_link_libraries(unstoppable
    android
    native_app_glue
    EGL
    GLESv1_CM
    GLESv2
    log
    png
    OpenSLES
)
