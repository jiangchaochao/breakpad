#include <jni.h>
#include <string>
#include "breakpad/src/client/linux/handler/minidump_descriptor.h"
#include "breakpad/src/client/linux/handler/exception_handler.h"

bool DumpCallback(const google_breakpad::MinidumpDescriptor &descriptor,
                  void *context,
                  bool succeeded) {
    printf("Dump path: %s\n", descriptor.path());
    // return false 是让我们自己的程序处理完之后交给系统处理
    return false;
}

void Crash() {
    volatile int *a = reinterpret_cast<volatile int *>(NULL);
    *a = 1;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_jiangc_breakpad_Bugly_buglyInit(JNIEnv *env, jobject thiz, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, nullptr);

    google_breakpad::MinidumpDescriptor descriptor(path);
    // 加static 是为了延长它的声明周期，不然方法执行完就没了，就监测不到了,也可以放全局
    static google_breakpad::ExceptionHandler eh(descriptor, nullptr, DumpCallback,
                                                nullptr, true, -1);
    env->ReleaseStringUTFChars(path_, path);
}
/**
 * 测试native crash
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_jiangc_breakpad_Bugly_testNativeCrash(JNIEnv *env, jobject thiz) {
    Crash();
}