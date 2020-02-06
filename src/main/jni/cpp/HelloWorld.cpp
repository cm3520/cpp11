#include <stdio.h>
#include "cpp11_java_App.h"

JNIEXPORT void JNICALL Java_cpp11_java_App_print
  (JNIEnv *, jobject) {
    printf("Hello World!\n");
    return;
}

JNIEXPORT jstring JNICALL Java_cpp11_java_App_getLine
  (JNIEnv *env, jobject object, jstring promt) {
    char buf[128];
#if 0
    const char *str;
    str = env->GetStringUTFChars(promt, NULL); //分配内存
    if (str == NULL)
        return NULL;//OutOfMemoryError
    
    printf("%s", str);
    env->ReleaseStringUTFChars(promt, str);
#else
    char outbuf[128];
    int len = env->GetStringUTFLength(promt); //不分配内存
    env->GetStringUTFRegion(promt, 0, len, outbuf);
    printf("%s", outbuf);
#endif
    
    scanf("%s", buf);
    return env->NewStringUTF(buf);
}

JNIEXPORT jint JNICALL Java_cpp11_java_App_sumArray
  (JNIEnv *env, jobject object, jintArray arr)
{
    jint i, sum = 0;
#if 0 
    jint buf[10];
    
    env->GetIntArrayRegion(arr, 0, 10, buf);
    for (i = 0; i < 10; ++i)
      sum += buf[i];
#else
    jint *carr;
    printf("array size = %d\n", env->GetArrayLength(arr));
    carr = env->GetIntArrayElements(arr, NULL);
    if (carr == NULL)
        return 0; //exception occurred
    for (i = 0; i < 10; ++i) {
      sum += carr[i];
    }
    env->ReleaseIntArrayElements(arr, carr, 0);
#endif

    return sum;
}

JNIEXPORT jobjectArray JNICALL Java_cpp11_java_App_initInt2DArray
  (JNIEnv *env, jobject object, jint size) {
    jobjectArray result;
    int i;
    //数据类类型
    jclass intArrCls = env->FindClass("[I");
    if (intArrCls == NULL)
        return NULL;
    //创建以数据类类型为元素的对象数组
    result = env->NewObjectArray(size, intArrCls, NULL);
    if (result == NULL)
        return NULL;
    
    for (int i = 0; i < size; i++) {
        jint tmp[256];
        int j;
        //实例化数据类类型的对象并初始化
        jintArray iarr = env->NewIntArray(size);
        if (iarr == NULL)
            return NULL;
        for (j = 0; j < size; j++) {
            tmp[j] = i + j;
        }
        env->SetIntArrayRegion(iarr, 0, size, tmp);
        //把初始化过的对象放到对象数组中
        env->SetObjectArrayElement(result, i, iarr);
        //释放本地对象
        env->DeleteLocalRef(iarr);
    }
    return result;
}

JNIEXPORT void JNICALL Java_cpp11_java_App_accessField(JNIEnv *env, jobject object)
{
    jfieldID fid;

    // For non-static Field
    jstring jstr;
    const char* str;

    // Get a reference to obj's class
    jclass cls = env->GetObjectClass(object);
    
    printf("In C:\n");

    // Look for the instance field s in class
    fid = env->GetFieldID(cls, "s", "Ljava/lang/String;");
    if (fid == NULL) {
        return;
    }

    // Read the instance field s
    jstr = (jstring)env->GetObjectField(object, fid);
    str = env->GetStringUTFChars(jstr, NULL);
    if (str == NULL)
       return;
    printf(" c.s = \"%s\"\n", str);
    env->ReleaseStringUTFChars(jstr, str);
    jstr = env->NewStringUTF("native layer");
    if (jstr == NULL)
        return;
    env->SetObjectField(object, fid, jstr);

    // For static Field
    jint si;
    fid = env->GetStaticFieldID(cls, "si", "I");
    if (fid == NULL)
        return;
    si = env->GetStaticIntField(cls, fid);
    printf(" App.si = %d\n", si);
    env->SetStaticIntField(cls, fid, 200);
}

static jstring MyNewString(JNIEnv *env, jchar *chars, jint len)
{
    jstring result;

    jclass stringClass = env->FindClass("Ljava/lang/String;");
    if (stringClass == NULL)
        return NULL; //exception thrown
    
    // caching MethodID
    static jmethodID mid = NULL;
    if (mid == NULL) {
        // Get the method ID for the String(char[]) constructor
        mid = env->GetMethodID(stringClass, "<init>", "([C)V");
        if (mid == NULL)
            return NULL; //exception thrown
    }
    
    // Create a char[] that holds the string characters
    jcharArray carry = env->NewCharArray(len);
    if (carry == NULL)
        return NULL; //exception thrown
    
    env->SetCharArrayRegion(carry, 0, len, chars);

    // Construct a java.lang.String object
    result = (jstring)env->NewObject(stringClass, mid, carry);

    // Free local references
    env->DeleteLocalRef(carry);
    env->DeleteLocalRef(stringClass);
    return result;
}


//callback from native to java
JNIEXPORT void JNICALL Java_cpp11_java_App_nativeMethod(JNIEnv *env, jobject object)
{
    jmethodID jmd;

    jclass cls = env->GetObjectClass(object);

    jmd = env->GetMethodID(cls, "callback", "()V");
    if (jmd == NULL)
        return; // Method not found
    printf("In C\n");
    env->CallVoidMethod(object, jmd);

    // for Calling Static Method
    jmd = env->GetStaticMethodID(cls, "scallback", "()V");
    if (jmd == NULL)
        return;
    env->CallStaticVoidMethod(cls, jmd);

    // for Calling Instance Methods of a SupperClass
    cls = env->FindClass("Lcpp11/java/SupperClass;");
    if (cls == NULL) {
        printf("error: SupperClass not Found\n");
    }
    jmd = env->GetMethodID(cls, "show", "()V");
    if (jmd == NULL)
        return;
    env->CallNonvirtualVoidMethod(object, cls, jmd);
    env->DeleteLocalRef(cls);


    // Invoking the java class to construct object in native layer
    cls = env->FindClass("Lcpp11/java/SupperClass;");
    if (cls == NULL) {
        printf("error: SupperClass not Found\n");
        return;
    }

    jmd = env->GetMethodID(cls, "<init>", "()V");
    if (jmd == NULL)
        return;

    jobject obj = env->NewObject(cls, jmd);
    if (obj == NULL)
        return;
    
    jmd = env->GetMethodID(cls, "printString", "(Ljava/lang/String;)V");
    if (jmd == NULL)
        return;
    
    jchar tmp[10] = {'H', 'e', 'l', 'l', 'o', 'I', 'n', 'i', 't', '.'};

    env->CallVoidMethod(obj, jmd, MyNewString(env, tmp, 10));

    // free localref
    env->DeleteLocalRef(cls);
    env->DeleteLocalRef(obj);
}