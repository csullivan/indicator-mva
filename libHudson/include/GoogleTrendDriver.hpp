/*
* Copyright (C) 2007, Alberto Giannetti
*
* $Id: GoogleTrendDriver.hpp,v1.1 2013-07-28  mreid Exp $
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

#ifndef _GOOGLETRENDDRIVER_HPP_
#define _GOOGLETRENDDRIVER_HPP_

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <fstream>
#include <string>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Series
#include "FileDriver.hpp"
#include "DayPrice.hpp"

namespace Series
{

  class GoogleTrendDriver: public FileDriver
  {
  public:
	  GoogleTrendDriver(void);
	  ~GoogleTrendDriver(void);

	  virtual bool open(const std::string& filename);
	  virtual void close(void);
	  virtual bool next( DayPrice& dp ) throw( DriverException );
	  virtual bool eof(void);

  private:
	  enum FIELDS_POS {
	    DATE = 0,
	    VOLUME,
	  };

  private:
	  std::ifstream m_infile;
	  std::string m_line;
	  unsigned m_linenum;
  };

} // namespace Series

#endif // _GOOGLETRENDDRIVER_HPP_
