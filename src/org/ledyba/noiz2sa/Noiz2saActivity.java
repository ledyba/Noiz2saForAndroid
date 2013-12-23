package org.ledyba.noiz2sa;

import java.io.File;

import org.libsdl.app.SDLActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.widget.FrameLayout;

public class Noiz2saActivity extends SDLActivity {
	
	private final static String TAG="Noiz2sa";

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		return super.onTouchEvent(event);
	}
	
	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		Log.d(TAG, "type: "+event.getKeyCode());
		if(event.getRepeatCount() == 0){
			if(event.getAction() == KeyEvent.ACTION_DOWN) {
				Helper.onKeyDown(event.getKeyCode());
			}else if(event.getAction() == KeyEvent.ACTION_UP){
				Helper.onKeyUp(event.getKeyCode());
			}
		}
		return super.dispatchKeyEvent(event);
	}

	@Override
	public boolean onGenericMotionEvent(MotionEvent event)
	{
		if(event.getSource() == InputDevice.SOURCE_GAMEPAD || event.getSource() == InputDevice.SOURCE_JOYSTICK){
			Helper.onMotion(event);
		}
		return super.onGenericMotionEvent(event);
	}

	private static Noiz2saActivity self = null;
	public static Noiz2saActivity getInstance(){
		return self;
	}
	public static String getPrefPath(){
		return new File(getInstance().getFilesDir(), "pref").getAbsolutePath();
	}
	public static float getScale(){
		if( self == null ) {
			Log.e(TAG, "Oops. Window Not initialized.");
			return 1;
		}else{
			Log.d(TAG, "Window scale (Java): "+self.scale_);
			return self.scale_;
		}
	}
	private float scale_;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		self = this;
		final Display disp = getWindowManager().getDefaultDisplay();
		this.scale_ = Math.min(disp.getWidth()/320.f, disp.getHeight()/480.f);
		
		final int w = (int)(320*this.scale_);
		final int h = (int)(480*this.scale_);
		
		final FrameLayout fm = (FrameLayout)this.findViewById(android.R.id.content);
		final ViewGroup abs = (ViewGroup)fm.getChildAt(0);
		final SurfaceView surf = (SurfaceView)abs.getChildAt(0);
		surf.getHolder().setFixedSize(320, 480);
		abs.removeAllViews();
		fm.removeAllViews();
		fm.addView(surf, new FrameLayout.LayoutParams(w,h,Gravity.CENTER));
		Log.d(TAG, "Fixed Size: " + w+"x"+h);
	}

	@Override
	public boolean onKeyMultiple(int keyCode, int repeatCount, KeyEvent event) {
		return super.onKeyMultiple(keyCode, repeatCount, event);
	}

	@Override
	protected void onPause() {
		super.finish();
		super.onPause();
	}
	
	@Override
	protected void onResume() {
		super.onResume();
	}

}
