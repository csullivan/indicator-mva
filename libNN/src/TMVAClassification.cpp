// STL include
#include <cstdlib>

// ROOT include
#include "TString.h"
#include "TCut.h"

// Local include
#include "TMVAClassification.hpp"

using namespace NN;


//**************************************************************************************************************************
TMVAClassification::TMVAClassification( const std::vector< std::string >& mvaMethods, const std::vector< std::string >& inputvars, const std::string& outname )
  : m_tmvaFactory(  new TMVA::Factory( "TMVAClassification", TFile::Open( outname.c_str(), "RECREATE"), "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" ) ), m_variablesset( false ), m_prepareTestAndTrain( false ), m_methodset( false ), m_sigandbackset( false ), m_methods( mvaMethods ) {
  m_method_strings.clear();
  m_methods_options.clear();
  //bookMethods();
  setVariables( inputvars );
  
}

//**************************************************************************************************************************
TMVAClassification::TMVAClassification( const std::vector< std::string >& mvaMethods, const std::map< std::string, std::string >& input_vars_options, const std::string& outname ) 
  : m_tmvaFactory(  new TMVA::Factory( "TMVAClassification", TFile::Open( outname.c_str(), "RECREATE"), "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" ) ), m_variablesset( false ), m_prepareTestAndTrain( false ), m_methodset( false ), m_sigandbackset( false ), m_methods_options( input_vars_options ) {

  std::map< std::string, std::string >::const_iterator it = m_methods_options.begin();
  const std::map< std::string, std::string >::const_iterator endit = m_methods_options.end();
  m_methods.clear();
  m_methods.resize( m_methods_options.size() );
  for ( size_t i(0) ; it != endit; ++it, ++i ) {
    m_methods[i] = it->first;
  }
  std::vector< std::string > inputvars( m_methods_options.size() );
  std::map< std::string, std::string >::const_iterator iter =  m_methods_options.begin();
  const std::map< std::string, std::string >::const_iterator enditer =  m_methods_options.end();

  for( int i(0); iter != enditer; ++iter, ++i ) {
    inputvars[i] = iter->first;
  }
  setVariables( inputvars );
  
}



//**************************************************************************************************************************
TMVAClassification::~TMVAClassification() {

  // clean up

  delete m_tmvaFactory; m_tmvaFactory = 0;
}


//**************************************************************************************************************************
void TMVAClassification::setMethodOption( const std::string& method, const std::string& option){
  
  if ( std::find( m_methods.begin(), m_methods.end(), method ) != m_methods.end() ) {
    
    std::cout << "INFO: TMVAClassification - changing method string for " << method << " to use " <<  option << std::endl;
    m_method_strings.insert( std::make_pair( method, option) );
  } else {
    std::cout << "WARNING: TMAVClassification - method with name " << method << " does not exist in TMVAClassification instance.";
  }
}


//**************************************************************************************************************************
void TMVAClassification::bookMethods(){
  
  if (m_variablesset && m_sigandbackset && m_prepareTestAndTrain ) {
    // --- Book the MVA methods
    std::vector< std::string >::iterator it = m_methods.begin();
    const  std::vector< std::string >::iterator endit = m_methods.end();
    for ( ; it != endit ; ++it ) {

      if ( DefinedMVAs.find(*it) != DefinedMVAs.end() ) {
      
        if( m_method_factory.find(*it) == m_method_factory.end() ) {
     
          // Initialise all to the default options
          TMVA::Types::EMVA mvatype; 
          bool ok = getType( *it, mvatype );   
          if(ok) {
	    std::cout << "INFO: Adding method " << *it << " to factory." << std::endl;
	    std::string methodString = DefinedMVAs.find( *it )->second ;
	    if( m_method_strings.find( *it ) != m_method_strings.end() ){
              methodString = m_method_strings.find( *it )->second;
	    }

            m_method_factory.insert( std::make_pair( *it, dynamic_cast< TMVA::MethodBase* >( m_tmvaFactory->BookMethod( mvatype, (*it).c_str(), ( methodString ).c_str() ) )  ) );
  	    m_methodset = true;
          } else {
            std::cerr << "Could not book method as type " << *it << " not found." << std::endl;
          }

        } else {
          std::cerr << "WARNING: " << *it << " method already added, skipping." << std::endl;
        }
      } 
      else {
          std::cerr << "ERROR: Method " << *it << " is not defined in this programme." << std::endl;
          exit( EXIT_FAILURE );
      }
    }
  } else {
    std::cout << "WARNING: TMVAClassification::bookMethods(). Not all parameters are set:\n";
    std::cout << "    variables("<< m_variablesset << "), methods(" << m_methodset << "), signal and background(" << m_sigandbackset <<  "), prepare test And train(" << m_prepareTestAndTrain << ").\n";
    
  }
}


