/***************************************************************************
                          canvas.h  -  description
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

#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPaintEvent>
#include <QPrinter>
#include <QTextStream>
#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QMenu>
#include <QAction>
#include <QSignalMapper>
#include <QXmlStreamWriter>
#include <QPaintDevice>

#include <math.h>

#include "map.h"
class Map;

#include "point.h"
class Point;

extern QString htmllabel[];
extern int nblabels;

class Canvas : public QWidget	{
	Q_OBJECT
public:
	Canvas( QWidget * parent );
	~Canvas();
	/** copy selected maps and translate result */
	void copy ();
	/** delete polygonal point or full maps according to context */
	void del ();
	/** only if one map is grabbed */
	void gotoMap ( int n );
	/** handle key events in canvas */
	void keyPressEvent ( QKeyEvent * event );
	/** read maps from a kmap file */
	bool loadKmapFile ( QString filename );
	/** read maps from HTML */
	bool loadHtmlFile ( QString htmlfile );
	/** display new fieldsets according to current maps */
	void reloadFormFieldSet ();
	/** save current map to KMAP */
	bool saveMaps2Kmap ( QString kmapfile, QString pictfile );
	/** save current map to HTML */
	bool saveMaps2Html ( QString kmapfile, QString pictfile );
	/** save current map to XML */
	bool saveMaps2Xml ( QString xmlfile, QString pictfile );
	/** select all maps */
	void select ();
	/** update text to _htmlfield i using selected maps */
	void updHtmlFieldForSelectedMaps ( int j, QString text );
	/** return a copy of background sized to current zoom factor */
	void zoom ( double newzoomfactor );
	/** show current map type in status2 */
	void showMapType ( char c );
	/** update the last segment of the current map */
	void drawSegment ();
	/** go to edit mode after a keypress event */
	void edit ();
	/** abort current selection */
	void abort ();
	/** print current maps to printer */
	void printMaps();
	double zoomfactor;
	QColor drawselectioncolor; // color for mouse selection
	QColor mapcurrentcolor;	// current map color
	QList<Map *> maps;			// double list pointers of Maps
	QPixmap bufferPicture; // buffer for picture layer
	QPixmap bufferMaps; // buffer for maps layer
	QPainter printerPainter; // printer buffer
private:
	/** add points to polygon */
	void add ();
	/** delete a map */
	void del ( Map * _map );
	/** delete a polygonal point */
	void del ( Point * _point );
	/** draw to screen and buffer if needed */
	void drawPoints ( Point * _p1, Point * _p2 );
	/** draw an object using points p1 and p2 to pixmap	*/
	void drawTo( QPainter * painter, Point * _p10, Point * _p20 );
	/** draw a single map */
	void drawMap ( Map * _map );
	/** used for moving a polygon point */
	void drawMapExcept( Point * _point );
	/** redraw all maps */
	void drawMaps ();
	/** return current map index */
	int findMap ( Map * _map );
	/** return current point index */
	int findMap ( Point * _point );
	/** return current point index */
	int findPoint ( Point * _point );
	/** grab all maps */
	void grab ();
	/** start to grab a map */
	void grab ( Map * _map );
	/** test if the given point is in an object */
	Map * isCursorInMap ( QPoint point );
	/** deselect all maps */
	void ungrab ();
	/** stop to grab a map */
	void ungrab ( Map * _map );
	/** search the nearest point using mouse position */
	Point * searchNearestPoint( QPoint p );
	bool isdrawing;			 // current drawing state
	bool ismoving;			 // current moving state
	bool isgrabbing;			 // current grabbing state
	bool isscrolling;			 // current scrolling state
	bool toprinter; // send to printer
	bool hidecontrolpoint; // hiding control points
	char mapcurrenttype;	// current map type
	Map * _currentmap;	// we need to store the current map while drawing
	Point * _currentpoint; // we need to store the current point while drawing
	QPoint lastmousepos; // last mouse position
	QSignalMapper * signalMapper1; // signal mapper for popup menu type
	QMenu * popupMenuMapType; // popup menu map type
protected:
	/** handle paint events in the canvas */
	virtual void paintEvent( QPaintEvent * event );
	/** handle mouse move events */
	virtual void mouseMoveEvent( QMouseEvent * event );
	/** handle mouse press events */
	virtual void mousePressEvent( QMouseEvent * event );
	/** handle mouse release buttons events */
	virtual void mouseReleaseEvent ( QMouseEvent * event );
	/** handle mouse wheel events */
	virtual void wheelEvent ( QWheelEvent * event );
public slots:
	/** load a background picture */
	void slotLoadPict ( QPixmap p );
	/** update current map type */
	void slotMapType ( int i );
	/** show or hide control points */
	void slotControlPoints ( bool b );
	/** handle actions on selected maps */
	void slotMapAction ( int i );
signals: // Signals
	/** update HTML entries */
	void signalChangeText( QStringList htmlfields );
	/** scroll the canvas with middle button mouse */
	void signalMouseScrolling( QPoint offset );
	/** load the background picture */
	void signalLoadPict( QString filename );
	/** send text to status 1 */
	void signalStatus1( const char * text );
	/** send text to status 2 */
	void signalStatus2( const char * text );
	/** send the new position to qscrollview */
	void signalScrollBy( QPoint pos );
	/** center the viewport according to real coordinates */
	void signalCenterView ( double x, double y );
	/** give focus to text element HREF */
	void signalFocusText();
	/** show formfieldset */
	void signalShowFormFieldSet( int i );
};

#endif
