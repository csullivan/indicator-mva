// include local
#include "TreeWriter.hpp"

// boost include
#include "boost/make_shared.hpp"

using namespace NN;


//**************************************************************************************************************************
TreeWriter::TreeWriter()
  : m_file( new TFile( "output.root", "RECREATE" ) ), m_tree( new TTree( "data", "data" ) ), m_temp(0) {
}


//**************************************************************************************************************************
TreeWriter::TreeWriter( const std::string& filename, const std::string& treename )
  : m_file( new TFile( filename.c_str(), "RECREATE" ) ), m_tree( new TTree( treename.c_str(), treename.c_str() ) ), m_temp(0) {

}


//**************************************************************************************************************************
TreeWriter::TreeWriter( const std::string& filename, TTree* tree ) 
  : m_file( new TFile( filename.c_str(), "RECREATE" ) ) {

  m_tree =  dynamic_cast< TTree* >( tree->CloneTree(0) );
  m_tree->SetName( tree->GetName() );
  //m_tree->SetBranchStatus("*",1); // select all branches
  m_temp = tree;
}


//**************************************************************************************************************************
TreeWriter::~TreeWriter() {
 
  m_file->cd();
  m_file->ls();
  m_tree->ResetBranchAddresses();
  //m_tree->Write(); 
  m_file->Write();
  m_file->Close();
  delete m_file; m_file = 0;
}


//**************************************************************************************************************************
void TreeWriter::add( const std::string& variable ) {
  // check if variable has already been added or not?
  if( m_variables.find( variable ) == m_variables.end() ) {

    m_variables.insert( std::make_pair( variable, 0.0 ) );
    m_tree->Branch( variable.c_str(), &m_variables[ variable ] );
	
  } else {

    std::cerr << "WARNING: Variable " << variable << " already added, skipping." << std::endl;
  }
}      


//**************************************************************************************************************************
void TreeWriter::write( ) {
  
  // Fill the tree with data
  m_tree->Fill();

}


//**************************************************************************************************************************
void TreeWriter::write( const unsigned int& i ) {

  // Fill the tree with data  
  if ( m_temp != 0 ) {
    m_temp->GetEntry(i);
  }
  m_tree->Fill();

}


//**************************************************************************************************************************
void TreeWriter::column( const std::string& variable, const boost::any& value ) throw( TreeWriterException ) {
     
  try {    
    if( m_variables.find( variable ) != m_variables.end() ) {
      m_variables[variable] = boost::any_cast< Float_t >( value ); 

    }
    else {
      std::cerr << "WARNING: Variable " << variable << " has not been added to tuple." << std::endl;
    }
  } catch ( boost::bad_any_cast& e ) {
    throw TreeWriterException( e.what() );
  }
  
}


//**************************************************************************************************************************
void TreeWriter::column( const std::string& variable, const Float_t& value ) throw( TreeWriterException ) {
     
  if( m_variables.find( variable ) != m_variables.end() ) {
    m_variables[variable] = value; 

  }
  else {
    std::cerr << "WARNING: Variable " << variable << " has not been added to tuple." << std::endl;
  }
}  
