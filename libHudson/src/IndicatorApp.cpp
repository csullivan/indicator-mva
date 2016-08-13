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

// STL
#include <iterator>     // std::advance
#include <limits>     // std::max limits

// Boost includes
//#include "boost/tuple/tuple_io.hpp"

// Hudson
#include "IndicatorApp.hpp"


//**************************************************************************************************************************
IndicatorApp::IndicatorApp( const Series::EODSeries& db )
  : m_db( db ), m_iter( m_db.begin() ), m_max_period( 0 ), m_ta() 
{}


//**************************************************************************************************************************
IndicatorApp::~IndicatorApp() {
  //delete m_ta; m_ta = 0;
}


//**************************************************************************************************************************
void IndicatorApp::initialise( ) {

  //++m_max_period;
  //std::cout << m_max_period << std::endl;
  m_iter = m_db.begin() ;
  std::advance( m_iter, m_max_period ); // Skip first events to give enough room too calculations

  std::cout << "INFO: Offset dataset by " << m_max_period << " units. Starting at " << to_simple_string( m_iter->first ) << std::endl;
  
  std::vector< std::string >::const_iterator it = m_added_indexes.begin();
  const std::vector< std::string >::const_iterator endit = m_added_indexes.end();
  std::cout << "INFO: Added indices are: ";
  for ( ; it != endit; ++it ) {
    std::cout << *it << "  ";
  }
  std::cout << std::endl;
  std::cout << "WE HAVE DONE INDICATOR APP\n"; 
   //TA::BBRes resBBANDS3 = m_ta.BBANDS( m_db.close( iter, 100), 100, 3, 3);
}


//**************************************************************************************************************************
void IndicatorApp::updatePeriod( const int& period ) {

  //std::cout << period << std::endl; 
  if ( period > m_max_period ) 
    m_max_period = period;
}


//**************************************************************************************************************************
bool IndicatorApp::next() {
  
  if( m_iter == m_db.end() ) { 
    //++m_iter;
    return false;
  }
  ++m_iter;
  return true;
}


//**************************************************************************************************************************
/*IndicatorApp::evaluate( const std::string& indicator )  {


  double value = -std::numeric_limits<double>::max(); 

  else {
    std::cerr << "WARNING: IndicatorApp cannot evaluate " << indicator << " and will skip." << std::endl;
  }
 
  return value;
}*/


/*boost::tuple< double, double> IndicatorApp::evaluate( const std::string& indicator ) {

  boost::tuple< double, double> values(-std::numeric_limits<double>::max(), 
					       -std::numeric_limits<double>::max()
					      );
  else {
    std::cerr << "WARNING: IndicatorApp cannot evaluate " << indicator << " and will skip." << std::endl;
  }
  return values;
}*/


//**************************************************************************************************************************
double IndicatorApp::evaluate( const std::string& indicator, const std::string& ext ) {
 
  double value( -std::numeric_limits<double>::max() );
  std::map< std::string, double >  thismap;
  bool found(false);
  if ( m_indicators.find( m_iter->first ) != m_indicators.end() ) {
    thismap = m_indicators[ m_iter->first ];
    if( ext == "" ) { 
      if ( thismap.find( indicator ) != thismap.end() ) {
        value = thismap[ indicator ];
        found = true;
      } 
    } 
    else {
      /*std::map< std::string, double >::const_iterator it = thismap.begin();
      const std::map< std::string, double >::const_iterator eit = thismap.end();
      for (; it != eit; ++it ){
        std::cout << it->first << " : " << it->second << std::endl;
      }*/
      if ( thismap.find( indicator+"_"+ext ) != thismap.end() ) {
        value = thismap[ indicator+"_"+ext ];
        found = true;     
      }     
    }
  } else {
    std::cerr << "WARNING: Date " << m_iter->first << " cannot be found in IndicatorApp\n";
  }

  if( !found ) {
    std::cerr << "WARNING: IndicatorApp cannot evaluate " << indicator << " and will skip." << std::endl;
  }
  return value;
}


