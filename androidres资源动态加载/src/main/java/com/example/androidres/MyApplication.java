package com.example.androidres;

import android.app.Application;
import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.lang.ref.WeakReference;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Map;

/**
 * Created by zk on 2017/7/12.
 */

public class MyApplication extends Application {

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        FileUtil.copyDex("res.zip", this);

        try {
            Class<?> aClass = Class.forName("android.app.ActivityThread");
            Class<?> aClass1 = Class.forName("android.app.LoadedApk");
            String ResPath = base.getFilesDir() + File.separator + "res.zip";

            Object currentActivityThread = aClass.getMethod("currentActivityThread").invoke(new Object[]{});
            Field mPackages = aClass.getDeclaredField("mPackages");
            mPackages.setAccessible(true);
            Map map = (Map) mPackages.get(currentActivityThread);
            WeakReference o = (WeakReference) map.get(base.getPackageName());
            Object loadedapk = o.get();
            Field mClassLoader = aClass1.getDeclaredField("mClassLoader");
            mClassLoader.setAccessible(true);


            Field mResDir = aClass1.getDeclaredField("mResDir");
            Field mResources = aClass1.getDeclaredField("mResources");
            mResDir.setAccessible(true);
            mResources.setAccessible(true);
            String mResDir_str_yuanshi = (String) mResDir.get(loadedapk);
            Resources mResources_yuanshi = (Resources) mResources.get(loadedapk);


          mResDir.set(loadedapk,ResPath);
            Resources resources = loadResources(ResPath, base);
            mResources.set(loadedapk,resources);


            String mResDir_str_xiugai = (String) mResDir.get(loadedapk);
            Resources mResources_xiugai = (Resources) mResources.get(loadedapk);

            Resources mResources_api = getResources();

            Log.e("wodelog","haha");
        } catch (Exception e) {
            e.printStackTrace();
        }

        int xxx=R.layout.activityxx;

        Resources mResources = getResources();

        int raw = mResources.getIdentifier("activity_main", "raw","com.example.dexx");
        Log.e("wodelog","getFromRaw:"+getFromRaw(raw,mResources));
        Log.e("wodelog","-----------------------------");
        Log.e("wodelog","getFromAssets:"+ getFromAssets("activity_main.xml",this,mResources));
        Log.e("wodelog","-----------------------------");



    }


    public Resources loadResources(String resPath, Context context) {
        AssetManager assetManager = null;
        try {
            assetManager = AssetManager.class.newInstance();
            Method addAssetPath = assetManager.getClass().getMethod("addAssetPath", String.class);
            addAssetPath.invoke(assetManager, resPath);
        } catch (Exception e) {
            e.printStackTrace();
        }
        Resources  mResources = new Resources(assetManager, context.getResources().getDisplayMetrics(),context.getResources().getConfiguration());

        Resources.Theme mTheme = mResources.newTheme();
        mTheme.setTo(context.getTheme());
        int raw = mResources.getIdentifier("activity_main", "raw","com.example.dexx");
        Log.e("wodelog","getFromRaw:"+getFromRaw(raw,mResources));
        Log.e("wodelog","-----------------------------");
        Log.e("wodelog","getFromAssets:"+ getFromAssets("activity_main.xml",context,mResources));
        Log.e("wodelog","-----------------------------");




        return mResources;
    }

    public static String getFromRaw(int id,Resources resources){
        try {
            InputStreamReader inputReader = new InputStreamReader(resources.openRawResource(id));
            BufferedReader bufReader = new BufferedReader(inputReader);
            String line="";
            String Result="";
            while((line = bufReader.readLine()) != null)
                Result += line;
            return Result;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }


    public static String getFromAssets(String fileName, Context context, Resources resources){
        try {
            InputStreamReader inputReader;
            if (resources==null){
                inputReader = new InputStreamReader(context.getResources()
                        .getAssets().open(fileName) );
            }
            else{
                inputReader = new InputStreamReader(resources.getAssets().open(fileName) );
            }



            BufferedReader bufReader = new BufferedReader(inputReader);
            String line="";
            String Result="";
            while((line = bufReader.readLine()) != null)
                Result += line;
            return Result;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }
}
