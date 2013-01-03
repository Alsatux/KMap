/***************************************************************************
                          mainwindow.cpp  -  description
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

#include "mainwindow.h"

MainWindow::MainWindow( QWidget *parent ): QMainWindow( parent ) {

	setBackgroundRole(QPalette::Base);

	QPixmap fileopenkmap = QPixmap("16x16/actions/fileopen.png");
	QPixmap fileopenpict = QPixmap("16x16/filesystems/folder_yellow.png");
	QPixmap fileopenhtml = QPixmap("16x16/filesystems/folder_green.png");
	QPixmap filesave = QPixmap("16x16/actions/filesave.png");
	QPixmap filesavehtml = QPixmap("16x16/filesystems/folder_red.png");
	QPixmap fileprint = QPixmap("16x16/actions/fileprint.png");
	QPixmap filequit = QPixmap("16x16/actions/exit.png");
	QPixmap viewmagplus = QPixmap("16x16/actions/viewmag+.png");
	QPixmap viewmag = QPixmap("16x16/actions/viewmag+.png");
	QPixmap viewmagminus = QPixmap("16x16/actions/viewmag-.png");
	QPixmap editcopy = QPixmap("16x16/actions/editcopy.png");
	QPixmap editdel = QPixmap("16x16/actions/editdelete.png");
	QPixmap edit = QPixmap("16x16/actions/list.png");
	QPixmap tux = QPixmap("16x16/apps/devel/tux.png");
	QPixmap wizard = QPixmap("16x16/actions/wizard.png");

	kmapfile = "";
	htmlfile = "";
	pictfile = "";

	_kmap = new Kmap;
	setCentralWidget(_kmap);

	connect( _kmap->_canvas, SIGNAL( signalStatus1(const char *) ), this, SLOT( slotStatus1(const char *) ) );
	connect( _kmap->_canvas, SIGNAL( signalStatus2(const char *) ), this, SLOT( slotStatus2(const char *) ) );
	connect( _kmap, SIGNAL( signalStatus1(const char *) ), this, SLOT( slotStatus1(const char *) ) );
	connect( _kmap, SIGNAL( signalStatus2(const char *) ), this, SLOT( slotStatus2(const char *) ) );

	// MAIN MENU	FILE

	QMenu *menuFile = menuBar()->addMenu(tr("&File"));
	menuFile->addAction( QIcon(fileopenkmap), "&Open kmap", this, SLOT(slotLoadKmap()), Qt::CTRL+Qt::Key_O );
	menuFile->addAction( QIcon(filesave), "&Save", this, SLOT(slotSaveKmap()), Qt::CTRL+Qt::Key_S );
	menuFile->addAction( "&Save as...", this, SLOT(slotSaveAsKmap()) );
	menuFile->addSeparator();
	menuFile->addAction( QIcon(fileopenpict), "Open p&icture", this, SLOT(slotLoadPict()), Qt::CTRL+Qt::Key_I );
	menuFile->addSeparator();
	menuFile->addAction( "Import html", this, SLOT(slotImportHtml()) );
	menuFile->addAction( "Export html", this, SLOT(slotExportHtml()) );
	menuFile->addAction( "Export xml", this, SLOT(slotExportXml()) );
	menuFile->addSeparator();
	menuFile->addAction( QIcon(fileprint), "&Print", _kmap, SLOT(slotFilePrint()) );
	menuFile->addSeparator();
	menuFile->addAction( QIcon(filequit), "E&xit", qApp, SLOT(quit()), Qt::CTRL+Qt::Key_Q );

	// MAIN MENU MAP TYPE

	QMenu *menuMapType = menuBar()->addMenu(tr("&Type"));
	QSignalMapper * signalMapper1 = new QSignalMapper(this);

	QAction * actionMapType1 = new QAction( "&rectangle", this );
	actionMapType1->setShortcut( Qt::Key_R );
	signalMapper1->setMapping( actionMapType1, 1 );
	connect( actionMapType1, SIGNAL(triggered()), signalMapper1, SLOT(map()) );
	menuMapType->addAction( actionMapType1 );

	QAction * actionMapType2 = new QAction( "&circle (center/radius)", this );
	actionMapType2->setShortcut( Qt::Key_C );
	signalMapper1->setMapping( actionMapType2, 2 );
	connect( actionMapType2, SIGNAL(triggered()), signalMapper1, SLOT(map()) );
	menuMapType->addAction( actionMapType2 );

	// TODO: CIRCLE RECTANGLE

	QAction * actionMapType4 = new QAction( "&ellipse (rectangle)", this );
	actionMapType4->setShortcut( Qt::Key_E );
	signalMapper1->setMapping( actionMapType4, 4 );
	connect( actionMapType4, SIGNAL(triggered()), signalMapper1, SLOT(map()) );
	menuMapType->addAction( actionMapType4 );

	QAction * actionMapType5 = new QAction( "e&llipse (center/radius)", this );
	actionMapType5->setShortcut( Qt::Key_L );
	signalMapper1->setMapping( actionMapType5, 5 );
	connect( actionMapType5, SIGNAL(triggered()), signalMapper1, SLOT(map()) );
	menuMapType->addAction( actionMapType5 );

	QAction * actionMapType6 = new QAction( "&polygon", this );
	actionMapType6->setShortcut( Qt::Key_P );
	signalMapper1->setMapping( actionMapType6, 6 );
	connect( actionMapType6, SIGNAL(triggered()), signalMapper1, SLOT(map()) );
	menuMapType->addAction( actionMapType6 );

	connect( signalMapper1, SIGNAL(mapped(int)), _kmap->_canvas, SLOT(slotMapType(int)) );

	// MAIN MENU MAPS ACTIONS

	QMenu *menuMapAction = menuBar()->addMenu(tr("&Maps"));
	QSignalMapper * signalMapper2 = new QSignalMapper(this);

	QAction * actionMapAction1 = new QAction( QIcon(editcopy), "&Copy", this );
	actionMapAction1->setShortcut( Qt::CTRL+Qt::Key_C );
	signalMapper2->setMapping( actionMapAction1, 3 );
	connect( actionMapAction1, SIGNAL(triggered()), signalMapper2, SLOT(map()) );
	menuMapAction->addAction( actionMapAction1 );

	QAction * actionMapAction2 = new QAction( QIcon(editdel), "&Delete", this );
	actionMapAction2->setShortcut( Qt::Key_D );
	signalMapper2->setMapping( actionMapAction2, 2 );
	connect( actionMapAction2, SIGNAL(triggered()), signalMapper2, SLOT(map()) );
	menuMapAction->addAction( actionMapAction2 );

	menuMapAction->addSeparator();

	QAction * actionMapAction3 = new QAction( QIcon(edit), "Ed&it", this );
	actionMapAction3->setShortcut( Qt::Key_I );
	signalMapper2->setMapping( actionMapAction3, 8 );
	connect( actionMapAction3, SIGNAL(triggered()), signalMapper2, SLOT(map()) );
	menuMapAction->addAction( actionMapAction3 );

	menuMapAction->addSeparator();

	QAction * actionMapAction4 = new QAction( "Select &All", this );
	actionMapAction4->setShortcut( Qt::CTRL+Qt::Key_A );
	signalMapper2->setMapping( actionMapAction4, 1 );
	connect( actionMapAction4, SIGNAL(triggered()), signalMapper2, SLOT(map()) );
	menuMapAction->addAction( actionMapAction4 );

	menuMapAction->addSeparator();

	QAction * actionMapAction5 = new QAction( "Select &first map", this );
	actionMapAction5->setShortcut( Qt::CTRL+Qt::Key_F );
	signalMapper2->setMapping( actionMapAction5, 4 );
	connect( actionMapAction5, SIGNAL(triggered()), signalMapper2, SLOT(map()) );
	menuMapAction->addAction( actionMapAction5 );

	QAction * actionMapAction6 = new QAction( "Select &previous map", this );
	actionMapAction6->setShortcut( Qt::CTRL+Qt::Key_P );
	signalMapper2->setMapping( actionMapAction6, 5 );
	connect( actionMapAction6, SIGNAL(triggered()), signalMapper2, SLOT(map()) );
	menuMapAction->addAction( actionMapAction6 );

	QAction * actionMapAction7 = new QAction( "Select &next map", this );
	actionMapAction7->setShortcut( Qt::CTRL+Qt::Key_N );
	signalMapper2->setMapping( actionMapAction7, 6 );
	connect( actionMapAction7, SIGNAL(triggered()), signalMapper2, SLOT(map()) );
	menuMapAction->addAction( actionMapAction7 );

	QAction * actionMapAction8 = new QAction( "Select &last map", this );
	actionMapAction8->setShortcut( Qt::CTRL+Qt::Key_L );
	signalMapper2->setMapping( actionMapAction8, 7 );
	connect( actionMapAction8, SIGNAL(triggered()), signalMapper2, SLOT(map()) );
	menuMapAction->addAction( actionMapAction8 );

	connect(signalMapper2, SIGNAL(mapped(int)), _kmap->_canvas, SLOT(slotMapAction(int)));

	// MAIN MENU POINTS

	QMenu *menuPoints = menuBar()->addMenu(tr("&Points"));
	menuPoints->addAction( QIcon(wizard), "Show/&hide control points", _kmap->_canvas, SLOT( slotControlPoints(bool) ), Qt::Key_H );

	// MAIN MENU FIELDS

	QMenu *menuFields = menuBar()->addMenu(tr("F&ields"));
	QSignalMapper * signalMapper4 = new QSignalMapper(this);

	for ( int i=0; i<nblabels; i++ ) {
		QString s1( htmllabel[i] );
		QAction * actionFields = new QAction( s1, this );
		signalMapper4->setMapping( actionFields, i );
		connect( actionFields, SIGNAL(triggered()), signalMapper4, SLOT(map()) );
		menuFields->addAction( actionFields );
	}

	connect(signalMapper4, SIGNAL(mapped(int)), _kmap, SLOT(slotToggleFormFieldSet(int)));

	// MAIN MENU ZOOM

	QMenu *menuZoom = menuBar()->addMenu(tr("&Zoom"));
	QSignalMapper * signalMapper5 = new QSignalMapper(this);

	QAction * actionZoom1 = new QAction( QIcon(viewmagminus), "&50%", this );
	signalMapper5->setMapping( actionZoom1, 50 );
	connect( actionZoom1, SIGNAL(triggered()), signalMapper5, SLOT(map()) );
	menuZoom->addAction( actionZoom1 );

	QAction * actionZoom2 = new QAction( QIcon(viewmag), "&100%", this );
	signalMapper5->setMapping( actionZoom2, 100 );
	connect( actionZoom2, SIGNAL(triggered()), signalMapper5, SLOT(map()) );
	menuZoom->addAction( actionZoom2 );

	QAction * actionZoom3 = new QAction( QIcon(viewmagplus), "&200%", this );
	signalMapper5->setMapping( actionZoom3, 200 );
	connect( actionZoom3, SIGNAL(triggered()), signalMapper5, SLOT(map()) );
	menuZoom->addAction( actionZoom3 );

	connect(signalMapper5, SIGNAL(mapped(int)), _kmap, SLOT(slotZoom(int)));

	// MAIN MENU HELP

	QMenu *menuhelp = menuBar()->addMenu(tr("&Help"));
	menuhelp->addAction( QIcon(tux), "A&bout", this, SLOT( slotHelp(bool) ), Qt::Key_B );
	menuhelp->addAction( "Shortcuts", this, SLOT( slotShortcuts(bool) ) );

	// STATUS BAR

	statusbar1 = new QLabel;
	statusbar1 -> setFrameStyle( QFrame::Panel | QFrame::Sunken );
	statusbar1 -> setMaximumHeight( 20 );
	statusBar()->addPermanentWidget( statusbar1, 1 );

	statusbar2 = new QLabel;
	statusbar2 -> setFrameStyle( QFrame::Panel | QFrame::Sunken );
	statusbar2 -> setMaximumHeight( 20 );
	statusBar()->addPermanentWidget( statusbar2, 1 );

	/*
		// TODO !
		QToolBar *toolbar1 = addToolBar(tr("File"));
		toolbar1->addAction( QIcon( fileopenhtml ), "Open HTML", this, SLOT(slotLoadKmap()) );
		toolbar1->addAction( QIcon( filesave ), "Save Maps", this, SLOT(slotSaveKmap()) );
		toolbar1->addAction( QIcon( fileprint ), "Print", this, SLOT(slotFilePrint()) );
		toolbar1->addAction( QIcon( filequit ), "Quit", qApp, SLOT(quit()) );

	*/

	connect( this, SIGNAL( signalLoadPict(QPixmap) ), _kmap->_canvas, SLOT( slotLoadPict(QPixmap) ) );
	connect( _kmap->_canvas, SIGNAL( signalLoadPict(QString) ), this, SLOT( slotLoadPict(QString) ) );
}

