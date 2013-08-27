#include <jni.h>
#include "org_ledyba_noiz2sa_Helper.h"
#include "java.h"
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

#include <stdlib.h>

#include "log.h"

static JavaVM* vm;
static jclass* klass;


static JNIEnv* attachJavaVM()
{
	JNIEnv *env;
	int status = (*vm)->AttachCurrentThread(vm, &env, NULL);
	if(status < 0) {
		LOGE("failed to attach current thread");
		return 0;
	}
	return env;
}
static void detatchJavaVM()
{
	/* if(attach){ */
	/* 	(*vm)->DetachCurrentThread(vm); */
	/* 	attach = 0; */
	/* } */
}

void JNICALL Java_org_ledyba_noiz2sa_Helper_onLoadJNI(JNIEnv* env, jclass klass_)
{
	if(env){
		LOGD("Env: %p", env);
	}else{
		LOGE("env is null.");
		exit(-1);
	}
	(*env)->GetJavaVM(env, &vm);
	if(vm){
		LOGD("VM: %p", vm);
	}else{
		LOGE("VM is null.");
		exit(-1);
	}
	klass = (jclass)((*env)->NewGlobalRef(env, klass_));
	if(klass){
		LOGD("Klass: %p", klass);
	}else{
		LOGE("klass is null.");
		exit(-1);
	}
	LOGD("org.ledyba.noiz2sa.Helper.onLoadJNI loaded.");
}

AAssetManager* getAAssetManager()
{
	LOGD("getAAssetManager called.");
	JNIEnv* env = attachJavaVM();
	LOGD("getAAssetManager / attatched: %p", env);
	jmethodID getAssetManagerId =
		(*env)->GetStaticMethodID(env, klass, "getAssetManager", "()Landroid/content/res/AssetManager;");
	LOGD("getAAssetManager / method: %p", getAssetManagerId);
	if( getAssetManagerId ){
		LOGD("getAssetManagerId: %p", getAssetManagerId);
	}else{
		LOGE("getAssetManagerId is null.");
		detatchJavaVM();
		exit(-1);
	}
	AAssetManager* mgr =
		AAssetManager_fromJava(env, (*env)->CallStaticObjectMethod(env, klass, getAssetManagerId));
	LOGD("getAAssetManager / mgr: %p", mgr);
	detatchJavaVM();
	LOGD("getAAssetManager / detatched.");
	return mgr;
}

static int keyImpl(const char* method)
{
	JNIEnv* env = attachJavaVM();
	//LOGD("keyImpl / attatched: %p", env);
	if(!env){
		LOGE("keyImpl / env not found.");
		return 0;
	}
	jmethodID mid = (*env)->GetStaticMethodID(env, klass, method, "()I");
	if( !mid ){
		LOGE("keyImpl / not found: %s", method);
		detatchJavaVM();
		return 0;
	}
	int r = (*env)->CallStaticIntMethod(env, klass, mid);
	detatchJavaVM();
	//LOGD("keyImpl / r: %d", r);
	return r;
}

int JOYPAD_getA()
{
	return keyImpl("keyAPressed");
}
int JOYPAD_getB()
{
	return keyImpl("keyBPressed");
}
int JOYPAD_getStart()
{
	return keyImpl("keyStartPressed");
}
int JOYPAD_getSelect()
{
	return keyImpl("keySelectPressed");
}

static float padImpl(const char* method)
{
	JNIEnv* env = attachJavaVM();
	//LOGD("padImpl / attatched: %p", env);
	if(!env){
		LOGE("padImpl / env not found.");
		return 0;
	}
	jmethodID mid = (*env)->GetStaticMethodID(env, klass, method, "()F");
	if( !mid ){
		LOGE("padImpl / not found: %s", method);
		detatchJavaVM();
		return 0;
	}
	float r = (*env)->CallStaticFloatMethod(env, klass, mid);
	//LOGD("padImpl / r: %f", r);
	detatchJavaVM();
	return r;
}
float JOYPAD_getX()
{
	return padImpl("getX");
}
float JOYPAD_getY()
{
	return padImpl("getY");
}
