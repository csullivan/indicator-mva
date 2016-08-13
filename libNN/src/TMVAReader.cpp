// STL include
#include <cstdlib>
#include <limits>       // std::numeric_limits

// ROOT include
#include "TString.h"

// Local include
#include "TMVAReader.hpp"

// boost include
#include "boost/make_shared.hpp"

using namespace NN;

//**************************************************************************************************************************
TMVAReader::TMVAReader() 
  //: IClassifierReader(), m_tmvaReader( boost::make_shared< TMVA::Reader >( "!Color:!Silent" ) ), m_outputFile( "outputFile.root" ) {
  : m_tmvaReader( new TMVA::Reader( "!Color:!Silent" ) ), m_outputFile( "outputFile.root" ), m_addedMVA(false), m_errorStore(false) {

}


//**************************************************************************************************************************
TMVAReader::TMVAReader( const std::vector< std::string >& mvaMethods, const std::vector< std::string >& inputVars, const std::string outputFile, const std::string& weightsDirPrefix )
  //: IClassifierReader(), m_tmvaReader( boost::make_shared< TMVA::Reader >( "!Color:!Silent" ) ), m_outputFile( outputFile ) {
  : m_tmvaReader( new TMVA::Reader( "!Color:!Silent" ) ), m_outputFile( outputFile ), m_nn_outputs( mvaMethods ), m_addedMVA(false), m_errorStore(false)  {

  
  setVariables( inputVars );

  // --------------------------------------------------------------------------------------------------
  // --- Book the MVA methods
  TString dir    = weightsDirPrefix+"/";
  TString prefix = "TMVAClassification";

  // Book method(s)
  std::vector< std::string >::const_iterator it = m_nn_outputs.begin();
  const std::vector< std::string >::const_iterator end = m_nn_outputs.end();
  for ( ; it != end; ++it ) {

    if ( DefinedMVAs.find(*it) != DefinedMVAs.end() ) {

      TString methodName = TString( *it + " method");
      TString weightfile = dir + prefix + "_" + TString( *it + ".weights.xml");
      std::cout << "weight file = " << weightfile << std::endl; 
      m_tmvaReader->BookMVA( methodName, weightfile );
      m_errors.insert( std::make_pair( *it, 0. ) );
      m_addedMVA = true; 
    } else {
      std::cerr << "ERROR: MVA method " << *it << " is not recognised." << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

//**************************************************************************************************************************
TMVAReader::~TMVAReader() {

  // clean up
  delete m_tmvaReader; m_tmvaReader = 0;
  delete m_treeReader; m_treeReader = 0;
  delete m_treeWriter; m_treeWriter = 0;
}


//**************************************************************************************************************************
void TMVAReader::setFileAndTree( const std::string& inputrootfile, const std::string& treepath ) {
 
  m_treeReader = new TreeReader( treepath ) ; 
  m_treeReader->AddFile( inputrootfile );
  m_treeReader->Initialize();

  m_treeWriter = new TreeWriter( m_outputFile, m_treeReader->GetTree() );
}


//**************************************************************************************************************************
void TMVAReader::setVariables( const std::vector< std::string >& inputVars ) {
    
  // sanity checks
  if (inputVars.size() <= 0) {
    std::cout << "WARNING: Problem in class \"TMVAReader\": empty input vector" << std::endl;
    //fStatusIsClean = false;
    return;
  }

  std::vector< std::string >::const_iterator iter = inputVars.begin();
  const std::vector< std::string >::const_iterator iterend = inputVars.end();

  // --------------------------------------------------------------------------------------------------
  // ---- Add the input variables that were used in the Classification 
  for( ; iter != iterend; ++iter ) {
    m_variables.insert( std::make_pair( *iter, 0. ) );
    m_tmvaReader->AddVariable( iter->c_str(), &m_variables[*iter] );
  } 

}


//**************************************************************************************************************************
void  TMVAReader::fillTuple() {
  
  if( m_addedMVA ) {
 
    // --------------------------------------------------------------------------------------------------
    // ---- Add nn output for writing to the tuple 
    for( std::vector< std::string >::iterator iter = m_nn_outputs.begin(); iter != m_nn_outputs.end(); ++iter ) {
    
      m_treeWriter->add( "nn_" + *iter );
      if(m_errorStore) {
        m_treeWriter->add( "nn_error_" + *iter );
      }
    }

    // --------------------------------------------------------------------------------------------------
    // ---- Begin looping over tuple and adding entries
    int N = getEntries();
    TMVA::Timer timer( N, "TMVAReader", kTRUE );
    for ( int i(0); i < N; ++i ) {
    
      updateVariables( i );
      m_treeWriter->write( i );
      timer.DrawProgressBar( i );
    }
    std::cout << "INFO: Reader finished and new tuple created." << std::endl;
  } else {
    std::cerr << "WARNING: No MVAs added so nothing to fill." << std::endl ;
  }
}


//**************************************************************************************************************************
Float_t TMVAReader::getMvaError( const std::string& method ) {

  return getMvaError(  TString(method) ); 
}


//**************************************************************************************************************************
Float_t TMVAReader::getMvaError( const TString& method ) {

  
  Float_t value = -std::numeric_limits< Float_t >::max();

  if( m_errors.find( TString( method ).Data() ) != m_errors.end() ) { 
    std::string name =  method.Data();
    name += " method";
    value = m_errors.find( method.Data() )->second;
  }
  else {
    std::cerr << "WARNING: TMVAReader - getMvaValue has issue as method containing name " << method << " does not exist.\n";
    // Better to use a throw here!	
    //exit(EXIT_FAILURE); 
  }
  return value;
}


//**************************************************************************************************************************
void TMVAReader::setMvaError( const TString& method ) {

 m_errors[ TString( method ).Data() ] = m_tmvaReader->GetMVAError(); 
}


//**************************************************************************************************************************
Float_t TMVAReader::getMvaValue( const std::string& method ) {

 return getMvaValue( TString( method ) ); 
  
}


//**************************************************************************************************************************
Float_t TMVAReader::getMvaValue( const TString& method ) {

  
  Float_t value = -std::numeric_limits< Float_t >::max();

  if( m_errors.find( method.Data() ) != m_errors.end() ) {
    value = static_cast<Float_t>( m_tmvaReader->EvaluateMVA( method + " method" ) );
    setMvaError( method );
  }
  else {
    std::cerr << "WARNING: TMVAReader - getMvaValue has issue as method containing name " << method << " does not exist.\n";
    // Better to use a throw here!	
    //exit(EXIT_FAILURE); 
  }

  return value; 
  
}


//**************************************************************************************************************************
void TMVAReader::setVariable( const std::string& name, const Float_t& value ) {

  if ( m_variables.find( name ) != m_variables.end() ) {
    m_variables[ name ] = value;
  }
  else { 
    std::cerr << "TMVAReader: setVariable - " << name << " does not exist in map.\n";
  }
}


//**************************************************************************************************************************
void TMVAReader::updateVariables( const unsigned int& i ) {

  m_treeReader->GetEntry( i );

  // --------------------------------------------------------------------------------------------------
  // ---- Set the values used for the training tuple
  std::map< std::string, Float_t >::iterator it = m_variables.begin();
  const std::map< std::string, Float_t >::iterator endit = m_variables.end();
  for ( ; it != endit; ++it ) {
    setVariable( it->first, m_treeReader->GetValue( it->first ) ); 
  }
  
  // --------------------------------------------------------------------------------------------------
  // ---- set the column entries for writing to the tuple
  std::vector< std::string >::const_iterator iter = m_nn_outputs.begin();
  const std::vector< std::string >::const_iterator enditer = m_nn_outputs.end();
  for ( ; iter != enditer; ++iter ) {
     
    TString method = TString( *iter );

    TString methodName = method + TString( " method" );
    //std::cout << "Method " << method << " has value " << m_tmvaReader->EvaluateMVA( methodName ) << " with error " << m_tmvaReader->GetMVAError()<<  " at i(" << i << ")"<< std::endl;
    //std::cout << "Method " << method << " has value " << getMvaValue( method ) << " with error " << m_tmvaReader->GetMVAError()<<  " at i(" << i << ")"<< std::endl;
    m_treeWriter->column( "nn_" + *iter, getMvaValue( method ) );
    if(m_errorStore) {
      m_treeWriter->column( "nn_error_" + *iter, getMvaError( method ) );
    }
  }
}
