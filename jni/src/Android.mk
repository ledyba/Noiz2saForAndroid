LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
SDL_mixer_PATH := ../SDL_mixer

LOCAL_C_INCLUDES :=\
	$(LOCAL_PATH)/$(SDL_PATH)/include\
	$(LOCAL_PATH)/$(SDL_mixer_PATH)\
	$(LOCAL_PATH)/../bulletml\
	$(LOCAL_PATH)/../bulletml/src\
	$(LOCAL_PATH)/../bulletml/src/boost

# Add your application source files here...
LOCAL_SRC_FILES := \
./soundmanager.c     \
./ship.c             \
./shot.c             \
./letterrender.c     \
./barragemanager.cc  \
./vector.c           \
./rand.c             \
./attractmanager.c   \
./background.c       \
./bonus.c            \
./screen.c           \
./clrtbl.c           \
./foecommand.cc      \
./foe.cc             \
./frag.c             \
./noiz2sa.c          \
./degutil.c          \
../bulletml/src/formula-variables.cpp           \
../bulletml/src/bulletmlparser-tinyxml.cpp      \
../bulletml/src/bulletmlrunnerimpl.cpp          \
../bulletml/src/calc.cpp                        \
../bulletml/src/bulletmlparser.cpp              \
../bulletml/src/bulletmltree.cpp                \
../bulletml/src/tinyxml/tinystr.cpp             \
../bulletml/src/tinyxml/tinyxml.cpp             \
../bulletml/src/tinyxml/tinyxmlerror.cpp        \
../bulletml/src/tinyxml/tinyxmlparser.cpp       \
../bulletml/src/bulletmlrunner.cpp              \
$(SDL_PATH)/src/main/android/SDL_android_main.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer
LOCAL_STATIC_LIBRARIES := android_support

LOCAL_LDLIBS := -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)
