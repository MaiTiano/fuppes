/***************************************************************************
 *            UUID.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2005 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "UUID.h"
#include "Common.h"

#ifdef WIN32
#include <objbase.h> /* for CoCreateGuid() */
#endif

#ifndef WC_NO_BEST_FIT_CHARS
#define WC_NO_BEST_FIT_CHARS        0x00000400
#endif

#include <iostream>
#include <sstream>
using namespace std;

std::string GenerateUUID()
{
  stringstream sResult;

#ifdef WIN32

  /* Generate GUID */
  GUID guid;
  CoCreateGuid(&guid);
  wchar_t szTemp[64];
  
  /* Get UUID as string */
  StringFromGUID2(guid, szTemp, sizeof(szTemp));
  
  /* Convert wide char string to ansi string */
  char szUUID[64];
  int nRet = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, szTemp, sizeof(szTemp),
    szUUID, sizeof(szUUID), NULL, NULL);

  /* Set result */
  string sTmp = szUUID;
  /* remove leading and trailing brackets */
  sResult << sTmp.substr(1, sTmp.length() - 2);

#else
  
  /* todo: create real UUIDs */
  
  srand(time(0));

  int nRandom;
  stringstream sRandom;

  do {
    nRandom = (rand() % 10000) + 1;
    sRandom << nRandom;
  } while (sRandom.str().length() < 8);


  sResult << sRandom.str().substr(0, 8) << "-aabb-0000-ccdd-1234eeff0000";

#endif

  return sResult.str();
}