//**************************************************************************************************************************
bool TMVAClassification::getType( const std::string& name, TMVA::Types::EMVA& type ) const {

  bool ok(true);
  if( name.find("Cut") != std::string::npos ) { 
    type = TMVA::Types::kCuts;
  } else if( name.find("LD") != std::string::npos ) {
    type = TMVA::Types::kLD; 
  } else if( name.find("Likelihood") != std::string::npos ) { 
    type = TMVA::Types::kLikelihood;
  } else if( name.find("PDE") != std::string::npos ) { 
    type = TMVA::Types::kPDERS;
  } else if( name.find("KNN") != std::string::npos ) { 
    type = TMVA::Types::kKNN;
  } else if( name.find("HMatrix") != std::string::npos ) { 
    type = TMVA::Types::kHMatrix;
  } else if( name.find("Fisher") != std::string::npos ) { 
    type = TMVA::Types::kFisher;
  } else if( name.find("MLP") != std::string::npos ) { 
    type = TMVA::Types::kMLP;
  } else if( name.find("CFMlpANN") != std::string::npos ) { 
    type = TMVA::Types::kCFMlpANN;
  } else if( name.find("TMlpANN") != std::string::npos ) { 
    type = TMVA::Types::kTMlpANN;
  } else if( name.find("BDT") != std::string::npos ) { 
    type = TMVA::Types::kBDT;
  } else if( name.find("SVM") != std::string::npos ) { 
    type = TMVA::Types::kSVM;
  } else {
    std::cerr << "WARNING: TMVA::Types for " << name << " not recognised. Will use defaults." << std::endl;
    ok = false;
  }
  return ok;
}


//**************************************************************************************************************************
void TMVAClassification::setOptions( const std::string& mvaMethod, const std::string& option ) {

  TMVA::MethodBase* method =  dynamic_cast< TMVA::MethodBase* > ( m_method_factory[ mvaMethod ] );
  //TMVA::IMethod* method =  m_tmvaFactory->GetMethod( mvaMethod.c_str() );
  if( method != 0 )  {
    m_method_factory[ mvaMethod ]->SetOptions( option.c_str() );
  } else {
    std::cerr << "WARNING: MVA method called " << mvaMethod << " was not found." << std::endl;
  }

}

    
//**************************************************************************************************************************
void TMVAClassification::setVariables( const std::vector< std::string >& inputVars ) {
    
  // sanity checks
  if (inputVars.size() <= 0) {
    std::cout << "WARNING: Problem in class \"TMVAClassification\": empty input vector" << std::endl;
  }

  std::vector< std::string >::const_iterator iter = inputVars.begin();
  const std::vector< std::string >::const_iterator iterend = inputVars.end();

  // Add everything as a float
  for( ; iter != iterend; ++iter ) {

    m_tmvaFactory->AddVariable( iter->c_str(), 'F' );
  }
  m_variablesset = true;
}


//**************************************************************************************************************************
void TMVAClassification::setBackgroundWeightExpression( const std::string& weight ) {

  m_tmvaFactory->SetBackgroundWeightExpression( weight.c_str() );
   
}


//**************************************************************************************************************************
void TMVAClassification::setSignalWeightExpression( const std::string& weight ) {

  m_tmvaFactory->SetSignalWeightExpression( weight.c_str() );
  
}


