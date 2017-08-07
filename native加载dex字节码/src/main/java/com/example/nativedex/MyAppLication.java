package com.example.nativedex;

import android.annotation.TargetApi;
import android.app.Application;
import android.app.Instrumentation;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import dalvik.system.DexFile;


/**
 * Created by zk on 2017/6/30.
 */

public class MyAppLication extends Application {

    static {
        System.loadLibrary("zkjg-lib");
    }

    //直接返回数据，读者可以添加自己加密方法
    private static byte[] decrpt(byte[] srcdata) {
        for (int i = 0; i < srcdata.length; i++) {
            srcdata[i] = (byte) (0xFF ^ srcdata[i]);
        }
        return srcdata;
    }

    public native void  attachBaseContext(Context base);


/*

    @TargetApi(Build.VERSION_CODES.KITKAT)
    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);

      */
/*  byte[] bytes1 = readDexFileFromApk();



        byte[] dexByte;
        String  dexPath = null;
        String  dexPath_jia = null;
        byte[] assets_byte=null;
        if (getPackageName().equals("com.example.nativedex")) {
            dexByte = FileUtil.copyDexToByte("classes.dex", this);
            dexPath = FileUtil.copyDex("classes.dex", this);
            dexPath_jia = FileUtil.copyDex("classesjia.dex", this);


            FileUtil.copyDex("res.zip", this);


        } else {
            byte[] size = getSizeByoff(bytes1, 4, bytes1.length - 4);
            int dexSize = bytesToInt(size);
            dexByte = getSizeByoff(bytes1, dexSize, bytes1.length - 4 - dexSize);
            dexByte = decrpt(dexByte);
            dexPath= getPackageResourcePath();
            byte[] assets_size = getSizeByoff(bytes1, 4, bytes1.length - 4 - dexSize - 4);
            int assets_dexSize = bytesToInt(assets_size);
            assets_byte = getSizeByoff(bytes1, assets_dexSize, bytes1.length - 4 - dexSize - 4 - assets_dexSize);
            assets_byte = decrpt(assets_byte);
            try {
                File f = new File(getFilesDir() + File.separator + "res.zip");
                FileOutputStream  fos = new FileOutputStream(f);
                BufferedOutputStream    bos = new BufferedOutputStream(fos);
                bos.write(assets_byte);

            } catch (Exception e) {
                e.printStackTrace();
            }
        }*//*





        Context applicationContext = this;
        MyDexClassLoader.loadDex(new byte[1024] ,applicationContext);
       // MyDexClassLoader.replaceResouce(this);



       // replacecookie(dexPath,dexByte);
            //Log.e("wodelog","\ndex:"+bytesToHexString(sizeByoff));


        // String path = FileUtil.copyDex("classes.dex", this);

    */
/*    myDexClassLoader = new MyDexClassLoader(
                this,
                dexByte,
                dexPath,// getPackageResourcePath(),
                getDir(".dex", MODE_PRIVATE).getAbsolutePath(),
                null,
                getClassLoader()
        );
        myDexClassLoader.replaceClassLoader(myDexClassLoader,this);*//*



      //  DexUtils.injectDexAtFirst(this,dexPath, getDir(".dex", MODE_PRIVATE).getAbsolutePath(),dexByte);





      // MyDexClassLoader.replaceClassLoader(applicationContext,dexByte);


      */
/*  try {
            Object newDexElements = DexUtils.getDexElements(DexUtils.getPathList(getClassLoader()));
            Object pathList = DexUtils.getPathList(getPathClassLoader());
            DexUtils.setField(pathList, pathList.getClass(), "dexElements", newDexElements);
        } catch (Exception e) {
            e.printStackTrace();
        }*//*



    }
*/

