#include <jni.h>
#include <string>
#include <algorithm>
#include <climits>
#include <android/log.h>

#define TAG_LOG "JNI_CUSTOM"
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, TAG_LOG, __VA_ARGS__)
#define LOG_ERR(...) __android_log_print(ANDROID_LOG_ERROR, TAG_LOG, __VA_ARGS__)

// Fonction 1 : message simple
extern "C" JNIEXPORT jstring JNICALL Java_com_example_jnidemo_MainActivity_getNativeMessage(
        JNIEnv* env,
        jobject /* obj */) {

    LOG_INFO("Message natif appelé");
    return env->NewStringUTF("Bienvenue depuis le code C++ !");
}

// Fonction 2 : calcul produit (factoriel modifié)
extern "C" JNIEXPORT jint JNICALL Java_com_example_jnidemo_MainActivity_computeProduct(
        JNIEnv* env,
        jobject /* obj */,
        jint value) {

    if (value < 0) {
        LOG_ERR("Valeur negative");
        return -1;
    }

    long long result = 1;
    for (int i = 1; i <= value; i++) {
        result *= i;

        if (result > INT_MAX) {
            LOG_ERR("Depassement detecte");
            return -2;
        }
    }

    return (jint) result;
}

// Fonction 3 : inversion texte
extern "C" JNIEXPORT jstring JNICALL Java_com_example_jnidemo_MainActivity_flipText(
        JNIEnv* env,
        jobject /* obj */,
        jstring inputText) {

    if (inputText == nullptr) {
        return env->NewStringUTF("Texte invalide");
    }

    const char* rawChars = env->GetStringUTFChars(inputText, nullptr);
    std::string buffer(rawChars);
    env->ReleaseStringUTFChars(inputText, rawChars);

    std::reverse(buffer.begin(), buffer.end());

    return env->NewStringUTF(buffer.c_str());
}

// Fonction 4 : somme tableau
extern "C" JNIEXPORT jint JNICALL Java_com_example_jnidemo_MainActivity_calculateSum(
        JNIEnv* env,
        jobject /* obj */,
        jintArray dataArray) {

    if (dataArray == nullptr) return -1;

    jsize length = env->GetArrayLength(dataArray);
    jint* values = env->GetIntArrayElements(dataArray, nullptr);

    long long total = 0;
    for (int i = 0; i < length; i++) {
        total += values[i];
    }

    env->ReleaseIntArrayElements(dataArray, values, 0);

    if (total > INT_MAX) return -2;

    return (jint) total;
}