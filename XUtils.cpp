#include "XUtils.h"

Window* getWindowList(Display* display, unsigned long* wCount) 
{
	Atom actualType;
	int format;
	unsigned long bytesAfter;
	unsigned char* list = NULL;

	Status status = XGetWindowProperty(
						display,
						DefaultRootWindow( display ),
						XInternAtom( display, "_NET_CLIENT_LIST", False ),
						0L,
						~0L,
						False,
						XA_WINDOW,
						&actualType,
						&format,
						wCount,
						&bytesAfter,
						&list
					);

	if( status != Success ) 
	{
		*wCount = 0;
		return NULL;
	}

	return reinterpret_cast< Window* >( list );
}

char* getWindowName(Display* display,Window w) 
{
	Atom actualType;
	int format;
	unsigned long count, bytesAfter;
	unsigned char* name = NULL;
	Status status = XGetWindowProperty(
						display,
						w,
						XInternAtom( display, "_NET_WM_NAME", False ),
						0L,
						~0L,
						False,
						XInternAtom( display, "UTF8_STRING", False ),
						&actualType,
						&format,
						&count,
						&bytesAfter,
						&name
					);

	if( status != Success ) {
		return NULL;
	}

	if( name == NULL ) {
		Status status = XGetWindowProperty(
							display,
							w,
							XInternAtom( display, "WM_NAME", False ),
							0L,
							~0L,
							False,
							AnyPropertyType,
							&actualType,
							&format,
							&count,
							&bytesAfter,
							&name
						);

		if( status != Success ) {
			return NULL;
		}
	}

	return reinterpret_cast< char* >( name );
}

char* getWindowClass(Display* display, Window w)
{
	Atom actualType;
	int format;
	unsigned long count, bytesAfter;
	unsigned char* name = NULL;

	Status status = XGetWindowProperty(
						display,
						w,
						XInternAtom( display, "WM_CLASS", False ),
						0L,
						~0L,
						False,
						AnyPropertyType,
						&actualType,
						&format,
						&count,
						&bytesAfter,
						&name
					);

	if( status != Success ) {
		return NULL;
	}

	return reinterpret_cast< char* >( name );
}

void setWindowOpacity(Display* display, Window w, unsigned long value)
{
	XChangeProperty(
		display, 
		w, 
		XInternAtom( display, "_NET_WM_WINDOW_OPACITY", False ), 
		XA_CARDINAL,
		32, 
		PropModeReplace, 
		(const unsigned char *)&value, 
		1);
}