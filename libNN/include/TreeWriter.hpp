#ifndef TreeWriter_HPP
#define TreeWriter_HPP 1

// STL include
#include <iosfwd>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <iterator>     // std::advance

// boost include
#include "boost/any.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

// Hudson include
//#include "TA.hpp"
//#include <EODSeries.hpp>

// ROOT include
#include "TFile.h"
#include "TTree.h"
#include "Rtypes.h"

/*******************************************************************
*
*  Matthew M Reid 27-07-2013: TreeWriter class can be used as
*  an interface to TTree class and conveniently writes floating
*  point data to a root TFile. 
*
*******************************************************************/

namespace NN {
  
  class TreeWriterException: public std::exception
  {
    public:
      TreeWriterException(const std::string& msg):
        _Str("TreeWriterException: ") {
        _Str += msg;
      }
    
      virtual ~TreeWriterException(void) throw() { }
      virtual const char *what() const throw() { return _Str.c_str(); }

    private:
      std::string _Str;
  };

  class TreeWriter {
    
    public:
      TreeWriter();
      TreeWriter( const std::string& filename, TTree* tree );
      TreeWriter( const std::string& filename, const std::string& treename );
      virtual ~TreeWriter();

      // OK to store everything as floats since that is what TMVA will do anyway.
      void add( const std::string& variable );
      
      void column( const std::string& variable, const boost::any& value ) throw( TreeWriterException ); 
      void column( const std::string& variable, const Float_t& value ) throw( TreeWriterException ); 
      void write( ); 
      void write( const unsigned int& i ); 
      
    private: 

    protected:
      TFile* m_file;
      TTree* m_tree;
      TTree* m_temp;
      std::map<std::string, Float_t > m_variables;
      //const Series::EODSeries& m_series; 
  };


}

#endif // TreeWriter_HPP