MainWindow::~MainWindow()
{
}

/** for popup messages */
void MainWindow::popupInformation( QString title, QString message ) {
	QMessageBox mb( title,
	message,
	QMessageBox::Information,
	QMessageBox::NoButton,
	QMessageBox::No |QMessageBox::Escape,
	QMessageBox::NoButton
	);
	mb.exec();
}

/** help */
void MainWindow::slotHelp( bool b ) {
	popupInformation( tr("About"),
	"<h4>KMAP - a KDE Imagemap Editor - v2.0</h4>"
	"<p>(c) 2012 - Jean Luc Biellmann</p>"
	"<p>contact@alsatux.com</p>"
	);
}

/** shortcuts help */
void MainWindow::slotShortcuts( bool b ) {
	popupInformation( tr("Shortcuts"),
	"<p>On drawing:</p>"
	"<ul><li>H: show/hide control points</li>"
	"<li>R: select rectangle tool</li>"
	"<li>C: select circle tool</li>"
	"<li>E: select ellipse/rectangle tool</li>"
	"<li>L: select ellipse/center-radius tool</li>"
	"<li>P: select polygon tool</li></ul>"
	"<p>On a polygon control point:</p>"
	"<ul><li>A: add two points</li>"
	"<li>D: delete the point</li></ul>"
	"<p>On selected map(s) (group operations):</p>"
	"<ul><li>CTRL+C: copy</li>"
	"<li>D: delete</li>"
	"<li>I: edit</li></ul>"
	"<p>On the canvas:</p>"
	"<ul><li>arrows: move to top, right, bottom or left</li>"
	"<li>page up/down: move to top or bottom</li></ul>"
	"<p>ESC : abort current operation</p>"
	);
}

