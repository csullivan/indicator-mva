/*
* Copyright (C) 2007, Alberto Giannetti
*
* This file is part of Hudson.
*
* Hudson is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hudson is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Hudson.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _TA_HPP_ 
#define _TA_HPP_ 1

// STL
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

// TA
#include <ta-lib/ta_libc.h>


class TAException: public std::exception
{
public:
  TAException(const std::string& msg):
    _Str("TAException: ")
  {
    _Str += msg;
  }

  virtual ~TAException(void) throw() { }
  virtual const char *what(void) const throw() { return _Str.c_str(); }

protected:
  std::string _Str;
};


class TA {

  public:
    typedef std::vector<double> vDouble;

    enum MAType {  Sma = 0, Ema, Wma, Dema, Tema, Trima, Kama, Mama, T3 };


    TA(void) throw(TAException);
    ~TA(void);

    typedef struct  
    {
      vDouble ma;
      int begIdx;
      int nbElement;
    } SMARes;
    
    typedef struct
    {
      vDouble ema;
      int begIdx;
      int nbElement;
    } EMARes;

    typedef struct  
    {
      vDouble rsi;
      int begIdx;
      int nbElement;
    } RSIRes;

    typedef struct  
    {
      vDouble lslr_m;
      int begIdx;
      int nbElement;
    } LSLR_MRes;

    typedef struct  
    {
      vDouble lslr_c;
      int begIdx;
      int nbElement;
    } LSLR_CRes;

    typedef struct  
    {
      vDouble lslr;
      int begIdx;
      int nbElement;
    } LSLRRes;

    typedef struct  
    {
      vDouble mom;
      int begIdx;
      int nbElement;
    } MOMRes;

    typedef struct  
    {
      vDouble roc;
      int begIdx;
      int nbElement;
    } ROCRes;

    typedef struct  
    {
      vDouble ht;
      int begIdx;
      int nbElement;
    } HTDCPRes;

    typedef struct  
    {
      vDouble htit;
      int begIdx;
      int nbElement;
    } HTITRes;

    typedef struct  
    {
      vDouble mfi;
      int begIdx;
      int nbElement;
    } MFIRes;

    typedef struct 
    {
      vDouble adosc;
      int begIdx;
      int nbElement;
    } ADOSCRes;

    typedef struct 
    {
      vDouble ado;
      int begIdx;
      int nbElement;
    } ADORes;

    typedef struct 
    {
      vDouble apo;
      int begIdx;
      int nbElement;
    } APORes;

    typedef struct 
    {
      vDouble bop;
      int begIdx;
      int nbElement;
    } BOPRes;

    typedef struct
    {
      vDouble correl;
      int begIdx;
      int nbElement;
    } CORRELRes;

    typedef struct 
    {
      vDouble cci;
      int begIdx;
      int nbElement;
    } CCIRes;

    typedef struct 
    {
      vDouble willr;
      int begIdx;
      int nbElement;
    } WILLRRes;

    typedef struct 
    {
      vDouble adx;
      int begIdx;
      int nbElement;
    } ADXRes;

    typedef struct 
    {
      vDouble beta;
      int begIdx;
      int nbElement;
    } BETARes;

    typedef struct 
    {
      vDouble rocr;
      int begIdx;
      int nbElement;
    } ROCRRes;

    typedef struct  
    {
      vDouble rocp;
      int begIdx;
      int nbElement;
    } ROCPRes;

    typedef struct  
    {
      vDouble cmo;
      int begIdx;
      int nbElement;
    } CMORes;

    typedef struct
    {
      vDouble var;
      int begIdx;
      int nbElement;
    } VARRes;

    typedef struct
    {
      vDouble stddev;
      int begIdx;
      int nbElement;
    } STDDEVRes;

    typedef struct
    {
      vDouble factors;
      int begIdx;
      int nbElement;
    } FACTORRes;

    typedef struct
    {
      vDouble upper_band;
      vDouble middle_band;
      vDouble lower_band;
      int begIdx;
      int nbElement;
    } BBRes;

    typedef struct
    {
      vDouble fastK;
      vDouble fastD;
      int begIdx;
      int nbElement;
    } STOCHRSIRes;

    typedef struct 
    {
      vDouble macd;
      vDouble macd_signal;
      vDouble macd_hist;
      int begIdx;
      int nbElement;
    } MACDRes;

    // Indicator function definitions;
    SMARes    SMA(const vDouble& vSeries,const  unsigned& ma_period = 7) const throw(TAException);
    EMARes    EMA(const vDouble& vSeries,const  unsigned& ema_period = 7) const throw(TAException);
    BETARes   BETA(const vDouble& vStock, const vDouble& vBenchmark, const unsigned& period = 7) const throw(TAException);
    CORRELRes    CORREL(const vDouble& vSeries1, const vDouble& vSeries2, const unsigned& period = 7) const throw(TAException);
    RSIRes    RSI(const vDouble& vSeries,const  unsigned& rsi_period = 7) const throw(TAException);
    MACDRes   MACD(const vDouble& vSeries,const  unsigned& macd_fast_period = 12, const  unsigned& macd_slow_period = 26, const unsigned& macd_signal_period = 9) throw(TAException);
    LSLR_MRes LSLR_M(const vDouble& vSeries, const unsigned& lslr_m_period = 7) const throw(TAException);
    LSLR_CRes LSLR_C(const vDouble& vSeries, const unsigned& lslr_c_period = 7) const throw(TAException);
    LSLRRes   LSLR(const vDouble& vSeries, const unsigned& lslr_period = 7) const throw(TAException);
    MOMRes    MOM(const vDouble& vSeries, const unsigned& mom_period = 7) const throw(TAException);
    ROCRes    ROC(const vDouble& vSeries, const unsigned& roc_period = 7) const throw(TAException);
    ROCRRes   ROCR(const vDouble& vSeries, const unsigned& roc_period = 7) const throw(TAException);
    ROCPRes   ROCP(const vDouble& vSeries, const unsigned& rocp_period = 7) const throw(TAException);
    VARRes    VAR(const vDouble& vSeries, const unsigned& period = 7, const double& sd2=9) const throw(TAException);
    STDDEVRes STDDEV(const vDouble& vSeries, const unsigned& stddev_period = 7, const double& sd=3) const throw(TAException);
    BBRes     BBANDS(const vDouble& vSeries, const unsigned& ma_period = 14, const double& sd_up = 3, const double& sd_down = 3) const throw(TAException);
    FACTORRes FACTORS(const vDouble& vSeries, const unsigned& period = 7) const throw(TAException);
    ADOSCRes  ADOSC(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const vDouble& vVolume, const unsigned& fast_period = 3, const unsigned& slow_period = 10 ) const throw(TAException);
    ADORes    ADO(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const vDouble& vVolume, const unsigned& fast_period = 3, const unsigned& slow_period = 10 ) const throw(TAException);
    WILLRRes  WILLR(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const unsigned& willr_period = 7 ) const throw(TAException);
    ADXRes    ADX(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const unsigned& adx_period = 7 ) const throw(TAException);
    APORes    APO(const vDouble& vSeries, const unsigned& fast_period = 4, const unsigned& slow_period = 10 ) const throw(TAException);
    BOPRes    BOP(const vDouble& vOpen, const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose ) const throw(TAException) ;
    CCIRes    CCI(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const unsigned& cci_period = 7 ) const throw(TAException) ;
    CMORes    CMO(const vDouble& vSeries, const unsigned& cmo_period = 7) const throw(TAException);
    HTDCPRes  HTDCP(const vDouble& vSeries) const throw(TAException);
    HTITRes   HTIT( const vDouble& vSeries ) const throw(TAException);
    MFIRes    MFI(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const vDouble& vVolume, const unsigned& mfi_period = 7 ) const throw(TAException) ;
    STOCHRSIRes STOCHRSI( const vDouble& vSeries, const unsigned& stochrsi_period = 7, const unsigned& stochrsi_fastK_period = 5, const unsigned& stochrsi_fastD_period = 3) throw(TAException) ;

  protected:
    std::string getError(TA_RetCode code) const;
};

#endif // _TA_HPP_
