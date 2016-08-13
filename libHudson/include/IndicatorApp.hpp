/*
* Copyright (C) 2007, Alberto Giannetti
*
* $Id: IndicatorApp.hpp,v1.1 2013-07-28  mreid Exp $
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

#ifndef INDICATORAPP_HPP
#define INDICATORAPP_HPP 1

#ifdef WIN32
#pragma warning (disable:4290)
#endif

// STL
#include <string>
#include <map>
#include <utility>
#include <vector>

// Hudson
#include "EODSeries.hpp"
#include "TA.hpp"

// Boost
#include <boost/date_time/gregorian/gregorian.hpp>
//#include <boost/tuple/tuple.hpp>
//#include <boost/any.hpp>

// Something more like a tool class would be better here were the indicies are
// callable by configuring a tool, this is a little bit of a cheat and labourious method
// but serves the purpose. i.e. An inherited interface where we can simply add more indices
// as and when we require them as opposed to adding them in the code here explicitly.

class IndicatorApp {

  public:

    enum DataType{
      High = 0,
      Low,
      Open,
      Close,
      AdjClose,
      Volume
      };
 
    explicit IndicatorApp( const Series::EODSeries& db );
    ~IndicatorApp();

    bool addIndicator( const std::string& indicator, const std::string& postfix = "" );
    bool addIndicator( const std::string& indicator, const int period, const double sig, const std::string& postfix = "" );
    bool addIndicator( const std::string& indicator, const int period, const double sd_up, const double sd_down, const std::string& postfix = "" );
    bool addIndicator( const std::string& indicator, const int period, const std::string& postfix = "" );
    bool addIndicator( const std::string& indicator, const int period, const int fast, const int slow, const std::string& postfix = "" );
    bool addIndicator( const std::string& indicator, const int fast, const int slow, const std::string& postfix = "" );

    //! OLD VERSION Should now use the addIndicator method.
    void add( const std::string& indicator, const double& period = -999, const double& fast = -999, const double& slow = -999 );

    //! variable transformations.
    bool addHTIT( const IndicatorApp::DataType& type = Close, const std::string& name = "" ) ;
    bool addHTDCP( const IndicatorApp::DataType& type = Close, const std::string& name = "" ) ;
    //! must be added by hand 
    bool addCORREL( const TA::vDouble& series2, const unsigned& period = -999, const std::string& name = "");


    // does not require a period.
    bool addBOP( const std::string& name = "" ) ;
    
    // period only indicators
    bool addSMA( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addEMA( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addGTND( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addMFI( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addCMO( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addADX( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addWILLR( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addRSI( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addCCI( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addROCP( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addROCR( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addROC( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addMOM( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addLSLR( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addLSLR_C( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addLSLR_M( const unsigned& period = -999, const std::string& name = "" ) ;
    bool addFACTORS( const unsigned& period = -999, const std::string& name = "" ) ;

    bool addMACD( const unsigned& period = -999, const unsigned& fast = -999, const unsigned& slow = -999, const std::string& name = "" ) ;
    bool addSTOCHRSI( const unsigned& period = -999, const unsigned& fast = -999, const unsigned& slow = -999, const std::string& name = "" ) ;
    
    bool addVAR( const unsigned& period = -999, const double& sd2 = 9, const std::string& name = "" ) ;
    bool addSTDDEV( const unsigned& period = -999, const double& sd = 3, const std::string& name = "" ) ;
    
    bool addAPO( const unsigned& fast = -999, const unsigned& slow = -999, const std::string& name = "" ) ;
    bool addADO( const unsigned& fast = -999, const unsigned& slow = -999, const std::string& name = "" ) ;
    bool addADOSC( const unsigned& fast = -999, const unsigned& slow = -999, const std::string& name = "" ) ;

    bool addBBANDS( const unsigned& period = -999, const double& sd_up = 3.0, const double& sd_down = 3.0, const std::string& name = "");



    // after adding all the indicator functions you wish to use,
    // initialise the function.
    void initialise() ;

    int getStartIdx() const { return m_max_period; } 
    boost::gregorian::date getEndDate() const { return m_db.last().key; } 
    boost::gregorian::date getStartDate() const { return m_db.begin()->first; } 
    boost::gregorian::date_period getPeriod() const { return m_db.period(); } 
    boost::gregorian::date getCurrentIterDate() const { return m_iter->first; } 
    // increments to the next event in the dataset
    bool next() ;
    
    // Evaluates historic indicator at or before a certain date.
    //template < class T >
    double evaluateAtOrBefore( const std::string& indicator, const boost::gregorian::date& date, const std::string& ext = "" ) ;
    double evaluateBefore( const std::string& indicator, const boost::gregorian::date& date, const std::string& ext = "" ) ;

    //template < class T >
    double evaluate( const std::string& indicator, const std::string& ext = "" );

    TA::vDouble getData( const IndicatorApp::DataType& type );
  private:
    // private member functions
    void updatePeriod( const int& period );
  
    // member variables
    const Series::EODSeries& m_db;
    Series::EODSeries::const_iterator m_iter;
    int m_max_period;
    TA m_ta;
    std::vector< std::string > m_added_indexes; 


    // returns double only
    std::map< const boost::gregorian::date, std::map< std::string, double > > m_indicators;

};


#endif // INDICATORAPP_HPP
