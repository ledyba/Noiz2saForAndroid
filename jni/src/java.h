
#pragma once
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

#ifdef __cplusplus
extern "C" {
#endif
extern AAssetManager* getAAssetManager();
extern float JOYPAD_getX();
extern float JOYPAD_getY();

extern int JOYPAD_getA();
extern int JOYPAD_getB();
extern int JOYPAD_getStart();
extern int JOYPAD_getSelect();

#ifdef __cplusplus
}
#endif


