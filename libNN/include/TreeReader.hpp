#ifndef TreeReader_HPP
#define TreeReader_HPP 1 

/*
 * Library to read tree from ROOT tuple in automatic way which gets variables
 * by names of formula string.
 */

// STL includes
#include <string>
#include <vector>
#include <iostream>
#include <functional>

// ROOT includes
#include "TChain.h"
#include "TTree.h"
#include <TTreeFormula.h>

namespace NN {

  typedef enum
  {
      INT, FLOAT, DOUBLE, BOOL, FORMULA 
  } Types;

  class variable {

    public:
  
      std::string name;
      std::string bname;
      std::string title;
      float float_value;
      double double_value;
      int int_value;
      bool bool_value;
      float float_array[2000];
      float double_array[2000];
      int arraySize;
      Types type;
      int m_nGets;
      TTreeFormula *m_formula;
      Float_t evaluateFormula( const int& i ) const;

  };

  class varEq : public std::unary_function<variable*, bool> {

    public:
      explicit varEq(const std::string& ss): m_s(ss) {}
      bool operator() (const variable* c) const  {
        return m_s.compare(c->name) ?  false : true;
      }

    private:
      std::string m_s;
  };

  class TreeReader {

    public:
      TreeReader( const std::string& treeName );
      TreeReader();
      ~TreeReader();

      void AddFile(std::string filename) { if ( m_fChain) m_fChain->Add( filename.c_str() );
      std::cout << "Number of trees in chain: " << m_fChain->GetNtrees() << std::endl; }
      Long64_t GetEntries() { return m_fChain->GetEntries();}
      int GetEntry( const Long64_t& ientry ) { return m_fChain->GetEntry( ientry ); }
  
      void Initialize();

      void BranchNewTree( TTree* tree );

      TChain* GetChain() { return m_fChain; }
      TTree* GetTree() { return dynamic_cast< TTree* >( m_fChain ); }
 
      Float_t GetValue( const std::string& name, const int& iValue = 0 );

    private:
      bool partialSort();
 
      TChain* m_fChain;
      std::vector<variable*> m_varList;

      int m_nGets;
      int m_nSwaps;
      bool m_continueSorting;

      //ClassDef(TreeReader,2)

  };

}  // end namespace NN
#endif // TreeReader

