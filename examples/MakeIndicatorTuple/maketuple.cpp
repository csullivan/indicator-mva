// My test of the class functionality

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

// Boost
#include <boost/program_options.hpp>

// include local
#include "DefinedMVAs.hpp"
#include "TMVAClassification.hpp"
#include "TMVAReader.hpp"
#include "TreeWriter.hpp"

// Hudson
#include <Database.hpp>
#include <PositionFactors.hpp>
#include <PositionFactorsSet.hpp>
#include <BnHTrader.hpp>
#include <EOMReturnFactors.hpp>
#include <EOMReport.hpp>
#include <PositionsReport.hpp>
#include <PortfolioReturns.hpp>
#include <PortfolioReport.hpp>
#include <IndicatorApp.hpp>

// include root
#include "TFile.h"
#include "TTree.h"

using namespace std;
using namespace boost::gregorian;
using namespace Series;
namespace po = boost::program_options;

bool trainer = true;
bool evaluate = true;


int main(int argc, const char* argv[]) {

  std::string begin_date, end_date;
  std::string spx_dbfile;
  std::string outputFileName = "outputFile_SPX.root";
  std::string treepath = "data";
  int dayshift = 7;

  /*
   * Extract simulation options
   */
  po::options_description desc("Allowed options");
  desc.add_options()
  	("help", "produce help message")
  	("spx_file",   po::value<std::string>(&spx_dbfile),     	"SPX series database")
  	("begin_date", po::value<std::string>(&begin_date),     	"start of trading period (YYYY-MM-DD)")
  	("end_date",   po::value<std::string>(&end_date),       	"end of trading period (YYYY-MM-DD)") 
  	("day_shift",   po::value<int>(&dayshift),       		"time period (day shift) for calculating signal and background weights (7)") 
  	("output_file",   po::value<std::string>(&outputFileName),   "output file (outputFile_SPX.root)") 
  	("tree_path",   po::value<std::string>(&treepath),   		"tuple tree path for output root file (data)") ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if( vm.count("help") ) {
  	cout << desc << endl;
 	exit(0);
  }

  if( vm["spx_file"].empty() ||
	vm["begin_date"].empty() || vm["end_date"].empty() ) {
 	cout << desc << endl;
	exit(1);
	}

	
  try {
    /*
     * Load series data
     */
    Series::Database::SeriesFile sf;
    Series::Database::SERIES_MAP mSeries;
	date load_begin(from_simple_string(begin_date));
	if( load_begin.is_not_a_date() ) {
		cerr << "Invalid begin date " << begin_date << endl;
		exit(EXIT_FAILURE);
	}

	date load_end(from_simple_string(end_date));
	if( load_end.is_not_a_date() ) {
		cerr << "Invalid end date " << end_date << endl;
		exit(EXIT_FAILURE);
	}
	
    // Load the series	
    const string spx_symbol = "SPX";
    std::cout << "Loading " << spx_dbfile << " from " << load_begin << " to " << load_end << "..." << std::endl;
    Series::EODDB::instance().load(spx_symbol, spx_dbfile, Series::EODDB::YAHOO, load_begin, load_end);
    const Series::EODSeries& spx_db = Series::EODDB::instance().get( spx_symbol );
   
    // Initialize and run strategy
    std::set< std::pair< std::string, std::string > > leaves;
    leaves.insert( std::make_pair( "SMA", "" ) ); 
    leaves.insert( std::make_pair( "EMA", "" ) ); 
    leaves.insert( std::make_pair( "MFI", "" ) ); 
    leaves.insert( std::make_pair( "BOP", "" ) ); 
    leaves.insert( std::make_pair( "ADOSC", "" ) ); 
    leaves.insert( std::make_pair( "APO", "" ) ); 
    leaves.insert( std::make_pair( "VAR", "" ) ); 
    leaves.insert( std::make_pair( "WILLR", "" ) ); 
    leaves.insert( std::make_pair( "STDDEV", "" ) ); 
    leaves.insert( std::make_pair( "CCI", "" ) ); 
    leaves.insert( std::make_pair( "ADO", "" ) ); 
    leaves.insert( std::make_pair( "ADX", "" ) ); 
    leaves.insert( std::make_pair( "GTND", "" ) ); 
    leaves.insert( std::make_pair( "LSLR", "" ) ); 
    leaves.insert( std::make_pair( "LSLR_C", "" ) ); 
    leaves.insert( std::make_pair( "LSLR_M", "" ) ); 
    leaves.insert( std::make_pair( "RSI", "" ) ); 
    leaves.insert( std::make_pair( "HTIT", "" ) ); 
    leaves.insert( std::make_pair( "HTDCP", "" ) ); 
    leaves.insert( std::make_pair( "MACD", "" ) ); 
    leaves.insert( std::make_pair( "MACD", "signal" ) ); 
    leaves.insert( std::make_pair( "MACD", "hist" ) ); 
    leaves.insert( std::make_pair( "STOCHRSI", "D" ) ); 
    leaves.insert( std::make_pair( "STOCHRSI", "K" ) ); 
    leaves.insert( std::make_pair( "BBANDS", "upper" ) ); 
    leaves.insert( std::make_pair( "BBANDS", "middle" ) ); 
    leaves.insert( std::make_pair( "BBANDS", "lower" ) ); 

   // Create the indicator app.
   NN::TreeWriter* treeWriter = new NN::TreeWriter( outputFileName, treepath );
   treeWriter->add( "Day" );
   treeWriter->add( "Month" );
   treeWriter->add( "Year" );
   treeWriter->add( "close" );
   treeWriter->add( "nsig" );
   treeWriter->add( "nbkg" );
   treeWriter->add( "nsig_S_sw" );
   treeWriter->add( "nbkg_B_sw" );
   for ( std::set< std::pair< std::string, std::string > >::const_iterator p = leaves.begin(); p != leaves.end(); ++p ) {
       if(p->second != "" ) {
         treeWriter->add( p->first+"_"+p->second );
       } else {
         treeWriter->add( p->first );
       }
   }
   treeWriter->add("STOCHRSIDK");
   IndicatorApp app( spx_db );
   app.addIndicator( "EMA", 14 );
   app.addIndicator( "SMA", 7 );
   app.addIndicator( "MFI", 7 );
   app.addIndicator( "WILLR", 7 );
   app.addIndicator( "RSI", 7 );
   app.addIndicator( "MOM", 7 );
   app.addIndicator( "ROCP", 7 );
   app.addIndicator( "GTND", 3 );
   app.addIndicator( "CCI", 7 );
   app.addIndicator( "BOP" );
   app.addIndicator( "ADX", 7 );
   app.addIndicator( "STDDEV", 7, 3.5 );
   app.addIndicator( "VAR", 7, 10.0 );
   app.addIndicator( "APO", 5, 7 );
   app.addIndicator( "ADO", 5, 7 );
   app.addIndicator( "ADOSC", 5, 7 );
   app.addIndicator( "CMO", 7 );
   app.addIndicator( "LSLR", 7 );
   app.addIndicator( "LSLR_C", 7 );
   app.addIndicator( "LSLR_M", 7 );
   app.addIndicator( "MACD", 7, 12, 26 );
   app.addIndicator( "STOCHRSI", 12, 3, 5 );
   app.addIndicator( "BBANDS", 7, 3.0, 3.0 );
   app.addHTIT();
   app.addHTDCP();

/*   app.add( "EMA", 14 );
   app.add( "SMA", 7 );
   app.add( "MFI", 7 );
   app.add( "ROCP", 7 );
   app.add( "GTND", 3 );
   app.add( "CCI", 7 );
   app.add( "BOP", -1 );
   app.add( "ADX", 7 );
   app.add( "STDDEV", 7, 3.5 );
   app.add( "APO", -1, 5, 7 );
   app.add( "ADO", -1, 5, 7 );
   app.add( "CMO", 7 );
   app.add( "MACD", 7, 12, 26 );
   app.add( "STOCHRSI", 12, 3, 5 );*/

   app.initialise();
  
   //bool status(true);    
   Series::EODSeries::const_iterator iter = spx_db.begin();
   std::advance( iter, app.getStartIdx() ); 
	
   Series::EODSeries::const_iterator endit = spx_db.end();
   std::advance (endit, -1.*dayshift ); 
   bool SD(false);
   double change(0.), stochdk(0.0);
   for (; iter != endit; ++iter ) { 
     // now determine if signal like or not?
     Series::EODSeries::const_iterator shiftediter = iter;
     std::advance( shiftediter, dayshift);
     change = std::fabs( spx_db.after( shiftediter->first )->second.close )/ (double) iter->second.close;
     //std::cout << change << std::endl; 

     // now loop over leaves in tuple
     for ( std::set< std::pair< std::string, std::string > >::const_iterator p = leaves.begin( ); p != leaves.end( ); ++p ) {

       if( change < 1) {
         double invert = 1. + ( 1. - change );
         treeWriter->column( "nsig", static_cast<Float_t>( 0 ) );
         treeWriter->column( "nbkg", static_cast<Float_t>( invert ) );
         treeWriter->column( "nbkg_B_sw", static_cast<Float_t>( invert ) );
         treeWriter->column( "nsig_S_sw", static_cast<Float_t>( -1. * ( 1. - change ) ) );
       } else if ( change > 1) {
         treeWriter->column( "nsig", static_cast<Float_t>( change ) );
         treeWriter->column( "nbkg", static_cast<Float_t>( 0 ) );
         treeWriter->column( "nsig_S_sw", static_cast<Float_t>( change ) );
         treeWriter->column( "nbkg_B_sw", static_cast<Float_t>( (1. - change) ) );
       } else continue;
       Float_t value = static_cast<Float_t>( app.evaluateAtOrBefore( p->first, iter->first, p->second ) );
       treeWriter->column( "Day", static_cast<Float_t>( iter->first.day() ) );
       treeWriter->column( "Month", static_cast<Float_t>( iter->first.month() ) );
       treeWriter->column( "Year", static_cast<Float_t>( iter->first.year() ) );
       treeWriter->column( "close", static_cast<Float_t>( iter->second.close ) );
       //std::cout << p->first+"_"+p->second << ":   " <<  value << std::endl;
       if(p->second != "" ) {
         treeWriter->column( p->first+"_"+p->second, value );
       } else {
         treeWriter->column( p->first, value );
       }

       if( p->first == "STOCHRSI" && p->second == "D" ) {
         stochdk = value;
         SD = true;
       }
       if( p->first == "STOCHRSI" && p->second == "K" && SD ) {
         stochdk -= value;
         SD = false;
         treeWriter->column( "STOCHRSIDK", stochdk );
       }


       treeWriter->write();
     }
   }
   delete treeWriter; treeWriter = 0;

  } catch( std::exception& ex ) {

    std::cerr << "Unhandled exception: " << ex.what() << std::endl;
    exit(EXIT_FAILURE);
  }

 
 
  return 0;
}
