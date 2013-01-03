/***************************************************************************
                          kmap.cpp  -  description
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

#include "kmap.h"

// list of HTML entities with default visible value
QString htmllabel[] = {"HREF", "ALT", "ACCESSKEY", "CLASS", "ID", "LANG", "ONBLUR", "ONCLICK", "ONDBLCLICK", "ONFOCUS", "ONKEYDOWN", "ONKEYPRESS", "ONKEYUP", "ONMOUSEDOWN", "ONMOUSEMOVE", "ONMOUSEOUT", "ONMOUSEOVER", "ONMOUSEUP", "STYLE", "TABINDEX", "TABORDER", "TARGET", "TITLE" };
int showlabel[] = {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int nblabels = 23;

Kmap::Kmap( QWidget *parent ): QWidget(parent) {

	// layout the layout from top to bottom
	QVBoxLayout * qvboxlayout1 = new QVBoxLayout( this );

	// qscrollview + CANVAS
	qscrollview = new Q3ScrollView;
	_canvas = new Canvas( this );
	_canvas->setCursor( Qt::CrossCursor );
	qscrollview->addChild( _canvas, 0, 0 );
	qvboxlayout1->addWidget( qscrollview, 10 );

	QBoxLayout * qboxlayout1 = new QHBoxLayout;
	qvboxlayout1->addLayout( qboxlayout1, 1 );

	// QButtonGroup make no visual change - just group some properties
	QButtonGroup * qbuttongroup1 = new QButtonGroup;

	QPixmap square( 20, 20 );
	for ( int i=0; i<16; i++ ) {
		QPushButton * qpushbutton1 = new QPushButton;
		qpushbutton1->setFixedSize( 22, 22 );
		square.fill( _qtcolor[i] );
		qpushbutton1->setIcon( square );
		QToolTip::add( qpushbutton1, "set map color" );
		qboxlayout1->addWidget( qpushbutton1, 1 );
		qbuttongroup1->addButton( qpushbutton1, i+1 );
	}

	qvboxlayout1->addLayout( qboxlayout1, 10 );
	connect( qbuttongroup1, SIGNAL( buttonClicked(int) ), this, SLOT( slotChangeMapCurrentColor(int) ) );

	// HTML FIELDSETS
	QGridLayout * qgridlayout1 = new QGridLayout;
	qvboxlayout1->addLayout( qgridlayout1, 1 );
	for ( int i=0; i<nblabels; i++ ) {
		// create new label and linedit fields
		_formfieldset.append( new FormFieldSet( htmllabel[i], showlabel[i] ) );
		qgridlayout1-> addWidget( _formfieldset.at(i)->qlabel, i, 0 );
		qgridlayout1-> addWidget( _formfieldset.at(i)->qlineedit, i, 1 );
		// connect to application
		connect( _formfieldset.at(i)->qlineedit, SIGNAL( textChanged(const QString &) ),
				this, SLOT(slotTextChanged(const QString &)) );
	}

	// SIGNALS / SLOTS
	connect( _canvas, SIGNAL( signalChangeText(QStringList) ), this, SLOT( slotChangeText(QStringList) ) );
	connect( _canvas, SIGNAL( signalFocusText() ), this, SLOT( slotFocusText() ) );
	connect( _canvas, SIGNAL( signalMouseScrolling(QPoint) ), this, SLOT( slotMouseScrolling(QPoint) ) );
	connect( _canvas, SIGNAL( signalScrollBy(QPoint) ), this, SLOT( slotScrollBy(QPoint) ) );
	connect( _canvas, SIGNAL( signalCenterView (double,double) ), this, SLOT( slotCenterView(double,double) ) );
}

Kmap::~Kmap()
{
}

/** print current picture with mapped parts */
void Kmap::slotFilePrint () {
	_canvas->printMaps();
}

/** change current map color */
void Kmap::slotChangeMapCurrentColor( int color ) {
	qDebug() << "slotChangeMapCurrentColor";
	_canvas->mapcurrentcolor = _qtcolor[ color-1 ];
	for ( int i=0; i<int( _canvas->maps.count() ); i++ )
		if ( _canvas->maps.at(i)->isgrabbed )
			_canvas->maps.at(i)->color = _canvas->mapcurrentcolor;
	QString s;
	s = QString("Color: %1").arg(_canvas->mapcurrentcolor.name());
	signalStatus1( s.toUtf8() );
}

/** change current draw color */
void Kmap::slotChangeDrawSelectionColor( int color ) {
	_canvas->drawselectioncolor = _qtcolor[ color ];
}

/** change HTML entries */
void Kmap::slotChangeText ( QStringList htmlfields ) {
	for (int i=0; i<nblabels; i++ ) {
		_formfieldset.at(i)->qlineedit->blockSignals( TRUE ); // disable signal textChanged()
		_formfieldset.at(i)->qlineedit->setText( htmlfields.at(i) );
		_formfieldset.at(i)->qlineedit->blockSignals( FALSE ); // enable signal textChanged()
	}
}

/** handle scrolling events from canvas */
void Kmap::slotMouseScrolling( QPoint offset ) {
	qscrollview->scrollBy( offset.x(), offset.y() );
}


/** scroll to new position */
void Kmap::slotScrollBy( QPoint pos ) {
	qscrollview -> scrollBy( pos.x(), pos.y() );
}

/** show/hide HTML fields */
void Kmap::slotToggleFormFieldSet( int i ) {
	qDebug() << "slotToggleFormFieldSet: " << i;
	_formfieldset.at(i)->toggle();
	repaint();
}

/** show/hide HTML fields */
void Kmap::slotShowFormFieldSet( int i ) {
	qDebug() << "slotShowFormFieldSet: " << i;
	_formfieldset.at(i)->show();
	repaint();
}

/** change current data for all selected maps */
void Kmap::slotTextChanged ( const QString &text ) {
	QWidget * qwidget = focusWidget();
	for ( int i=0; i<nblabels; i++ )
		if ( qwidget == _formfieldset.at(i)->qlineedit )
			_canvas->updHtmlFieldForSelectedMaps( i, text );
}

/** update qscrollview according to current edited object */
void Kmap::slotCenterView ( double x, double y ) {
	int i = int( x*_canvas->zoomfactor/100. );
	int j = int( y*_canvas->zoomfactor/100. );
	qscrollview->center( i, j );
	_canvas->repaint();
}

/** give focus to HREF */
void Kmap::slotFocusText () {
	_formfieldset[0]->qlineedit->setFocus();
}

/** set zoom factor */
void Kmap::slotZoom ( int zoom ) {
	qDebug() << "slotZoom: ";
	// record center of the qscrollview as percentage
	double x = ((double)qscrollview->contentsX() + (double)qscrollview->visibleWidth()/2)/(double)qscrollview->contentsWidth();
	double y = ((double)qscrollview->contentsY() + (double)qscrollview->visibleHeight()/2)/(double)qscrollview->contentsHeight();
	qDebug() << "x=" << x << "% y=" << y << "%";
	// update canvas
	_canvas->zoom( (double) zoom );
	// zoom will change contents size
	int w = _canvas->width();
	int h = _canvas->height();
	qscrollview->resizeContents( w, h );
	// new qscrollview position
	qscrollview->center( (int)(x*w), (int)(y*h) );
}

/** handle events of menu Maps */
void Kmap::slotMaps ( int i ) {
	switch( i ) {
		case 1: _canvas->select(); break;
		case 2: _canvas->del(); break;
		case 3: _canvas->copy(); break;
	}
}


