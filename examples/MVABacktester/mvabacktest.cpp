// My test of the class functionality

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <cstdlib>

// Boost
#include <boost/program_options.hpp>


// include local
#include "DefinedMVAs.hpp"
#include "TMVAClassification.hpp"
#include "TMVAReader.hpp"
#include "TreeWriter.hpp"
#include "MVABacktester.hpp"

// Hudson
#include <YahooDriver.hpp>
#include <DMYCloseDriver.hpp>
#include <EODSeries.hpp>
#include <Database.hpp>
#include <PositionsReport.hpp>
#include <PositionFactors.hpp>
#include <PositionFactorsSet.hpp>
#include <BnHTrader.hpp>
#include <EOMReturnFactors.hpp>
#include <EOMReport.hpp>
#include <PortfolioReturns.hpp>
#include <PortfolioReport.hpp>

using namespace std;
using namespace boost::gregorian;
using namespace Series;

namespace po = boost::program_options;


bool evaluate = true;


int main(int argc, const char* argv[]) {

	// SPX
	std::string outputFileName("mva_backtester.root"),  treepath = "testing";
	std::string begin_date, end_date;
	std::string spx_dbfile;
	Float_t cutValue = -0.01,  cutval_min(-0.001), cutval_max(1.0);
	std::string mvaMethod("TMlpANN");
	int dayshift = 7, N(1), type(0);
	std::vector<std::string> inputvars;

	/*
	 * Extract simulation options
	 */
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
	  	("output_file",   po::value<std::string>(&outputFileName), 	"name when running the optimisation to store the data.") 
  		("tree_path", po::value<std::string>(&treepath),     		"the output tree path name.")
		("spx_file",   po::value<string>(&spx_dbfile),     "SPX series database.")
		("begin_date", po::value<string>(&begin_date),     "start of trading period (YYYY-MM-DD).")
		("end_date",   po::value<string>(&end_date),       "end of trading period (YYYY-MM-DD).")
		("mva_type",   po::value<string>(&mvaMethod),      "the mva type you wish to test (BDTD).")
		("cut_value",  po::value<Float_t>(&cutValue),      "the minimum mva cut value in which to buy an asset.")
		("iters",      po::value<int>(&N),	           "the number of iterations to loop over toy.")
		("cut_type",   po::value<int>(&type),	           "set the mva cut type where it is standard (0), random (1) or a probability transfrom between 0-1 (2).")
		("cut_min",  po::value<Float_t>(&cutval_min),      "the minimum range mva cut value in which to buy an asset.")
		("cut_max",  po::value<Float_t>(&cutval_max),      "the maximum range mva cut value in which to buy an asset.")
  		("day_shift",   po::value<int>(&dayshift),         "time period (day shift) for calculating signal and background weights (7).") 
        	( "var", po::value< std::vector< std::string > >( &inputvars )->multitoken(), "specify the training variables to use." )
		;

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

	/*
	 * Load series data
	 */
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


	try {

		/*
		 * Load series data
		 */
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
		const string spx_symbol = "SPX";
		std::cout << "Loading " << spx_dbfile << " from " << load_begin << " to " << load_end << "..." << std::endl;
		Series::EODDB::instance().load(spx_symbol, spx_dbfile, Series::EODDB::YAHOO, load_begin, load_end);
		const Series::EODSeries& spx_db = Series::EODDB::instance().get(spx_symbol);

		NN::TreeWriter treeWriter( outputFileName, treepath );
		treeWriter.add( "mvaCutValue");
		treeWriter.add( "ROI");
		treeWriter.add( "STDDEV");
		treeWriter.add( "SKEW");
		treeWriter.add( "MAXDD");
		treeWriter.add( "AVE");
		treeWriter.add( "nPositions");
		treeWriter.add( "SHARPE" );
		treeWriter.add( "ROIEOM" );
		treeWriter.add( "CAGR" );
		treeWriter.add( "GSDM" );
	
		double rf_rate = 3.0;
		if( evaluate ) {

			    // The input variables
  if( inputvars.empty() ) {
    inputvars.push_back("BOP"); 
    inputvars.push_back("APO"); 
    //inputvars.push_back("MFI"); 
    inputvars.push_back("STDDEV" ); 
    inputvars.push_back("CCI"); 
    inputvars.push_back("ADO"); 
    inputvars.push_back("ADX"); 
    //inputvars.push_back("STOCHRSIDK"); 
    inputvars.push_back("MACD_signal"); 
  }

			std::cout << "Records: " << spx_db.size() << std::endl;
			std::cout << "Period: " << spx_db.period() << std::endl;
			std::cout << "Total days: " << spx_db.duration().days() << std::endl;

			// Initialize and run strategy
			const Series::EODSeries& spx_db = Series::EODDB::instance().get( "SPX" );

			std::set< std::pair< std::string, std::string > > leaves;
			leaves.insert( std::make_pair( "BOP", "" ) ); 
			leaves.insert( std::make_pair( "APO", "" ) ); 
			//leaves.insert( std::make_pair( "MFI", "" ) ); 
			leaves.insert( std::make_pair( "STDDEV", "" ) ); 
			leaves.insert( std::make_pair( "CCI", "" ) ); 
			leaves.insert( std::make_pair( "ADO", "" ) ); 
			leaves.insert( std::make_pair( "ADX", "" ) ); 
			//leaves.insert( std::make_pair( "STOCHRSIDK", "" ) ); 
			//leaves.insert( std::make_pair( "MACD", "hist" ) ); 
			leaves.insert( std::make_pair( "MACD", "signal" ) ); 

			IndicatorApp app( spx_db );
			app.add( "CCI", 7 );
			app.add( "BOP", -1 );
			app.add( "ADX", 7 );
			//app.add( "MFI", 7 );
			app.add( "STDDEV", 7, 3.5 );
			app.add( "APO", -1, 5, 7 );
			app.add( "ADO", -1, 5, 7 );
			app.add( "MACD", 7, 12, 26 );
   			//app.add( "STOCHRSI", 12, 3, 5 );
			app.initialise();
			
			MVABacktester::Type define_type = static_cast<MVABacktester::Type>( type );
			for (int i(0); i < N ; ++i ) {
				MVABacktester backtester( spx_db, app, mvaMethod, cutValue);
				backtester.setup( inputvars, leaves, "outputFile.root", "weights" );	
				
				if( N > 1 && define_type != MVABacktester::Random ) {
				    cutValue = cutval_min + ( ( cutval_max - cutval_min) / N ) * i;
                                }
				
				backtester.setCutValue( cutValue );
				backtester.setSeed( UInt_t( i * rand() ) );

				backtester.run( dayshift, static_cast<MVABacktester::Type>(define_type) );

				std::cout <<  "Tester with mva value @ " << cutValue << std::endl;
				// Print trades  
				//Report::header("Closed trades");
				//backtester.positions("SPX").closed().print();

				//Report::header("Open trades");
				//backtester.positions("SPX").open().print();

				// SPX stats
				Report::header("Strategy trades");
				backtester.positions().stratPos().print();
				// SPX stats
				Report::header("SPX Stats");
				EOMReturnFactors spx_eomrf2( backtester.positions("SPX"), load_begin, load_end, rf_rate);
				EOMReport eomrp(spx_eomrf2);
				eomrp.print();

				// Print simulation reports
				//Report::header("Trade results");
				ReturnFactors spx_eomrf( backtester.positions() );
				Report rp(spx_eomrf);
				rp.print();

				// Print Position stats
				/*Report::header("Position stats");
				  PositionFactorsSet pf( backtester.positions().stratPos().closed());
				  PositionsReport pr(pf);
				  pr.print();*/

				// Position analysis
				//Report::header("Positions");
				PositionFactorsSet pfs(backtester.positions());
				PositionsReport pr(pfs);
				pr.print();

				// Portfolio stats
				Report::header("Portfolio Stats");
				PortfolioReturns prns;
				prns.add( &spx_eomrf2 );
				PortfolioReport preport(prns);
				preport.print();
				//Report::precision(2);

				/*eomrp.roi();
				  eomrp.cagr();
				  eomrp.gsdm();
				  eomrp.sharpe();
				  eomrp.roi();*/

				treeWriter.column( "mvaCutValue", cutValue );
				treeWriter.column( "SHARPE", spx_eomrf2.sharpe() );
				treeWriter.column( "ROIEOM", spx_eomrf2.roi() );
				treeWriter.column( "CAGR", spx_eomrf2.cagr() );
				treeWriter.column( "GSDM", spx_eomrf2.gsd() );

  				PositionSet pset = spx_eomrf.dd(); 
				double maxdd(0.0);
				if( !pset.empty() ) {
					ReturnFactors ddrf( pset );
					maxdd = ddrf.roi();
				}
				
				treeWriter.column( "MAXDD", maxdd );

				treeWriter.column( "ROI", spx_eomrf.roi() );
				treeWriter.column( "STDDEV", spx_eomrf.stddev() );
				treeWriter.column( "SKEW", spx_eomrf.skew() );
				treeWriter.column( "AVE", spx_eomrf.avg() );
				treeWriter.column( "nPositions", spx_eomrf.num() );
				treeWriter.write();
				// BnH
				/*Report::header("SPX BnH");
				  BnHTrader bnh(spx_db); 
				  bnh.run();
				  bnh.positions().print();
				  EOMReturnFactors bnh_eomrf(bnh.positions(), load_begin, load_end);
				  EOMReport bnh_rp(bnh_eomrf);
				 */

			}
		}

	} catch( std::exception& ex ) {

		std::cerr << "Unhandled exception: " << ex.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	return 0;
}
