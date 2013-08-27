package org.ledyba.noiz2sa;

import org.libsdl.app.SDLActivity;

import android.content.res.AssetManager;

public class Helper {

	private Helper() {
	}
	
	static {
		System.loadLibrary("main");
	}
	static public void init(){
		onLoadJNI();
	}
	private native static void onLoadJNI();
	
	public static AssetManager getAssetManager(){
		return SDLActivity.getContext().getAssets();
	}

}
