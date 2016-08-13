#include "TreeReader.hpp"

#include <TROOT.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TLeaf.h>
#include <TTree.h>
#include <TString.h>

#include <algorithm>

using namespace NN;
//ClassImp(TreeReader)

Float_t variable::evaluateFormula( const int& i ) const {
  Float_t value = -999;
  if( m_formula != 0 ) { 
    m_formula->UpdateFormulaLeaves( );
    value = m_formula->EvalInstance( i ); 
  } 
  return value;
}


TreeReader::TreeReader( const std::string& treeName ) {

  m_fChain = new TChain( treeName.c_str() );
  m_nGets = 0;
  m_nSwaps = 0;
  m_continueSorting = true;
}

TreeReader::TreeReader() {

  m_fChain = new TChain( "" );
  m_nGets=0;
  m_nSwaps=0;
  m_continueSorting=true;
}

TreeReader::~TreeReader() {

  // Some statistics for debugging
  std::cout << "Total calls of GetValue function: " << m_nGets << std::endl;
  std::cout<<"Variables accessed with more than 1% frequency and their positions:\n";

  for( unsigned int i(0); i < m_varList.size(); ++i ) {
    if ( 100.*m_varList[i]->m_nGets/m_nGets >= 1 ) {
          std::cout<<"   "<<m_varList[i]->name<<" "<<100*m_varList[i]->m_nGets/m_nGets<<"  "<<i<<std::endl;
      }
  }
  std::cout << "Number of calls to sort function: " << m_nSwaps << std::endl;
  
  std::vector< variable* >::iterator iter = m_varList.begin() ;
  const std::vector< variable* >::iterator enditer = m_varList.end() ;
  for ( ; iter != enditer; ++iter ) {
    if( (*iter)->m_formula != 0 ) {
      delete (*iter)->m_formula; (*iter)->m_formula = 0;
    }
    delete *iter; *iter = 0;
  }

  delete m_fChain; m_fChain = 0;
}

/*
 * In this place we need to get list of all branches and loop over them.
 * For each branch, determine type, fill in variable which gets pushed to
 * m_varList and tree branch address will be initialized to appropriate
 * variable.
 */
void TreeReader::Initialize()
{

    TObjArray* branches = dynamic_cast<TObjArray*>( m_fChain->GetListOfBranches() );
    int nBranches = branches->GetEntries();

    for( int i(0); i < nBranches; ++i ) {

        TBranch* branch = dynamic_cast<TBranch*>( branches->At(i) );
        TLeaf* leaf = dynamic_cast<TLeaf*>( branch->GetLeaf( branch->GetName() ) );
        if ( leaf == 0 )  // leaf name is different from branch name
        {
            TObjArray* leafs = dynamic_cast<TObjArray*>( branch->GetListOfLeaves() );
            leaf = dynamic_cast<TLeaf*>( leafs->At(0) );
        }
        variable* tmpVar;
        m_varList.push_back( new variable() ); 
        tmpVar = ( m_varList[ m_varList.size()-1 ] );

        tmpVar->name = leaf->GetName();
        tmpVar->bname = branch->GetName();
        tmpVar->title = leaf->GetTitle();
        // This does not work before reading event and even then I would have
        // to make assumption.
        //tmpVar->arraySize=leaf->GetLen();
        // Find out whether we have array by inspecting leaf title
        if ( tmpVar->title.find("[")!=std::string::npos )
        {
            tmpVar->arraySize=2000; // Dirty hack, tree knows actual size, I do not have to
        }
        else
        {
            tmpVar->arraySize=0;
        }
        tmpVar->m_nGets=0;

        if ( strcmp(leaf->GetTypeName(),"Float_t")==0 )
        {
            tmpVar->type=FLOAT;
        }
        else if ( strcmp(leaf->GetTypeName(),"Int_t")==0 )
        {
            tmpVar->type=INT;
        }
        else if ( strcmp(leaf->GetTypeName(),"Bool_t")==0 )
        {
            tmpVar->type=BOOL;
        }
        else if ( strcmp(leaf->GetTypeName(),"Double_t")==0 )
        {
            tmpVar->type=DOUBLE;
        }

        variable* lastAdded=tmpVar;

        switch (lastAdded->type)
        {
            case FLOAT:
                if ( lastAdded->arraySize>1 )
                    m_fChain->SetBranchAddress(lastAdded->bname.c_str(),(void*)lastAdded->float_array);
                else
                    m_fChain->SetBranchAddress(lastAdded->bname.c_str(),(void*)&lastAdded->float_value);
                break;

            case DOUBLE:
                if ( lastAdded->arraySize>1 )
                    m_fChain->SetBranchAddress(lastAdded->bname.c_str(),(void*)lastAdded->double_array);
                else
                    m_fChain->SetBranchAddress(lastAdded->bname.c_str(),(void*)&lastAdded->double_value);
                break;

            case INT:
                m_fChain->SetBranchAddress(lastAdded->bname.c_str(),(void*)&lastAdded->int_value);
                break;

            case BOOL:
                m_fChain->SetBranchAddress(lastAdded->bname.c_str(),(void*)&lastAdded->bool_value);
                break;
            default:
                break;
        }
        //    std::cout<<"Branch: "<<lastAdded->name<<"; "<<lastAdded->type<<"; "<<lastAdded->arraySize<<std::endl;
    }

    std::cout<<"Set up "<<m_varList.size()<<" branches\n";

}


