#ifndef IClassifierReader_HPP
#define IClassifierReader_HPP 1

// include stl headers
#include <vector>

namespace NN {

  class IClassifierReader {

    public:
      IClassifierReader() : m_fStatusIsClean( true ) { }
      virtual ~IClassifierReader() { }
      virtual double GetMvaValue( const unsigned int& i ) const = 0;
      bool IsStatusClean() const { return m_fStatusIsClean; }
  
    protected:
      bool m_fStatusIsClean; 
  };

}

#endif // IClassifierReader_HPP


