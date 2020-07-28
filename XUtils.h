#include <X11/Xlib.h>
#include <X11/Xatom.h>

Window* getWindowList(Display* display, unsigned long* wCount);

char* getWindowName(Display* display, Window w);
char* getWindowClass(Display* display, Window w);
void setWindowOpacity(Display* display, Window w, unsigned long value);


// Display* display = XOpenDisplay(NULL);
// unsigned long count = 0;
// Window* windows = getWindowList(display, &count);

// for (unsigned long i = 0; i < count; ++i)
// {
// 	Window w = windows[i];

// 	if (char* classname = getWindowClass(display, w)) 
// 	{
// 		if (classes.find(classname) != classes.end())
// 		{

// 			cout << w << " : " << classname << endl;
// 		}

// 		XFree(classname);
// 	}
// }

// if (windows) 
// {
// 	XFree(windows);
// }