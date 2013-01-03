"""
	KMap - KDE Image Mapper

	Copyright (C) 2012 Jean Luc Biellmann (contact@alsatux.com)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""

Image Mapper - version 0.2 - 2012

PRESENTATION
------------

This is a simple Image Mapping tool for webmasters, which let you import a picture, then drawing rectangles, circles, ellipses, polynoms on it. For each map created, you can fix HTML attributes like HREF, ALT, TARGET, etc.

The attributes list is controlled by the application's menu. You can show/hide control points, change the zoom level, move forms after creating them, create or delete points for polynoms, change map color, group maps to copy, delete, change color on them...

Take a look in menus for keyboard shortcuts, which are very handy when working on big files.

After saving your work in kmap format, you can export it to XML or HTML.

WARNING
-------

Kmap records only relative paths to pictures. So if you move a picture after working on it, you need to reimport the picture to update the path manually.

TEST
----

Compile/run the software, import the picture from the test directory, then open test.kmap from the same.

TO KNOW
-------

If Image Mapping is no longer used today, thanks to SVG and others AJAX techniques, KMap is still an interesting projet for who want to learn the Qt library. This release use the Qt4 version whith Qt3 compatibility flag (needed for QScrollView).

And please, don't hesitate to contact me to improve or correct this work !
