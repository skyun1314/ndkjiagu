package com.example.dexx;

import android.app.Application;
import android.util.Log;

/**
 * Created by zk on 2017/7/7.
 */

public class MyApplication extends Application {


    @Override
    public void onCreate() {
        super.onCreate();
        Log.e("wodelog","MyApplication：我进来了");
    }
}
