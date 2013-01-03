/***************************************************************************
                          formfieldset.h  -  description
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

#ifndef FORMFIELDSET_H
#define FORMFIELDSET_H

#include <qlabel.h>
#include <qlineedit.h>

class FormFieldSet {
public:
	FormFieldSet( QString label, int isvisible );
	~FormFieldSet();
	/** toggle fieldset */
	void toggle();
	/** show or hide from visible */
	void display();
	/** show */
	void show();
	/** hide */
	void hide();
	QLabel * qlabel;
	QLineEdit * qlineedit;
	bool visible;
};

#endif
