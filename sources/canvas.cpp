/***************************************************************************
                          canvas.cpp  -  description
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

#include "canvas.h"

Canvas::Canvas( QWidget * parent ) {

	popupMenuMapType = new QMenu( this );
	signalMapper1 = new QSignalMapper( this );

	QAction * actionMapType1 = new QAction( "&rectangle", this );
	actionMapType1->setShortcut( Qt::Key_R );
	signalMapper1->setMapping( actionMapType1, 1 );
	connect( actionMapType1, SIGNAL(triggered()), signalMapper1, SLOT(map()) );
	popupMenuMapType->addAction( actionMapType1 );

	QAction * actionMapType2 = new QAction( "&circle (center/radius)", this );
	actionMapType2->setShortcut( Qt::Key_C );
	signalMapper1->setMapping( actionMapType2, 2 );
	connect( actionMapType2, SIGNAL(triggered()), signalMapper1, SLOT(map()) );
	popupMenuMapType->addAction( actionMapType2 );

	// TODO: CIRCLE RECTANGLE

	QAction * actionMapType4 = new QAction( "&ellipse (rectangle)", this );
	actionMapType4->setShortcut( Qt::Key_E );
	signalMapper1->setMapping( actionMapType4, 4 );
	connect( actionMapType4, SIGNAL(triggered()), signalMapper1, SLOT(map()) );
	popupMenuMapType->addAction( actionMapType4 );

	QAction * actionMapType5 = new QAction( "e&llipse (center/radius)", this );
	actionMapType5->setShortcut( Qt::Key_L );
	signalMapper1->setMapping( actionMapType5, 5 );
	connect( actionMapType5, SIGNAL(triggered()), signalMapper1, SLOT(map()) );
	popupMenuMapType->addAction( actionMapType5 );

	QAction * actionMapType6 = new QAction( "&polygon", this );
	actionMapType6->setShortcut( Qt::Key_P );
	signalMapper1->setMapping( actionMapType6, 6 );
	connect( actionMapType6, SIGNAL(triggered()), signalMapper1, SLOT(map()) );
	popupMenuMapType->addAction( actionMapType6 );

	connect( signalMapper1, SIGNAL(mapped(int)), this, SLOT(slotMapType(int)) );
	connect( this, SIGNAL(signalShowFormFieldSet(int)), parent, SLOT(slotShowFormFieldSet(int)) );

	// set default size for buffers
	bufferPicture = QPixmap( size() );
	bufferPicture.fill( Qt::transparent );
	bufferMaps = QPixmap( size() );
	bufferMaps.fill( Qt::transparent );

	// default trace color
	drawselectioncolor = "red";

	// default map parameters
	slotMapType(1); // mapcurrenttype = 'R' (rectangle)
	mapcurrentcolor = "blue";
	_currentmap = NULL;
	_currentpoint = NULL;

	// default drawing parameters
	isdrawing = false; // not currently drawing something
	ismoving = false; // not moving a polygon point (isdrawing required)
	isgrabbing = false; // no map grabbed
	isscrolling = false; // no mouse scrolling on canvas

	// show control points
	hidecontrolpoint = false;

	// do not send to printer
	toprinter = false;

	setMouseTracking( true ); // enable mouse tracking for mouseMoveEvent

	// default zoom factor is 100% with initialisation of backgoundz
	zoomfactor = 100.;
}

Canvas::~Canvas() {
}

/** load a background picture */
void Canvas::slotLoadPict ( QPixmap pict ) {
	// WE DON'T CHANGE CANVAS SIZE HERE !
	// we just adapt sizes of picture and maps buffer, then record picture
	QSize pictsize = pict.size();
	bufferPicture = QPixmap( pictsize );
	bufferMaps = QPixmap( pictsize );
	bufferPicture = pict.copy();
	//qDebug() << "slotLoadPict:";
	//qDebug() << "bufferPicture size: " << bufferPicture.size();
	//qDebug() << "bufferMaps size: " << bufferMaps.size();
	repaint();
}

