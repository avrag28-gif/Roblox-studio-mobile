#include <jni.h>
#include "core/data_model.h"
static rsm::DataModel game;
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*,void*){game.InitializeDefaultServices();return JNI_VERSION_1_6;}