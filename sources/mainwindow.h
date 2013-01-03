/***************************************************************************
                          mainwindow.h  -  description
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Qt3Support>

#include "kmap.h"
class Kmap;

#include "canvas.h"
class Canvas;

#include "formfieldset.h"
class FormFieldSet;

/** Kmap is the base class of the project */
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	/** construtor */
	MainWindow( QWidget* parent=0 );
	/** destructor */
	~MainWindow();
	/** copy html exported maps to clipboard */
	QString kmapfile; // current KMAP filename
	QString htmlfile; // current HTML filename
	QString pictfile; // current picture
	void copyClipboard ( QString filename );
private:
	Kmap * _kmap;
	QLabel * statusbar1;
	QLabel * statusbar2;
	QProcess * process; // use for external commmunication with perl wrappers
signals:
	/** load a picture to canvas */
	void signalLoadPict( QPixmap pict );
public slots:
	/** for popup messages */
	void popupInformation( QString title, QString message );
	/** help */
	void slotHelp( bool b );
	/** shortcuts help */
	void slotShortcuts( bool b );
	/** status of kmap main application */
	void slotStatus1 ( const char * message );
	/** status of childs widgets */
	void slotStatus2 ( const char * message );
	/** load an external picture with dialog box */
	void slotLoadPict ();
	/** load an external picture with direct filename */
	void slotLoadPict ( QString filename );
	/** load a new map from a KMAP format */
	void slotLoadKmap ();
	/** save current map to KMAP format */
	void slotSaveKmap ();
	/** save current work under filename */
	void slotSaveAsKmap ();
	/** import map from html file */
	void slotImportHtml ();
	/** export current map to html format */
	void slotExportHtml ();
	/** export current map to xml format */
	void slotExportXml ();
};

#endif
