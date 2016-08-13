#ifndef TMVAReader_HPP
#define TMVAReader_HPP 1

// STL include
#include <map>
#include <utility>
#include <vector>
#include <cmath>
#include <exception>

// ROOT include
#include "TMVA/Reader.h"
#include "TMVA/Timer.h"
#include "TFile.h"
#include "TTree.h"

// local include
#include "IClassifierReader.hpp"
#include "TreeWriter.hpp"
#include "TreeReader.hpp"
#include "DefinedMVAs.hpp"

// Boost include
#include "boost/shared_ptr.hpp"
#include "boost/any.hpp"


namespace NN {

  /** @class TMVAReader TMVAReader.hpp
  *
  *  Class to read .xml output from a TMVA classification.
  *  Returns mva value for given input params.
  *
  *  No protection to make sure the variables are all set.
  *  @author Matthew M Reid
  *  @date   2013-07-12
  */

  class TMVAReader  {
  //class TMVAReader : public IClassifierReader  {

    public:

      TMVAReader();
      TMVAReader( const std::vector< std::string >& mvaMethods, const std::vector< std::string >& inputVars, const std::string outputFile = "outputFile.root" , const std::string& weightsDirPrefix = "weights");
      ~TMVAReader();

      // the classifier response
      // "inputValues" is a vector of input values in the same order as the
      // variables given to the constructor
      //double GetMvaValue( const unsigned int& i ) const;
      
      // Set the .root TFile and path to TTree. 
      void setFileAndTree( const std::string& inputrootfile, const std::string& treepath );
  
      // Set the input variable names.
      void setErrorStore( const bool& store = false ) { m_errorStore = store; }
      void setVariables( const std::vector< std::string >& inputVars );
      void setVariable( const std::string& name, const Float_t& value );
      
      // Get the mva value
      Float_t getMvaValue( const std::string& method ) ;
      Float_t getMvaValue( const TString& method ) ;
      // Gets the mva error
      Float_t getMvaError( const std::string& method ) ;
      Float_t getMvaError( const TString& method ) ;

      int getEntries() const { return m_treeReader->GetEntries(); }
      void fillTuple(); 

    private:

      void updateVariables( const unsigned int& i );
      void setMvaError( const TString& method ) ;
  
      TMVA::Reader* m_tmvaReader;
      TreeWriter* m_treeWriter;
      TreeReader* m_treeReader;
      std::string m_outputFile;
  
      std::map< std::string, Float_t > m_variables;
      std::map< std::string, Float_t > m_errors;
      std::vector< std::string > m_nn_outputs;
      bool m_addedMVA;
      bool m_errorStore;
  };

}

#endif // TMVAReader_HPP
