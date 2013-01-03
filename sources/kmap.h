/***************************************************************************
                          kmap.h  -  description
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

#ifndef KMAP_H
#define KMAP_H

//#ifdef HAVE_CONFIG_H
//#include <config.h>
//#endif

#include "mainwindow.h"
class MainWindow;

#include "canvas.h"
class Canvas;

#include "formfieldset.h"
class FormFieldSet;

// default qt colors values and names
const QColor _qtcolor[17] = {
    QColor( Qt::black ),
    QColor( Qt::darkGray ),
    QColor( Qt::gray ),
    QColor( Qt::lightGray ),
    QColor( Qt::white ),
    QColor( Qt::red ),
    QColor( Qt::green),
    QColor( Qt::blue ),
    QColor( Qt::cyan ),
    QColor( Qt::magenta ),
    QColor( Qt::yellow ),
    QColor( Qt::darkRed ),
    QColor( Qt::darkGreen ),
    QColor( Qt::darkBlue),
    QColor( Qt::darkCyan ),
    QColor( Qt::darkMagenta ),
    QColor( Qt::darkYellow ),
};

/** Kmap is the base class of the project */
class Kmap : public QWidget
{
	Q_OBJECT
public:
	/** construtor */
	Kmap( QWidget* parent=0 );
	/** destructor */
	~Kmap();
	Canvas * _canvas;
	Q3ScrollView * qscrollview;
private:
	// formular fields
	QList<FormFieldSet *> _formfieldset;
public slots:
	/** print current picture with mapped parts */
	void slotFilePrint ();
	/** change current draw color */
	void slotChangeDrawSelectionColor( int color );
	/** change current map color */
	void slotChangeMapCurrentColor( int color );
	/** change HTML entries */
	void slotChangeText ( QStringList _htmlfields );
	/** handle scrolling events from canvas */
	void slotMouseScrolling( QPoint offset );
	/** scroll to new position */
	void slotScrollBy( QPoint pos );
	/** show/hide HTML fields */
	void slotToggleFormFieldSet( int i );
	/** show HTML fields */
	void slotShowFormFieldSet( int i );
	/** change current data for all selected maps */
	void slotTextChanged ( const QString & );
	/** update qscrollview according to current edited object */
	void slotCenterView ( double x, double y );
	/** give focus to HREF */
	void slotFocusText ();
	/** set zoom factor */
	void slotZoom ( int zoom );
	/** handle events of menu Maps */
	void slotMaps ( int i );

signals: // Signals
	/** send text to status 1 */
	void signalStatus1( const char * text );
	/** send text to status 2 */
	void signalStatus2( const char * text );
};

#endif
