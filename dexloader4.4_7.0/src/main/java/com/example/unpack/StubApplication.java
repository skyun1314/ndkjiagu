package com.example.unpack;

import android.app.Application;
import android.content.Context;

public class StubApplication extends Application {
	 static{
	        System.loadLibrary("DexPacker");
	        String a;
	    }

    public StubApplication() {    	
        super();     
    }
    
	public  native  void onCreate() ;
	
	public native void  attachBaseContext(Context base);
	
}