/** draw an object using points p1 and p2 to pixmap	*/
void Canvas::drawTo( QPainter * painter, Point * _p10, Point * _p20 ) {
	Map * _map = _p10->_parentmap;

	if ( ( ismoving && _map->ismoved ) || ( isgrabbing && _map->isgrabbed ) )
		painter->setPen( drawselectioncolor );
	else
		painter->setPen( _p10->_parentmap->color );

	QPoint p1 = _p10->xy, p2 = _p20->xy , p3;

	if ( _map->type == 'R' ) {
		QRect r( p1, p2 );
		painter->drawRect( r );
	}

	if ( _map->type == 'C' ) {
	 p3 = p2-p1;
	 p3.setX( abs(p3.x()) );
	 p3.setY( abs(p3.y()) );
	 if ( p3.x() > p3.y() )
	 	p3.setY( p3.x() );
	 else
	 	p3.setX( p3.y() );
	 QRect r(p1-p3,p1+p3);
	 painter->drawEllipse( r );
	}

	if ( _map->type == 'E' ) {
	 QRect r(p1,p2);
	 painter->drawEllipse( r );
	}

	if ( _map->type == 'L' ) {
	 QRect r(p1-(p2-p1),p2);
	 painter->drawEllipse( r );
	}

	if ( _map->type == 'P' ) {
	 painter->drawLine( p1, p2 );
	}

	// draw a little circle around control points
	if ( !hidecontrolpoint ) {
		QPoint c ( 2, 2 );
		QRect r1(p1-c,p1+c);
		painter->drawEllipse(r1);
		QRect r2(p2-c,p2+c);
		painter->drawEllipse(r2);
	}
}

/** draw to screen and bufferDefault if needed */
void Canvas::drawPoints ( Point * _p1, Point * _p2 ) {
	QPainter painter;

	//qDebug() << "drawPoints:";
	//qDebug() << bufferMaps.size();
	painter.begin( &bufferMaps ); // always record to screen
	drawTo( &painter, _p1, _p2 );
	painter.end();

	/*
	if ( toprinter )
		drawTo( &printerPainter, _p1, _p2 );
*/
}

/** handle paint events in the canvas */
void Canvas::paintEvent( QPaintEvent * event ) {
	//qDebug() << "paintEvent:";

	resize( bufferPicture.size()*(zoomfactor/100.) );

	//qDebug() << "Canvas size:" << size();
	//qDebug() << "bufferPicture size:" << bufferPicture.size();
	//qDebug() << "bufferMaps size:" << bufferMaps.size();

	QPainter painter;
	painter.begin( this );

	QPixmap background( size() );
	background.fill( Qt::white );
	painter.drawPixmap( 0, 0, background );

	QTransform matrix( zoomfactor/100., 0., 0., zoomfactor/100., 0., 0. );
	painter.setTransform( matrix );

	painter.drawPixmap( 0, 0, bufferPicture );
	drawMaps();
	painter.drawPixmap( 0, 0, bufferMaps );

	painter.end();
}

/** handle mouse press events in canvas */
void Canvas::mousePressEvent( QMouseEvent * event ) {
	//qDebug() << "mousePressEvent: " << event;

	QPoint realpos = event->pos() * 100. / zoomfactor;
	Map * _map;
	int i;

	if ( !isscrolling ) { // priority to scrolling
		if ( isgrabbing ) { // currently grabbing
				if ( event->button() == Qt::LeftButton ) {
				_map = isCursorInMap( realpos );
				if ( event->modifiers().testFlag(Qt::ControlModifier) ) {
					if ( _map != NULL ) { // map found
						if ( !_map->isgrabbed ) { // add the new map to grabbing selection
							grab( _map );
							repaint();
						} else { // remove map from current selection
							ungrab( _map );
							repaint();
						}
					}
				}
				if ( event->modifiers().testFlag(Qt::NoModifier) ) {
					if ( _map == NULL ) { // no map found
						ungrab(); // deselect all
						repaint();
					}
				}
			}
			if ( event->button() == Qt::RightButton ) // deselect all
				ungrab();
				repaint();
		} else { // not currently grabbing object
			if ( event->button() == Qt::LeftButton ) {
				if ( isdrawing == false ) { // not currently drawing
					qDebug() << "realpos = " << realpos;
					_currentpoint = searchNearestPoint( realpos );
					qDebug() << _currentpoint;
					if ( _currentpoint == NULL ) { // no control point selected
						qDebug() << "_currentpoint is NULL (no ctrl point selected)";
						_map = isCursorInMap( realpos );
						if ( _map != NULL ) { // we are currently in an object
							grab( _map );
							repaint();
						} else { // not in an object : create a new one
							isdrawing = true; // start to draw
							qDebug() << "isdrawing = true";
							maps.append( new Map( mapcurrenttype, mapcurrentcolor ) );
							_currentmap = maps.last();
							_currentmap->_points.append( new Point ( _currentmap, realpos ) );
							_currentmap->_points.append( new Point ( _currentmap, realpos ) );
							_currentpoint = _currentmap->_points.last();
							drawSegment();
						}
					} else {	 // a control point was selected
						qDebug() << "_currentpoint is nearest found.";
						isdrawing = true; // start to draw
						qDebug() << "isdrawing = true";
						ismoving = true;	// start to move
						i = findMap( _currentpoint->_parentmap ); // search current map
						_currentmap = maps.at(i); // store the map pointer
						_currentmap->ismoved = true; // change state of map
						_currentpoint->xy = realpos;	 // update coordinates
						if ( _currentmap->type == 'P' ) { // polygon
							drawMapExcept( _currentpoint );
						}
					}
				} else { // isdrawing is true
					_currentpoint->xy = realpos; // update coordinates of last point
					if ( ismoving ) {	// finish to move a point
						isdrawing = false;	 // stop to draw
						ismoving = false; // stop to move
						_currentmap->ismoved = false; // change state of map
						repaint();
					} else { // drawing something
						if ( _currentmap->type == 'P' ) { // polygon
							drawSegment();
							_currentmap->_points.append( new Point ( _currentmap, realpos ) );
							_currentpoint = _currentmap->_points.last();
							drawSegment();
						} else { // not a polygon : finish to draw
							isdrawing = false;	 // stop to draw
							repaint();
						}
					}
				}
			}

			if ( event->button() == Qt::RightButton ) {
				if ( isdrawing == false ) { // not currently drawing
					popupMenuMapType->exec( QCursor::pos() ); // popup menu
				} else { // isdrawing = true
					if ( _currentmap->type == 'P' ) { // close the polygon
						isdrawing = false;	 // stop to draw
						if ( _currentmap->_points.count() >= 3 ) { // we need 3 points for a polygon
							repaint();
						} else {
							del( _currentmap );
							repaint();
						}
					}
				}
			}

			if ( event->button() == Qt::MidButton )
				isscrolling = true;
		}
	}

	// always record last mouse position
	lastmousepos = event->pos();
}

