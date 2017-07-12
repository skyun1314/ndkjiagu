package com.example;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * Created by zk on 2017/7/1.
 */

public class CMDUtil {


    public static String runCMDDir="";

    public static  void getDir(String strNmae){
        String[] split = strNmae.split("/");
        int length = split[split.length - 1].length();

        runCMDDir = strNmae.substring(0, strNmae.length()-length);

    }

    public static  void setDir(String strNmae){

        runCMDDir = strNmae;

    }



    public static void runCMD(String cmd){
        try {
            Process process=Runtime.getRuntime().exec(cmd,null,new File(runCMDDir));
            BufferedReader br=new BufferedReader(new InputStreamReader(process.getInputStream()));
            String line=null;
            while ((line=br.readLine())!=null){
                System.out.println(line);
                Main_JiaGu.TextViewSHow(line);
            }

            if (br==null){
                br.close();;
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void runCMD(String cmd,String dir){
      //  cmd="cmd /c "+cmd;
        try {
            Process process=Runtime.getRuntime().exec(cmd,null,new File(dir));
            BufferedReader br=new BufferedReader(new InputStreamReader(process.getInputStream()));
            String line=null;
            while ((line=br.readLine())!=null){
                System.out.println(line);
            }

            if (br==null){
                br.close();;
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }



}