// This needs update
void TreeReader::BranchNewTree(TTree* tree)
{

    for (std::vector<variable*>::iterator it=m_varList.begin(); it!=m_varList.end(); ++it)
    {
        variable* var=*it;
        switch ( var->type )
        {
            case FLOAT:
                if ( var->arraySize>1 )
                    tree->Branch(var->name.c_str(),(void*)(var->float_array),(var->title+"/F").c_str());
                else
                    tree->Branch(var->name.c_str(),(void*)&(var->float_value),(var->title+"/F").c_str());
                break;
            case DOUBLE:
                if ( var->arraySize>1 )
                    tree->Branch(var->name.c_str(),(void*)(var->double_array),(var->title+"/D").c_str());
                else
                    tree->Branch(var->name.c_str(),(void*)&(var->double_value),(var->title+"/D").c_str());
                break;
            case INT:
                tree->Branch(var->name.c_str(),(void*)&(var->int_value),(var->title+"/I").c_str());
                break;
            case BOOL:
                tree->Branch(var->name.c_str(),(void*)&(var->bool_value),(var->title+"/O").c_str());
                break;
            default:
                break;
        }
    }

    return;
}


Float_t TreeReader::GetValue(const std::string& name, const int& iValue)
{

  ++m_nGets;

  //  std::cout<<"DEBUG GetValue: "<<name<<std::endl;

  if ( m_continueSorting && m_nGets%1000==0 && m_varList.size()>15 )
  {
      m_continueSorting=partialSort();
  }

  variable* myVar=(*(std::find_if(m_varList.begin(), m_varList.end(), varEq(name))));
  if ( ! ( (myVar) != (*(m_varList.end()))) ) {
    std::cerr<<"WARNING: TreeReader - Trying to access non-existing variable with name "<<name<<std::endl;
      //std::exit(1);
 
     
    std::cout << "INFO: TreeReader - Attempting to assign a formula variable." << std::endl;
    TTreeFormula* TTF = new TTreeFormula( name.c_str(), name.c_str(), GetTree() );
  
    TTF->UpdateFormulaLeaves();
  
    if( TTF->EvalInstance( 0 ) ) {
      //std::cout << "HARROOOOOOOOOOOOOOOO " << TTF->EvalInstance( 0 ) << std::endl;
      Float_t value = TTF->EvalInstance( 0 );
      variable* var = new variable();
      var->float_value = value;
      var->title = name;
      var->name = name;
      var->m_nGets = 0;
      var->type = FORMULA;
      m_varList.push_back( var );
      //delete TTF; TTF=0;
      return value;
    } else {
      std::cerr << "ERROR: TreeReader - non-existing variable or formula with name "<<name<<std::endl;
      std::exit(EXIT_FAILURE);
    } 
  }
  ++(myVar->m_nGets);
  //  std::find_if(m_varList.begin(), m_varList.end(), varEq(name));

  //  std::cout<<"Accessing branch "<<myVar->name<<" of type "<<myVar->type
  //            <<" with double value "<<myVar->double_value<<std::endl;

  switch (myVar->type)
  {
      case FLOAT:
          if ( myVar->arraySize>1 )
              return myVar->float_array[iValue];
          else
              return myVar->float_value;
          break;

      case DOUBLE:
          if ( myVar->arraySize>1 )
              return myVar->double_array[iValue];
          else
              return myVar->double_value;
          break;

      case INT:
          return myVar->int_value;
          break;

      case BOOL:
          return myVar->bool_value;
          break;

      case FORMULA:
          return myVar->evaluateFormula(0);
          break;
      default:
          return -999;
  }


  //  float value=m_fChain->GetBranch(name.c_str())->GetLeaf(name.c_str())->GetValue(iValue);
  //  return value;

  return -999;
}

/*
 * In future I might keep statistics of which variables are accessed often
 * and once in a while swap those to early positions in list to get better
 * than n*ln(n) of find. Ideally if I access only 10 variables, at some
 * point they should occupy first 10 positions while others (order of
 * hundred) ones should be behind. This way find should be more efficient.
 * Have to see whether I can implement and tune idea to actually obtain
 * gain as it will need some statistics collection, checks whether we
 * should still do it and at least some sorting of list.
 *
 * Using chain GetBranch and GetLeaf runs at about twice the speed of my
 * implementation without doing anything special in searching for branch.
 * Should definitely try to be smart.
 */

bool TreeReader::partialSort()
{
    bool didSwap=false;
    ++m_nSwaps;

    // Do not care about first 10 variables
    for (unsigned int i=10;i<m_varList.size();++i)
    {
        if ( 100.0*m_varList[i]->m_nGets/m_nGets >= 1 )  // Care only about cases which are used more often
        {
            for (unsigned int j=0;j<i;++j)  // Makes sense to swap only to earlier place and if variable itself is not used often
            {
                if ( m_varList[i]->m_nGets > 2*m_varList[j]->m_nGets &&
                        100.0*m_varList[j]->m_nGets/m_nGets < 1.0 )
                {
                    variable* tmp=m_varList[i];
                    m_varList[i]=m_varList[j];
                    m_varList[j]=tmp;
                    didSwap=true;
                }      
            }
        }
    }

    return didSwap;
}
