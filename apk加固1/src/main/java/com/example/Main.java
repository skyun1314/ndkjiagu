package com.example;

import java.awt.Button;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.Panel;
import java.awt.TextField;

/**
 * Created by FYCME on 2017/6/27.
 */
public class Main {
    public static void main(String[] args) {

        PanelTest();
        //FlowLayoutTest();
    }

    //使用Frame对象
    public static void FramTest(){
        Frame f = new Frame("测试窗口");
        //设置窗口的大小,位置
        f.setBounds(30,30,400,300);
        //将窗口显示出来
        f.setVisible(true);
    }

    //使用Panel容器
    public static void PanelTest(){
        Frame frame = new Frame("你好 java GUI！我是Frame");
        //创建一个Panel容器
        Panel panel = new Panel();
        //向容器中添加两个组件
        panel.add(new TextField(20));
        panel.add(new Button("点击我"));
        panel.add(new Button("点击我"));

        //将Panel容器添加到Frame窗口中
        frame.add(panel);
        frame.setBounds(30,30,300,300);
        frame.setVisible(true);
    }

    //FlowLayout布局
    public static void FlowLayoutTest(){
        Frame frame = new Frame("测试窗口");
        frame.setLayout(new FlowLayout(FlowLayout.LEFT,20,5));
        for (int i=0;i<50;i++){
            frame.add(new Button("按钮:"+i));
        }
        //设置窗口为最佳大小
        frame.pack();
        //将窗口显示出来
        frame.setVisible(true);

    }
}


