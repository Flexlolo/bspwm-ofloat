#include <array>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <unordered_map> 
#include <unordered_set>
#include <vector>

#include "bspwm-ofloat.h"
#include "docopt/docopt.h"
#include "XUtils.h"

using namespace std;

static const char USAGE[] =
R"(Usage:
  bspwm-ofloat [options] [-]

Options:
  -h --help         Show this message.
  --version         Show version.
  --classes STRING  List of window classes [default: urxvt,urxvt-floating]
  --active VALUE    Active opacity [default: 100]
  --inactive VALUE  Inactive opacity [default: 25]
)";

int transparency_active;
int transparency_inactive;

unordered_set<string> classes;

typedef unordered_map<string, bool> state_map;
typedef unordered_map<string, state_map> desktop_map;

Display* display = NULL;



template <typename Out>
void split(const string &s, char delim, Out result) 
{
    istringstream iss(s);
    string item;
    while (getline(iss, item, delim)) {
        *result++ = item;
    }
}

vector<string> string_split_delim(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}


vector<string> string_split(string s)
{
	stringstream ss(s);
	istream_iterator<string> begin(ss);
	istream_iterator<string> end;

	return vector<string>(begin, end);
}

string exec(const char* cmd) 
{
	array<char, 128> buffer;
	string result;
	unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

	if (!pipe) 
	{
		throw runtime_error("popen() failed!");
	}

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) 
	{
		result += buffer.data();
	}

	return result;
}

bool window_in_map(state_map windows, string window)
{
	return windows.find(window) != windows.end();
}

bool get_state(state_map windows, string window)
{
	if (window_in_map(windows, window))
		return windows[window];

	return false;
}

int pick_transparency(bool floating, bool focus_floating)
{
	if (floating)
		return focus_floating ? transparency_active : transparency_inactive;

	return -1;
}

void set_transparency(string window, unsigned long value)
{
	unsigned long transparency = 0xffffffff;

	if (value != 100)
	{
		transparency *= value;
		transparency /= 100;
	}

	Window w = stol(window, 0, 16);
	setWindowOpacity(display, w, transparency);
}

int main(int argc, char const *argv[])
{
	map<string, docopt::value> args = docopt::docopt(USAGE, 
												{ argv + 1, argv + argc },
												true,
												"bspwm-ofloat 1.0");

	transparency_active = args["--active"].asLong();
	transparency_inactive = args["--inactive"].asLong();

	for (string c : string_split_delim(args["--classes"].asString(), ','))
	{
		classes.insert(c);
	}

	state_map windows;
	bool focus_floating_old = exec("bspc query -N -n focused.floating").length();

	display = XOpenDisplay(NULL);
	bool sync = false;

	for (string window : string_split(exec("bspc query -N -n .floating")))
	{
		Window w = stol(window, 0, 16);

		if (char* classname = getWindowClass(display, w)) 
		{
			if (classes.find(classname) != classes.end())
			{
				windows[window] = true;
				
				set_transparency(window, focus_floating_old ? transparency_active : transparency_inactive);
				sync = true;
			}

			XFree(classname);		
		}
	}

	if (sync)
		XSync(display, False);

	for (string line; getline(cin, line);) 
	{
		sync = false;
		vector<string> p = string_split(line);

		if (p[0] == "node_focus")
		{
			bool focus_floating = exec("bspc query -N -n focused.floating").length();

			if (focus_floating != focus_floating_old)
			{
				for (pair<string, bool> window : windows)
				{
					int transparency = pick_transparency(window.second, focus_floating);

					if (transparency != -1)
					{
						set_transparency(window.first, transparency);
						sync = true;
					}
				}
			}

			focus_floating_old = focus_floating;
		}
		else if (p[0] == "node_state")
		{
			if (p[State_Name] != "floating")
				continue;

			bool tracked = window_in_map(windows, p[State_Window]);
			bool floating = p[State_Value] == "on";

			if (tracked)
				windows[p[State_Window]] = floating;

			bool focus_floating = exec("bspc query -N -n focused.floating").length();

			if (focus_floating != focus_floating_old)
			{
				for (pair<string, bool> window : windows)
				{
					int transparency = pick_transparency(window.second, focus_floating);

					if (transparency != -1)
					{
						set_transparency(window.first, transparency);
						sync = true;
					}
				}
			}

			if (tracked)
			{
				if (focus_floating)
					set_transparency(p[State_Window], floating ? transparency_active : transparency_inactive);
				else
					set_transparency(p[State_Window], floating ? transparency_inactive : transparency_active);

				sync = true;
			}

			focus_floating_old = focus_floating;
		}
		else if (p[0] == "node_remove")
		{
			if (window_in_map(windows, p[Transfer_Window]))
			{
				windows.erase(p[Remove_Window]);
			}
		}
		else if (p[0] == "node_add")
		{
			Window w = stol(p[Add_Window], 0, 16);

			if (char* classname = getWindowClass(display, w)) 
			{
				if (classes.find(classname) != classes.end())
				{
					char cmd[128];
					sprintf(cmd, "bspc query -T -n %s | jq -r '.client.state'", p[Add_Window].c_str());
					string state = exec(cmd);

					bool floating = state == "floating";
					windows[p[Add_Window]] = floating;
				}

				XFree(classname);		
			}
		}

		if (sync)
			XSync(display, False);
	}

	XCloseDisplay(display);
	return 0;
}