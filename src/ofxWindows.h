#pragma once

//Here is the following Windows-specific functions:
//- controlling OpenGL windows (make borderless, set position and size for borderless fullscreen) 
//- console window(hide, show),
//- mouse move and click emulation
//- sending keyboard hit to a window, found by window class name(see windows / winlister for getting window class).
//- capture raw keyboard state (for synths, games)
//- screen grabber (GDI method) ofxWindowsScreenGrabber

#include "ofMain.h"

struct ofxWindows {
	//Functions for borderless fullscreen emulation

	//Make current oF window borderless - call it before setting window position and size.
	static void SetMainWindowBorderless();

	//Restore current oF window border
	//NOTE: currently it works bad in oF, because as a result y-coord of mouse is shifted
	static void RestoreMainWindowBorder();

	//Set size and position of the current oF window
	//(works more stable that oF implementation
	static void SetMainWindowPositionSize(int x, int y, int w, int h);


	//Show and hide console window
	static void ShowConsole();
	static void HideConsole();

	//Mouse move and click emulation
	static void MouseExec(int screen_x, int screen_y, bool leftClick, bool rightClick);

	//Send keyboard to a window with given class name
	//To search windows class use WinLister in 'windows/winlister' folder of the addon
	//Note: the window which receives the keyboard press is making active (to be able to receive keypresses)
	static bool SendKeyboardByWindowClass(string window_class, int key);

	//Show/hide window given by window_class
	//To looking for current runned windows classes names run 'utilities/winlister' in addon's folder
	static bool showWindow(string window_class);
	static bool hideWindow(string window_class);

	//Set current window to foreground
	static void set_foreground_window();

	// Capture raw keyboard state(for synths, games)
	// See implementation for details
	static short capture_key_state(int windowsVirtualKey);
};


//Screen grabber
//TODO release after usage hdcScr, hdcMem, hbmScr, see CPP code
struct ofxWindowsScreenGrabber {
	bool grab(int x, int y, int w, int h);
	ofPixels pixels;
};

