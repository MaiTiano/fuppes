/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            fuppes_db_connection_plugin.h
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2009-2010 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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

#ifndef _FUPPES_DB_PLUGIN_H
#define _FUPPES_DB_PLUGIN_H

#include <map>
#include <list>
#include <string>
#include <sys/types.h>
#include <stdlib.h>

#include <iostream>

#include "fuppes_types.h"


class CDatabaseConnection;

class CSQLResult
{
	public:
		virtual ~CSQLResult() {	}
		
    virtual bool isNull(std::string fieldName) = 0;

    // returns the value as a string
		virtual std::string	asString(std::string fieldName) = 0;

    // returns the values as an unsigned integer
		virtual unsigned int asUInt(std::string fieldName) = 0;
    // return the value as an integer
		virtual int asInt(std::string fieldName) = 0;
    
		virtual CSQLResult* clone() = 0;
};

class ISQLQuery
{
	public:
		virtual ~ISQLQuery() { }

		virtual bool select(const std::string sql) = 0;
		virtual bool exec(const std::string sql) = 0;
		virtual fuppes_off_t insert(const std::string sql) = 0;
		
		virtual bool eof() = 0;
		virtual void next() = 0;
		virtual CSQLResult* result() = 0;
		virtual fuppes_off_t lastInsertId() = 0;
		virtual void clear() = 0;
		
		virtual CDatabaseConnection* connection() = 0;

   virtual unsigned int size() = 0;
};

struct CConnectionParams
{
	std::string type;
	std::string filename;
	std::string hostname;
	std::string username;
	std::string password;
	std::string dbname;
  bool        readonly;
	
	CConnectionParams& operator=(const CConnectionParams& params) {
		type = params.type;
		filename = params.filename;
		hostname = params.hostname;
		username = params.username;
		password = params.password;
		dbname   = params.dbname;
    readonly = params.readonly;
		return *this;
	}
};


enum fuppes_sql_no {
  SQL_UNKNOWN               = 0,

  // browse
  SQL_COUNT_CHILD_OBJECTS   = 1,
  SQL_GET_CHILD_OBJECTS     = 2,
  SQL_GET_OBJECT_TYPE       = 3,
  SQL_GET_OBJECT_DETAILS    = 4,
  SQL_GET_ALBUM_ART_DETAILS = 5,

  // search
  SQL_SEARCH_PART_SELECT_FIELDS = 6,
  SQL_SEARCH_PART_SELECT_COUNT  = 7,
  SQL_SEARCH_PART_FROM          = 8,
  SQL_SEARCH_GET_CHILDREN_OBJECT_IDS = 9,
  
  // create
  SQL_TABLES_EXIST                = 10,
  SQL_CREATE_TABLE_DB_INFO        = 11,
  SQL_SET_DB_INFO                 = 12,
  SQL_CREATE_TABLE_OBJECTS        = 13,
  SQL_CREATE_TABLE_OBJECT_DETAILS = 14,
  SQL_CREATE_INDICES              = 15,
  
  // status
  SQL_GET_OBJECT_TYPE_COUNT = 16
};

struct fuppes_sql
{
  fuppes_sql_no   no;
  const char*     sql;
};

class CDatabaseConnection
{
	public:
		virtual ~CDatabaseConnection() { }

		virtual ISQLQuery*			query() = 0;
		virtual bool connect(const CConnectionParams params) = 0;
    virtual bool setup() = 0;
    
		virtual bool startTransaction() = 0;
		virtual bool commit() = 0;
		virtual void rollback() = 0;

 		virtual const char* getStatement(fuppes_sql_no number) = 0;

    virtual void vacuum() = 0;
};





#endif // _FUPPES_DB_PLUGIN_H