/** start to grab a map */
void Canvas::grab ( Map * _map ) {
	if ( _map != NULL ) {
		isgrabbing = true;
		isdrawing = ismoving = false;
		_map->isgrabbed = true;
		// update html field with values from selected map
		emit signalChangeText( _map->htmlfields );
		emit signalStatus1( tr("Map selected") );
	}
}

/** select all maps */
void Canvas::grab () {
	if ( maps.count() ) {
		for ( int i=0; i<int( maps.count() ); i++ )
			grab( maps.at(i) );
		repaint();
		emit signalStatus1( tr("Maps selected") );
	}
}

/** deselect all maps */
void Canvas::ungrab () {
	if ( maps.count() ) {
		for ( int i=0; i<int( maps.count() ); i++ )
			ungrab( maps.at(i) );
		// clean redraw
		isgrabbing = isdrawing = ismoving = isscrolling = false;
	}
	emit signalStatus1( tr("No selection") );
}

/** stop to grab a map */
void Canvas::ungrab ( Map * _map ) {
	_map->isgrabbed = false;
	emit signalStatus1( tr("Map realease") );
}

/** handle mouse move events */
void Canvas::mouseMoveEvent( QMouseEvent * event ) {
	//qDebug() << "mouseMoveEvent: ";

	setFocus();

	QPoint realpos = event->pos() * 100. / zoomfactor;

	if ( isscrolling ) {
		/** scroll the canvas using last mouse position */
		emit signalMouseScrolling( lastmousepos - event->pos() );
	} else {
		// currently drawing
		if ( isdrawing && _currentpoint!=NULL ) {
			_currentpoint->xy = realpos;
			drawSegment();
		}
		// currently grabbing : tranlate objects
		if ( isgrabbing && event->button() == Qt::LeftButton ) {
			for ( int i=0; i<int( maps.count() ); i++ )
				if ( maps.at(i)->isgrabbed ) {
					for ( int j=0; j<int(maps.at(i)->_points.count()); j++ ) {
						QPoint p = event->pos()-lastmousepos;
						maps.at(i)->_points.at(j)->xy += p * 100. / zoomfactor;
					}
					drawMap( maps.at(i) );
				}
			repaint();
		}
		// show maps if no current actions
		if ( !isdrawing && !isgrabbing && !ismoving && !isscrolling ) {
			Map * _map = isCursorInMap( realpos );
			if ( _map != NULL ) {
				showMapType( _map->type );
				emit signalChangeText( _map->htmlfields );
			}
		}
	}

	// show information about control point
	Point * _point = searchNearestPoint( realpos );
	if ( _point != NULL ) {
		int i = findPoint( _point );
		if ( i >= 0 ) {
			QString s = QString("%1:%2").arg(_point->_parentmap->type).arg(i+1);
			if ( isdrawing && _point->_parentmap->type == 'P' ) // polygon
				s += tr(" - press ESC to finish");
			emit signalStatus2( s );
		}
	}

	// always record last mouse position
	lastmousepos = event->pos();
}

/** update current map type */
void Canvas::slotMapType ( int item ) {
	switch (item) {
		case 1: mapcurrenttype='R'; break;
		case 2: mapcurrenttype='C'; break;
		case 4: mapcurrenttype='E'; break;
		case 5: mapcurrenttype='L'; break;
		case 6: mapcurrenttype='P'; break;
	}
	showMapType( mapcurrenttype );
}

