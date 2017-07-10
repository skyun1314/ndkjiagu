package com.example;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.datatransfer.DataFlavor;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DropTarget;
import java.awt.dnd.DropTargetAdapter;
import java.awt.dnd.DropTargetDropEvent;
import java.io.File;
import java.util.List;

import javax.swing.DefaultListModel;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.border.EmptyBorder;

//问题: 当滑动滑块时 JTextArea不随着变大
public class 界面 {

    private static JLabel jLabel;
    private static JScrollPane jScrollPane;
    private static JTextArea mJTextArea;
    private static  DefaultListModel DefaultListModel1;
    public static void TextViewSHow(String msg){

        if (jScrollPane==null)
            return;

        JScrollBar bar=jScrollPane.getVerticalScrollBar();
        bar.setValue(bar.getMaximum());
        mJTextArea.append(msg +"\n");
    }
    public static void main(String[] args) {
        JFrame jFrame = new JFrame("jiagu-0.1");
        jFrame.setSize(800, 500);
        jFrame. setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        jFrame.setLocationRelativeTo(null);     //null 表示屏幕居中
        //1.设置滑块容器
         jScrollPane = new JScrollPane();

        //显示发送的信息的控件
        //用于显示聊天信息
        //创建一个能插入多行的文本的控件对象
        mJTextArea = new JTextArea();
        mJTextArea.setEditable(false);
        //将多行文本添加入滑块容器中
        jScrollPane.setViewportView(mJTextArea);

        //2.容器中保存3个控件(标签,单行文本框,按钮)
        JPanel jPaneSend = new JPanel(new GridLayout(2,0));
        //设置边距(该控件距离主窗口边距的距离)
        jPaneSend.setBorder(new EmptyBorder(5,5,5,5));


         jLabel = new JLabel("拖动文件到此区域");   //创建标签
        jPaneSend.add(jLabel);

        JList List = new JList();

         DefaultListModel1=new DefaultListModel();
        List.setModel(DefaultListModel1);
        jPaneSend.add(List);


        jFrame.add(jScrollPane,BorderLayout.CENTER);
        jFrame.add(jPaneSend,BorderLayout.WEST);
        jFrame.setVisible(true);
        drag(jFrame);
    }

    static String absolutePath;
    public static void drag(JFrame jFrame)//定义的拖拽方法
    {
        //panel表示要接受拖拽的控件
        new DropTarget(jFrame, DnDConstants.ACTION_COPY_OR_MOVE, new DropTargetAdapter()
        {
            @Override
            public void drop(DropTargetDropEvent dtde)//重写适配器的drop方法
            {
                try
                {

                    if (dtde.isDataFlavorSupported(DataFlavor.javaFileListFlavor))//如果拖入的文件格式受支持
                    {
                        dtde.acceptDrop(DnDConstants.ACTION_COPY_OR_MOVE);//接收拖拽来的数据
                        final List<File> list = (List<File>) dtde.getTransferable().getTransferData(DataFlavor.javaFileListFlavor);
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

                                    DefaultListModel1.addElement(list.get(0).getName());

                                    mJTextArea.setText("");
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