//**************************************************************************************************************************
double IndicatorApp::evaluateAtOrBefore( const std::string& indicator, const boost::gregorian::date& date, const std::string& ext ) {

  //std::cout << m_iter->first << std::endl;
  if( date < getStartDate() ) {
    std::cerr << "FATAL: IndicatorApp - trying to acces date that doesn't exist.\n";
    exit(EXIT_FAILURE);
  }
  m_iter = m_db.at_or_before( date );
  return evaluate( indicator, ext );
}


//**************************************************************************************************************************
double IndicatorApp::evaluateBefore( const std::string& indicator, const boost::gregorian::date& date, const std::string& ext ) {

  //std::cout << m_iter->first << std::endl;
  if( date < getStartDate() ) {
    std::cerr << "FATAL: IndicatorApp - trying to acces date that doesn't exist.\n";
    exit(EXIT_FAILURE);
  }
  m_iter = m_db.before( date );
  return evaluate( indicator, ext );
}


//**************************************************************************************************************************
bool IndicatorApp::addSMA( const unsigned& period, const std::string& name ) {

  std::string title="SMA";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise SMA with invalid period( " << period << ")." << std::endl;
    return false;
  }
  TA::SMARes sma = m_ta.SMA( m_db.close(), period );
  int begIdx = sma.begIdx;
  // Shift series iterator to the beginning of SMA signals in SMA results vector
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addSMA() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, sma.ma[i] )  );
  }
  m_added_indexes.push_back( title ); 
  updatePeriod( begIdx );
  updatePeriod( period );
  return true; 
}


