package org.ledyba.noiz2sa;

import org.libsdl.app.SDLActivity;

import android.annotation.SuppressLint;
import android.os.Build;
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
	
	@SuppressLint("NewApi")
	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
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
		final int src = event.getDevice().getSources();
		if(((src & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD)) {
			Helper.onMotion(event);
		}
		return super.onGenericMotionEvent(event);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		final Display disp = getWindowManager().getDefaultDisplay();
		final float scale = Math.min(disp.getWidth()/320.f, disp.getHeight()/480.f);
		final int w = (int)(320*scale);
		final int h = (int)(480*scale);
		
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
