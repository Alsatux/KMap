/***************************************************************************
                          point.h  -  description
                             -------------------
    copyright            : (C) 2012 by Jean Luc BIELLMANN
    email                : contact@alsatux.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POINT_H
#define POINT_H

#include <QPoint>
#include <stdlib.h>

#include "map.h"
class Map;

class Point {
public:
	Point( Map * _parent, QPoint p );
	~Point();
	Map * _parentmap;
	QPoint xy;
	/** show coordinates */
	QString show ();
	/** return true if point pt is in rectangle defined by p0 and p1 */
	bool inRectangle ( QPoint p0, QPoint p1 );
	/** return true if point pt is in circle defined by center p0 and radius p1-p0 */
	bool inCircle ( QPoint p0, QPoint p1 );
	/** return true if pt is in ellipse defined by center p1 and radius p1 */
	bool inEllipseRectangle ( QPoint p0, QPoint p1 );
	/** return true if pt is in ellipse defined by center p1 and radius p1-p0 */
	bool inEllipseCenterRadius ( QPoint p0, QPoint p1 );
	/** return true if the current point is inside the map */
	bool inPolygon ( Map * _map );
};

#endif