/** search the nearest point using mouse position */
Point * Canvas::searchNearestPoint( QPoint p0 ) {
	Map * _map;
	Point * _nearestpoint = NULL, * _point;
	QPoint p1, p2;
	double d, dmin=3.;

	if ( maps.count() )
		for ( int i=0; i < int( maps.count() ); i++ ) {
			_map = maps.at(i);
			if ( _map->_points.count() )
				for ( int j=0; j < int( _map->_points.count() ); j++ ) {
					_point = _map->_points.at(j);
					p2 = _point->xy;
					p1 = p2 - p0;
					d = sqrt(p1.x()*p1.x()+p1.y()*p1.y());
					if ( d < dmin ) {
						dmin = d;
						_nearestpoint = _point;
					}
				}
		}
	return _nearestpoint;
}

/** redraw all maps */
void Canvas::drawMaps () {
	bufferMaps.fill( Qt::transparent );
	if ( maps.count() )
		for ( int i=0; i < int( maps.count() ); i++ )
			drawMap( maps.at(i) );
}

/** draw a single map */
void Canvas::drawMap ( Map * _map ) {
	if ( _map->_points.count() > 1 ) { // need two points...
		// redraw all points
		for ( int i=0; i<int(_map->_points.count()-1); i++ )
		 drawPoints( _map->_points.at(i), _map->_points.at(i+1) );
		// for polygon, close first and last points
		if ( _map->type == 'P' )
		 drawPoints( _map->_points.first(), _map->_points.last() );
	}
}

/** used for moving a polygon point */
void Canvas::drawMapExcept( Point * _point ) {
	if ( _point != NULL ) {
		Map * _map = _point->_parentmap;
		if ( _map != NULL ) {
			if ( _map->_points.count() ) {
				for ( int i=0; i<int(_map->_points.count()-1); i++ )
					if ( _map->_points.at(i) != _point && _point != _map->_points.at(i+1) )
						drawPoints( _map->_points.at(i), _map->_points.at(i+1) );
			}
			if ( _map->_points.first() != _point && _point != _map->_points.last() )
				drawPoints( _map->_points.first(), _map->_points.last() );
		}
	}
}

/** test if the given point is in an object */
Map * Canvas::isCursorInMap ( QPoint p ) {
	bool ret;
	if ( maps.count() ) {
		for ( int i=0; i < int( maps.count() ); i++ ) {
			Map * _map = maps.at(i);
			if ( _map->_points.count()>1 ) {
				QPoint p1 = _map->_points.at(0)->xy;
				QPoint p2 = _map->_points.at(1)->xy;
				Point * _testpoint = new Point ( NULL, p );
				switch ( _map->type ) {
					case 'R' : ret = _testpoint->inRectangle( p1, p2 ); break;
					case 'C' : ret = _testpoint->inCircle( p1, p2 ); break;
					case 'E' : ret = _testpoint->inEllipseRectangle( p1, p2 ); break;
					case 'L' : ret = _testpoint->inEllipseCenterRadius( p1, p2 ); break;
					case 'P' : ret = _testpoint->inPolygon( _map ); break;
				}
				if ( ret ) return _map;
			}
		}
	}
	return NULL;
}

/** handle key events in canvas */
void Canvas::keyPressEvent ( QKeyEvent * event ) {
	if ( event->modifiers() == Qt::NoModifier ) {
		switch ( event->key() ) {
			// add points to polygon
			case Qt::Key_A: add(); break;
			// delete a point or a map
			case Qt::Key_D: del(); break;
			// give focus to HREF
			case Qt::Key_I: edit(); break;
			// abort selection
			case Qt::Key_Escape: abort(); break;
			// scrollings
			case Qt::Key_Up:	 signalScrollBy( QPoint(0,-20) ); break;
			case Qt::Key_Down: signalScrollBy( QPoint(0,20) ); break;
			case Qt::Key_Left: signalScrollBy( QPoint(-20,0) ); break;
			case Qt::Key_Right:signalScrollBy( QPoint(20,0) ); break;
			case Qt::Key_PageUp:
			 signalScrollBy( QPoint( 0,-qMax( height()/10,10) ) );
				break;
			case Qt::Key_PageDown:
			 signalScrollBy( QPoint( 0, qMax(height()/10,10) ) );
				break;
		}
	}
}

/** update text to _htmlfield i using selected maps */
void Canvas::updHtmlFieldForSelectedMaps ( int j, QString text ) {
	// update all selected maps
	for ( int i=0; i<int( maps.count() ); i++ )
		if ( maps.at(i)->isgrabbed )
			maps.at(i)->htmlfields[j] = text;
}

/** handle mouse release buttons events */
void Canvas::mouseReleaseEvent ( QMouseEvent * event ) {
	if ( event->button() == Qt::MidButton )
		isscrolling = false;
	// always record last mouse position
	lastmousepos = event->pos();
}

