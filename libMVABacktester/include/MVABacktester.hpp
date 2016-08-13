/*
* Copyright (C) 2007, Alberto Giannetti
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

#ifndef _MVABACKTESTER_HPP_
#define _MVABACKTESTER_HPP_ 1

// STL
#include <vector>
#include <string>
#include <set>

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>

// Hudson
#include <EODSeries.hpp>
#include <Trader.hpp>
#include <IndicatorApp.hpp>
#include "TMVAReader.hpp"

// ROOT
#include "TRandom3.h"

//! Asset Allocator Trader.
/*!
  MVABacktester trades using a evaluated MVA output. SP500.
*/
class MVABacktester: public Trader
{
public:
  enum Type { MVABasic = 0, Random, ProbTransform };
  /*!
    The constructor gets historical data series for different asset classes.
    \param spx_db SP500 historical data
  */
  explicit MVABacktester(const Series::EODSeries& db, const IndicatorApp app, const std::string& mva, const Float_t& cutValue=0.1 );

  //! Run the trader.
  void run( const unsigned& dayshift = 7, const MVABacktester::Type& type = MVABasic ) throw(TraderException);
  
  //! set the seed of the random variable.
  void setSeed( const UInt_t& seed = 7 ) ;


  /*!
    Set the initial startup required for running.
    \param set containing pairs of indicators to apply to the dataset.
    \param sma simple moving average series
  */
  void setup( const std::vector< std::string >& inputvars, const std::set< std::pair< std::string, std::string > >& leaves, const std::string& outputFile = "outputFile.root", const std::string& weightsDirPrefix = "weights");

  void setCutValue( const Float_t& value ) { m_cutValue = value; }
  /*!
    Run trading loop over select calendar period. Called for each asset class.
    \param db historical data
    \param iter begin of trading period
    \param sma simple moving average series
  */
private:
  void trade( Series::EODSeries::const_iterator& iter);
  /*!
    Execute the buy strategy
    \param db historical data
    \param iter begin of trading period
    \param sma Simple moving average series
    \param i begin of SMA series matching iter
  */
  void check_buy( Series::EODSeries::const_iterator& iter, const Float_t& value );
  /*!
    Execute the sell strategy
    \param db historical data
    \param iter begin of trading period
    \param sma Simple moving average series
    \param i begin of SMA series matching iter   
  */  
//  void check_sell(const Series::EOMSeries& db, Series::EOMSeries::const_iterator& iter, const TA::MACDRes& sma, int i);

private:

  const Series::EODSeries& m_db;
  IndicatorApp m_app;
  std::string m_mvaName;
  unsigned m_dayshift;
  Float_t m_cutValue;
  bool m_setup;
  NN::TMVAReader* m_reader;
  std::vector<std::string> m_inputvars;
  std::set< std::pair< std::string, std::string > > m_leaves;
  TRandom3 m_random;
  Type m_type;
};

#endif // _MVABacktester_HPP_