/** status of kmap main application */
void MainWindow::slotStatus1 ( const char * message ) {
	QString s(message);
	statusbar1->clear();
	statusbar1->setText( s );
}

/** status of childs widgets */
void MainWindow::slotStatus2 ( const char * message ) {
	QString s(message);
	statusbar2->clear();
	statusbar2->setText( s );
}

/** load an external picture with dialog box */
void MainWindow::slotLoadPict () {
	QString filename = QFileDialog::getOpenFileName( this, tr("Load a picture"), kmapfile, tr("Pictures")+" (*.png *.xpm *.jpg)" );
	slotLoadPict( filename );
}

/** load an external picture with direct filename */
void MainWindow::slotLoadPict ( QString filename ) {
	QPixmap pict;
	if ( !filename.isEmpty() ) {
		if ( !pict.load(filename) ) {
			QMessageBox::warning( 0, tr("Error"), tr("Cannot load the file") );
		} else {
			pictfile = filename;
			emit signalLoadPict( pict );
		}
		slotStatus1( tr("Picture loaded") );
		slotStatus2( QString( tr("Picture") + ": %1x%2" ).arg(pict.width()).arg(pict.height()) );
	} else {
		slotStatus1( tr("No file to load ?") );
		slotStatus2( "" );
	}
}

