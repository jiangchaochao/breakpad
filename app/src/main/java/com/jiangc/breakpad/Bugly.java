package com.jiangc.breakpad;

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
