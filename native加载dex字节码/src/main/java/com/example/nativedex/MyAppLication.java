package com.example.nativedex;

import android.app.Application;
import android.content.Context;


/**
 * Created by zk on 2017/6/30.
 */

public class MyAppLication extends Application {

    static {
        System.loadLibrary("zkjg-lib");
    }


    protected native void attachBaseContext(Context base);

    public native void onCreate();


}