/** return current map index */
int Canvas::findMap ( Map * _map ) {
	for ( int i=0; i<int(maps.count()); i++ )
		if ( maps.at(i) == _map )
			return i;
	return -1;
}

/** return current point index */
int Canvas::findMap ( Point * _point ) {
	Map * _map = _point->_parentmap;
	for ( int i=0; i<int(_map->_points.count()); i++ )
		if ( _map->_points.at(i) == _point )
			return i;
	return -1;
}

/** return current point index */
int Canvas::findPoint( Point * _point ) {
	Map * _map = _point->_parentmap;
	if ( findMap( _map ) != -1 ) {
		for ( int i=0; i<int(_map->_points.count()); i++ )
			if ( _map->_points.at(i) == _point )
				return i;
	}
	return -1;
}

/** return a copy of background sized to current zoom factor */
void Canvas::zoom ( double newzoomfactor ) {
	//qDebug() <<	"Zoom: ";
	zoomfactor = newzoomfactor;
	QString s = QString("Zoom: %1%").arg((int)zoomfactor);
	emit signalStatus1( s );
	repaint();
}

/** delete a map */
void Canvas::del ( Map * _map ) {
	int i;
	i = findMap( _map );
	if ( i != -1 ) {
		maps.removeAt( i );
		// clean redraw
		isgrabbing = isdrawing = ismoving = false;
		emit signalStatus1( tr("Map deleted") );
	}
}

/** delete a polygonal point */
void Canvas::del ( Point * _point ) {
	if ( _point != NULL ) {
		Map * _map = _point->_parentmap;
		_map->_points.removeAll( _point );
		// for paintEvent
		_currentmap = NULL;
		_currentpoint = NULL;
		// clean redraw
		isgrabbing = isdrawing = ismoving = false;
		emit signalStatus1( tr("Point deleted") );
	}
}

/** only if one map is grabbed */
void Canvas::gotoMap ( int n ) {
	Map * _map = NULL;
	int i,j;
	qDebug() << "gotoMap:" << n;
	if ( !isdrawing && maps.count() ) {
		// user ask for first of last map
		if ( n==0 || n==3 ) {
			// ungrab all maps
			ungrab();
			switch (n) {
				case 0 : _map = maps.first(); break;
				case 3 : _map = maps.last(); break;
			}
		} else {
			// count the number of maps
			j = 0;
			for ( i=0; i<int( maps.count() ); i++ )
				if ( maps.at(i)->isgrabbed ) {
					_map = maps.at(i);
					j++;
				}
			if ( j==1 ) { // only one map grabbed
				ungrab( _map );
				i = findMap( _map );
				switch (n) {
				case 0 : _map = maps.first(); break;
				case 1 : _map = i==0 ? maps.last() : maps.at(i-1); break;
				case 2 : _map = i==int(maps.count() - 1) ? maps.first() : maps.at(i+1); break;
				case 3 : _map = maps.last(); break;
				}
			}
		}
		if (_map != NULL) {
			grab( _map );
			repaint();
			QPoint c(0,0); // gravity center
			for ( i=0; i<int( _map->_points.count() ); i++ )
				c += _map->_points.at(i)->xy;
			c /=	int(_map->_points.count());
			emit signalCenterView( (double)(c.x()), (double)(c.y()) );
		}
	}
}

/** save current map */
bool Canvas::saveMaps2Kmap ( QString kmapfile, QString pictfile ) {
	qDebug() << "saveMaps2Kmap:" << kmapfile;
	QFile file(kmapfile);
	int i,j;

	if ( !file.open( QIODevice::WriteOnly ) )
		return false;

	QTextStream ts( &file );
	// write KMAP file
	if ( !pictfile.isEmpty() )
		ts << "PICTURE=" << pictfile << "\n\n";
	if ( maps.count() ) {
		for ( i=0; i<int( maps.count() ); i++ ) {

			switch ( maps.at(i)->type ) {
				case 'R': ts << "RECTANGLE" << "\n"; break;
				case 'C': ts << "CIRCLE" << "\n"; break;
				case 'E': ts << "ELLIPSERECTANGLE" << "\n"; break;
				case 'L': ts << "ELLIPSECENTERRADIUS" << "\n"; break;
				default : ts << "POLYGON" << "\n";
			}

			ts << maps.at(i)->color.name() << "\n";

			for ( j=0; j<int( maps.at(i)->_points.count() ); j++ ) {
				QPoint p = maps.at(i)->_points.at(j)->xy;
				ts << p.x() << "," << p.y();
				if ( j != int( maps.at(i)->_points.count() -1 ) )
					ts << ",";
			}

			ts << "\n";

			for ( j=0; j<nblabels; j++ )
				if ( maps.at(i)->htmlfields.at(j).length() )
					ts << htmllabel[j] << "=" << maps.at(i)->htmlfields.at(j) << "\n";

			ts << "\n";
		}
		file.close();
		return true;
	}
	return false;
}

