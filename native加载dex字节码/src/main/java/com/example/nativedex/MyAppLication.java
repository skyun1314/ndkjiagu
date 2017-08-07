package com.example.nativedex;

import android.annotation.TargetApi;
import android.app.Application;
import android.app.Instrumentation;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;


/**
 * Created by zk on 2017/6/30.
 */

public class MyAppLication extends Application {

    static {
        System.loadLibrary("zkjg-lib");
    }


    protected native void  attachBaseContext(Context base);
    //public native void  onCreate();




    @Override
    public void onCreate() {
        Log.e("wodelog",this.toString());
        getOldAppCation();

    }




    @TargetApi(Build.VERSION_CODES.KITKAT)
    public void getOldAppCation() {
        // 如果源应用配置有Appliction对象，则替换为源应用Applicaiton，以便不影响源程序逻辑。
        String appClassName = null;
        ApplicationInfo ai = null;
        try {
            PackageManager packageManager= this.getPackageManager();
            ai = packageManager.getApplicationInfo(this.getPackageName(),  PackageManager.GET_META_DATA);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        Bundle bundle = ai.metaData;
        if (bundle != null && bundle.containsKey("APPLICATION_CLASS_NAME")) {
            appClassName = bundle.getString("APPLICATION_CLASS_NAME");//className 是配置在xml文件中的。
        } else {
            Log.i("demo", "have no application class name");
            return;
        }


        //有值的话调用该Applicaiton
        Object currentActivityThread = invokeDeclaredStaticMethod("android.app.ActivityThread", "currentActivityThread", new Class[]{}, new Object[]{});


        Object mBoundApplication = getFieldOjbect(
                "android.app.ActivityThread", currentActivityThread,
                "mBoundApplication");


        Object loadedApkInfo = getFieldOjbect(
                "android.app.ActivityThread$AppBindData",
                mBoundApplication, "info");
        //把当前进程的mApplication 设置成了null


        setDeclaredFieldOjbect("android.app.LoadedApk", "mApplication",
                loadedApkInfo, null);


        Object oldApplication = getFieldOjbect(
                "android.app.ActivityThread", currentActivityThread,
                "mInitialApplication");
        //http://www.codeceo.com/article/android-context.html
        ArrayList<Application> mAllApplications = (ArrayList<Application>)
                getFieldOjbect("android.app.ActivityThread",
                        currentActivityThread, "mAllApplications");
        mAllApplications.remove(oldApplication);//删除oldApplication

        ApplicationInfo appinfo_In_LoadedApk = (ApplicationInfo)
                getFieldOjbect("android.app.LoadedApk", loadedApkInfo,
                        "mApplicationInfo");
        ApplicationInfo appinfo_In_AppBindData = (ApplicationInfo)
                getFieldOjbect("android.app.ActivityThread$AppBindData",
                        mBoundApplication, "appInfo");
        appinfo_In_LoadedApk.className = appClassName;
        appinfo_In_AppBindData.className = appClassName;
        Application app = (Application) invokeMethod(
                "android.app.LoadedApk", "makeApplication", loadedApkInfo,
                new Class[]{boolean.class, Instrumentation.class},
                new Object[]{false, null});//执行 makeApplication（false,null）

        setDeclaredFieldOjbect("android.app.ActivityThread",
                "mInitialApplication", currentActivityThread, app);


        Map mProviderMap = (Map) getFieldOjbect(
                "android.app.ActivityThread", currentActivityThread,
                "mProviderMap");
        Iterator it = mProviderMap.values().iterator();
        while (it.hasNext()) {
            Object providerClientRecord = it.next();
            Object localProvider = getFieldOjbect(
                    "android.app.ActivityThread$ProviderClientRecord",
                    providerClientRecord, "mLocalProvider");
            setDeclaredFieldOjbect("android.content.ContentProvider",
                    "mContext", localProvider, app);
        }

        Log.i("wodelog", "app:" + app);
        app.onCreate();
    }


    public static Object invokeDeclaredMethod(String class_name, String method_name, Class[] pareTyple, Object[] pareVaules,Object obj) {

        try {

            Class obj_class = Class.forName(class_name);

            //MyDexClassLoader.enumMethod(obj_class.getName());

            Method method = obj_class.getDeclaredMethod(method_name, pareTyple);
            method.setAccessible(true);
            return method.invoke(obj, pareVaules);
        } catch (Exception e) {
            Log.i("jw", "invoke static method:" + Log.getStackTraceString(e));
        }
        return null;

    }


    public static Object invokeMethod(String class_name, String method_name, Object obj, Class[] pareTyple, Object[] pareVaules) {

        try {
            Class obj_class = Class.forName(class_name);
            Method method = obj_class.getMethod(method_name, pareTyple);
            method.setAccessible(true);
            return method.invoke(obj, pareVaules);
        } catch (Exception e) {
            Log.i("jw", "invoke method :" + Log.getStackTraceString(e));
        }
        return null;

    }


    public static Object invokeDeclaredStaticMethod(String class_name, String method_name, Class[] pareTyple, Object[] pareVaules) {

        try {
            Class obj_class = Class.forName(class_name);
            Method method = obj_class.getDeclaredMethod(method_name, pareTyple);
            method.setAccessible(true);
            return method.invoke(null, pareVaules);
        } catch (Exception e) {
            Log.i("jw", "invoke static method:" + Log.getStackTraceString(e));
        }
        return null;

    }

    public static Object getFieldOjbect(String class_name, Object obj, String filedName) {
        try {
            Class obj_class = Class.forName(class_name);
            Field field = obj_class.getDeclaredField(filedName);
            field.setAccessible(true);
            return field.get(obj);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return null;

    }

    public static void setDeclaredFieldOjbect(String classname, String filedName, Object obj, Object filedVaule) {
        try {
            Class obj_class = Class.forName(classname);
            Field field = obj_class.getDeclaredField(filedName);
            field.setAccessible(true);
            field.set(obj, filedVaule);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }






}
