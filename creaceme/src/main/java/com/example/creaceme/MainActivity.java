package com.example.creaceme;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    static{

        System.loadLibrary("native-lib");
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button button= (Button) findViewById(R.id.b1);
        final EditText editText1= (EditText) findViewById(R.id.e1);
        final EditText editText2= (EditText) findViewById(R.id.e2);




        String str="15pb";

        System.out.println((0x31^0x59)+"--------------");
        System.out.println((0x35^0x50)+"--------------");
        System.out.println((0x70^0x18)+"--------------");
        System.out.println((0x62^0x07)+"--------------");



       /* 59 50 18 07
        31 35 70 62*/

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {


                Toast.makeText(MainActivity.this,add(editText1.getText().toString(),editText2.getText().toString()),Toast.LENGTH_LONG).show();
            }
        });



      //  add();
    }


    public native String add(String str1,String str2);
}
