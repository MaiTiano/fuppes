/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            PresentationPage.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _PRESENTATIONPAGE_H
#define _PRESENTATIONPAGE_H

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#include <string>
#include <sstream>

class PresentationPage
{
  public:
    virtual std::string alias() = 0;
    virtual std::string title() = 0;
    virtual std::string js() { return ""; }

    virtual std::string content() = 0;

};


#endif // _PRESENTATIONPAGE_H