//**************************************************************************************************************************
void TMVAClassification::setSignalAndBackgroundTrees( TTree* signal, TTree* background, const Double_t& signalWeight, const Double_t& backgroundWeight ) {

   // You can add an arbitrary number of signal or background trees
  if( signal != 0 ) {
    m_tmvaFactory->AddSignalTree    ( signal,     signalWeight     );
  } else {
    std::cerr << "ERROR: signal tree is not set." << std::endl;
    exit(EXIT_FAILURE);
  } 
   
  if( background != 0 ) {
    m_tmvaFactory->AddBackgroundTree( background, backgroundWeight );
  } else {
    std::cerr << "ERROR: background tree is not set." << std::endl;
    exit(EXIT_FAILURE);
  } 

  m_sigandbackset = true;
}


//**************************************************************************************************************************
bool TMVAClassification::train( const bool& optimise ) const {
  
  bool status(false);
  // train and classify
  if( m_variablesset && m_sigandbackset && m_prepareTestAndTrain && m_methodset) {
  
    // --------------------------------------------------------------------------------------------------

    // ---- Now you can optimize the setting (configuration) of the MVAs using the set of training events
    // This only works for BDTs at present
    // factory->OptimizeAllMethods("SigEffAt001","Scan");
    if( optimise ) {
      
      std::cout << "INFO: Applying ROCIntegral GA optimisation. " << std::endl;
      m_tmvaFactory->OptimizeAllMethods("ROCIntegral","FitGA");
    }

    // --------------------------------------------------------------------------------------------------
    // ---- Now you can tell the factory to train, test, and evaluate the MVAs

    // Train MVAs using the set of training events
    m_tmvaFactory->TrainAllMethods();

    // ---- Evaluate all MVAs using the set of test events
    m_tmvaFactory->TestAllMethods();

    // ----- Evaluate and compare performance of all configured MVAs
    m_tmvaFactory->EvaluateAllMethods();

    status = true;
    
    summary();

  } else {
    std::cout << "WARNING: TMVAClassification::train(). Not all parameters are set:\n";
    std::cout << "    variables("<< m_variablesset << "), methods(" << m_methodset << "), signal and background(" << m_sigandbackset <<  "), prepare test And train(" << m_prepareTestAndTrain << ").\n";
  }

  return status;
  
}

//**************************************************************************************************************************
void TMVAClassification::basicSetup( TTree* signal, TTree* background, const std::string& sWeight, const std::string& bWeight, const Double_t signalWeight, const Double_t& backgroundWeight ) {
 
  setSignalAndBackgroundTrees( signal, background, signalWeight, backgroundWeight);
  
  if( (sWeight != "" ) && ( bWeight != "" ) ) {
    setSignalWeightExpression( sWeight );
    setBackgroundWeightExpression( bWeight );
  }

  prepareTestAndTrain();
  bookMethods();

}


//**************************************************************************************************************************
void TMVAClassification::prepareTestAndTrain( const std::string& splitOpts ) {
  
  // train and classify
  if( m_variablesset && m_sigandbackset ) {
  
    //--- <FATAL> Tools: <GetSeparation> signal and background histograms have different or invalid dimensions
    // IF WE SEE THIS THEN WE CAN APPLY A SIMPLE CUT TO ALL TTREE's
    //factory->PrepareTrainingAndTestTree("troublesome_var > 0", "troublesome_var > 0", splitOptions);
    TCut cuts("");
    std::string opts = splitOpts;
    if ( splitOptions.find( splitOpts ) != splitOptions.end() ) { 
	opts = splitOptions.find( splitOpts )->second;
    }
    m_tmvaFactory->PrepareTrainingAndTestTree( cuts, opts.c_str() );
    //m_tmvaFactory->PrepareTrainingAndTestTree( cuts splitOpts.c_str() );
   
    // --------------------------------------------------------------------------------------------------

  } else {
    std::cout << "WARNING: TMVAClassification::prepareTestAndTrain(). Not all parameters are set:\n";
    std::cout << "    variables("<< m_variablesset << "), signal and background(" << m_sigandbackset << ").\n";
  }

  m_prepareTestAndTrain = true;
}


//**************************************************************************************************************************
void TMVAClassification::summary() const {


  std::cout << "INFO: Classification for " ;
  
  std::vector< std::string >::const_iterator it = m_methods.begin();
  const std::vector< std::string >::const_iterator endit = m_methods.end();
  
  for( ; it != endit; ++it ) {
    std::cout << *it << " "; 
  }
  
  std::cout << " completed. " << std::endl;

}