/** read maps from HTML */
bool Canvas::loadHtmlFile ( QString htmlfile ) {
	qDebug() << "loadHtmlFile: ";
	QFile file( htmlfile );
	QString line;
	if ( !file.open( QIODevice::ReadOnly ) )
		return false;
	QTextStream ts( &file );
	char t;
	QRegExp pict ("^<img src=\"([^\"]+)\".*$");
	QRegExp area ("^<area shape=\"([^\"]+)\" ([^>]+)>$");
	while ( !ts.atEnd() ) {
		line = ts.readLine(); // line of text excluding '\n'
		qDebug() << line;
		if ( line.isEmpty() )
			continue;
		if ( pict.indexIn( line ) != -1 ) {
			qDebug() << "image: " << line;
			emit signalLoadPict( pict.cap(1) );
			continue;
		}
		if ( area.indexIn( line ) != -1 ) {
			t = '\0';
			qDebug() << "area: " << line;
			if ( area.cap(1)=="rect" )
				t = 'R';
			if ( area.cap(1)=="circle" )
				t = 'C';
			if ( area.cap(1)=="poly" )
				t = 'P';
			if ( t != '\0' ) {
				Map * _map = new Map( t, Qt::black );
				if ( _map->readFromHtml( t, area.cap(2) ) )
					maps.append( _map );
			}
		}
	}
	file.close();
	reloadFormFieldSet();
	repaint();
	return true;
}

/** display new fieldsets according to current maps */
void Canvas::reloadFormFieldSet () {
	for ( int j=0; j<int( maps.count() ); j++ ) {
		Map * _map = maps.at(j);
		// qDebug() << "reloadFormFieldSet:" << j;
		for ( int i=0; i<nblabels; i++ )
			if ( _map->htmlfields[i].length() )
				emit(signalShowFormFieldSet(i));
	}
}

/** save maps to HTML */
bool Canvas::saveMaps2Html ( QString htmlfile, QString pictfile ) {
	QFile file( htmlfile );
	if ( !file.open( QIODevice::WriteOnly ) )
		return false;
	QTextStream ts( &file );
	if ( !pictfile.isEmpty() )
		ts << "<img src=\"" << pictfile << "\" alt=\"map\" usemap=\"#kmap\" />\n";
	ts << "<map name=\"kmap\" id=\"kmap\">\n";
	if ( maps.count() )
		for ( int i=0; i<int( maps.count() ); i++ )
			ts << maps.at(i)->convert2Html() << "\n";
	ts << "</map>\n";
	file.close();
	return true;
}

/** save maps to XML */
bool Canvas::saveMaps2Xml ( QString xmlfile, QString pictfile ) {
	qDebug() << "saveMaps2Xml:" << xmlfile;
	QFile file(xmlfile);
	int i,j;

	if ( !file.open( QIODevice::WriteOnly ) )
		return false;

	QTextStream ts( &file );

	QXmlStreamWriter stream(&file);
	stream.setAutoFormatting(true);
	stream.writeStartDocument();

	stream.writeStartElement("kmap");

	if ( !pictfile.isEmpty() ) {
		stream.writeStartElement("img");
		stream.writeAttribute("src", pictfile);
		stream.writeEndElement();
	}

	if ( maps.count() )
		for ( i=0; i<int( maps.count() ); i++ ) {
			stream.writeStartElement("area");
			QString type;
			switch ( maps.at(i)->type ) {
				case 'R': type = "rectangle"; break;
				case 'C': type = "circle"; break;
				case 'E': type = "ellipse_rectangle"; break;
				case 'L': type = "ellipse_center_radius"; break;
				default : type = "polygon";
			}
			stream.writeAttribute("type", type);
			stream.writeAttribute("color", maps.at(i)->color.name());
			for ( j=0; j<nblabels; j++ )
				if ( maps.at(i)->htmlfields.at(j).length() )
					stream.writeAttribute( htmllabel[j].toLower(), maps.at(i)->htmlfields.at(j) );
			for ( j=0; j<int( maps.at(i)->_points.count() ); j++ ) {
				QPoint p = maps.at(i)->_points.at(j)->xy;
				stream.writeStartElement( "point" );
				stream.writeAttribute( "x", QString::number(p.x()) );
				stream.writeAttribute( "y", QString::number(p.y()) );
				stream.writeEndElement();
			}
			stream.writeEndElement();
		}
	stream.writeEndDocument();
	file.close();
	return true;
}

