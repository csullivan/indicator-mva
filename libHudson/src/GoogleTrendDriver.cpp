/*
* Copyright (C) 2007, Alberto Giannetti
*
* $Id: GoogleTrendDriver.cpp,v1.1 2013-07-28  mreid Exp $
*
* This file is part of Hudson.
*
* Hudson is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hudson is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Hudson.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "StdAfx.hpp"

// STDLIB
#include <cstdlib>

// Boost
#include <boost/tokenizer.hpp>

// Series
#include "GoogleTrendDriver.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;


Series::GoogleTrendDriver::GoogleTrendDriver(void):
  m_linenum(0)
{
}


Series::GoogleTrendDriver::~GoogleTrendDriver(void)
{
  if( m_infile.is_open() )
	  m_infile.close();
}


bool Series::GoogleTrendDriver::open(const std::string& filename)
{
  // Check if another file was open previously
  if( m_infile.is_open() ) {
	  m_infile.close();
	  m_linenum = 0;
  }

  m_infile.open(filename.c_str());
  if( m_infile.is_open() == false )
	return false;

  // First line is header line
  getline(m_infile, m_line);
  m_linenum = 1;

  return m_infile.good();
}


void Series::GoogleTrendDriver::close(void)
{
  if( m_infile.is_open() )
	  m_infile.close();

  m_infile.clear();

  m_linenum = 0;
}


bool Series::GoogleTrendDriver::next( DayPrice& dp) throw(Series::DriverException)
{
  if( m_infile.eof() )
  	return false;

  typedef tokenizer<boost::char_separator<char> > TokensRdr;
  getline(m_infile, m_line);
  
  ++m_linenum; // skips headers

  dp.adjclose = 0;
  dp.close = 0;
  dp.volume = 0;
  dp.high = 0;
  dp.low = 0;
  dp.key = date(); // not_a_date_time
  dp.open = 0;

  // Get line fields
  int i = 0;
  TokensRdr tok(m_line, boost::char_separator<char>(" ,\t\n\r"));
  for( TokensRdr::iterator iter = tok.begin(); iter != tok.end(); ++iter, ++i ) {

	  string field = *iter;		// extract line field (.csv file)

	  switch( i ) {

	    case DATE: {
	      dp.key = from_string(field);
	      if( dp.key.is_not_a_date() ) {
		      stringstream ss;
		      ss << "Invalid key at line " << m_linenum;
		      throw DriverException(ss.str());
	      }
	    }
	    break;

	    case VOLUME:
	      dp.volume = dp.high = dp.low = dp.open = dp.close = dp.adjclose = atoi(field.c_str());
	      break;

	    default: {
		    stringstream ss;
		    ss << "Unknown field at line " << m_linenum;
		    throw DriverException(ss.str());
	    }
	  } // token type switch
  } // for each token in line

  return true;
}


bool Series::GoogleTrendDriver::eof(void)
{
  return m_infile.eof();
}

