

# breakpad

### 简单介绍

1. C/C++ 源码地址：

   https://github.com/google/breakpad.git

   该库主要用于监控native相关的crash，这里通过ndk编译出来供Android使用。库中有demo，下载下来直接使用即可

2. demo的使用

   native库 关联的类如下：

   ```java
   public class Bugly {
       static {
           System.loadLibrary("bugly");
       }

       /**
        * 初始化，传入存放文件的目录
        *
        * @param path 目录地址
        */
       public native void buglyInit(String path);

       /**
        * 测试本地崩溃
        */
       public native void testNativeCrash();
   }
   ```

   native 文件如下：

   ```c++
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
   ```

   最主要的方法就是buglyInit方法，其中path_为Java层传过来的路径，用于存放crash文件的，只需要在app启动的时候初始化一下就行了



   MainActivity 如下：

   ```java
   public class MainActivity extends AppCompatActivity {


       @Override
       protected void onCreate(@Nullable Bundle savedInstanceState) {
           super.onCreate(savedInstanceState);
           setContentView(R.layout.activity_main);
           Bugly bugly = new Bugly();

           File externalCacheDir = getExternalCacheDir();
           // 在app缓存目录下创建一个目录
           File path = new File(externalCacheDir + "/crash_info");
           if (!path.exists()) {
               path.mkdirs();
           }
           // 初始化crash 监控
           bugly.buglyInit(path.getPath());
   		// 这个接口用来测试crash，调用它会导致app崩溃，这里只是测试，实际项目不需要
           bugly.testNativeCrash();
       }
   }
   ```

   运行后我们会得到一个后缀为.dump的文件在app的缓存目录中。

   ![image-20210522171225991](https://github.com/jiangchaochao/breakpad/blob/main/image-20210522171225991.png?raw=true)



3. 在得到crash文件之后可自行选择处理方式，上传到服务器或者其他操作，由于产生的文件不可以直接查看，可是用Android studio中带的工具来解析

4. 解析工具使用

1. 路径

   在Android studio 安装目录的bin\lldb\bin下

    Android Studio\bin\lldb\bin\minidump_stackwalk.exe

2. 使用方式

   通过命令：minidump_stackwalk  778994aa-3f76-4e76-6cc60896-dab3eea7.dmp > crash.txt

   得到 可以看得懂的crash文件

   如下：

   ```C
   Operating system: Android      // 崩溃的系统
                     0.0.0 Linux 5.4.61-android11-0-00791-gbad091cc4bf3-ab6833933 #1 SMP PREEMPT 2020-09-14 14:42:20 i686
   CPU: x86                       // CPU架构，这里注意，什么架构下崩溃的，我们需要用不同的工具进行解析
        GenuineIntel family 6 model 31 stepping 1
        4 CPUs

   GPU: UNKNOWN

   Crash reason:  SIGSEGV                     // 崩溃的原因
   Crash address: 0x0                         // 崩溃的地址
   Process uptime: not available

   Thread 0 (crashed)
    0  libbugly.so + 0x1d715                  // 引起崩溃的代码的地址，这里我们还是看不懂，通过addr2line工具来看
       eip = 0xc7e40715   esp = 0xfffdefb0   ebp = 0xfffdefb8   ebx = 0xc7eb5508
       esi = 0xd5c673ec   edi = 0xfffdf1cc   eax = 0x00000000   ecx = 0xfffdf008
       edx = 0xf6b80220   efl = 0x00010282
       Found by: given as instruction pointer in context
    1  libbugly.so + 0x1dd0b
       eip = 0xc7e40d0b   esp = 0xfffdefc0   ebp = 0xfffdefe8
       Found by: previous frame's frame pointer
    2  libart.so + 0x11133
       eip = 0xf0db3133   esp = 0xfffdeff0   ebp = 0xfffdf010
       Found by: previous frame's frame pointer
   ```



3. 通过addr2line 工具得到崩溃地址的代码的方法名以及所在行

   *** 注意：必须是带有符号表的库文件，一般我们在debug目录中可以找到***

   我们通过addr2line工具解析崩溃原因，addr2line工具在ndk下，我们根据崩溃的信息，找到对应的架构的addr2line工具

   如下：Android\Sdk\ndk\21.1.6352462\toolchains\x86-4.9\prebuilt\windows-x86_64\bin

   ![image-20210522172806657](https://github.com/jiangchaochao/breakpad/blob/main/image-20210522172806657.png?raw=true)



我们是x86下崩溃的，我们就找到x86下的addr2line工具解析，同时注意的是，我们解析的so库也要是x86下的

![image-20210522172928559](https://github.com/jiangchaochao/breakpad/blob/main/image-20210522172928559.png?raw=true)

可通过如下命令进行解析：

i686-linux-android-addr2line.exe -f -C -e C:\Users\huawei\Desktop\libbugly.so 0x1d715

格式：i686-linux-android-addr2line.exe -f -C -e  库名 崩溃地址

![image-20210522173145742](https://github.com/jiangchaochao/breakpad/blob/main/image-20210522173145742.png?raw=true)

可以看到崩溃的方法在Crash()

崩溃的地址在bugly.cpp 的第16行

我们回过来看一下这个文件的第16行 Crash方法

![image-20210522173412406](https://github.com/jiangchaochao/breakpad/blob/main/image-20210522173412406.png?raw=true)



到此，native的监控完成。我们可以根据需要，把dump文件上传到服务器，然后定位崩溃的原因。



