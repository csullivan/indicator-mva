#ifndef TMVAClassification_HPP
#define TMVAClassification_HPP 1

// STL include
#include <map>
#include <utility>
#include <vector>
#include <string>

// ROOT include
//#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Factory.h"
#include <TMVA/MethodTMlpANN.h>
#include <TMVA/Tools.h>
#include "TMVA/Types.h"
#include "TMVA/IMethod.h"
#include "TMVA/MethodBase.h"
//#endif

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

// local include
#include "DefinedMVAs.hpp"

namespace NN {

  /** @class TMVAClassification TMVAClassification.hpp
  *
  *  Class to train a TMVA classification.
  *
  *  @author Matthew M Reid
  *  @date   2013-07-12
  */

  class TMVAClassification  {

    public:

      TMVAClassification( const std::vector< std::string >& mvaMethods, const std::vector< std::string >& inputvars, const std::string& outname = "TMVA.root" );
      TMVAClassification( const std::vector< std::string >& mvaMethods, const std::map< std::string, std::string >& input_vars_options, const std::string& outname = "TMVA.root" );
      ~TMVAClassification();

      // the classifier response
      // "inputValues" is a vector of input values in the same order as the
      // variables given to the constructor
      
      // Set the input variable names.
      void setVariables( const std::vector< std::string >& inputVars );
      void setSpectators( const std::vector< std::string >& inputVars );
      // Set the options for given mva.
      void setOptions( const std::string& mvaMethod, const std::string& option ) ;

      void setSignalAndBackgroundTrees( TTree* signal, TTree* background, const Double_t& signalWeight = 1.0, const Double_t& backgroundWeight = 1.0 );
      void setMethodOption( const std::string& method, const std::string& option = "" );
      void setBackgroundWeightExpression( const std::string& weight = "nsig_S_sw" );
      void setSignalWeightExpression( const std::string& weight = "nbkg_B_sw" );
      void prepareTestAndTrain( const std::string& splitOpts = "default" );
      void bookMethods();

      void basicSetup( TTree* signal, TTree* background, const std::string& sWeight = "", const std::string& bWeight = "", const Double_t signalWeight = 1.0, const Double_t& backgroundWeight = 1.0 );
      bool train( const bool& optimise = false ) const;

    private:
      bool getType( const std::string& name, TMVA::Types::EMVA& type ) const ;
      void summary() const;
 
      TMVA::Factory *m_tmvaFactory;
      bool m_variablesset, m_prepareTestAndTrain, m_methodset, m_sigandbackset;
      std::vector< std::string > m_methods;
      std::map< std::string, std::string > m_method_strings;
      std::map< std::string, std::string > m_methods_options;
      std::map< std::string, TMVA::MethodBase* > m_method_factory;

  };

}

#endif // TMVAClassification_HPP
