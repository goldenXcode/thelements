/*
 * Element Works: Copyright (C) 2010 IDKJava
 * ----------------------------------------------------------
 * A sandbox type game in which you can play with different elements
 * which interact with each other in unique ways.
 */

package sand.wallpaper.opengl;

import java.util.Random;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorListener;
import android.hardware.SensorManager;
import android.opengl.GLWallpaperService;
import android.os.Handler;
import android.view.MotionEvent;

public class DemoActivity extends GLWallpaperService implements SensorListener
{

	class DemoRenderer implements GLWallpaperService.Renderer
	{
		public void onDrawFrame(GL10 gl)
		{
			nativeRender(); // Actual rendering - everything happens here
		}

		public void onSurfaceChanged(GL10 gl, int w, int h)
		{
			// gl.glViewport(0, 0, w, h);
			nativeResize(w, h);
			if (DemoActivity.loaddemov == true) // loads the demo from the
			// sdcard on first run.
			{
				DemoActivity.loaddemo();
				DemoActivity.loaddemov = false;
			}
		}

		public void onSurfaceCreated(GL10 gl, EGLConfig config)
		{
			nativeInit();
		}

	}

	class SandView extends GLEngine
	{
		private int fd; // Set the "finger down" variable

		DemoRenderer mRenderer;

		SandView()
		{
			super();
			// handle prefs, other initialization
			mRenderer = new DemoRenderer();
			setRenderer(mRenderer);
			setTouchEventsEnabled(true);
			
		}

		// When finger is held down, flood of events killing framerate, need to
		// put in it's own thread at some point
		// and then use the sleep tactic
		public void onTouchEvent(final MotionEvent event) // Finger down
		{

			// Gets the mouse position
			if (event.getAction() == MotionEvent.ACTION_DOWN)
			{
				if (fd != 1) // if it has changed
				{
					DemoActivity.fd(1); // sets the finger state in jni
				}
				else
				{
					fd = 1;
				}
			}
			else if (event.getAction() == MotionEvent.ACTION_UP)
			{
				DemoActivity.fd(2);
				elementsetter(generator.nextInt(23));

			}

			if (DemoActivity.size == 0)
			{
				// Both x and y are halved because it needs to be zoomed in
				DemoActivity.mp((int) event.getX() / 2, (int) event.getY() / 2); // sets
				// the
				// mouse
				// position
				// in
				// jdk
			}
			else
			{
				// Not zoomed in
				DemoActivity.mp((int) event.getX(), (int) event.getY()); // sets
				// the
				// mouse
				// position
				// in
				// jdk
			}
		}
	}

	public static boolean loaddemov = false;

	static final int maxx = 319; // 319 for g1, 479 for droid
	static final int maxy = 414; // 454 for g1, 815 for droid
	static public boolean play = true;
	private static final int SHAKE_THRESHOLD = 800;
	static public int size = 0;
	static public int skip = 1;
	static public int speed = 1;

	static
	{
		System.loadLibrary("sanangeles"); // Load the JNI library
		// (libsanangeles.so)
	}

	public native static void clearquicksave();

	public native static void fd(int fstate); // sets finger up or down, 1 is

	public native static int getelement();

	public native static void jPause(); // Jni pause

	public native static int load();

	public native static void loadcustom();

	public native static int loaddemo();

	public native static void mp(int jxm, int jym); // sets x mouse and y mouse

	public native static void nativePause();

	public native static void Play(); // Jni play

	public native static void quickload();

	public native static void quicksave();

	// JNI functions
	public native static int save();

	public native static void savecustom();

	// down

	public native static void sendxg(float xgrav);

	public native static void sendyg(float ygrav);

	public native static void setAccelOnOff(int state);

	public native static void setBackgroundColor(int colorcode);

	public native static void setblue(int blueness);

	public native static void setBrushSize(int jsize);

	public native static void setcollision(int custnumber, int elementnumb,
			int collisionspot, int collisionnumber);

