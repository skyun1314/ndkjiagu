package com.example.nativedex;

import android.content.Context;
import android.content.res.AssetManager;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by zk on 2017/6/29.
 */

public class FileUtil {

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
