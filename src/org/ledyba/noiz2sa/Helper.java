package org.ledyba.noiz2sa;

import org.libsdl.app.SDLActivity;

import android.content.res.AssetManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class Helper {

	private static Helper ins_;
	private static Helper getInstance(){
		if(ins_ == null){
			ins_ = new Helper();
		}
		return ins_;
	}
	private float x_;
	private float y_;
	private int pressedA;
	private int pressedB;
	private int pressedStart;
	private int pressedSelect;
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
	private void onMotionI(MotionEvent ev){
		if(ev.getAction()==MotionEvent.ACTION_MOVE){
			x_ = ev.getX();
			y_ = ev.getY();
		}
	}
	public static void onMotion(MotionEvent ev){
		Log.e("Helper", "motion: "+ev.toString());
		getInstance().onMotionI(ev);
	}
	private void onKeyUpI(int keyCode){
		switch(keyCode){
		case KeyEvent.KEYCODE_BUTTON_A:
			this.pressedA--;
			break;
		case KeyEvent.KEYCODE_BUTTON_B:
			this.pressedB--;
			break;
		case KeyEvent.KEYCODE_BUTTON_START:
			this.pressedStart--;
			break;
		case KeyEvent.KEYCODE_BUTTON_SELECT:
			this.pressedSelect--;
			break;
		}
	}
	public void onKeyDownI(int keyCode)
	{
		switch(keyCode){
		case KeyEvent.KEYCODE_BUTTON_A:
			this.pressedA++;
			break;
		case KeyEvent.KEYCODE_BUTTON_B:
			this.pressedB++;
			break;
		case KeyEvent.KEYCODE_BUTTON_START:
			this.pressedStart++;
			break;
		case KeyEvent.KEYCODE_BUTTON_SELECT:
			this.pressedSelect++;
			break;
		}
	}
	public static void onKeyUp(int keyCode){
		getInstance().onKeyUpI(keyCode);
	}
	public static void onKeyDown(int keyCode){
		getInstance().onKeyDownI(keyCode);
	}
	public static float getX()
	{
		return getInstance().x_;
	}
	public static float getY()
	{
		return getInstance().y_;
	}
	public static int keyAPressed(){
		return getInstance().pressedA;
	}
	public static int keyBPressed(){
		return getInstance().pressedB;
	}
	public static int keyStartPressed(){
		return getInstance().pressedStart;
	}
	public static int keySelectPressed(){
		return getInstance().pressedSelect;
	}

}
