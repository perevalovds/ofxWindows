#include "ofxWindows.h"

#include<iostream>
#include<windows.h>

//--------------------------------------------------------------
//Functions for borderless fullscreen emulation

//Make current oF window borderless
void ofxWindows::SetMainWindowBorderless() {	
	HWND hw = WindowFromDC(wglGetCurrentDC());

	if (hw) {
		SetWindowLong(hw, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	}
}

//--------------------------------------------------------------
//Restore current oF window border
//NOTE: currently it works bad in oF, because y-coord of mouse is shifted
void ofxWindows::RestoreMainWindowBorder() {
	HWND hw = WindowFromDC(wglGetCurrentDC());

	if (hw) {
		SetWindowLong(hw, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_BORDER);
		//SetActiveWindow(hw);
	}
}

//--------------------------------------------------------------
//Set size and position of the current oF window 
void ofxWindows::SetMainWindowPositionSize(int x, int y, int w, int h) {
	HWND hw = WindowFromDC(wglGetCurrentDC());

	if (hw) {
		//https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos
		SetWindowPos(hw, HWND_TOP, x, y, w, h, SWP_SHOWWINDOW);
	}
}

//--------------------------------------------------------------
//Show and hide console window
void ofxWindows::ShowConsole() {
	ShowWindow(GetConsoleWindow(), SW_SHOW);
}

//--------------------------------------------------------------
void ofxWindows::HideConsole() {
	ShowWindow(GetConsoleWindow(), SW_HIDE);
}

//--------------------------------------------------------------
//Mouse move and click emulation
//NOTE: it's old Windows-API method
void ofxWindows::MouseExec(int screen_x, int screen_y, bool leftClick, bool rightClick) {

	int x = (screen_x * 65535 / GetSystemMetrics(SM_CXSCREEN));
	int y = (screen_y * 65535 / GetSystemMetrics(SM_CYSCREEN));
	//screen for this commans has size 65535,65535

	INPUT inp[3];
	memset(inp, 0, sizeof(inp));

	inp[0].type = INPUT_MOUSE;
	inp[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE; // движение мышк?
	inp[0].mi.dx = x;
	inp[0].mi.dy = y;

	bool click = (leftClick || rightClick);
	if (click) {
		inp[1].type = INPUT_MOUSE;
		inp[1].mi.dwFlags = (leftClick) ?
			MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;

		inp[2].type = INPUT_MOUSE;
		inp[2].mi.dwFlags = (leftClick) ?
			MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;  // отпускание правой кнопки
	}

	int count = (click) ? 3 : 1;
	SendInput(count, inp, sizeof(INPUT));

}


//--------------------------------------------------------------
bool ofxWindows::SendKeyboardByWindowClass(string window_class, int key) {
	
	//https://stackoverflow.com/questions/2113950/how-to-send-keystrokes-to-a-window
	//https://batchloaf.wordpress.com/2012/04/17/simulating-a-keystroke-in-win32-c-or-c-using-sendinput/
	//https://stackoverflow.com/questions/11438110/sendinput-for-keyboard-only-lowercase


	HWND window_handle;

	//To search windows class use 'utilities/winlister' in addon's folder

	//Get the handle of the Notepad window.
	window_handle = FindWindowA(window_class.c_str(), NULL);
	if (window_handle == NULL) {
		return false;
	}

	//Bring the Notepad window to the front.
	if (!SetForegroundWindow(window_handle)) {
		return 0;
	}

	//Virtual keys
	//https://docs.microsoft.com/ru-ru/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN

	//Fill in the array of keystrokes to send.
	UINT keystrokes_to_send = 2;
	INPUT keystroke[2];

	const SHORT key1 = VkKeyScan(key);
	const UINT mappedKey1 = MapVirtualKey(LOBYTE(key1), 0);


	keystroke[0].type = INPUT_KEYBOARD;
	//keystroke[0].ki.wVk = 0;
	keystroke[0].ki.wScan = mappedKey1; //key;
	keystroke[0].ki.dwFlags = KEYEVENTF_SCANCODE; //KEYEVENTF_UNICODE;
	keystroke[0].ki.time = 0;
	keystroke[0].ki.dwExtraInfo = GetMessageExtraInfo();

	keystroke[1].type = INPUT_KEYBOARD;
	//keystroke[1].ki.wVk = 0;
	keystroke[1].ki.wScan = mappedKey1;  //key;
	keystroke[1].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP; //KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	keystroke[1].ki.time = 0;
	keystroke[1].ki.dwExtraInfo = GetMessageExtraInfo();


	//Send the keystrokes.
	UINT keystrokes_sent = SendInput((UINT)keystrokes_to_send, keystroke, sizeof(*keystroke));

	return keystrokes_sent == keystrokes_to_send;

}

bool ofxWindows::showWindow(string window_class)
{
	HWND window_handle;

	//To search windows class use 'utilities/winlister' in addon's folder

	//Get the handle of the Notepad window.
	window_handle = FindWindowA(window_class.c_str(), NULL);
	if (window_handle == NULL) {
		return false;
	}

	ShowWindow(window_handle, SW_SHOW);

	return true;
}

bool ofxWindows::hideWindow(string window_class)
{
	HWND window_handle;

	//To search windows class use 'utilities/winlister' in addon's folder

	//Get the handle of the Notepad window.
	window_handle = FindWindowA(window_class.c_str(), NULL);
	if (window_handle == NULL) {
		return false;
	}

	ShowWindow(window_handle, SW_HIDE);

	return true;
}

//--------------------------------------------------------------
void ofxWindows::set_foreground_window() {
	//https://forum.openframeworks.cc/t/set-foreground-window/2354
	HWND wnd = WindowFromDC(wglGetCurrentDC());
	SetForegroundWindow(wnd);
	SetActiveWindow(wnd);
}

//--------------------------------------------------------------
//Screen grabber

HDC hdcScr = 0;
HDC hdcMem = 0;
HBITMAP hbmScr = 0;

//--------------------------------------------------------------
int CaptureBMP(int grabX, int grabY, int grabW, int grabH, unsigned char *outData32)
{
	// Source[1]


	BITMAP bmp;
	//int iXRes, iYRes;

	// Create a normal DC and a memory DC for the entire screen. The
	// normal DC provides a "snapshot" of the screen contents. The
	// memory DC keeps a copy of this "snapshot" in the associated
	// bitmap.
	if (!hdcScr) {
		hdcScr = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);		//ка?вариан?- CreateCompatibleDC(0);
		hdcMem = CreateCompatibleDC(hdcScr);
	}

	//iXRes = GetDeviceCaps(hdcScr, HORZRES);
	//iYRes = GetDeviceCaps(hdcScr, VERTRES);

	// Create a compatible bitmap for hdcScreen.
	if (hbmScr == 0) {							//! если сменилс¤ размер, то надо пересоздат?
		hbmScr = CreateCompatibleBitmap(hdcScr, grabW, grabH);
	}
	if (hbmScr == 0) {
		cout << "CaptureBMP Error 1" << endl;
		return 0;
	}

	// Select the bitmaps into the compatible DC.
	if (!SelectObject(hdcMem, hbmScr)) {
		cout << "CaptureBMP Error 2" << endl;
		return 0;
	}

	// Copy color data for the entire display into a
	// bitmap that is selected into a compatible DC.
	if (!StretchBlt(hdcMem,
		0, 0, grabW, grabH,
		hdcScr,
		grabX, grabY, grabW, grabH,
		SRCCOPY)) {
		cout << "CaptureBMP Error 3" << endl;
		return 0;
	}


	// Source[2]
	PBITMAPINFO pbmi;
	WORD cClrBits;

	// Retrieve the bitmap's color format, width, and height.
	if (!GetObjectW(hbmScr, sizeof(BITMAP), (LPSTR)&bmp)) {
		cout << "CaptureBMP Error 4" << endl;
		return 0;
	}

	// Convert the color format to a count of bits.
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD
	// data structures.)
	if (cClrBits != 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for the 24-bit-per-pixel format.
	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color
	// indices and store the result in biSizeImage.
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) / 8
		* pbmi->bmiHeader.biHeight * cClrBits;

	// Set biClrImportant to 0, indicating that all of the
	// device colors are important.
	pbmi->bmiHeader.biClrImportant = 0;

//	HANDLE hf;                  // file handle
//	BITMAPFILEHEADER hdr;       // bitmap file-header
	PBITMAPINFOHEADER pbih;     // bitmap info-header
	LPBYTE lpBits;              // memory pointer
//	DWORD dwTotal;              // total count of bytes
//	DWORD cb;                   // incremental count of bytes
//	BYTE *hp;                   // byte pointer
//	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER)pbmi;


	int dataSize = pbih->biSizeImage;
	if (dataSize != grabW * grabH * 4) {
		cout << "CaptureBMP ERROR Grab, bad data size " << dataSize << ", expect " << grabW * grabH * 4 << endl;
	}

	//outData32.resize(dataSize);

	lpBits = outData32; //(LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
	//lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) {
		cout << "CaptureBMP Error 5" << endl;
		return 0;
	}


	// Retrieve the color table (RGBQUAD array) and the bits
	// (array of palette indices) from the DIB.
	if (!GetDIBits(hdcMem, hbmScr, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) {
		cout << "CaptureBMP Error 6" << endl;
		return 0;
	}

	return 1;
}

//--------------------------------------------------------------
bool ofxWindowsScreenGrabber::grab(int x, int y, int w, int h) {
	if (pixels.getWidth() != w || pixels.getHeight() != h) {
		pixels.allocate(w, h, 4);
	}

	int res = CaptureBMP(x, y, w, h, pixels.getData());

	if (res) {
		
		//flip y & R<->B
		unsigned char *data = pixels.getData();
		for (int y = 0; y < h/2; y++) {
			for (int x = 0; x < w; x++) {
				int i = 4 * (x + w * y);
				int j = 4 * (x + w * (h-1-y));

				swap(data[i + 0], data[j + 2]);
				swap(data[i + 1], data[j + 1]);
				swap(data[i + 2], data[j + 0]);
				swap(data[i + 3], data[j + 3]);
			}
		}
	}
	return (res);
}

//--------------------------------------------------------------