/** read maps from a kmap file */
bool Canvas::loadKmapFile ( QString kmapfile ) {
	QFile file(kmapfile);
	QString line;
	QStringList coords;
	char t = '\0';
	QColor c;
	int i,x,y;
	Map * _map = NULL;
	bool ok;
	if ( !file.open( QIODevice::ReadOnly ) )
		return false;
	QTextStream ts( &file );
	while ( !ts.atEnd() ) {
		line = ts.readLine(); // line of text excluding '\n'
		if ( line.isEmpty() )
			continue;
		QRegExp pict ("^PICTURE=(.*)$");
		if ( pict.indexIn( line ) != -1 ) {
			qDebug() << "image: " << line;
			emit signalLoadPict( pict.cap(1) );
			continue;
		}
		if ( line == "RECTANGLE" ) t = 'R';
		if ( line == "CIRCLE" ) t = 'C';
		if ( line == "ELLIPSERECTANGLE" ) t = 'E';
		if ( line == "ELLIPSECENTERRADIUS" ) t = 'L';
		if ( line == "POLYGON" ) t = 'P';
		QRegExp hexacolor ("^#[a-zA-Z0-9]{6}$");
		if ( hexacolor.indexIn( line ) != -1 )
			c.setNamedColor(line);
		QRegExp coordinates ("^[-\\d,]+$");
		if ( coordinates.indexIn( line ) != -1 ) {
			coords = line.split( "," );
			maps.append( new Map( t, c ) );
			_map = maps.last();
			for ( i=0; i<int(coords.count()); i+=2 ) {
				x = coords[i].toInt( &ok );
				y = coords[i+1].toInt( &ok );
				_map->_points.append( new Point ( _map, QPoint(x,y) ) );
			}
		}
		QRegExp hash ("^([a-zA-Z]+)=(.*)$");
		if ( hash.indexIn( line ) != -1 ) {
			if ( _map != NULL )
				for ( i=0; i<nblabels; i++ )
					if ( hash.cap(1).toLower() == htmllabel[i].toLower() )
						_map->htmlfields[i] = hash.cap(2);
		}
	}
	file.close();
	reloadFormFieldSet();
	repaint();
	return true;
}

/** copy selected maps and translate result */
void Canvas::copy () {
	// copy grabbed maps
	if ( isgrabbing ) {
		for ( int i=int( maps.count() -1 ); i>=0; i-- )
			if ( maps.at(i)->isgrabbed ) {
				Map * _map = maps.at(i);
				ungrab( maps.at(i) );
				maps.append( new Map( _map->type, _map->color ) );
				Map * m = maps.last();
				for ( int j=0; j<int( _map->_points.count() ); j++ )
					m->_points.append( new Point ( m, _map->_points.at(j)->xy + QPoint(40,20) ) );
				for ( int j=0; j<nblabels; j++ )
					m->htmlfields[j] = _map->htmlfields[j];
				grab( m );
			}
		repaint();
		emit signalStatus1( tr("Copy -> Ok") );
	}
}

/** select all maps */
void Canvas::select () {
	if ( !isdrawing && !ismoving && !isscrolling ) {
		grab();
		repaint();
	}
}

/** delete polygonal point or full maps according to context */
void Canvas::del () {
	QPoint realpos = mapFromGlobal( QCursor::pos() ) * 100. / zoomfactor;
	if ( isgrabbing ) { // one or several map selected
		for ( int i=int( maps.count() -1 ); i>=0; i-- )
			if ( maps.at(i)->isgrabbed )
				del( maps.at(i) );
		repaint();
	} else {	// try to delete a single point
		// no actions needed
		if ( !isdrawing && !ismoving && !isgrabbing && !isscrolling ) {
			Point * _point = searchNearestPoint( realpos );
			if ( _point != NULL ) { // control point selected
				Map * _map = _point->_parentmap;
				if ( _map->type!='P'
					|| (_map->type=='P' && _map->_points.count()<4) ) // delete map
						del( _map );
				else	// polygon with more than 3 points
					del( _point );
				repaint();
			}
		}
	}
}

/** add points to polygon */
void Canvas::add () {
	QPoint realpos = mapFromGlobal( QCursor::pos() ) * 100. / zoomfactor;
	Point * _p;
	int j;
	// no actions needed
	if ( !isdrawing && !ismoving && !isgrabbing && !isscrolling ) {
		Point * _point = searchNearestPoint( realpos );
		if ( _point != NULL ) { // control point founded
			Map * _map = _point->_parentmap;
			if ( _map->type == 'P' ) {
				j = findPoint( _point );
				if ( j )
					_p = new Point ( _map, (_point->xy + _map->_points.at(j-1)->xy)/2 );
				else
					_p = new Point ( _map, (_point->xy + _map->_points.last()->xy)/2 );
				_map->_points.insert( j, _p );
				j = findPoint( _point );
				if ( j < int(_map->_points.count()-1) )
					_p = new Point ( _map, (_point->xy + _map->_points.at(j+1)->xy)/2 );
				else
					_p = new Point ( _map, (_point->xy + _map->_points.first()->xy)/2 );
				_map->_points.insert( j+1, _p );
				repaint();
				emit signalStatus1( tr("Points added") );
			}
		}
	}
}

