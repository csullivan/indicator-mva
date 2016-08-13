// My test of the class functionality

#include <iostream>
#include <vector>
#include <string>

// include local
#include "DefinedMVAs.hpp"
#include "TMVAClassification.hpp"
#include "TMVAReader.hpp"
#include "TreeWriter.hpp"

// Boost
#include <boost/program_options.hpp>

// Hudson
#include <Database.hpp>
#include <PositionFactors.hpp>
#include <PositionFactorsSet.hpp>
#include <BnHTrader.hpp>
#include <EOMReturnFactors.hpp>
#include <IndicatorApp.hpp>

// include root
#include "TFile.h"
#include "TTree.h"

bool trainer = true;
bool evaluate = true;

using namespace std;
using namespace boost::gregorian;
using namespace Series;
namespace po = boost::program_options;

void usage( const char* name ) {
  std::cerr << "Usage:" << std::endl;
  std::cerr << name << "  <input.root> <treename> (<output.root>)" << std::endl;
  std::cerr << "<input.root>    the input root file name to be used for calculations." << std::endl;
  std::cerr << "<treename>      the path to the tree inside the input.root file passed in." << std::endl;
  std::cerr << "<output.root>   optional argument to store data from input.root as well the MVA response to a new file, if not argument set, input.root get over written." << std::endl;
}


