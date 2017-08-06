package com.example.nativedex;

import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.util.Log;

import java.io.File;
import java.lang.ref.WeakReference;
import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Map;

import dalvik.system.DexClassLoader;

import static com.example.nativedex.MyAppLication.getFromAssets;
import static com.example.nativedex.MyAppLication.getFromRaw;

/**
 * Created by zk on 2017/6/29.
 */

public class MyDexClassLoader extends DexClassLoader {
    private static final String TAG1 = "wodelog";
    ClassLoader mClassLoader;
    private int mCookie;
    Context mContext;
    static Context mContext1;
    private String TAG = "wodelog";

    public static native int loadDex(byte[] bytes,Context context);
    public static native int loadDex1(byte[] bytes,Context context);

    public MyDexClassLoader(Context context, byte[] dexBytes, String dexPath, String optimizedDirectory, String librarySearchPath, ClassLoader parent) {
        super(dexPath, optimizedDirectory, librarySearchPath, parent);
        mClassLoader = parent;
        mCookie = loadDex(dexBytes,context);
        mContext = context;
        mContext1 = context;


    }

    public static void enumMethod(String className) {

        try {
            // 1. 获取DexFile类类型
            Class clsDexFile = Class.forName(className);
            // 2. 遍历类中所有的方法
            Method[] methods = clsDexFile.getDeclaredMethods();
            for (int i = 0; i < methods.length; i++) {
                Log.d(TAG1, "==========================");
                String methodName = methods[i].getName();
                Log.d(TAG1, "methodName: " + methodName);
                String methodReturnType = methods[i].getReturnType().getName();
                Log.d(TAG1, "methodReturnType: " + methodReturnType);
                Class ParameterTypes[] = methods[i].getParameterTypes();
                for (int j = 0; j < ParameterTypes.length; j++) {
                    String methodArg = ParameterTypes[j].getName();
                    Log.d(TAG1, "methodArg: " + methodArg);
                }
                Log.d(TAG1, "==========================");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    public static void enumConstructors(Class userClass) {

        //---- 获取所有构造方法
        Constructor[] cons = userClass.getDeclaredConstructors();
        System.out.println("类User的构造方法包括:  ");
        //---- 打印出构造方法的前缀
        for (int i = 0; i < cons.length; i++) {
            Constructor con = cons[i];    //取出第i个构造方法。
            System.out.print(Modifier.toString(con.getModifiers()));
            //---- 打印该构造方法的前缀修饰符
            System.out.print(" " + con.getName() + "(");  //打印该构造方法的名字
            //---- 打印该构造方法的参数。
            Class[] parameterTypes = con.getParameterTypes();    //构造方法参数集但是 数组类型显示特殊
            for (int j = 0; j < parameterTypes.length; j++) {
                System.out.print(parameterTypes[j].getName() + ",");
            }
            System.out.println(")");
        }

    }


    public static void enumFields(Class userClass, Object duixiang) {

        Field[] declaredFields = userClass.getDeclaredFields();

        for (int i = 0; i < declaredFields.length; i++) {
            Field con = declaredFields[i];    //取出第i个构造方法。
            System.out.print(Modifier.toString(con.getModifiers()));
            //---- 打印该构造方法的前缀修饰符
            System.out.print(" " + con.getName() + "(");  //打印该构造方法的名字

            con.setAccessible(true);
            if (con.getType() == String.class) {
                try {
                    String value = (String) con.get(duixiang);
                    System.out.print(value);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                }
            }

            System.out.println(")");
        }
    }

    @Override
    public Class<?> loadClass(String className) throws ClassNotFoundException {

        if (className.equals("android.widget.ViewStub")) {
            className = "android.view.ViewStub";
        }

        Class<?> clazz = findLoadedClass(className);
        if (clazz == null) {
            try {
                clazz = mClassLoader.loadClass(className);
            } catch (ClassNotFoundException e) {
                // Don't want to see this.
            }

            if (clazz == null) {
                try {
                    // 1. 获取DexFile类类型
                    Class clsDexFile = Class.forName("dalvik.system.DexFile");
                    // 2. 获取defineClass方法

                    enumMethod("dalvik.system.DexFile");

                    Method method_defineClass = clsDexFile.getDeclaredMethod("defineClassNative", String.class, ClassLoader.class, long.class );
                    method_defineClass.setAccessible(true);
                    // 3. 调用defineClass方法,加载类
                    clazz = (Class) method_defineClass.invoke(null, className, mClassLoader, mCookie);
                    Log.d("wodelog", "defineClass : " + clazz);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }

        return clazz;
    }

    public static void replaceClassLoader(Context context,byte[] dexBytes) {
        try {
            int i1 = loadDex(dexBytes,context);

            Class<?> aClass = Class.forName("android.app.ActivityThread");
            Class<?> aClass1 = Class.forName("android.app.LoadedApk");
            String ResPath = context.getFilesDir() + File.separator + "res.zip";

            Object currentActivityThread = aClass.getMethod("currentActivityThread").invoke(new Object[]{});
            Field mPackages = aClass.getDeclaredField("mPackages");
            mPackages.setAccessible(true);
            Map map = (Map) mPackages.get(currentActivityThread);
            WeakReference o = (WeakReference) map.get(context.getPackageName());
            Object loadedapk = o.get();
            Field mClassLoader = aClass1.getDeclaredField("mClassLoader");
            mClassLoader.setAccessible(true);


            Field mResDir = aClass1.getDeclaredField("mResDir");
            Field mResources = aClass1.getDeclaredField("mResources");
            mResDir.setAccessible(true);
            mResources.setAccessible(true);
            String mResDir_str = (String) mResDir.get(loadedapk);
            Resources mResources_str = (Resources) mResources.get(loadedapk);


            mResDir.set(loadedapk, ResPath);
            mResources.set(loadedapk, loadResources(ResPath, context));

            Object classLoader = mClassLoader.get(loadedapk);
            Class clzBaseDexClassLoader = Class.forName("dalvik.system.BaseDexClassLoader");
            Class clzDexPathList = Class.forName("dalvik.system.DexPathList");
            Field field_pathList = clzBaseDexClassLoader.getDeclaredField("pathList");
            field_pathList.setAccessible(true);
            Object dexPathList = field_pathList.get(classLoader);
            Field field_dexElements = clzDexPathList.getDeclaredField("dexElements");
            field_dexElements.setAccessible(true);
            Class clzElement = Class.forName("dalvik.system.DexPathList$Element");
            Object dexElemennts = field_dexElements.get(dexPathList);

            //int cookie=MmClassLoader.getcookie();
            int length = Array.getLength(dexElemennts);

            for (int i = 0; i < length; i++) {
                Object ele = Array.get(dexElemennts, i);

                try {
                    Field field_dexFile = clzElement.getDeclaredField("dexFile");
                    field_dexFile.setAccessible(true);
                    Object dexFile = field_dexFile.get(ele);

                    Class clzDexFile = Class.forName("dalvik.system.DexFile");
                    Field field_mcookie = clzDexFile.getDeclaredField("mCookie");
                    field_mcookie.setAccessible(true);
                    field_mcookie.set(dexFile, i1);



                } catch (Exception e) {
                    e.printStackTrace();
                }
                break;
            }


        } catch (Exception e) {
            e.printStackTrace();
        }
    }


   public void replaceClassLoader(MyDexClassLoader MmClassLoader, Context context) {
        try {
            Class<?> aClass = Class.forName("android.app.ActivityThread");
            Class<?> aClass1 = Class.forName("android.app.LoadedApk");
            String ResPath = context.getFilesDir() + File.separator + "res.zip";

            Object currentActivityThread = aClass.getMethod("currentActivityThread").invoke(new Object[]{});


            Field mPackages = aClass.getDeclaredField("mPackages");
            mPackages.setAccessible(true);
            Map map = (Map) mPackages.get(currentActivityThread);
            WeakReference o = (WeakReference) map.get(context.getPackageName());
            Object loadedapk = o.get();
            Field mClassLoader = aClass1.getDeclaredField("mClassLoader");
            mClassLoader.setAccessible(true);


            Field mResDir = aClass1.getDeclaredField("mResDir");
            Field mResources = aClass1.getDeclaredField("mResources");
            mResDir.setAccessible(true);
            mResources.setAccessible(true);
            String mResDir_str = (String) mResDir.get(loadedapk);
            Resources mResources_str = (Resources) mResources.get(loadedapk);


            mResDir.set(loadedapk, ResPath);
            mResources.set(loadedapk, loadResources(ResPath, context));

            Object classLoader = mClassLoader.get(loadedapk);
            Class clzBaseDexClassLoader = Class.forName("dalvik.system.BaseDexClassLoader");
            Class clzDexPathList = Class.forName("dalvik.system.DexPathList");
            Field field_pathList = clzBaseDexClassLoader.getDeclaredField("pathList");
            field_pathList.setAccessible(true);
            Object dexPathList = field_pathList.get(classLoader);
            Field field_dexElements = clzDexPathList.getDeclaredField("dexElements");
            field_dexElements.setAccessible(true);
            Class clzElement = Class.forName("dalvik.system.DexPathList$Element");
            Object dexElemennts = field_dexElements.get(dexPathList);

            //int cookie=MmClassLoader.getcookie();
            int length = Array.getLength(dexElemennts);

            for (int i = 0; i < length; i++) {
                Object ele = Array.get(dexElemennts, i);

                try {
                    Field field_dexFile = clzElement.getDeclaredField("dexFile");
                    field_dexFile.setAccessible(true);
                    Object dexFile = field_dexFile.get(ele);

                    Class clzDexFile = Class.forName("dalvik.system.DexFile");
                    Field field_mcookie = clzDexFile.getDeclaredField("mCookie");
                    field_mcookie.setAccessible(true);
                    field_mcookie.set(dexFile, mCookie);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                break;
            }


        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    public static Resources loadResources(String resPath, Context context) {
        AssetManager assetManager = null;
        try {
            assetManager = AssetManager.class.newInstance();
            Method addAssetPath = assetManager.getClass().getMethod("addAssetPath", String.class);
            addAssetPath.invoke(assetManager, resPath);
        } catch (Exception e) {
            e.printStackTrace();
        }
        Resources mResources = new Resources(assetManager, context.getResources().getDisplayMetrics(), context.getResources().getConfiguration());

       // Resources.Theme mTheme = mResources.newTheme();
        //mTheme.setTo(context.getTheme());
        int raw = mResources.getIdentifier("activity_main", "raw", "com.example.dexx");
        Log.e("wodelog", "getFromRaw:" + getFromRaw(raw, mResources));
        Log.e("wodelog", "-----------------------------");
        Log.e("wodelog", "getFromAssets:" + getFromAssets("activity_main.xml", context, mResources));
        Log.e("wodelog", "-----------------------------");


        return mResources;
    }

}