//**************************************************************************************************************************
bool IndicatorApp::addGTND( const unsigned& period, const std::string& name ) {

  std::string title="GTND";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise GTND with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  TA::SMARes sma = m_ta.SMA( m_db.volume(), period );
  int begIdx = sma.begIdx+1;
  // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addGTND() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, iter->second.volume - sma.ma[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addMFI( const unsigned& period, const std::string& name ) {

  std::string title="MFI";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise MFI with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  TA::MFIRes mfi = m_ta.MFI( m_db.high(), m_db.low(), m_db.close(), m_db.volume(), period );
  int begIdx = mfi.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addMFI() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, mfi.mfi[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addAPO( const unsigned& fast, const unsigned& slow, const std::string& name ) {

  std::string title="APO";
  if( name != "" ) {
    title += "_" + name;
  }

  if( fast <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise APO with invalid fast period( " << fast << ")." << std::endl;
    return false;
  } 

  if( slow <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise APO with invalid slow period( " << slow << ")." << std::endl;
    return false;
  }   
    
  TA::APORes apo = m_ta.APO( m_db.close(), fast, slow );
  int begIdx = apo.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addAPO() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, apo.apo[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( fast );  
  updatePeriod( slow );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addADOSC( const unsigned& fast, const unsigned& slow, const std::string& name ) {

  std::string title="ADOSC";
  if( name != "" ) {
    title += "_" + name;
  }

  if( fast <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise ADOSC with invalid fast period( " << fast << ")." << std::endl;
    return false;
  } 

  if( slow <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise ADOSC with invalid slow period( " << slow << ")." << std::endl;
    return false;
  }   

  TA::ADOSCRes adosc = m_ta.ADOSC( m_db.high(), m_db.low(), m_db.close(), m_db.volume(), fast, slow );
  int begIdx = adosc.begIdx;
  // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addADOSC() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, adosc.adosc[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( fast );  
  updatePeriod( slow );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addADO( const unsigned& fast, const unsigned& slow, const std::string& name ) {

  std::string title="ADO";
  if( name != "" ) {
    title += "_" + name;
  }

  if( fast <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise ADO with invalid fast period( " << fast << ")." << std::endl;
    return false;
  } 

  if( slow <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise ADO with invalid slow period( " << slow << ")." << std::endl;
    return false;
  }   

  TA::ADORes ado = m_ta.ADO( m_db.high(), m_db.low(), m_db.close(), m_db.volume(), fast, slow );
  int begIdx = ado.begIdx;
  // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addADO() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, ado.ado[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( fast );  
  updatePeriod( slow );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addCMO( const unsigned& period, const std::string& name ) {

  std::string title="CMO";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise CMO with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  TA::CMORes cmo = m_ta.CMO( m_db.close(), period );
  int begIdx = cmo.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addCMO() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, cmo.cmo[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addADX( const unsigned& period, const std::string& name ) {

  std::string title="ADX";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise ADX with invalid period( " << period << ")." << std::endl;
    return false;
  }    

  TA::ADXRes adx = m_ta.ADX( m_db.high(), m_db.low(), m_db.close(), period );
  int begIdx = adx.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addADX() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, adx.adx[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addBOP( const std::string& name ) {

  std::string title="BOP";
  if( name != "" ) {
    title += "_" + name;
  }

  TA::BOPRes bop = m_ta.BOP( m_db.open(), m_db.high(), m_db.low(), m_db.close() );
  int begIdx = bop.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addBOP() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title , bop.bop[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  return true;
}


//**************************************************************************************************************************
TA::vDouble IndicatorApp::getData( const IndicatorApp::DataType& type ) {

  TA::vDouble vec;

  switch( type ) {
      case Open:
        vec =  m_db.open();
        break;
      case High:
        vec =  m_db.high();
        break;
      case Low:
        vec =  m_db.low();
        break;
      case Close:
        vec =  m_db.close();
        break;
      case Volume:
        vec =  m_db.volume();
        break;
      default: 
	std::cerr << "DataType unknown " << type << std::endl;
  } 
  return vec;
}


//**************************************************************************************************************************
bool IndicatorApp::addHTDCP( const IndicatorApp::DataType& type, const std::string& name ) {

  std::string title="HTDCP";
  if( name != "" ) {
    title += "_" + name;
  }

  TA::vDouble data = getData( type ) ;
  
  TA::HTDCPRes htdcp = m_ta.HTDCP( data );
  int begIdx = htdcp.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addHTDCP() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title , htdcp.ht[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addHTIT( const IndicatorApp::DataType& type, const std::string& name ) {

  std::string title="HTIT";
  if( name != "" ) {
    title += "_" + name;
  }

  TA::vDouble data = getData( type ) ;
  
  TA::HTITRes htit = m_ta.HTIT( data );
  int begIdx = htit.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addHTIT() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title , htit.htit[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addWILLR( const unsigned& period, const std::string& name ) {

  std::string title="WILLR";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise WILLR with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  TA::WILLRRes willr = m_ta.WILLR( m_db.high(), m_db.low(), m_db.close(), period );
  int begIdx = willr.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addWILLR() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, willr.willr[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addCCI( const unsigned& period, const std::string& name ) {

  std::string title="CCI";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise CCI with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  TA::CCIRes cci = m_ta.CCI( m_db.high(), m_db.low(), m_db.close(), period );
  int begIdx = cci.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
   std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addCCI() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, cci.cci[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addVAR( const unsigned& period, const double& sd2, const std::string& name ) {

  std::string title="VAR";
  if( name != "" ) {
    title += "_" + name;
  }

  if( sd2 <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise VAR with invalid significance( " << sd2 << ")." << std::endl;
    return false;
  }  
  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise VAR with invalid period( " << period << ")." << std::endl;
    return false;
  }   
  TA::VARRes var = m_ta.VAR( m_db.close(), period, sd2 );
  int begIdx = var.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addVAR() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, var.var[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period ); 
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addSTDDEV( const unsigned& period, const double& sd, const std::string& name ) {

  std::string title="STDDEV";
  if( name != "" ) {
    title += "_" + name;
  }

  if( sd <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise STDDEV with invalid standard dev( " << sd << ")." << std::endl;
    return false;
  }  
  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise STDDEV with invalid period( " << period << ")." << std::endl;
    return false;
  }   
  TA::STDDEVRes stddev = m_ta.STDDEV( m_db.close(), period, sd );
  int begIdx = stddev.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addSTDDEV() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, stddev.stddev[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period ); 
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addFACTORS( const unsigned& period, const std::string& name ) {

  std::string title="FACTORS";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise FACTORS with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  TA::FACTORRes factors = m_ta.FACTORS( m_db.close(), period );
  int begIdx = factors.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
   std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addFACTORS() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, factors.factors[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}  


//**************************************************************************************************************************
bool IndicatorApp::addCORREL( const TA::vDouble& series2, const unsigned& period, const std::string& name ) {

  std::string title="CORREL";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise CORREL with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  if( series2.empty() ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise CORREL with empty comparison vector series2." << std::endl;
    return false;
  }   
  TA::CORRELRes cor = m_ta.CORREL( m_db.close(), series2, period );
  int begIdx = cor.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addCORREL() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, cor.correl[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}   


//**************************************************************************************************************************
bool IndicatorApp::addLSLR( const unsigned& period, const std::string& name ) {

  std::string title="LSLR";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise LSLR with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  
  TA::LSLRRes lslr = m_ta.LSLR( m_db.close(), period );
  int begIdx = lslr.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addLSLR() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, lslr.lslr[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}   


//**************************************************************************************************************************
bool IndicatorApp::addLSLR_M( const unsigned& period, const std::string& name ) {

  std::string title="LSLR_M";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise LSLR_M with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  
  TA::LSLR_MRes lslr_m = m_ta.LSLR_M( m_db.close(), period );
  int begIdx = lslr_m.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addLSLR_M() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, lslr_m.lslr_m[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}   

//**************************************************************************************************************************
bool IndicatorApp::addLSLR_C( const unsigned& period, const std::string& name ) {

  std::string title="LSLR_C";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise LSLR_C with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  
  TA::LSLR_CRes lslr_c = m_ta.LSLR_C( m_db.close(), period );
  int begIdx = lslr_c.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addLSLR_C() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, lslr_c.lslr_c[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}   


//**************************************************************************************************************************
bool IndicatorApp::addRSI( const unsigned& period, const std::string& name ) {

  std::string title="RSI";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise RSI with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  
  TA::RSIRes rsi = m_ta.RSI( m_db.close(), period );
  int begIdx = rsi.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addRSI() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, rsi.rsi[i] ) );
  }
  
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}   


//**************************************************************************************************************************
bool IndicatorApp::addMOM( const unsigned& period, const std::string& name ) {

  std::string title="MOM";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise MOM with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  
  TA::MOMRes mom = m_ta.MOM( m_db.close(), period );
  int begIdx = mom.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addMOM() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, mom.mom[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}   


//**************************************************************************************************************************
bool IndicatorApp::addROC( const unsigned& period, const std::string& name ) {

  std::string title="ROC";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise ROC with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  
  TA::ROCRes roc = m_ta.ROC( m_db.close(), period );
  int begIdx = roc.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addROC() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, roc.roc[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}   


//**************************************************************************************************************************
bool IndicatorApp::addROCR( const unsigned& period, const std::string& name ) {

  std::string title="ROCR";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise ROCR with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  
  TA::ROCRRes rocr = m_ta.ROCR( m_db.close(), period );
  int begIdx = rocr.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addROCR() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, rocr.rocr[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}   


//**************************************************************************************************************************
bool IndicatorApp::addROCP( const unsigned& period, const std::string& name ) {

  std::string title="ROCP";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise ROCP with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  
  TA::ROCPRes rocp = m_ta.ROCP( m_db.close(), period );
  int begIdx = rocp.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addROCP() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, rocp.rocp[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}    


//**************************************************************************************************************************
bool IndicatorApp::addEMA( const unsigned& period, const std::string& name ) {

  std::string title="EMA";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise EMA with invalid period( " << period << ")." << std::endl;
    return false;
  }  
  
  TA::EMARes ema = m_ta.EMA( m_db.close(), period );
  int begIdx = ema.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addEMA() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, ema.ema[i] ) );
  }
  m_added_indexes.push_back( title );
  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}    


//**************************************************************************************************************************
bool IndicatorApp::addMACD( const unsigned& period, const unsigned& fast, const unsigned& slow, const std::string& name ) {

  std::string title="MACD";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise MACD with invalid period( " << period << ")." << std::endl;
    return false;
  }
 
  if( fast <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise MACD with invalid fast period( " << fast << ")." << std::endl;
    return false;
  } 

  if( slow <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise MACD with invalid slow period( " << slow << ")." << std::endl;
    return false;
  }   

  TA::MACDRes macd = m_ta.MACD( m_db.close(), fast, slow, period );
  int begIdx = macd.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addMACD() variable with name " << name << " has already been added.\n";
    return false;
  }
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title, macd.macd[i] ) );
    m_indicators[iter->first].insert( std::make_pair( title+"_signal", macd.macd_signal[i] ) );
    m_indicators[iter->first].insert( std::make_pair( title+"_hist", macd.macd_hist[i] ) );
  }
  m_added_indexes.push_back( title );
  m_added_indexes.push_back( title+"_signal" );
  m_added_indexes.push_back( title+"_hist" );
  updatePeriod( begIdx );
  updatePeriod( period );  
  updatePeriod( fast );  
  updatePeriod( slow );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addSTOCHRSI( const unsigned& period, const unsigned& fast, const unsigned& slow, const std::string& name ) {

  std::string title="STOCHRSI";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise STOCHRSI with invalid period( " << period << ")." << std::endl;
    return false;
  }
 
  if( fast <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise STOCHRSI with invalid fast period( " << fast << ")." << std::endl;
    return false;
  } 

  if( slow <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise STOCHRSI with invalid slow period( " << slow << ")." << std::endl;
    return false;
  }   

  TA::STOCHRSIRes stochrsi = m_ta.STOCHRSI( m_db.close(), period, slow, fast );
  int begIdx = stochrsi.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addSTOCHRSI() variable with name " << name << " has already been added.\n";
    return false;
  } 
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title + "_K", stochrsi.fastK[i] ) );
    m_indicators[iter->first].insert( std::make_pair( title + "_D", stochrsi.fastD[i] ) );
  }
  m_added_indexes.push_back( title+"_K" );
  m_added_indexes.push_back( title+"_D" );

  updatePeriod( begIdx );
  updatePeriod( period );  
  updatePeriod( fast );  
  updatePeriod( slow );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addBBANDS( const unsigned& period, const double& sd_up, const double& sd_down, const std::string& name ) {

  std::string title="BBANDS";
  if( name != "" ) {
    title += "_" + name;
  }

  if( period <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise " + title + " with invalid period( " << period << ")." << std::endl;
    return false;
  }
 
  if( sd_up <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise " + title + " with invalid fast period( " << sd_up << ")." << std::endl;
    return false;
  } 

  if( sd_down <= 0 ) {
    std::cerr << "WARNING: IndicatorApp - trying to initialise " + title + " with invalid slow period( " << sd_down << ")." << std::endl;
    return false;
  }   

  TA::BBRes resBBAND = m_ta.BBANDS( m_db.close(), period, sd_up, sd_down );
  int begIdx = resBBAND.begIdx;
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  std::advance( iter, begIdx );
  if( m_indicators[iter->first].find( title ) != m_indicators[iter->first].end() ){
    std::cerr << "WARNING: IndicatorApp - addBBANDS() variable with name " << name << " has already been added.\n";
    return false;
  } 
  for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
    m_indicators[iter->first].insert( std::make_pair( title +"_upper"  , resBBAND.upper_band[i] ) );
    m_indicators[iter->first].insert( std::make_pair( title +"_middle" , resBBAND.middle_band[i] ) );
    m_indicators[iter->first].insert( std::make_pair( title +"_lower"  , resBBAND.lower_band[i] ) );
  }
  m_added_indexes.push_back( title +"_upper" );
  m_added_indexes.push_back( title +"_middle" );
  m_added_indexes.push_back( title +"_lower" );

  updatePeriod( begIdx );
  updatePeriod( period );  
  return true;
}


//**************************************************************************************************************************
bool IndicatorApp::addIndicator( const std::string& indicator, const int period, const double sig, const std::string& postfix ) {

  bool ok(false);

  if( indicator == "VAR" ) {
    ok = addVAR( period, sig, postfix );
  }
  else if( indicator == "STDDEV" ) {
    ok = addSTDDEV( period, sig, postfix );
  }
  else {
    ok = false;
    std::cerr << "WARNING: IndicatorApp - indicator with name ( " << indicator << " ) is not recognised for period and significance parameters. Nothing added!" << std::endl;
  }

  return ok;
}


//**************************************************************************************************************************
bool IndicatorApp::addIndicator( const std::string& indicator, const std::string& postfix ) {

  bool ok(false);

  if( indicator == "BOP" ) {
    ok = addBOP( postfix );
  }
  else {
    ok = false;
    std::cerr << "WARNING: IndicatorApp - indicator with name ( " << indicator << " ) is not recognised. Nothing added!" << std::endl;    
  }

  return ok;
}


//**************************************************************************************************************************
bool IndicatorApp::addIndicator( const std::string& indicator, const int fast, const int slow, const std::string& postfix ) {

  bool ok(false);

  if( indicator == "APO" ) {
    ok = addAPO( fast, slow, postfix );
  }
  else if( indicator == "ADO" ) {
    ok = addADO( fast, slow, postfix );
  }
  else if( indicator == "ADOSC" ) {
    ok = addADOSC( fast, slow, postfix );
  }
  else {
    ok = false;
    std::cerr << "WARNING: IndicatorApp - indicator with name ( " << indicator << " ) is not recognised for fast, slow parameters. Nothing added!" << std::endl;
  }
  return ok;
}


//**************************************************************************************************************************
bool IndicatorApp::addIndicator( const std::string& indicator, const int period, const double sd_up, const double sd_down, const std::string& postfix ) {

  bool ok(false);
  if( indicator == "BBANDS" ) {
    ok = addBBANDS( period, sd_up, sd_down, postfix ) ;
  }
  else {
    ok = false;
    std::cerr << "WARNING: IndicatorApp - indicator with name ( " << indicator << " ) is not recognised for period, sd_up and sd_down parameters. Nothing added!" << std::endl;
  }

  return ok;
}


//**************************************************************************************************************************
bool IndicatorApp::addIndicator( const std::string& indicator, const int period, const int fast, const int slow, const std::string& postfix ) {

  bool ok(false);

  if( indicator == "MACD" ) {
    ok = addMACD( period, fast, slow, postfix ) ;
  }
  else if ( indicator == "STOCHRSI" ) {
    ok = addSTOCHRSI( period, fast, slow, postfix ) ;
  }
  else {
    ok = false;
    std::cerr << "WARNING: IndicatorApp - indicator with name ( " << indicator << " ) is not recognised for period, fast, slow parameters. Nothing added!" << std::endl;
  }

  return ok;
}

//**************************************************************************************************************************
bool IndicatorApp::addIndicator( const std::string& indicator, const int period, const std::string& postfix ) {

  bool ok(false);
  // add all the period only dependant variables.
  if ( indicator == "SMA" ) {
    ok = addSMA( period, postfix );
  }
  else if( indicator == "EMA" ) {
    ok = addEMA( period, postfix );
  }
  else if( indicator == "EMA" ) {
    ok = addEMA( period, postfix );
  }
  else if( indicator == "GTND" ) {
    ok = addGTND( period, postfix );
  }
  else if( indicator == "MFI" ) {
    ok = addMFI( period, postfix );
  }
  else if( indicator == "CMO" ) {
    ok = addCMO( period, postfix );
  }
  else if( indicator == "ADX" ) {
    ok = addADX( period, postfix );
  }
  else if( indicator == "WILLR" ) {
    ok = addWILLR( period, postfix );
  }
  else if( indicator == "CCI" ) {
    ok = addCCI( period, postfix );
  }
  else if( indicator == "ROCP" ) {
    ok = addROCP( period, postfix );
  }
  else if( indicator == "ROCR" ) {
    ok = addROCR( period, postfix );
  }
  else if( indicator == "ROC" ) {
    ok = addROC( period, postfix );
  }
  else if( indicator == "RSI" ) {
    ok = addRSI( period, postfix );
  }
  else if( indicator == "MOM" ) {
    ok = addMOM( period, postfix );
  }
  else if( indicator == "LSLR" ) {
    ok = addLSLR( period, postfix );
  }
  else if( indicator == "LSLR_C" ) {
    ok = addLSLR_C( period, postfix );
  }
  else if( indicator == "LSLR_M" ) {
    ok = addLSLR_M( period, postfix );
  }
  else if( indicator == "FACTORS" ) {
    ok = addFACTORS( period, postfix );
  }
  else {
    ok = false;
    std::cerr << "WARNING: IndicatorApp - indicator with name ( " << indicator << " ) is not recognised for period only parameter. Nothing added!" << std::endl;
  }

  return ok;

}


//**************************************************************************************************************************
void IndicatorApp::add( const std::string& indicator, const double& period, const double& fast, const double& slow ) {
 
  int begIdx(0); 
  Series::EODSeries::const_iterator iter = m_db.begin() ;
  if( period <= 0 ) {
    std::cerr << "WARNING: Indicator " << indicator << " will be added with default values, period(" << period << "), fast(" << 
    fast << "), slow(" << slow << ")" << std::endl;
  }
 
  std::cout << "INFO: Adding indicator " << indicator << std::endl;
  if ( indicator.find( "SMA" ) != std::string::npos ) {
    
    TA::SMARes sma = m_ta.SMA( m_db.close(), period );
    begIdx = sma.begIdx;
    // Shift series iterator to the beginning of SMA signals in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, sma.ma[i] )  );
    }
    m_added_indexes.push_back( indicator );
  }
  if ( indicator.find( "GTND" ) != std::string::npos ) {
    TA::SMARes sma = m_ta.SMA( m_db.volume(), period );
    begIdx = sma.begIdx+1;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      //std::cout << "CHECKING variables ( " << iter->first << "  " << iter->second.volume << std::endl; 
      m_indicators[iter->first].insert( std::make_pair( indicator, iter->second.volume - sma.ma[i] ) );
    }
    m_added_indexes.push_back( indicator );
  } 
  if ( indicator.find( "MFI" ) != std::string::npos ) {
    TA::MFIRes mfi = m_ta.MFI( m_db.high(), m_db.low(), m_db.close(), m_db.volume(), period );
    begIdx = mfi.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, mfi.mfi[i] ) );
    }
    m_added_indexes.push_back( indicator );
  } 
  if ( indicator.find( "CMO" ) != std::string::npos ) {
    TA::CMORes cmo = m_ta.CMO( m_db.close(), period );
    begIdx = cmo.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, cmo.cmo[i] ) );
    }
    m_added_indexes.push_back( indicator );
  }
  // THIS IS ONLY USING FAST AND SLOW PERIODS MAYBE WE NEED ANOTHER SET OF FUNCTIONS FOR
  // EACH INPUTs,1 2 or 3?...
  if ( indicator.find( "ADO" ) != std::string::npos ) {
    TA::ADORes ado = m_ta.ADO( m_db.high(), m_db.low(), m_db.close(), m_db.volume(), fast, slow );
    begIdx = ado.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, ado.ado[i] ) );
    }
    m_added_indexes.push_back( indicator );
  }
  // AGAIN TAKES TWO INPUT FAST AND SLOE
  if ( indicator.find( "APO" ) != std::string::npos ) {
    std::cout << fast << "," << slow << std::endl;
    TA::APORes apo = m_ta.APO( m_db.close(), fast, slow );
    begIdx = apo.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, apo.apo[i] ) );
    }
    m_added_indexes.push_back( indicator );
  }
  if ( indicator.find( "ADX" ) != std::string::npos ) {
    //std::cout << "(high,low,close):("<< m_db.high().size()<<","<< m_db.low().size() << ","<<m_db.close().size() << ")" << std::endl;
    TA::ADXRes adx = m_ta.ADX( m_db.high(), m_db.low(), m_db.close(), period );
    begIdx = adx.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, adx.adx[i] ) );
    }
    m_added_indexes.push_back( indicator );
  }
  if ( indicator.find( "BOP" ) != std::string::npos ) {
    TA::BOPRes bop = m_ta.BOP( m_db.open(), m_db.high(), m_db.low(), m_db.close() );
    begIdx = bop.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, bop.bop[i] ) );
    }
    m_added_indexes.push_back( indicator );
  }
  if ( indicator.find( "CCI" ) != std::string::npos ) {
    TA::CCIRes cci = m_ta.CCI( m_db.high(), m_db.low(), m_db.close(), period );
    begIdx = cci.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, cci.cci[i] ) );
    }
    m_added_indexes.push_back( indicator );
  }
  if ( indicator.find( "STDDEV" ) != std::string::npos ) {
    TA::STDDEVRes stddev = m_ta.STDDEV( m_db.close(), period, fast );
    begIdx = stddev.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, stddev.stddev[i] ) );
    }
    m_added_indexes.push_back( indicator );
  }  
  if ( indicator.find( "FACTORS" ) != std::string::npos ) {
    TA::FACTORRes factors = m_ta.FACTORS( m_db.close(), period );
    begIdx = factors.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, factors.factors[i] ) );
    }
    m_added_indexes.push_back( indicator );
  } 
  if ( indicator.find( "ROCP" ) != std::string::npos ) {
    TA::ROCPRes rocp = m_ta.ROCP( m_db.close(), period );
    begIdx = rocp.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, rocp.rocp[i] ) );
    }
    m_added_indexes.push_back( indicator );
  } 
  if ( indicator.find( "EMA" ) != std::string::npos ) {
    TA::EMARes ema = m_ta.EMA( m_db.close(), period );
    begIdx = ema.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, ema.ema[i] ) );
    }
    m_added_indexes.push_back( indicator );
  } 
  if ( indicator.find( "MACD" ) != std::string::npos ) {
    TA::MACDRes macd = m_ta.MACD( m_db.close(), fast, slow, period );
    begIdx = macd.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      m_indicators[iter->first].insert( std::make_pair( indicator, macd.macd[i] ) );
      //std::cout << "macd (signal,hist) : (" <<  macd.macd_signal[i] << "," << macd.macd_hist[i] << ")" << std::endl;
      m_indicators[iter->first].insert( std::make_pair( indicator+"_signal", macd.macd_signal[i] ) );
      m_indicators[iter->first].insert( std::make_pair( indicator+"_hist", macd.macd_hist[i] ) );
    }
    m_added_indexes.push_back( indicator );
    m_added_indexes.push_back( indicator+"_signal" );
    m_added_indexes.push_back( indicator+"_hist" );
  }  
  if ( indicator.find( "STOCHRSI" ) != std::string::npos ) {
    TA::STOCHRSIRes stochrsi = m_ta.STOCHRSI( m_db.close(), period, slow, fast );
    begIdx = stochrsi.begIdx;
    // Shift series iterator to the beginning of SMA signals + 1 in SMA results vector
    std::advance( iter, begIdx );
    for ( int i(0); iter != m_db.end(); ++iter, ++i ) {
      //std::cout << "stochrsi (K,D) : (" <<  stochrsi.fastK[i] << "," << stochrsi.fastD[i] << ")" << std::endl;

      m_indicators[iter->first].insert( std::make_pair( indicator+"_K", stochrsi.fastK[i] ) );
      m_indicators[iter->first].insert( std::make_pair( indicator+"_D", stochrsi.fastD[i] ) );
    }
    m_added_indexes.push_back( indicator+"_K" );
    m_added_indexes.push_back( indicator+"_D" );
  }  
  updatePeriod( begIdx );
  updatePeriod( period );
  updatePeriod( period );
  updatePeriod( period );
  //std::cout << "fast period:" << fast << std::endl;
  //std::cout << "slow period:" << slow << std::endl;
  if( fast != 0 ){ updatePeriod( fast ) ; }
  if( slow != 0 ){ updatePeriod( slow ) ; } 

}

/*

    BBRes     BBANDS(const vDouble& vSeries, const unsigned& ma_period = 14, const double& sd_up = 3, const double& sd_down = 3) const throw(TAException);


*/

