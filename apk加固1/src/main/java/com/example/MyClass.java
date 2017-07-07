package com.example;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.SAXReader;
import org.dom4j.io.XMLWriter;

import java.io.File;
import java.io.FileWriter;

public class MyClass {

    public static void main(String[] args) {

        String strNmae = "/Users/zk/Documents/andriod/Development/workspace/ndk练习/制作dexx文件/build/outputs/apk/制作dexx文件-debug.apk";
        String path = strNmae.substring(0, strNmae.length() - 4);
        System.out.println("反编译开始");
        CMDUtil.getDir(strNmae);
        CMDUtil.runCMD("apktool d -f -s  " + strNmae);
        System.out.println("反编译结束");


        CMDUtil.setDir(path);
       // CMDUtil.runCMD("mkdir assets");
        //CMDUtil.runCMD("cp classes.dex assets/");
        //CMDUtil.runCMD("cp -r /Users/zk/Desktop/netive_dex/smali " + path);
       DexUtil.DexWith(path+"/classes.dex",path+"/classes.dex");
        CMDUtil.runCMD("cp -r /Users/zk/Desktop/netive_dex/lib " + path);
        writeManifest(path);
        CMDUtil.getDir(strNmae);
        CMDUtil.runCMD("apktool b   " + path);
        String signs = "jarsigner -verbose -keystore /Users/zk/Desktop/kai.keystore -storepass 111111 -keypass 111111 -signedjar  " + path + "signed.apk " + path + "/dist/制作dexx文件-debug.apk " + "kai.keystore -digestalg SHA1 -sigalg MD5withRSA";
        CMDUtil.runCMD(signs);
    }

    public static void writeManifest(String file) {
        file = file + "/" + "AndroidManifest.xml";
        SAXReader reader = new SAXReader();
        try {
            Document document = reader.read(new File(file));

            Element root = document.getRootElement();

            Element application3 = root.element("application");

            String name = application3.attributeValue("name");
            if (name == null) {
                application3.addAttribute("android:name", "com.example.nativedex.MyAppLication");


                XMLWriter writer = new XMLWriter(new FileWriter(new File(file)));


                writer.write(document);
                writer.close();

            }


        } catch (Exception e) {
            e.printStackTrace();
        }
    }


}
