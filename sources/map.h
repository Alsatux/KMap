/***************************************************************************
                          map.h  -  description
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

#ifndef MAP_H
#define MAP_H

#include <QList>
#include <QColor>
#include <QHash>
#include <QDebug>

#include <math.h>

#include "point.h"
class Point;

#include "canvas.h"
class Canvas;

struct DoublePoint {
	double x;
	double y;
};

extern QString htmllabel[];
extern int nblabels;

class Map {
public:
	Map( char maptype, QColor mapcolor );
	~Map();
	/** read a map using key=values stringlist */
	bool readFromHtml( char t, QString args );
	/** translate map to HTML */
	QString convert2Html ();
	/** Return html options to hash */
	QHash<QString, QString> getAttributes2Hash ();
	/** Return html options to string */
	QString getAttributes2String ();
	char type; // cf. enumerated types R C E L P
	QColor color; // map color
	QList<Point *> _points; // map points
	bool isgrabbed; // true if current map is grabbing
	bool ismoved; // true if current map is moving
	QStringList htmlfields; // HTML elements
private:
	/** convert ellipes to polygon */
	QString ellipse2polygon ( double xcenter, double ycenter, double xradius, double yradius );

signals: // Signals
	/** show formfieldset */
	void signalShowFormFieldSet( int i );
};

#endif
