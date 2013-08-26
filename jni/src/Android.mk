LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
SDL_mixer_PATH := ../SDL_mixer

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/$(SDL_mixer_PATH)

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c $(shell find $(LOCAL_PATH) -name *.c | cut -c 9-)

LOCAL_SHARED_LIBRARIES := SDL2 SDL_mixer
LOCAL_STATIC_LIBRARIES := android_support

LOCAL_LDLIBS := -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)
