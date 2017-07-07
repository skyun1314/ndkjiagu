package com.example.nativedex;

import android.annotation.TargetApi;
import android.app.Application;
import android.content.Context;
import android.os.Build;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * Created by zk on 2017/6/30.
 */

public class MyAppLication extends Application {
    public static  MyDexClassLoader myDexClassLoader;
    static {
        System.loadLibrary("native-lib");
    }



    @TargetApi(Build.VERSION_CODES.KITKAT)
    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);


        byte[] bytes1 = readDexFileFromApk();

        byte[] size = getSizeByoff(bytes1, 4, bytes1.length - 4);


        int dexSize = bytesToInt(size);

        Log.e("wodelog","dexSize:"+dexSize);

        byte[] sizeByoff = getSizeByoff(bytes1, dexSize, bytes1.length - 4 - dexSize);

        for (int i = 0; i < sizeByoff.length; i++) {
         //   sizeByoff[i]^=0xFF;
        }
        
        
        //Log.e("wodelog","\ndex:"+bytesToHexString(sizeByoff));


      //  byte[] sizeByoff1 = FileUtil.copyDexToByte("classes.dex", this);
       // String path = FileUtil.copyDex("classes.dex", this);


     /*   for (int i = 0; i < sizeByoff.length; i++) {
            if (sizeByoff[i]!=sizeByoff1[i]){
                Log.e("wodelog","字节码内容是否相等："+i+" : "+ sizeByoff[i]+"---"+sizeByoff1[i]);
            }

        }
        Log.e("wodelog","拷贝的dex大小"+sizeByoff1.length);

        boolean equals = Arrays.equals(sizeByoff, sizeByoff1);
        Log.e("wodelog","字节码内容是否相等："+equals);
        */
        Log.e("wodelog","读取的dex大小"+sizeByoff.length);
        myDexClassLoader = new MyDexClassLoader(
                this,
                sizeByoff,
                getPackageResourcePath(),
                getDir(".dex", MODE_PRIVATE).getAbsolutePath(),
                null,
                getClassLoader()
        );
       // myDexClassLoader.replaceClassLoader(myDexClassLoader,this);
        myDexClassLoader.replaceClassLoader(myDexClassLoader,this);

        /*try {
            Class obj_class  =myDexClassLoader.loadClass("com.example.nativedex.MyTestActivity");

            Intent intent=new Intent(this,obj_class);
            startActivity(intent);

        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
*/    }


    private byte[] readDexFileFromApk(){
        ByteArrayOutputStream dexByteArrayOutputStream = new ByteArrayOutputStream();
        ZipInputStream localZipInputStream = null;
        try {
            localZipInputStream = new ZipInputStream(
                    new BufferedInputStream(new FileInputStream( this.getApplicationInfo().sourceDir)));

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


    public static byte[] getSizeByoff(byte[]DexFileBytes, int lenth,int DexFileBytesIndex) {

        byte[] bytes = new byte[lenth];
        for (int i = 0; i < lenth; i++) {
            bytes[i] = DexFileBytes[DexFileBytesIndex+ i];
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


    public  static int bytesToInt(byte[] bytes) {
        int addr=0;
        if(bytes.length==1){
            addr = bytes[0] & 0xFF;
        }else{
            addr = bytes[0] & 0xFF;
            addr = (addr << 8) | (bytes[1] & 0xff) ;
            addr = (addr << 8) | (bytes[2] & 0xff) ;
            addr = (addr << 8) | (bytes[3] & 0xff) ;
        }
        return addr;
    }

}
