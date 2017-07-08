package com.example;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.datatransfer.DataFlavor;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DropTarget;
import java.awt.dnd.DropTargetAdapter;
import java.awt.dnd.DropTargetDropEvent;
import java.io.File;
import java.util.List;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

/**
 * 最简单的Java拖拽代码示例
 * @author 刘显安
 * 2013年1月24日
 */
//




public class DragTest extends JFrame
{

    public static JTextArea mJTextArea;
    public static void TextViewSHow(String msg){
        mJTextArea.append(msg +"\n");
    }

    JLabel jLabel = new JLabel("把需要加固的apk拖入到窗口");   //创建标签
    JPanel panel;//要接受拖拽的面板
    public DragTest()
    {
        panel = new JPanel();

        //panel.setBackground(Color.YELLOW);
        getContentPane().add(panel, BorderLayout.CENTER);


       // getContentPane().add(jLabel, BorderLayout.CENTER);
        panel.add(jLabel);
        setSize(800, 500);
        mJTextArea.setEditable(false);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLocationRelativeTo(null);
        setTitle("jiagu-0.1");
        JScrollPane jScrollPane = new JScrollPane();
        //创建一个能插入多行的文本的控件对象
         mJTextArea = new JTextArea();
        //将多行文本添加入滑块容器中
        jScrollPane.setViewportView(mJTextArea);

        jScrollPane.setPreferredSize(new Dimension(200,200));

        panel.add(jScrollPane,BorderLayout.CENTER);

        drag();//启用拖拽

    }
    public static void main(String[] args) throws Exception
    {




       // UIManager.setLookAndFeel("com.sun.java.swing.plaf.nimbus.NimbusLookAndFeel");//设置皮肤
        new DragTest().setVisible(true);;
    }
    String absolutePath;
    public void drag()//定义的拖拽方法
    {
        //panel表示要接受拖拽的控件
        new DropTarget(panel, DnDConstants.ACTION_COPY_OR_MOVE, new DropTargetAdapter()
        {
            @Override
            public void drop(DropTargetDropEvent dtde)//重写适配器的drop方法
            {
                try
                {

                    if (dtde.isDataFlavorSupported(DataFlavor.javaFileListFlavor))//如果拖入的文件格式受支持
                    {
                        dtde.acceptDrop(DnDConstants.ACTION_COPY_OR_MOVE);//接收拖拽来的数据
                        List<File> list = (List<File>) dtde.getTransferable().getTransferData(DataFlavor.javaFileListFlavor);
                        String temp="";
                        for(File file:list)
                            temp+=file.getAbsolutePath();
                        //JOptionPane.showMessageDialog(null, temp);
                        int isDelete = JOptionPane.showConfirmDialog(null,temp,"是否加固如下文件",
                                JOptionPane.OK_OPTION);


                        if (isDelete==JOptionPane.OK_OPTION){
                            absolutePath = list.get(0).getAbsolutePath();
                            System.out.println("File:"+absolutePath);


                            new Thread(new Runnable() {
                                @Override
                                public void run() {
                                    MyClass.jiagu(absolutePath);
                                }
                            }).start();;


                        }



                        dtde.dropComplete(true);//指示拖拽操作已完成
                    }
                    else
                    {
                        dtde.rejectDrop();//否则拒绝拖拽来的数据
                    }
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
            }
        });
    }
}