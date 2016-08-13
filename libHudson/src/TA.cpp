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

// STL
#include <vector>

// Hudson
#include "TA.hpp"

using namespace std;


//**************************************************************************************************************************
TA::TA(void) throw(TAException)
{
  TA_RetCode ta_ret;

  if( (ta_ret = TA_Initialize()) != TA_SUCCESS )
    throw TAException(getError(ta_ret));
}


//**************************************************************************************************************************
TA::~TA( void )
{
  TA_Shutdown();
}


//**************************************************************************************************************************
TA::SMARes TA::SMA(const vDouble& vSeries, const unsigned& ma_period) const throw(TAException)
{
  if( vSeries.size() < ma_period )
    throw TAException("SMA: Not enough data");

  SMARes res;
  res.ma.resize(vSeries.size());

  TA_RetCode ta_ret = TA_MA(0, vSeries.size()-1, &vSeries[0], ma_period, TA_MAType_SMA, &res.begIdx, &res.nbElement, &res.ma[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("SMA: No output elements");

  return res;
}


//**************************************************************************************************************************
TA::CORRELRes TA::CORREL(const vDouble& vSeries1, const vDouble& vSeries2, const unsigned& period) const throw(TAException)
{

  if( vSeries1.size() < period )
    throw TAException("CORREL: dataset 1 contains too few data points");
  if( vSeries2.size() < period )
    throw TAException("CORREL: dataset 2 contains too few data points");
  if( vSeries1.size() != vSeries2.size() )
    throw TAException("CORREL: dimensions not the same"); 

  CORRELRes res;
  res.correl.resize( vSeries1.size() );

  TA_RetCode ta_ret = TA_CORREL(0, vSeries1.size()-1, &vSeries1[0], &vSeries2[0], period, &res.begIdx, &res.nbElement, &res.correl[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("CORREL: No output elements");

  return res;
}


//**************************************************************************************************************************
TA::BETARes TA::BETA(const vDouble& vStock, const vDouble& vBenchmark, const unsigned& period) const throw(TAException)
{
  // measure of a stocks volatility vs from index. r - Rf = beta.(Km - Rf) + alpha, it is slope of the regression analysis on the 
  // provide equation looking at the difference between price points stock/portfolio returns, vs the benchmark.

  if( vStock.size() < period )
    throw TAException("BETA: stock price contains too few data points");
  if( vBenchmark.size() < period )
    throw TAException("BETA: benchmark contains too few data points");
  if( vBenchmark.size() != vStock.size() )
    throw TAException("BETA: dimensions not the same"); 

  BETARes res;
  res.beta.resize( vStock.size() );

  TA_RetCode ta_ret = TA_BETA(0, vStock.size()-1, &vStock[0], &vBenchmark[0], period, &res.begIdx, &res.nbElement, &res.beta[0] );
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("BETA: No output elements");

  return res;
}


//**************************************************************************************************************************
TA::EMARes TA::EMA(const vDouble& vSeries, const unsigned& ema_period ) const throw(TAException)
{
  if( vSeries.size() < ema_period )
    throw TAException("EMA: Not enough data");

  EMARes res;
  res.ema.resize(vSeries.size());

  TA_RetCode ta_ret = TA_MA(0, vSeries.size()-1, &vSeries[0], ema_period, TA_MAType_EMA, &res.begIdx, &res.nbElement, &res.ema[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("EMA: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::RSIRes TA::RSI(const vDouble& vSeries, const unsigned& rsi_period ) const throw(TAException)
{
  if( vSeries.size() < rsi_period )
    throw TAException("RSI: Not enough data");

  RSIRes res;
  res.rsi.resize(vSeries.size());

  TA_RetCode ta_ret = TA_RSI(0, vSeries.size()-1, &vSeries[0], rsi_period, &res.begIdx, &res.nbElement, &res.rsi[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("RSI: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::LSLR_MRes TA::LSLR_M(const vDouble& vSeries, const unsigned& lslr_m_period) const throw(TAException)
{
  // Least Squares Linear Regression (LSLR) returns slope 'm' from the minimisation y =  m*x + c
  if( vSeries.size() < lslr_m_period )
    throw TAException("LSLR_M: Not enough data");

  LSLR_MRes res;
  res.lslr_m.resize(vSeries.size());

  TA_RetCode ta_ret = TA_LINEARREG_SLOPE(0, vSeries.size()-1, &vSeries[0], lslr_m_period, &res.begIdx, &res.nbElement, &res.lslr_m[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("LSLR_M: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::LSLR_CRes TA::LSLR_C(const vDouble& vSeries, const unsigned& lslr_c_period) const throw(TAException)
{
  // Least Squares Linear Regression (LSLR) returns intercept 'c' from the minimisation y =  m*x + c
  if( vSeries.size() < lslr_c_period )
    throw TAException("LSLR_C: Not enough data");

  LSLR_CRes res;
  res.lslr_c.resize(vSeries.size());

  TA_RetCode ta_ret = TA_LINEARREG_INTERCEPT(0, vSeries.size()-1, &vSeries[0], lslr_c_period, &res.begIdx, &res.nbElement, &res.lslr_c[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("LSLR_C: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::LSLRRes TA::LSLR(const vDouble& vSeries, const unsigned& lslr_period) const throw(TAException)
{
  // Least Squares Linear Regression (LSLR) returns y =  m*x + c
  if( vSeries.size() < lslr_period )
    throw TAException("LSLR: Not enough data");

  LSLRRes res;
  res.lslr.resize(vSeries.size());

  TA_RetCode ta_ret = TA_LINEARREG(0, vSeries.size()-1, &vSeries[0], lslr_period, &res.begIdx, &res.nbElement, &res.lslr[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("LSLR: No out elements");

  return res;
}

//**************************************************************************************************************************
TA::ADOSCRes TA::ADOSC(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const vDouble& vVolume, const unsigned& fast_period, const unsigned& slow_period ) const throw(TAException)
{
  // ADOSC Measures the momentum of the Accumulation Distribution Line using the MACD formula. 
  // This makes it an indicator of an indicator. The Chaikin Oscillator is the difference between
  // the 3-day EMA of the Accumulation Distribution Line and the 10-day EMA of the Accumulation 
  // Distribution Line. Like other momentum indicators, this indicator is designed to anticipate 
  // directional changes in the Accumulation Distribution Line by measuring the momentum behind 
  // the movements. A momentum change is the first step to a trend change.

  if( !( ( ( vHigh.size() == vLow.size() ) == ( vClose.size()==vVolume.size() ) ) 
      && ( ( vHigh.size() == vClose.size() ) == ( vLow.size() == vVolume.size() ) ) ) ) {
    throw TAException("ADOSC: dimensions not the same");    
  }
  if( fast_period < 2 || slow_period < 2 ) {
    throw TAException("ADOSC: period range not reasonable");
  }  
  if( vClose.size() < fast_period || vClose.size() < slow_period ) {
    throw TAException("ADOSC: Not enough data");
  }

  ADOSCRes res;
  res.adosc.resize( vClose.size() );
  
  TA_RetCode ta_ret = TA_ADOSC( 0, vHigh.size() - 1, &vHigh[0], &vLow[0], &vClose[0], &vVolume[0], fast_period, slow_period, &res.begIdx, &res.nbElement, &res.adosc[0] );
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("ADOSC: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::MOMRes TA::MOM(const vDouble& vSeries, const unsigned& mom_period) const throw(TAException)
{
  if( vSeries.size() < mom_period )
    throw TAException("MOM: Not enough data");

  MOMRes res;
  res.mom.resize(vSeries.size());

  TA_RetCode ta_ret = TA_MOM(0, vSeries.size()-1, &vSeries[0], mom_period, &res.begIdx, &res.nbElement, &res.mom[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("MOM: No out elements");

  return res;
}

//**************************************************************************************************************************
TA::ROCRes TA::ROC(const vDouble& vSeries, const unsigned& roc_period) const throw(TAException)
{
  if( vSeries.size() < roc_period )
    throw TAException("ROC: Not enough data");

  ROCRes res;
  res.roc.resize(vSeries.size());

  TA_RetCode ta_ret = TA_ROC(0, vSeries.size()-1, &vSeries[0], roc_period, &res.begIdx, &res.nbElement, &res.roc[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("ROC: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::CMORes TA::CMO(const vDouble& vSeries, const unsigned& cmo_period) const throw(TAException)
{
  if( vSeries.size() < cmo_period )
    throw TAException("CMO: Not enough data");

  CMORes res;
  res.cmo.resize(vSeries.size());

  TA_RetCode ta_ret = TA_CMO(0, vSeries.size()-1, &vSeries[0], cmo_period, &res.begIdx, &res.nbElement, &res.cmo[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("CMO: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::ADORes TA::ADO(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const vDouble& vVolume, const unsigned& fast_period, const unsigned& slow_period ) const throw(TAException)
{
  if( !( ( ( vHigh.size() == vLow.size() ) == ( vClose.size()==vVolume.size() ) ) 
      && ( ( vHigh.size() == vClose.size() ) == ( vLow.size() == vVolume.size() ) ) ) ) {
    throw TAException("ADO: dimensions not the same");    
  }
  if( fast_period < 2 || slow_period < 2 ) {
    throw TAException("ADO: period range not reasonable");
  }  
  if( vClose.size() < fast_period || vClose.size() < slow_period ) {
    throw TAException("ADO: Not enough data");
  }

  ADORes res;
  res.ado.resize( vClose.size() );
  
  TA_RetCode ta_ret = TA_ADOSC( 0, vHigh.size() - 1, &vHigh[0], &vLow[0], &vClose[0], &vVolume[0], fast_period, slow_period, &res.begIdx, &res.nbElement, &res.ado[0] );
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("ADO: No out elements");

  return res;

}


//**************************************************************************************************************************
TA::APORes TA::APO(const vDouble& vSeries, const unsigned& fast_period, const unsigned& slow_period  ) const throw(TAException) {

  if( fast_period < 2 || slow_period < 2 ) {
    throw TAException("APO: period range not reasonable");
  } 
  if( vSeries.size() < slow_period || vSeries.size() < fast_period ) {
    throw TAException("APO: Not enough data");
  }

  APORes res;
  res.apo.resize( vSeries.size() );
  
  TA_RetCode ta_ret = TA_APO( 0, vSeries.size() - 1, &vSeries[0], fast_period, slow_period, TA_MAType_EMA, &res.begIdx, &res.nbElement, &res.apo[0] );
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("APO: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::MFIRes TA::MFI(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const vDouble& vVolume, const unsigned& mfi_period ) const throw(TAException) {

  if( !( ( ( vHigh.size() == vLow.size() ) == ( vClose.size()==vVolume.size() ) ) 
      && ( ( vHigh.size() == vClose.size() ) == ( vLow.size() == vVolume.size() ) ) ) ) {
    throw TAException("MFI: dimensions not the same");    
  }
  if( vHigh.size() < mfi_period )
    throw TAException("MFI: Not enough data");
  

  MFIRes res;
  res.mfi.resize( vHigh.size() );
  
  TA_RetCode ta_ret = TA_MFI( 0, vHigh.size() - 1, &vHigh[0], &vLow[0], &vClose[0], &vVolume[0], mfi_period, &res.begIdx, &res.nbElement, &res.mfi[0] );


  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("MFI: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::STOCHRSIRes TA::STOCHRSI( const vDouble& vSeries, const unsigned& stochrsi_period, const unsigned& stochrsi_fastK_period, const unsigned& stochrsi_fastD_period ) throw(TAException)
{
  if( vSeries.size() < stochrsi_period ||
      vSeries.size() < stochrsi_fastD_period ||
      vSeries.size() < stochrsi_fastK_period )
    throw TAException("STOCHRSI: Not enough data");

  STOCHRSIRes res;
  res.fastK.resize(vSeries.size());
  res.fastD.resize(vSeries.size());

  TA_RetCode ta_ret = TA_STOCHRSI(0, vSeries.size()-1, &vSeries[0], stochrsi_period, stochrsi_fastK_period, stochrsi_fastD_period, 
                              TA_MAType_SMA, &res.begIdx, &res.nbElement, &res.fastK[0], &res.fastD[0] );

  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("STOCHRSI: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::BOPRes TA::BOP(const vDouble& vOpen, const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose ) const throw(TAException) {

  if( !( ( ( vHigh.size() == vLow.size() ) == (vClose.size()==vOpen.size() ) ) 
      && ( ( vHigh.size() == vClose.size() ) == ( vLow.size() == vOpen.size() ) ) ) ) {
    throw TAException("BOP: dimensions not the same");    
  }

  BOPRes res;
  res.bop.resize( vOpen.size() );
  
  TA_RetCode ta_ret = TA_BOP( 0, vOpen.size() - 1, &vOpen[0], &vHigh[0], &vLow[0], &vClose[0], &res.begIdx, &res.nbElement, &res.bop[0] );
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("BOP: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::CCIRes TA::CCI(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const unsigned& cci_period ) const throw(TAException){

  if( !( ( vHigh.size() == vLow.size() ) && ( vHigh.size() == vClose.size() ) ) ) {
    throw TAException("CCI: dimensions not the same");    
  }
  if( cci_period < 2 ) {
    throw TAException("CCI: period range not reasonable");
  } 
  if( vClose.size() < cci_period ) {
    throw TAException("CCI: Not enough data");
  }

  CCIRes res;
  res.cci.resize( vClose.size() );
  
  TA_RetCode ta_ret = TA_CCI( 0, vHigh.size() - 1, &vHigh[0], &vLow[0], &vClose[0], cci_period, &res.begIdx, &res.nbElement, &res.cci[0] );
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("CCI: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::WILLRRes TA::WILLR(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const unsigned& willr_period ) const throw(TAException){

  // WILLR is a momentum indicator that is the inverse of the Fast Stochastic Oscillator. 
  // Also referred to as %R, Williams %R. It reflects the level of the close relative to the 
  // highest high for the look-back period.

  if( !( ( vHigh.size() == vLow.size() ) && ( vHigh.size() == vClose.size() ) ) ) {
    throw TAException("WILLR: dimensions not the same");    
  }
  if( willr_period < 2 ) {
    throw TAException("WILLR: period range not reasonable");
  } 
  if( vClose.size() < willr_period ) {
    throw TAException("WILLR: Not enough data");
  }

  WILLRRes res;
  res.willr.resize( vClose.size() );
  
  TA_RetCode ta_ret = TA_WILLR( 0, vHigh.size() - 1, &vHigh[0], &vLow[0], &vClose[0], willr_period, &res.begIdx, &res.nbElement, &res.willr[0] );
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("WILLR: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::ADXRes TA::ADX(const vDouble& vHigh, const vDouble& vLow, const vDouble& vClose, const unsigned& adx_period ) const throw(TAException){

  if( !( ( vHigh.size() == vLow.size() ) && ( vHigh.size() == vClose.size() ) ) ) {
    throw TAException("ADX: dimensions not the same");    
  }
  if( adx_period < 2 ) {
    throw TAException("ADX: period range not reasonable");
  } 
  if( vClose.size() < adx_period ) {
    throw TAException("ADX: Not enough data");
  }

  ADXRes res;
  res.adx.resize( vClose.size() );
  
  TA_RetCode ta_ret = TA_ADX( 0, vHigh.size() - 1, &vHigh[0], &vLow[0], &vClose[0], adx_period, &res.begIdx, &res.nbElement, &res.adx[0] );
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("ADX: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::ROCRRes TA::ROCR(const vDouble& vSeries, const unsigned& rocr_period ) const throw(TAException)
{
  if( vSeries.size() < rocr_period )
    throw TAException("Not enough data");

  ROCRRes res;
  res.rocr.resize(vSeries.size());

  TA_RetCode ta_ret = TA_ROCR(0, vSeries.size()-1, &vSeries[0], rocr_period, &res.begIdx, &res.nbElement, &res.rocr[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("ROCR: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::ROCPRes TA::ROCP(const vDouble& vSeries, const unsigned& rocp_period ) const throw(TAException)
{
  if( vSeries.size() < rocp_period )
    throw TAException("ROCP: Not enough data");

  ROCPRes res;
  res.rocp.resize(vSeries.size());

  TA_RetCode ta_ret = TA_ROCP(0, vSeries.size()-1, &vSeries[0], rocp_period, &res.begIdx, &res.nbElement, &res.rocp[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("ROCP: No out elements");

  return res;
}

//**************************************************************************************************************************
TA::VARRes TA::VAR(const vDouble& vSeries, const unsigned& period, const double& sd2 ) const throw(TAException)
{
  // Variance of the input series. Probably more efficienct to use this over the STDDEV function since no sqrt calculation required (but not a worry unless super optimisation is an issue.)
  if( vSeries.size() < period )
    throw TAException("VAR: Not enough data");

  VARRes res;
  res.var.resize( vSeries.size() );

  TA_RetCode ta_ret = TA_VAR(0, vSeries.size()-1, &vSeries[0], period, sd2, &res.begIdx, &res.nbElement, &res.var[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("VAR: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::STDDEVRes TA::STDDEV(const vDouble& vSeries, const unsigned& stddev_period, const double& sd ) const throw(TAException)
{
  if( vSeries.size() < stddev_period )
    throw TAException("STDDEV: Not enough data");

  STDDEVRes res;
  res.stddev.resize(vSeries.size());

  TA_RetCode ta_ret = TA_STDDEV(0, vSeries.size()-1, &vSeries[0], stddev_period, sd, &res.begIdx, &res.nbElement, &res.stddev[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("STDDEV: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::BBRes TA::BBANDS( const vDouble& vSeries, const unsigned& ma_period, const double& sd_up, const double& sd_down ) const throw(TAException)
{
  if( vSeries.size() < ma_period )
    throw TAException("BBANDS: Not enough data");

  BBRes res;
  res.lower_band.resize(vSeries.size());
  res.middle_band.resize(vSeries.size());
  res.upper_band.resize(vSeries.size());

  TA_RetCode ta_ret = TA_BBANDS(0, vSeries.size()-1, &vSeries[0], ma_period, sd_up, sd_down, TA_MAType_SMA,
                                &res.begIdx, &res.nbElement, &res.upper_band[0], &res.middle_band[0], &res.lower_band[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("BBANDS: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::MACDRes TA::MACD( const vDouble& vSeries, const unsigned& macd_fast_period, const unsigned& macd_slow_period, const unsigned& macd_signal_period ) throw(TAException)
{
  if( vSeries.size() < macd_slow_period ||
      vSeries.size() < macd_fast_period ||
      vSeries.size() < macd_signal_period )
    throw TAException("MACD: Not enough data");

  MACDRes res;
  res.macd.resize(vSeries.size());
  res.macd_hist.resize(vSeries.size());
  res.macd_signal.resize(vSeries.size());

  TA_RetCode ta_ret = TA_MACD(0, vSeries.size()-1, &vSeries[0], macd_fast_period, macd_slow_period, macd_signal_period,
                              &res.begIdx, &res.nbElement, &res.macd[0], &res.macd_signal[0], &res.macd_hist[0]);

  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("MACD: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::HTDCPRes TA::HTDCP( const vDouble& vSeries ) const throw(TAException)
{
  if( vSeries.empty() )
    throw TAException("HTDCP: Not enough data");

  HTDCPRes res;
  res.ht.resize(vSeries.size());

  TA_RetCode ta_ret = TA_HT_DCPHASE(0, vSeries.size()-1, &vSeries[0], &res.begIdx, &res.nbElement, &res.ht[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("HTDCP: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::HTITRes TA::HTIT( const vDouble& vSeries ) const throw(TAException)
{
  if( vSeries.empty() )
    throw TAException("HTIT: Not enough data");

  HTITRes res;
  res.htit.resize(vSeries.size());

  TA_RetCode ta_ret = TA_HT_TRENDLINE(0, vSeries.size()-1, &vSeries[0], &res.begIdx, &res.nbElement, &res.htit[0]);
  if( ta_ret != TA_SUCCESS )
    throw TAException(getError(ta_ret));

  if( res.nbElement <= 0 )
    throw TAException("HTIT: No out elements");

  return res;
}


//**************************************************************************************************************************
TA::FACTORRes TA::FACTORS(const vDouble& vSeries, const unsigned& period ) const throw(TAException)
{
  if( (period + 1) > vSeries.size() )
    throw TAException("FACTORS: Not enough data");

  FACTORRes res;
  res.factors.resize(vSeries.size());

  double last_element = vSeries[0];
  for( unsigned i = period; i < vSeries.size(); i += period ) {
    res.factors.push_back(vSeries[i] / last_element);
    last_element = vSeries[i];
  }

  return res;
}


//**************************************************************************************************************************
string TA::getError(TA_RetCode code) const
{
  TA_RetCodeInfo ta_retinfo;
  TA_SetRetCodeInfo(code, &ta_retinfo);
  return string(ta_retinfo.infoStr);
}

