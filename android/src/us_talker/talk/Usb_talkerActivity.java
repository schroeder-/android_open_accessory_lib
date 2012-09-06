/*
    Usb Talker: a demo Application for use of my LibUsb code.
    Copyright (C) 2012  Alexander Schrode (Midix01[at]googlemail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

package us_talker.talk;


//import com.android.future.usb.UsbManager;
//import com.android.future.usb.UsbAccessory;
import android.hardware.usb.UsbAccessory;
import android.hardware.usb.UsbManager;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileDescriptor;
import java.io.IOException;



import android.app.Activity;
import android.app.PendingIntent;
import android.app.AlertDialog;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.widget.Toast;
import android.widget.TextView;
import android.util.Log;

public class Usb_talkerActivity extends Activity  implements SensorEventListener{






	private String TAG = "USB_Talker";
	private UsbManager mUsbManager;
	private PendingIntent mPermissionIntent;

	private UsbAccessory mAccessory;
	ParcelFileDescriptor mFileDescriptor;
	FileInputStream mInputStream;
	FileOutputStream mOutputStream;
	private TextView tv;
	private Thread readTh;
	private boolean avalible = false;
	private SensorManager mSensorManager;
	private Sensor sens;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent intent = getIntent(); 
		mSensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		sens = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
		//mUsbManager = UsbManager.getInstance(this);
		mPermissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);
		IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
		filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
		registerReceiver(mUsbReceiver, filter);
		mAccessory = (UsbAccessory) intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
		//mAccessory = UsbManager.getAccessory(intent);
		setContentView(R.layout.main);
		tv = (TextView)findViewById(R.id.textView2);
		mUsbManager.requestPermission(mAccessory, mPermissionIntent);
	}

	@Override
	protected void onPause() {
		super.onPause();
		mSensorManager.unregisterListener(this);
	}

	@Override
	protected void onResume() {
		super.onResume();
		mSensorManager.registerListener(this, sens, SensorManager.SENSOR_DELAY_NORMAL);
	}


	private void openAccessory() {
		Log.d(TAG, "openAccessory: " + mAccessory);
		mFileDescriptor = mUsbManager.openAccessory(mAccessory);
		avalible = true;
		if (mFileDescriptor != null) { 
			FileDescriptor fd = mFileDescriptor.getFileDescriptor();
			mInputStream = new FileInputStream(fd);
			mOutputStream = new FileOutputStream(fd);
			avalible = true;
			readTh = new Thread(){
				@Override
				public void run(){
					final byte[] buffer = new byte[512];
					while(true){
						try {
							if(mInputStream.available() > 0){
								mInputStream.read(buffer, 0, 512);
								tv.post(new Runnable(){
									private byte[] buf = buffer.clone();
									public void run() {
										tv.append(new String(buf));
									}});
							}
							else{
								Thread.sleep(50);
							}
						} catch (IOException e) {
							e.printStackTrace();
							return;
						} catch (InterruptedException e) {
							e.printStackTrace();
							return;
						}
					}
				}
			};
		}
	}
	private void closeAccessory()
	{
		readTh.interrupt();
		try {
			Thread.sleep(50);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
		try
		{
			if(mFileDescriptor != null)
			{
				mFileDescriptor.close();
			}
		}
		catch(IOException e)
		{
		}
		try{
			if( mInputStream != null){
				mInputStream.close();
			}
		}
		catch(IOException e)
		{
		}
		try{
			if(mOutputStream != null){
				mOutputStream.close();
			}
		}
		catch(IOException e){
		}
		mFileDescriptor = null;
		mOutputStream = null;
		mInputStream = null;
		mAccessory = null;
		avalible = false;
		showExitNotification();
	}


	private void showExitNotification(){
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setMessage("Device was removed.\n Application will be closed.").setCancelable(false)
		.setPositiveButton("OK", new DialogInterface.OnClickListener() {	
			public void onClick(DialogInterface dialog, int which) {
				System.exit(0);
			}
		});
		AlertDialog alert = builder.create();
		alert.show();
	}





	//Broadcast Receiver
	private static final String ACTION_USB_PERMISSION =
		"com.android.example.USB_PERMISSION";
	private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {

		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (ACTION_USB_PERMISSION.equals(action)) {
				synchronized (this) {
					//mAccessory = UsbManager.getAccessory(intent);
					mAccessory = (UsbAccessory)intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
					if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
						if(mAccessory != null){
							openAccessory();//call method to set up accessory communication
						}
					}
					else {
						Log.d(TAG, "permission denied for accessory " + mAccessory);
					}
				}
			}
			else if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action)) {
				//UsbAccessory accessory = UsbManager.getAccessory(intent);
				UsbAccessory accessory = (UsbAccessory)intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
				if (accessory != null && accessory.equals(mAccessory)) {
					closeAccessory();// call your method that cleans up and closes communication with the accessory
				}
				else
				{
					Context contextss = getApplicationContext();
					CharSequence texts = "Something went wrong.";
					int durations = Toast.LENGTH_LONG;
					Toast toasts = Toast.makeText(contextss, texts, durations);
					toasts.show();
					System.exit(10);
				}
			}

		}
	};

	public void onAccuracyChanged(Sensor sensor, int accuracy) {

	}
	private float[] gravity = new float[3];
	private float[] linear_acceleration = new float[3];

	public void onSensorChanged(SensorEvent event) {

		if(avalible)
		{

			// In this example, alpha is calculated as t / (t + dT),
			// where t is the low-pass filter's time-constant and
			// dT is the event delivery rate.

			final float alpha = (float) 0.8;

			// Isolate the force of gravity with the low-pass filter.
			gravity[0] = alpha * gravity[0] + (1 - alpha) * event.values[0];
			gravity[1] = alpha * gravity[1] + (1 - alpha) * event.values[1];
			gravity[2] = alpha * gravity[2] + (1 - alpha) * event.values[2];

			// Remove the gravity contribution with the high-pass filter.
			linear_acceleration[0] = event.values[0] - gravity[0];
			linear_acceleration[1] = event.values[1] - gravity[1];
			linear_acceleration[2] = event.values[2] - gravity[2];
			String text = "X: " + Float.toString(linear_acceleration[0]) + " Y: " + Float.toString(linear_acceleration[1]) + " Z: "
			+ Float.toString(linear_acceleration[2]) + "\0";
			try{
				mOutputStream.write(text.getBytes());
			}
			catch(IOException e){
				Log.d(TAG, "IO Execption");
			}
		}
		else {
			Context context = getApplicationContext();
			CharSequence text = "Please Connect Device";
			int duration = Toast.LENGTH_LONG;
			Toast toast = Toast.makeText(context, text, duration);
			toast.show();
		}

	}
}

