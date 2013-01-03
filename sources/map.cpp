/***************************************************************************
													map.cpp  -  description
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

#include "map.h"
Map::Map( char maptype, QColor mapcolor ) {
	type = maptype;
	color = mapcolor;
	isgrabbed = false;
	ismoved = false;
	for ( int i=0; i<nblabels; i++ )
		htmlfields.append("");
}

Map::~Map() {
}

/** convert ellipses to polygon */
QString Map::ellipse2polygon ( double xcenter, double ycenter, double xradius, double yradius ) {
	const int res = 32; // resolution : 4*32 = 128 points
	DoublePoint p[4*res];
	QString s;
	double x=0., y=0.;
	int i;
	double a = abs(xradius);
	double b = abs(yradius);
	xcenter = abs(xcenter);
	ycenter = abs(ycenter);
	qDebug() << "ellipse2polygon: center(" << xcenter << ";" << ycenter << ") xradius=" << a << " yradius=" << b;
	if (!a && !b )
		return "1,1,1,1"; // null ellipse ?
	// record the first points
	for ( i=0; i<=res; i++ ) {
		if ( a > b ) {
			y = (b*i) / res;
			x = a * sqrt( 1 - (y*y)/(b*b) );
		}
		if ( a <= b ) {
			x = (a*i) / res;
			y = b * sqrt( 1 - (x*x)/(a*a) );
		}
		p[i].x = x;
		p[i].y = y;;
		qDebug() << "i=" << i << " x=" << p[i].x << " y=" << p[i].y;
	}
	if ( a > b ) {
		for ( i=1; i<=res; i++ ) {
			p[res+i].x = -p[res-i].x;
			p[res+i].y = p[res-i].y;
			qDebug() << "i=" << i << " x=" << p[res+i].x << " y=" << p[res+i].y;
		}
		for ( i=1; i<=res; i++ ) {
			p[res*2+i].x = p[2*res-i].x;
			p[res*2+i].y = -p[2*res-i].y;
			qDebug() << "i=" << i << " x=" << p[2*res+i].x << " y=" << p[2*res+i].y;
		}
		for ( i=1; i<res; i++ ) {
			p[res*3+i].x = -p[3*res-i].x;
			p[res*3+i].y = p[3*res-i].y;
			qDebug() << "i=" << i << " x=" << p[3*res+i].x << " y=" << p[3*res+i].y;
		}
	}
	if ( a <= b ) {
		for ( i=1; i<=res; i++ ) {
			p[res+i].x = p[res-i].x;
			p[res+i].y = -p[res-i].y;
		}
		for ( i=1; i<=res; i++ ) {	
			p[res*2+i].x = -p[2*res-i].x;
			p[res*2+i].y = p[2*res-i].y;
		}
		for ( i=1; i<res; i++ ) {
			p[res*3+i].x = p[3*res-i].x;
			p[res*3+i].y = -p[3*res-i].y;
		}
	}
	for ( i=0; i<4*res; i++ )
		s.append( QString("%1,%2,").arg( qRound(p[i].x+xcenter) ).arg( qRound(p[i].y+ycenter) ) );
	s.chop(1);
	return s;
}

/** read a map using key=values stringlist */
bool Map::readFromHtml( char t, QString args ) {
	QStringList coords;
	qDebug() << "readFromHtml: type: " << t << " args: " << args;
	int i, x, y, radius, pos = 0;
	QRegExp nodes("[^a-z]*([a-z]+)=\"([^\"]*)\"");
	while ((pos = nodes.indexIn(args, pos)) != -1) {
		if ( nodes.cap(1).toLower()=="color" ) {
			color = nodes.cap(2);
		} else {
			qDebug() << "cap1=" << nodes.cap(1) << " cap2=" << nodes.cap(2);
			if ( nodes.cap(1).toLower()=="coords" ) {
				coords = nodes.cap(2).split(",");
				qDebug() << "coords: " << coords;
				if ( t=='C' ) {
					x = coords.at(0).simplified().toInt();
					y = coords.at(1).simplified().toInt();
					radius = coords.at(2).simplified().toInt();
					_points.append( new Point( this, QPoint(x,y) ) );
					_points.append( new Point( this, QPoint(x+radius,y) ) );
				} else { // R or P
					for ( i=0; i<coords.count(); i+=2 )
						if (i+1<coords.count()) {
						qDebug() << "x=" << coords.at(i).simplified() << " y=" << coords.at(i+1).simplified();
						x = coords.at(i).simplified().toInt();
						y = coords.at(i+1).simplified().toInt();
						_points.append( new Point( this, QPoint(x,y) ) );
					}
				}
			} else {
				for ( i=0; i<nblabels; i++ )
					if ( htmllabel[i].toLower()==nodes.cap(1).toLower() )
						htmlfields[i] = nodes.cap(2).replace( QString("&quot;"), QString("\"") );
			}
		}
		pos += nodes.matchedLength();
	}
	return true;	
}

/** translate map to HTML */
QString Map::convert2Html () {
	QString shape, coords, attrs;
	if ( _points.count()<2 )
		return QString("");
	int x1 = _points.at(0)->xy.x();
	int y1 = _points.at(0)->xy.y();
	int x2 = _points.at(1)->xy.x();
	int y2 = _points.at(1)->xy.y();
	int radius;
	switch (type) {
		case 'R':
			shape = "rect";
			qDebug() << "R : x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2;
			coords = QString("%1,%2,%3,%4").arg(x1).arg(y1).arg(x2).arg(y2);
			break;
		case 'C':
			shape = "circle";
			radius = qMax( abs(x2-x1), abs(y2-y1) );
			qDebug() << "C: x1: " << x1 << " y1: " << y1 << "radius: " << radius;
			coords = QString("%1,%2,%3").arg(x1).arg(y1).arg(radius);
			break;
		case 'E': // ellipse (rectangle)
			shape = "poly";
			qDebug() << "E (rectangle): x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2;
			coords = ellipse2polygon( (double)(x1+x2)/2., (double)(y1+y2)/2., (double)(x2-x1)/2., (double)(y2-y1)/2. );
			break;
		case 'L': // ellipse (center/radius)
			shape = "poly";
			qDebug() << "L (center/radius): x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2;
			coords = ellipse2polygon( (double)x1, (double)y1, (double)(x2-x1), (double)(y2-y1) );
			break;
		case 'P':
			shape = "poly";
			for ( int i=0;i<_points.count(); i++ )
				coords.append( QString("%1,%2,").arg(_points[i]->xy.x()).arg(_points[i]->xy.y()) );
			coords.chop(1);
			qDebug() << "P: coord=" << coords;
			break;
	}
	// remember : color attribute don't exists in HTML !
	// we just save it as extra parameter for import to kmap again !
	QString s = QString("<area shape=\"%1\" coords=\"%2\" color=\"%3\" %4 />").arg(shape).arg(coords).arg( color.name() ).arg( getAttributes2String() );
	return s;
}

/** Return html options to hash */
QHash<QString, QString> Map::getAttributes2Hash () {
	QHash<QString, QString> hash;
	for ( int i=0; i<nblabels; i++ )
		if ( htmlfields.at(i).length() )
			hash[ htmllabel[i].toLower() ] = htmlfields.at(i);
	return hash;
}

/** Return html options to string */
QString Map::getAttributes2String () {
	QStringList s;
	for ( int i=0; i<nblabels; i++ )
		if ( htmlfields.at(i).length() ) {
			QString value = htmlfields.at(i);
			value.replace( QString("\""), QString("&quot;") );
			s.append( htmllabel[i].toLower() + "=\"" +  value + "\"" );
		}
	return s.join(" ");
}
