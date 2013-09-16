package org.ledyba.noiz2sa;

import android.app.Activity;
import android.content.SharedPreferences;

class Setting {
	private final Activity activity;
	private final boolean isSoundEnabled;
	Setting(final Activity activity){
		this.activity = activity;
		final SharedPreferences sp = activity.getSharedPreferences("Noiz2sa", Activity.MODE_PRIVATE);
		this.isSoundEnabled = sp.getBoolean("isSoundEnabled", true);
	}
}

public class Pref {
	private Pref(){
		
	}
}
