package com.jiangc.breakpad;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.PersistableBundle;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Bugly bugly = new Bugly();

        File externalCacheDir = getExternalCacheDir();
        File path = new File(externalCacheDir + "/crash_info");
        if (!path.exists()) {
            path.mkdirs();
        }
        bugly.buglyInit(path.getPath());

        bugly.testNativeCrash();
    }
}