	public native static void setelement(int element);

	public native static void setexplosiveness(int explosiveness);

	public native static void setFlip(int flipped);

	public native static void setgreen(int greenness);

	public native static void setred(int redness);

	public native static void setup(); // set up arrays and such

	public native static void tester();

	public native static void togglesize(); // Jni toggle size

	Random generator = new Random();

	private float last_x, last_y, last_z;

	private long lastUpdate = -1;

	private final Handler mHandler = new Handler();

	private SensorManager sensorMgr;

	private float x, y, z;

	public DemoActivity()
	{
		super();
	}

	public void elementsetter(int item)
	{
		if (item == 0) // Sand
		{
			setelement(0);
		}
		else if (item == 1) // Water
		{
			setelement(1);
		}
		else if (item == 2) // Plant
		{
			setelement(4);
		}
		else if (item == 3) // Wall
		{
			setelement(2);
		}
		else if (item == 4) // Fire
		{
			setelement(5);
		}
		else if (item == 5) // Ice
		{
			setelement(6);
		}
		else if (item == 6)// Generator
		{
			setelement(7);
		}
		else if (item == 7)// Oil
		{
			setelement(9);
		}
		else if (item == 8)// Magma
		{
			setelement(10);
		}
		else if (item == 9)// Stone
		{
			setelement(11);
		}
		else if (item == 10)// C4
		{
			setelement(12);
		}
		else if (item == 11)// C4++
		{
			setelement(13);
		}
		else if (item == 12)// Fuse
		{
			setelement(14);
		}
		else if (item == 13)// Destructible wall
		{
			setelement(15);
		}
		else if (item == 14)// Drag
		{
			setelement(16);
		}
		else if (item == 15)// Acid
		{
			setelement(17);
		}
		else if (item == 16)// Steam
		{
			setelement(18);
		}
		else if (item == 17)// Salt
		{
			setelement(19);
		}
		else if (item == 18)// Salt-water
		{
			setelement(20);
		}
		else if (item == 19)// Glass
		{
			setelement(21);
		}
		else if (item == 20)// Custom 1
		{
			setelement(22);
		}
		else if (item == 21)// Mud
		{
			setelement(23);
		}
		else if (item == 22)// Custom 3
		{
			setelement(24);
		}
	}

	private native void nativeInit(); // Jni init

	private native void nativeRender();

	private native void nativeResize(int w, int h); // Jni resize

	public void onAccuracyChanged(int arg0, int arg1)
	{
		// TODO Auto-generated method stub
	}

	@Override
	public void onCreate()
	{
		super.onCreate(); // Uses onCreate from the general

		loadcustom();

		sensorMgr = (SensorManager) getSystemService(SENSOR_SERVICE);
		boolean accelSupported = sensorMgr.registerListener(this,
				SensorManager.SENSOR_ACCELEROMETER,
				SensorManager.SENSOR_DELAY_GAME);

		if (!accelSupported)
		{
			// on accelerometer on this device
			sensorMgr.unregisterListener(this,
					SensorManager.SENSOR_ACCELEROMETER);
		}
	}

	@Override
	public Engine onCreateEngine()
	{
		return new SandView();
	}

	public void onSensorChanged(int sensor, float[] values)
	{
		if (sensor == SensorManager.SENSOR_ACCELEROMETER)
		{
			long curTime = System.currentTimeMillis();
			// only allow one update every 100ms.
			if ((curTime - lastUpdate) > 100)
			{
				long diffTime = (curTime - lastUpdate);
				lastUpdate = curTime;

				x = values[SensorManager.DATA_X];
				y = values[SensorManager.DATA_Y];
				z = values[SensorManager.DATA_Z];

				float speed = Math.abs(x + y + z - last_x - last_y - last_z)
						/ diffTime * 10000;
				if (speed > SHAKE_THRESHOLD)
				{
					clearquicksave();
					setup();
				}
				last_x = x;
				last_y = y;
				last_z = z;
			}
		}
	}
}