/** load a new map from a KMAP file */
void MainWindow::slotLoadKmap () {
	QString filename = QFileDialog::getOpenFileName( this, tr("Load kmap file"), kmapfile, "Kmap (*.kmap)" );
	if ( !filename.isEmpty() ) {
		kmapfile = filename;
		slotStatus1( tr("Reading current maps") );
		if ( _kmap->_canvas->loadKmapFile( kmapfile ) )
			slotStatus1( tr("Read -> Ok") );
		else
			slotStatus1( tr("Read -> Failed") );
	}
}

/** save current map to KMAP format */
void MainWindow::slotSaveKmap () {
	if ( kmapfile.isEmpty() ) {
		slotSaveAsKmap();
	} else {
		slotStatus1( tr("Saving current maps") );
		_kmap->_canvas->saveMaps2Kmap( kmapfile, pictfile );
		slotStatus1( tr("Save -> Ok") );
	}
}

/** save current work under filename */
void MainWindow::slotSaveAsKmap () {
	qDebug() << "slotSaveAsKmap: ";
	QString filename = QFileDialog::getSaveFileName( this, tr("Save as"), kmapfile, "Kmap (*.kmap)" );
	if ( !filename.isEmpty() ) {
		if ( !filename.endsWith(".kmap") )
			filename += ".kmap";
		kmapfile = filename;
		slotStatus1( tr("Saving current maps") );
		_kmap->_canvas->saveMaps2Kmap( kmapfile, pictfile );
		slotStatus1( tr("Save -> Ok") );
	}
}

