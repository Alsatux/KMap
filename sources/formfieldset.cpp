/***************************************************************************
                          formfieldset.cpp  -  description
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

#include "formfieldset.h"

FormFieldSet::FormFieldSet( QString label, int isvisible ) {
	// create new label and linedit fields
	qlabel = new QLabel ( label );
	qlineedit = new QLineEdit;
	qlineedit -> setMaximumHeight( 40 );
	visible = ( isvisible ? true : false );
	display();
}

FormFieldSet::~FormFieldSet() {
}

/** toggle fieldset */
void FormFieldSet::toggle() {
	visible = !visible;
	display();
}

/** show or hide from visible */
void FormFieldSet::display() {
	// show or hide field
	visible ? show() : hide();
}

/** show */
void FormFieldSet::show() {
	visible = true;
	qlabel->show();
	qlineedit->show();
}

/** hide */
void FormFieldSet::hide() {
	visible = false;
	qlabel->hide();
	qlineedit->hide();
}
