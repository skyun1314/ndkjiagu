package com.example.nativedex;

import android.content.Context;
import android.util.Log;

import java.lang.ref.WeakReference;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Map;

import dalvik.system.DexClassLoader;

/**
 * Created by zk on 2017/6/29.
 */

public class MyDexClassLoader extends DexClassLoader{
    private static final String TAG1 ="wodelog" ;
    private static final String TAG = "wodelog";
    ClassLoader mClassLoader;
    private int mCookie;
    Context mContext;

    public  native int loadDex(byte[]bytes);

    public MyDexClassLoader(Context context,byte[] dexBytes,String dexPath, String optimizedDirectory, String librarySearchPath, ClassLoader parent) {
        super(dexPath, optimizedDirectory, librarySearchPath, parent);
       // super(null, null, null, null);
       mClassLoader=parent;
        mCookie = loadDex(dexBytes);
        mContext=context;
    }

    public static void enumMethod(String className) {

        try {
            // 1. 获取DexFile类类型
            Class clsDexFile = Class.forName(className);
            // 2. 遍历类中所有的方法
            Method[] methods = clsDexFile.getDeclaredMethods();
            for (int i = 0; i < methods.length; i++) {
                Log.d(TAG1, "==========================" );
                String methodName = methods[i].getName();
                Log.d(TAG1, "methodName: " + methodName);
                String methodReturnType = methods[i].getReturnType().getName();
                Log.d(TAG1, "methodReturnType: " + methodReturnType);
                Class ParameterTypes[] = methods[i].getParameterTypes();
                for (int j = 0; j < ParameterTypes.length; j++) {
                    String methodArg =  ParameterTypes[j].getName();
                    Log.d(TAG1, "methodArg: " + methodArg);
                }
                Log.d(TAG1, "==========================" );
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

   @Override
     public Class<?> loadClass(String className) throws ClassNotFoundException {

            if (className.equals("android.widget.ViewStub")){
                className="android.view.ViewStub";
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
                        Method method_defineClass = clsDexFile.getDeclaredMethod("defineClassNative", String.class,ClassLoader.class,int.class
                        );
                        method_defineClass.setAccessible(true);
                        // 3. 调用defineClass方法,加载类
                        clazz = (Class) method_defineClass.invoke(null,  className,mClassLoader,mCookie);
                        Log.d("wodelog", "defineClass : " + clazz);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }

            return clazz;
        }

    public ClassLoader replaceClassLoader(ClassLoader dexClassLoader, Context context){
        try {

            Class<?> aClass = Class.forName("android.app.ActivityThread");
            Object currentActivityThread = aClass.getMethod("currentActivityThread").invoke(new Object[]{});
            Field mPackages = aClass.getDeclaredField("mPackages");
            mPackages.setAccessible(true);
            Map map = (Map) mPackages.get(currentActivityThread);
            WeakReference o = (WeakReference) map.get(context.getPackageName());
            Class<?> aClass1 = Class.forName("android.app.LoadedApk");
            Field mClassLoader = aClass1.getDeclaredField("mClassLoader");
            mClassLoader.setAccessible(true);
            mClassLoader.set(o.get(),dexClassLoader);
            return dexClassLoader;
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }
public void replaceClassLoader(MyDexClassLoader MmClassLoader, Context context){
    try {
        Class<?> aClass = Class.forName("android.app.ActivityThread");
        Object currentActivityThread = aClass.getMethod("currentActivityThread").invoke(new Object[]{});
        Field mPackages = aClass.getDeclaredField("mPackages");
        mPackages.setAccessible(true);
        Map map = (Map) mPackages.get(currentActivityThread);
        WeakReference o = (WeakReference) map.get(context.getPackageName());
        Class<?> aClass1 = Class.forName("android.app.LoadedApk");
        Field mClassLoader = aClass1.getDeclaredField("mClassLoader");
        mClassLoader.setAccessible(true);
        Object loadedapk = o.get();
        Object classLoader=mClassLoader.get(loadedapk);
        Class clzBaseDexClassLoader=Class.forName("dalvik.system.BaseDexClassLoader");
        Class clzDexPathList=Class.forName("dalvik.system.DexPathList");
        Field field_pathList=clzBaseDexClassLoader.getDeclaredField("pathList");
        field_pathList.setAccessible(true);
        Object dexPathList=field_pathList.get(classLoader);
        Field field_dexElements=clzDexPathList.getDeclaredField("dexElements");
        field_dexElements.setAccessible(true);
        Class clzElement=Class.forName("dalvik.system.DexPathList$Element");
        Object dexElemennts=field_dexElements.get(dexPathList);

        //int cookie=MmClassLoader.getcookie();
        int length= Array.getLength(dexElemennts);
        System.out.println("dex元素数组大小:"+length);

        for (int i = 0; i < length; i++) {
            Object ele=Array.get(dexElemennts,i);

            try {
                Field field_dexFile=clzElement.getDeclaredField("dexFile");
                field_dexFile.setAccessible(true);
                Object dexFile=field_dexFile.get(ele);

                Class clzDexFile=Class.forName("dalvik.system.DexFile");
                Field field_mcookie=clzDexFile.getDeclaredField("mCookie");
                field_mcookie.setAccessible(true);
                field_mcookie.set(dexFile,mCookie);
            } catch (Exception e) {
                e.printStackTrace();
            }
            break;
        }

    } catch (Exception e) {
        e.printStackTrace();
    }
}

/*
    private String[] getClassNameList(int cookie) {
        return (String[]) RefInvoke.invokeStaticMethod(DexFile.class.getName(),
                "getClassNameList", new Class[] { int.class },
                new Object[] { cookie });
    }


    private Class defineClass(String name, ClassLoader loader, int cookie) {
        return (Class) RefInvoke.invokeStaticMethod(DexFile.class.getName(),
                "defineClass", new Class[] { String.class, ClassLoader.class,
                        int.class }, new Object[] { name, loader, cookie });
    }

    @Override
    protected Class<?> findClass(String name) throws ClassNotFoundException {
        Log.d(TAG, "findClass-" + name);
        Class<?> cls = null;

        String as[] = getClassNameList(mCookie);
        for (int z = 0; z < as.length; z++) {
            if (as[z].equals(name)) {
                cls = defineClass(as[z].replace('.', '/'),
                        mContext.getClassLoader(), mCookie);
            } else {
                defineClass(as[z].replace('.', '/'), mContext.getClassLoader(),
                        mCookie);
            }
        }

        if (null == cls) {
            cls = super.findClass(name);
        }

        return cls;
    }

    @Override
    protected URL findResource(String name) {
        Log.d(TAG, "findResource-" + name);
        return super.findResource(name);
    }

    @Override
    protected Enumeration<URL> findResources(String name) {
        Log.d(TAG, "findResources ssss-" + name);
        return super.findResources(name);
    }

    @Override
    protected synchronized Package getPackage(String name) {
        Log.d(TAG, "getPackage-" + name);
        return super.getPackage(name);
    }

    @Override
    protected Class<?> loadClass(String className, boolean resolve)
            throws ClassNotFoundException {
        Log.d(TAG, "loadClass-" + className + " resolve " + resolve);
        Class<?> clazz = super.loadClass(className, resolve);
        if (null == clazz) {
            Log.e(TAG, "loadClass fail,maybe get a null-point exception.");
        }
        return clazz;
    }

    @Override
    protected Package[] getPackages() {
        Log.d(TAG, "getPackages sss-");
        return super.getPackages();
    }

    @Override
    protected Package definePackage(String name, String specTitle,
                                    String specVersion, String specVendor, String implTitle,
                                    String implVersion, String implVendor, URL sealBase)
            throws IllegalArgumentException {
        Log.d(TAG, "definePackage" + name);
        return super.definePackage(name, specTitle, specVersion, specVendor,
                implTitle, implVersion, implVendor, sealBase);
    }

*/

}
