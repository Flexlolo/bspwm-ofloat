README
======

Description
-----------

This program is created for X11 + bspwm.
It reads events from bspc through pipe and keeps track focused window.
Depending on focused window state (floating or not) it changes other floating windows transparency.

Dependencies
------------

	docopt.cpp (https://github.com/docopt/docopt.cpp)

Compilation and installation
----------------------------

	make
	make install

Usage
-----
	bspc subscribe node_focus node_state node_transfer node_add node_remove | bspwm-ofloat

### Options

- `--classes			List of window classes [default: urxvt,urxvt-floating]` 
- `--active				Active opacity [default: 100]` 
- `--inactive			Inactive opacity [default: 25]`