    private void replacecookie(String dexPath,byte[]dexByte) {
        try {
            DexFile dexFile=DexFile.loadDex(getPackageCodePath(),null,0);
            Field mCookie = dexFile.getClass().getDeclaredField("mCookie");
            mCookie.setAccessible(true);
           // int i1 = MyDexClassLoader.loadDex(dexByte);
            //mCookie.set(dexFile,i1);


            Object pathList = DexUtils.getPathList(this.getClassLoader());

            Object base_ClassLoader_dexElements = DexUtils.getDexElements(pathList);


            int length = Array.getLength(base_ClassLoader_dexElements);

            for (int i = 0; i < length; i++) {
                Object ele = Array.get(base_ClassLoader_dexElements, i);
                Object base_ClassLoader_dexFile=   DexUtils.getField(ele, ele.getClass(), "dexFile");
                DexUtils.setField(ele, ele.getClass(), "dexFile",dexFile);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
    //    getOldAppCation();

    }

    private byte[] readDexFileFromApk() {
        ByteArrayOutputStream dexByteArrayOutputStream = new ByteArrayOutputStream();
        ZipInputStream localZipInputStream = null;
        try {
            localZipInputStream = new ZipInputStream(
                    new BufferedInputStream(new FileInputStream(this.getApplicationInfo().sourceDir)));

            while (true) {
                ZipEntry localZipEntry = localZipInputStream.getNextEntry();
                if (localZipEntry == null) {
                    localZipInputStream.close();
                    break;
                }
                if (localZipEntry.getName().equals("classes.dex")) {
                    byte[] arrayOfByte = new byte[1024];
                    while (true) {
                        int i = localZipInputStream.read(arrayOfByte);
                        if (i == -1)
                            break;
                        dexByteArrayOutputStream.write(arrayOfByte, 0, i);
                    }
                }
                localZipInputStream.closeEntry();
            }
            localZipInputStream.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return dexByteArrayOutputStream.toByteArray();
    }


    public static byte[] getSizeByoff(byte[] DexFileBytes, int lenth, int DexFileBytesIndex) {

        byte[] bytes = new byte[lenth];
        for (int i = 0; i < lenth; i++) {
            bytes[i] = DexFileBytes[DexFileBytesIndex + i];
        }
        return bytes;

    }


    public static int byte2int(byte[] res) {


        if (res.length == 1) {
            return (res[0] & 0xff);
        } else if (res.length == 2) {
            return (res[0] & 0xff) | ((res[1] << 8) & 0xff00);
        } else if (res.length == 3) {
            return (res[0] & 0xff) | ((res[1] << 8) & 0xff00)
                    | ((res[2] << 24) >>> 8);
        } else if (res.length == 4) {
            return (res[0] & 0xff) | ((res[1] << 8) & 0xff00)
                    | ((res[2] << 24) >>> 8) | (res[3] << 24);
        }


        int targets = (res[0] & 0xff) | ((res[1] << 8) & 0xff00)
                | ((res[2] << 24) >>> 8) | (res[3] << 24);
        return targets;
    }

    public static String bytesToHexString(byte[] src) {
        //byte[] src = reverseBytes(src1);
        StringBuilder stringBuilder = new StringBuilder("");
        if (src == null || src.length <= 0) {
            return null;
        }
        for (int i = 0; i < src.length; i++) {
            int v = src[i] & 0xFF;
            String hv = Integer.toHexString(v);
            if (hv.length() < 2) {
                stringBuilder.append(0);
            }
            stringBuilder.append(hv + " ");
        }
        return stringBuilder.toString();
    }


    public static int bytesToInt(byte[] bytes) {
        int addr = 0;
        if (bytes.length == 1) {
            addr = bytes[0] & 0xFF;
        } else {
            addr = bytes[0] & 0xFF;
            addr = (addr << 8) | (bytes[1] & 0xff);
            addr = (addr << 8) | (bytes[2] & 0xff);
            addr = (addr << 8) | (bytes[3] & 0xff);
        }
        return addr;
    }


    @TargetApi(Build.VERSION_CODES.KITKAT)
    public void getOldAppCation() {
        // 如果源应用配置有Appliction对象，则替换为源应用Applicaiton，以便不影响源程序逻辑。
        String appClassName = null;

        ApplicationInfo ai = null;
        try {
            ai = this.getPackageManager()
                    .getApplicationInfo(this.getPackageName(),
                            PackageManager.GET_META_DATA);
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



    public static class DexUtils {

        public static void injectDexAtFirst(Context context,String dexPath, String defaultDexOptPath,byte[] dexBytes) {
            try {
                //MyDexClassLoader dexClassLoader = new MyDexClassLoader(context,dexBytes,dexPath,defaultDexOptPath, dexPath, context.getClassLoader());
               //dexClassLoader.replaceClassLoader(dexClassLoader, context);


                DexFile dexFile=DexFile.loadDex(dexPath,null,0);
                Field mCookie = dexFile.getClass().getDeclaredField("mCookie");
                mCookie.setAccessible(true);
                int i1 = MyDexClassLoader.loadDex(dexBytes,context);
                mCookie.set(dexFile,i1);


                Object pathList = getPathList(context.getClassLoader());

                Object base_ClassLoader_dexElements = getDexElements(pathList);



                int length = Array.getLength(base_ClassLoader_dexElements);

                for (int i = 0; i < length; i++) {
                    Object ele = Array.get(base_ClassLoader_dexElements, i);
                    Object base_ClassLoader_dexFile=   getField(ele, ele.getClass(), "dexFile");
                    setField(ele, ele.getClass(), "dexFile",dexFile);
                }




             //   setField(pathList, pathList.getClass(), "dexElements", newDexElements);

            } catch (Exception e) {
                e.printStackTrace();
            }
        }


        public static Object getDexElements(Object paramObject)
                throws IllegalArgumentException, NoSuchFieldException, IllegalAccessException {
            return getField(paramObject, paramObject.getClass(), "dexElements");
        }

        public static Object getPathList(Object baseDexClassLoader)



                throws IllegalArgumentException, NoSuchFieldException, IllegalAccessException, ClassNotFoundException {



            return getField(baseDexClassLoader, Class.forName("dalvik.system.BaseDexClassLoader"), "pathList");
        }

        public static Object combineArray(Object firstArray, Object secondArray) {
            Class<?> localClass = firstArray.getClass().getComponentType();
            int firstArrayLength = Array.getLength(firstArray);
            int allLength = firstArrayLength + Array.getLength(secondArray);
            Object result = Array.newInstance(localClass, allLength);
            for (int k = 0; k < allLength; ++k) {
                if (k < firstArrayLength) {
                    Array.set(result, k, Array.get(firstArray, k));
                } else {
                    Array.set(result, k, Array.get(secondArray, k - firstArrayLength));
                }
            }
            return result;
        }

        public static Object getField(Object obj, Class<?> cl, String field)
                throws NoSuchFieldException, IllegalArgumentException, IllegalAccessException {
            Field localField = cl.getDeclaredField(field);
            localField.setAccessible(true);
            return localField.get(obj);
        }

        public static void setField(Object obj, Class<?> cl, String field, Object value)
                throws NoSuchFieldException, IllegalArgumentException, IllegalAccessException {
            Field localField = cl.getDeclaredField(field);
            localField.setAccessible(true);
            localField.set(obj, value);
        }

    }


    public static class FileUtil {

        public static String copyDex(String dexName, Context context){
            AssetManager assetManager=context.getAssets();
            try {
                InputStream open = assetManager.open(dexName);
                String outputPath=context.getFilesDir()+ File.separator+dexName;
                FileOutputStream fileOutputStream=new FileOutputStream(outputPath);

                byte b[]=new byte[1024];
                int len=0;
                while ((len=open.read(b))!=-1){
                    fileOutputStream.write(b,0,len);
                }

                fileOutputStream.close();

                return outputPath;

            } catch (IOException e) {
                e.printStackTrace();
                return "";
            }
        }

        public static byte[] copyDexToByte(String dexName, Context context){
            AssetManager assetManager=context.getAssets();
            try {
                InputStream open = assetManager.open(dexName);
                ByteArrayOutputStream fileOutputStream=new ByteArrayOutputStream();

                byte b[]=new byte[1024];
                int len=0;
                while ((len=open.read(b))!=-1){
                    fileOutputStream.write(b,0,len);
                }
                return fileOutputStream.toByteArray();

            } catch (IOException e) {
                e.printStackTrace();
                return null;
            }
        }

    }

}
