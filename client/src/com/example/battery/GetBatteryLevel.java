package com.example.battery;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class GetBatteryLevel {
	private int battery = 50;

	//private int GetBatteryLevel(){
		//int battery = 0;
		BroadcastReceiver mBatteryInfoReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                    String action = intent.getAction();
                    if (Intent.ACTION_BATTERY_CHANGED.equals(action)) {

                            int level = intent.getIntExtra("level", 0);
                            int scale = intent.getIntExtra("scale", 100);
                            battery = level * 100 / scale;
//                            tvBatteryLevel.setText("Battery level: "
//                                            + String.valueOf(level * 100 / scale) + "%");
                    }
            }
    };
	//	return 0;
		
	//}
}