int main(int argc, const char* argv[]) {

  std::string inputFileName(""), treepath("data"), outputFileName("");
  std::vector<std::string> mvaMethods, inputvars, inputfiles;
  try {

    /*
     * Extract simulation options
     */
    po::options_description desc("Allowed options");
    desc.add_options()
  	("help", "produce help message")
        ( "train,t", 	   boost::program_options::value<std::string>()->implicit_value( "stdin" ),  	  	       "sets MVA training flag" )
        ( "eval,e",	   boost::program_options::value<std::string>()->implicit_value( "stdin" ),  	  	       "sets MVA evaluation flag" )
  	("input_file",   po::value<std::string>(&inputFileName),     	"the input root file name to be used for calculations")
  	("tree_path", po::value<std::string>(&treepath),     		"the path to the tree inside the input.root file passed in (data)")
  	("output_file",   po::value<std::string>(&outputFileName), 	"optional argument to store data from input.root as well the MVA response to a new file, if no argument is set, input.root get over written")       
	( "mva", 		   boost::program_options::value< std::vector< std::string > >( &mvaMethods )->multitoken(), "specifies the training MVAs to use. Options are: Cuts, CutsD, CutsPCA, CutsGA, CutsSA\nLikelihood, LikelihoodD, LikelihoodPCA, LikelihoodKDE, LikelihoodMIX\nPDERS, PDERSD, PDERSPCA, PDEFoam, PDEFoamBoost, KNN\nMLP, MLPBFGS, MLPBNN, CFMlpANN, TMlpANN\nSVM, BDT, BDTG, BDTB, BDTD" )
        ( "var",             boost::program_options::value< std::vector< std::string > >( &inputvars )->multitoken(), "specify the training variables to use." );

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if( vm.count("help") ) {
  	cout << desc << endl;
 	exit(0);
    }

    if( vm["input_file"].empty() ) {
 	cout << desc << endl;
	exit(1);
    }
    if( vm.count( "train" ) ) {
      trainer = true;
    }

    if( vm.count( "eval" ) ) {
      evaluate = true;
    }
   
    if( !trainer && !evaluate ) {
      std::cerr << "No point running this programme is you are not going to do anything...\nSpecify '-t' or '-e' flag for trainer or evaluation.\n";
      std::cout << desc << std::endl;
      std::exit( EXIT_FAILURE );
    }
    if(vm["output_file"].empty()) outputFileName = inputFileName;
   

    // The input MVAs 
  // The input MVAs 
  //mvaMethods.push_back("Cuts");
  if( mvaMethods.empty() ) {
    std::cout << "Will train with default mva methods: LD, SVM, MLP, BDT, BDTB, BDTD, BDTG" << std::endl;
    //mvaMethods.push_back("Cuts");
    //mvaMethods.push_back("LD");
    //mvaMethods.push_back("LikelihoodKDE");//LikelihoodKDE
    //mvaMethods.push_back("SVM");
    mvaMethods.push_back("MLP");
    mvaMethods.push_back("MLPBFGS");
    mvaMethods.push_back("MLPBNN");
    //mvaMethods.push_back("BDT");
    //mvaMethods.push_back("BDTB");
    mvaMethods.push_back("BDTD");
    //mvaMethods.push_back("BDTG");
    //mvaMethods.push_back("PDERS");
    //mvaMethods.push_back("PDERSPCA");
    //mvaMethods.push_back("BoostedFisher");
    mvaMethods.push_back("TMlpANN");
    //mvaMethods.push_back("CFMlpANN");
  }

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

    // Begin training the networks
    if( trainer ) {
      NN::TMVAClassification* classifier = new NN::TMVAClassification( mvaMethods, inputvars);

      TFile *file = TFile::Open( inputFileName.c_str() );
      TTree *treeS = dynamic_cast< TTree* > ( file->Get( treepath.c_str() ) );
      TTree *treeB = dynamic_cast< TTree* > ( file->Get( treepath.c_str() ) );

      std::cout << "Before cut Signal entries: " << treeS->GetEntries() << std::endl;
      std::cout << "Before cut Background entries: " << treeB->GetEntries() << std::endl;
      treeS = treeS->CopyTree( "nsig>1.01" );
      treeB = treeB->CopyTree( "nbkg>1.01" );
      std::cout << "After cut Signal entries: " << treeS->GetEntries() << std::endl;
      std::cout << "After cut Background entries: " << treeB->GetEntries() << std::endl;
      // set the signal and background trees
      classifier->setSignalAndBackgroundTrees( treeS, treeB, 1., 1.);
  
      // set the weighting expressions if you have them in the trees.
      classifier->setBackgroundWeightExpression( "nsig_S_sw" );
      classifier->setSignalWeightExpression( "nbkg_B_sw" );
      classifier->prepareTestAndTrain( );

      //classifier->prepareTestAndTrain("SOME WIERD STRING SETTINGS");
      classifier->bookMethods();
      classifier->setOptions( "BDTD", NN::DefinedMVAs.find("BDTD")->second + ":PrunStrength=100" );
      classifier->setOptions( "BDT", "!H:!V:NTrees=150:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.2:SeparationType=GiniIndex:nCuts=20:VarTransform=D,P,G,N:CreateMVAPdfs:PruneStrength=50000:PruneBeforeBoost" );
      //classifier->setOptions( "MLP", "!H:!V:NeuronType=tanh:VarTransform=P,D,N:NCycles=400:HiddenLayers=N+2:TestRate=10:!UseRegulator:CreateMVAPdfs:UseRegulator:EpochMonitoring" );  
      //classifier->basicSetup( treeS, treeB );
      //classifier->setOptions( "TMlpANN", NN::DefinedMVAs.find("TMlpANN")->second + ":VarTransform=D,G_Signal,N" );

      //classifier->setOptions( "SVM", "!H:!V:Gamma=0.2:Tol=0.0001:C=0.9:MaxIter=1500:NSubSets=2:CreateMVAPdfs:VarTransform=D,P,G,N");
      classifier->train( false );
      delete classifier; classifier = 0;
    }
 
    if( evaluate ) {
      NN::TMVAReader* reader = new NN::TMVAReader( mvaMethods, inputvars, outputFileName );
      reader->setFileAndTree( inputFileName, treepath );
      reader->fillTuple();

      delete reader; reader = 0;
    }

  } catch( std::exception& ex ) {

    std::cerr << "Unhandled exception: " << ex.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  return 0;
}

