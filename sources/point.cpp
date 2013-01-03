/***************************************************************************
                          point.cpp  -  description
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

#include "point.h"

Point::Point( Map * _parent, QPoint p ) {
	_parentmap = _parent;
	xy = p;
}

Point::~Point() {
}

/** show coordinates */
QString Point::show () {
	QString s = QString( "(%1,%2)" ).arg(this->xy.x()).arg(this->xy.y());
	return s;
}

/** return true if point pt is in rectangle defined by p0 and p1 */
bool Point::inRectangle ( QPoint p0, QPoint p1 ) {
	QPoint pt = this->xy;
	bool inside;
	inside = false; // we suppose to be out of the rectangle
	int x0, y0, x1, y1;
	if ( p0.x() > p1.x() ) {
		x0 = p1.x(); x1 = p0.x();
	} else {
		x0 = p0.x(); x1 = p1.x();
	}
	if ( p0.y() > p1.y() ) {
		y0 = p1.y(); y1 = p0.y();
	} else {
		y0 = p0.y(); y1 = p1.y();
	}
	if ( x0 <= pt.x() && pt.x() <= x1 && y0 <= pt.y() && pt.y() <= y1 )
	 inside = true;
	return inside;
}


/** return true if point pt is in circle defined by center p0 and radius p1-p0 */
bool Point::inCircle ( QPoint p0, QPoint p1 ) {
	QPoint pt = this->xy;
	bool inside;
	inside = false; // we suppose to be out of the circle
	QPoint p2;
	p2 = p1 - p0;
	int r;
	r = ( abs( p2.x() ) > abs( p2.y() )) ? ( abs( p2.x() ) ) : ( abs( p2.y() ) );
	int x0, y0, xt, yt;
	x0 = p0.x(); y0 = p0.y(); xt = pt.x(); yt = pt.y();
	if ( (xt-x0)*(xt-x0)+(yt-y0)*(yt-y0) <= r*r )
		inside = true;
	return inside;
}

/** return true if pt is in ellipse/rectangle defined by p0 and p1 */
bool Point::inEllipseRectangle ( QPoint p0, QPoint p1 ) {
	QPoint pt = this->xy;
	bool inside;
	inside = false; // we suppose to be out of the ellipse
	if ( this->inRectangle( p0, p1 ) ) {
		int x0, y0, x1, y1;
		if ( p0.x() > p1.x() ) {
			x0 = p1.x(); x1 = p0.x();
		} else {
			x0 = p0.x(); x1 = p1.x();
		}
		if ( p0.y() > p1.y() ) {
			y0 = p1.y(); y1 = p0.y();
		} else {
			y0 = p0.y(); y1 = p1.y();
		}
		if ( x0 == x1 || y0 == y1 ) { // rectangle is a line...
			return true;
		}
		int xt, yt;
		xt = pt.x(); yt = pt.y();
		float xc, yc, a, b;
		xc = (x0+x1)/2; yc = (y0+y1)/2;
		a = (x1-xc); b = (y1-yc);
		if ( ((xt-xc)*(xt-xc))/(a*a)+((yt-yc)*(yt-yc))/(b*b) <= 1 )
			inside = true;
	}
	return inside;
}

/** return true if pt is in ellipse defined by center p1 and radius p1-p0 */
bool Point::inEllipseCenterRadius ( QPoint p0, QPoint p1 ) {
	bool inside;
	QPoint p2;
	p2 = p0 - ( p1 - p0 );
	inside = this->inEllipseRectangle( p2, p1 );
	return inside;
}

/** return true if the current point is inside the map */
bool Point::inPolygon ( Map * _map ) {
	QPoint pt = this->xy;
	QPoint p1, p2, pold, pnew;
	long x1, y1, x2, y2, xt, yt;
	bool inside;
	xt = (long)pt.x(); yt = (long)pt.y(); // point to test
	inside = false; // we suppose to be out of the polygon
	// we will begin at last point of polygon
	pold = _map->_points.at( _map->_points.count()-1 )->xy;
	// if we have three points, we must check three segments
	for ( int i=0; i<int( _map->_points.count() ); i++ ) {
		pnew = _map->_points.at(i)->xy;
		if ( pnew.x() > pold.x() ) { // always p1 to left and p2 to right
			p1 = pold; p2 = pnew;
		} else {
			p1 = pnew; p2 = pold;
		}
		// algorithm of Bob STEIN founded in
		// Linux Journal February 17, 2000

		// explanation :
		// the purpose is to take the tested point,
		// to build a segment starting at this point and going to north
		// and see how many times the segment will cut the polygon.
		//
		// we study only the segment p1,p2 excluding point pold
		// the current point must be in the segment or , if segment is vertical,
		// the inside will be count only for a point, not twice...
		//
		// the second condition is a dot product for [01,02] and [01,0t].
		// According to the fact we travel to the north pole, this product must be negative.
		// to count the point inside the polygon
		x1 = (long)p1.x(); y1 = (long)p1.y(); x2 = (long)p2.x(); y2 = (long)p2.y();
		//printf("%d : (%d,%d)(%d,%d) (%d,%d) (p1.x()<xt)=%d (xt<=p2.x())=%d (yt-y1)*(x2-x1)=%d (y2-y1)*(xt-x1)=%d\n",
		//(inside ? 1 : 0 ),xt,yt,x1,y1,x2,y2,p1.x()<xt,xt<=p2.x(),(yt-y1)*(x2-x1),(y2-y1)*(xt-x1));
		if ( ( p1.x() < xt ) == ( xt <= p2.x() )
			&& (( yt-y1 ) * ( x2-x1 )) < (( y2-y1 ) * ( xt-x1 )) )
				inside = !inside;
		// prepare next segment
		pold = pnew;
	}
	return inside;
}

