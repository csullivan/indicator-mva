/*
 * Copyright (C) 2007,2008 Alberto Giannetti
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
#include <cmath>

// Hudson
#include "MVABacktester.hpp"


using namespace std;
using namespace boost::gregorian;
using namespace Series;


MVABacktester::MVABacktester( const EODSeries& db, const IndicatorApp app, const std::string& mva, const Float_t& cutValue)
 :
  m_db( db ),
  m_app( app ),
  m_mvaName( mva ),
  m_dayshift( 0 ),
  m_cutValue( cutValue ),
  m_setup( false ),
  m_reader( 0 ),
  m_random( 100 )
{
}

void MVABacktester::setSeed( const UInt_t& seed )
{
  m_random.SetSeed( seed );
}


void MVABacktester::run( const unsigned& dayshift, const MVABacktester::Type& type ) throw(TraderException)
{

 m_type = type;
 if (! m_setup ) {
    std::cerr << "MVABacktester: run - Cannot run as setup has not been set.\n";
    exit(EXIT_FAILURE);
    return;
  }
  m_dayshift = dayshift;

  Series::EODSeries::const_iterator iter( m_db.begin() ); 

  std::advance( iter, m_app.getStartIdx() ); 
  TMVA::Timer timer( std::distance(iter, m_db.end() ), "MVABacktester", kTRUE ); 

  for( int i = 0; iter != m_db.end(); ++iter, ++i ) {
    try {

      trade( iter );
      timer.DrawProgressBar( i );
    } catch( std::exception& e ) {

      cerr << e.what() << endl;
      continue;
    }
  }
}


void MVABacktester::trade( Series::EODSeries::const_iterator& iter )
{
  Float_t mvaValue(0.0), value(0.0);//, error(0.);

  // now loop over leaves and set the values
  for ( std::set< std::pair< std::string, std::string > >::const_iterator p = m_leaves.begin( ); p != m_leaves.end( ); ++p ) {
    value = static_cast<Float_t>( m_app.evaluateAtOrBefore( p->first, iter->first, p->second ) );
    if( p->second != "" ) {
      m_reader->setVariable( p->first+"_"+p->second, value );
    } else {
      m_reader->setVariable( p->first, value );
    }
    if( p->first == "STOCHRSIDK" ) {
      m_reader->setVariable( "STOCHRSIDK", m_app.evaluateAtOrBefore( "STOCHRSI", iter->first, "D" ) - m_app.evaluateAtOrBefore( "STOCHRSI", iter->first, "K" ) );
    }
  }

  // get the mva output value
  switch ( m_type ) {
    case MVABasic:
//      mvaValue  = m_app.evaluateAtOrBefore( "MACD", iter->first, "signal" ) ; 
      mvaValue = m_reader->getMvaValue( m_mvaName );  
      break;
    case Random:
      // Random number generated between -1.1 and 1.1 since this is a common range for the TMVA networks
      mvaValue = m_random.Uniform(-1.1, 1.1);
      break;
    case ProbTransform:
      //mvaValue = m_reader->getMvaValue( m_mvaName );
      // Transform to a probability measure
      mvaValue = 0.5*(1.0 + m_reader->getMvaValue( m_mvaName ) );     
      break;
    default:
      std::cerr << "WARNING: MVABacktester - trade() has set mvaValue to take the default mva value.\n";
      mvaValue = m_reader->getMvaValue( m_mvaName );
      break;
  }
   
  //error = m_reader->getMvaError( m_mvaName );
  //std::cout << "MVA value " << mvaValue << " and the error is " << error << std::endl; 
  check_buy( iter, mvaValue );
  //check_sell(db, iter, macd);
}

void MVABacktester::setup( const std::vector< std::string >& inputvars, const std::set< std::pair< std::string, std::string > >& leaves, const std::string& outputFile, const std::string& weightsDirPrefix ) {
 
  m_leaves = leaves;
  // delete anything that has been set before now.
  if( m_reader ) {
    delete m_reader; m_reader=0;
  }
  std::vector< std::string > mvaMethods(1);
  mvaMethods[0] = m_mvaName;

  m_reader = new NN::TMVAReader( mvaMethods, inputvars, outputFile, weightsDirPrefix );

  m_setup=true;
}

void MVABacktester::check_buy( Series::EODSeries::const_iterator& iter, const Float_t& value )
{
  // Buy on MACD cross and MACD Hist > 0.5%
  if( _miPositions.open( m_db.name() ).empty() && 
       m_db.after( iter->first, m_dayshift ) != m_db.end() ) {
   
    if( value < m_cutValue ) return;
    //cout << db.name() << " MACD " << macd.macd[i] << " MACD Signal " << macd.macd_signal[i] << " MACD Hist " << macd.macd_hist[i] << endl;
    
    // Buy tomorrow's open
    EODSeries::const_iterator iter_entry = m_db.after(iter->first);
    if( iter_entry == m_db.end() ) {
      cerr << "Warning: can't open " << m_db.name() << " position after " << iter->first << endl;
      return;
    }
    cout << "Buying on " << iter_entry->first << " at " << iter_entry->second.open << endl;
    buy( m_db.name(), iter_entry->first, Price( iter_entry->second.open ) );

    // Sell m_dayshift days from now!
    EODSeries::const_iterator iter_exit = m_db.after(iter->first, m_dayshift );
    if( iter_exit == m_db.end() ) {
      cerr << "Warning: can't close " << m_db.name() << " position after " << iter->first << endl;
      return;
    }
    // Close all open positions at tomorrow's open
    PositionSet ps = _miPositions.open( m_db.name() );
    for( PositionSet::const_iterator pos_iter = ps.begin(); pos_iter != ps.end(); ++pos_iter ) {
      PositionPtr pPos = (*pos_iter);
      // Sell at tomorrow's open
      //cout << "Selling on " << iter_exit->first << " at " << iter_exit->second.open << endl;
      close(pPos->id(), iter_exit->first, Price( iter_exit->second.open ) );
    } // end of all open positions
  }
}


/*void MVABacktester::check_sell( const EOMSeries& db, EOMSeries::const_iterator& iter, const TA::MACDRes& macd, int i )
{
  if( ! _miPositions.open(db.name()).empty() && macd.macd[i] < macd.macd_signal[i] && (::abs(macd.macd_hist[i]) / iter->second.close ) > 0.005 ) {
  
    //cout << db.name() << "MACD " << macd.macd[i] << " MACD Signal " << macd.macd_signal[i] << " MACD Hist " << macd.macd_hist[i] << endl;
    
    EOMSeries::const_iterator iter_exit = db.after(iter->first);
    if( iter_exit == db.end() ) {
      cerr << "Warning: can't close " << db.name() << " position after " << iter->first << endl;
      return;
    }

    // Close all open positions at tomorrow's open
    PositionSet ps = _miPositions.open(db.name());
    for( PositionSet::const_iterator pos_iter = ps.begin(); pos_iter != ps.end(); ++pos_iter ) {
      PositionPtr pPos = (*pos_iter);
      // Sell at tomorrow's open
      //cout << "Selling on " << iter_exit->first << " at " << iter_exit->second.open << endl;
      close(pPos->id(), iter_exit->first, Price(iter_exit->second.open));
    } // end of all open positions
  }
}*/
