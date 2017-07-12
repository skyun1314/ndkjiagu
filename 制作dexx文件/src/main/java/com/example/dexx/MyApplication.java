package com.example.dexx;

import android.app.Application;
import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * Created by zk on 2017/7/7.
 */

public class MyApplication extends Application {

    protected AssetManager mAssetManager;//资源管理器
    protected Resources mResources;//资源
    protected Resources.Theme mTheme;//主题

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);


        try {
            readZipFile(this.getApplicationInfo().sourceDir);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.e("wodelog", "MyApplication：我进来了");
    }

    public static void readZipFile(String file) throws Exception {
        InputStream in = new BufferedInputStream(new FileInputStream(file));
        ZipInputStream zin = new ZipInputStream(in);
        ZipEntry ze;

        while ((ze = zin.getNextEntry()) != null) {
            Log.e("wodelog", "file - " + ze.getName() + " : " + ze.getSize() + " bytes");
        }
        zin.closeEntry();
    }


}
