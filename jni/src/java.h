
#pragma once
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

#ifndef ANDROID
typedef int AAssetManager;
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern AAssetManager* getAAssetManager();
extern const char* getPrefPath();
extern float JOYPAD_getX();
extern float JOYPAD_getY();

extern int JOYPAD_getA();
extern int JOYPAD_getB();
extern int JOYPAD_getStart();
extern int JOYPAD_getSelect();

extern float Window_getScale();

extern void startDonationActivity();

#ifdef __cplusplus
}
#endif