/** show or hide control points */
void Canvas::slotControlPoints ( bool b ) {
	hidecontrolpoint = !hidecontrolpoint;
	if ( !isdrawing && !ismoving ) {
		repaint();
	}
	if ( hidecontrolpoint )
		signalStatus1( "Hide control points" );
	else
		signalStatus1( "Show control points" );
}

/** handle actions on selected maps */
void Canvas::slotMapAction ( int i ) {
	switch ( i ) {
		case 1: select(); break; // select all map
		case 2: del(); break; // delete selected maps
		case 3: copy(); break; // copy selected maps
		case 4: gotoMap(0); break; // go to first map
		case 5: gotoMap(1); break; // go to prev map
		case 6: gotoMap(2); break; // go to next map
		case 7: gotoMap(3); break; // go to last map
		case 8: edit(); break; // go to edit mode
	}
}

/** handle mouse wheel events */
void Canvas::wheelEvent ( QWheelEvent * event ) {
	QPoint p(0, 0);
	event->accept();
	if ( event->modifiers() == Qt::NoModifier )
		p = QPoint( 0, -event->delta() );
	if ( event->modifiers() == Qt::AltModifier )
		p = QPoint( event->delta(), 0 );
	emit signalScrollBy ( p );
	repaint();
}

/** show current map type in status2 */
void Canvas::showMapType ( char c ) {
	switch (c) {
		case 'R': emit signalStatus2( "Rectangle" ); break;
		case 'C': emit signalStatus2( "Circle (center+radius)" ); break;
		case 'E': emit signalStatus2( "Ellipse (rectangle)" ); break;
		case 'L': emit signalStatus2( "Ellipse (center+radius)" ); break;
		case 'P': emit signalStatus2( "Polygon" ); break;
	}
}

/** update the last segment of the current map */
void Canvas::drawSegment () {
	if ( _currentmap != NULL )
		if ( _currentpoint != NULL ) {
				int i = findPoint( _currentpoint );
				if ( i != -1 ) {
					// if current point is not the first one or the last one,
					// draw the two respective segments
					if ( i > 0 )
						drawPoints( _currentmap->_points.at(i-1), _currentmap->_points.at(i) );
					if ( i < int(_currentmap->_points.count()-1) )
						drawPoints( _currentmap->_points.at(i), _currentmap->_points.at(i+1) );
					// for polygon, draw the segment form the first to the last point
					if ( _currentmap->type == 'P' && ismoving )
						if ( i==0 || i==int(_currentmap->_points.count()-1) )
						 drawPoints( _currentmap->_points.first(), _currentmap->_points.last() );
				}
		}
	repaint();
}

/** go to edit mode after a keypress event */
void Canvas::edit () {
	if ( isgrabbing && !ismoving )
		emit signalFocusText();
}

/** abort current action */
void Canvas::abort () {
	// delete polygon if nbpoints<3...
	if ( isdrawing && mapcurrenttype=='P' ) {
		_currentmap = maps.last();
		if (_currentmap->_points.count() < 3 )
			del( _currentmap );
	}
	ungrab();
	repaint();
}

/** print current maps to printer */
void Canvas::printMaps() {

	qDebug() << "printMaps:";

	QPrinter printer(QPrinter::HighResolution);

	printer.setFullPage( true );
	printer.setPageSize( QPrinter::A4 );
	printer.setOrientation( QPrinter::Portrait );
	printer.setColorMode( QPrinter::Color );
	printer.setDocName( "KMap" );
	printer.setCreator( "KMap" );
	printer.setOutputFileName( "/tmp/kmap.pdf" );
	printer.setResolution( 300 ); // dpi

// 
	int w = (double)qMax( bufferPicture.width(), bufferMaps.width() );
	int h = (double)qMax( bufferPicture.height(), bufferMaps.height() );

	if ( printer.setup(this) ) {
		QPainter painter;
		if( painter.begin( &printer ) ) {
			double xscale = printer.pageRect().width()/w;
			double yscale = printer.pageRect().height()/h;
			double scale = qMin(xscale, yscale);
			painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
														printer.paperRect().y() + printer.pageRect().height()/2);
			painter.scale(scale, scale);
			painter.translate(-w/2, -h/2);

			//QFont font( "arial", 12 );
			//QFontMetrics fm = painter.fontMetrics();

			qDebug() << "printMaps2";
			painter.drawPixmap( 0, 0, bufferPicture );
			drawMaps();
			painter.drawPixmap( 0, 0, bufferMaps );
			qDebug() << "end printing";
			painter.end();
		}
	}
}