/** copy html exported maps to clipboard */
void MainWindow::copyClipboard ( QString filename ) {
	QFile f( filename );
	if ( !f.open( QIODevice::ReadOnly ) )
		return;
	QClipboard *clipboard = QApplication::clipboard();
	QTextStream ts( &f );
	clipboard->setText( ts.readAll() );
	f.close();
}

// IMPORT AND EXPORT

/** import map from html file */
void MainWindow::slotImportHtml () {
	qDebug() << "slotImportHtml: ";
	QString htmlfilename = QFileDialog::getOpenFileName( this, tr("Import HTML file"), "", "Html (*html)" );
	if ( !htmlfilename.isEmpty() ) {
		if ( !htmlfilename.endsWith( ".html" ) )
				htmlfilename += ".html";
		htmlfile = htmlfilename;
		if ( _kmap->_canvas->loadHtmlFile( htmlfile ) ) {
			slotStatus1( tr("Reading maps") );
		} else {
			slotStatus1( tr("Import -> Failed") );
		}
	}
}

/** export current map to html format */
void MainWindow::slotExportHtml () {
	qDebug() << "slotExportHtml: ";
	QString htmlfilename = QFileDialog::getSaveFileName( this, tr("Export to HTML"), "", "Html (*html)" );
	if ( !htmlfilename.isEmpty() ) {
		if ( !htmlfilename.endsWith( ".html" ) )
				htmlfilename += ".html";
		htmlfile = htmlfilename;
		slotStatus1( tr("Saving maps") );
		if ( _kmap->_canvas->saveMaps2Html( htmlfile, pictfile ) )
			slotStatus1( tr("Export -> Ok") );
		else
			slotStatus1( tr("Export -> Failed") );
	}
}

/** export current map to xml format */
void MainWindow::slotExportXml () {
	QString xmlfilename = QFileDialog::getSaveFileName( this, tr("Export to XML"), "", "Xml (*.xml)" );
	if ( !xmlfilename.isEmpty() ) {
		if ( !xmlfilename.endsWith( ".xml" ) )
				xmlfilename += ".xml";
		if ( _kmap->_canvas->saveMaps2Xml( xmlfilename, pictfile ) )
			slotStatus1( tr("Save Xml -> Ok") );
		else
			slotStatus1( tr("Save Xml -> Failed") );
	}
}



