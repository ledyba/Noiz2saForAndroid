package org.ledyba.noiz2sa;

import org.libsdl.app.SDLActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class Noiz2saActivity extends SDLActivity {
	
	private final static String TAG="Noiz2sa";

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		return super.onTouchEvent(event);
	}
	
	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		if(KeyEvent.isGamepadButton(event.getKeyCode()) && event.getRepeatCount() == 0){
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
	}

	@Override
	public boolean onKeyMultiple(int keyCode, int repeatCount, KeyEvent event) {
		return super.onKeyMultiple(keyCode, repeatCount, event);
	}

}
