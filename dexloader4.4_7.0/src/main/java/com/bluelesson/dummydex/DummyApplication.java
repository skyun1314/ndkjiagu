package com.bluelesson.dummydex;

import android.app.Application;
import android.content.Context;

/**
 * Created by lantie on 2017/7/27.
 */

public class DummyApplication extends Application {
    static{
        System.loadLibrary("dex15pbloader");
    }

    public DummyApplication() {
        super();
    }

    public  native  void onCreate() ;

    public native void  attachBaseContext(Context base);
}
