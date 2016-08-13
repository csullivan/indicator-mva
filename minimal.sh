#!/bin/bash
#variables="ADX MACD MACD_hist MACD_signal"
variables="BOP APO STDDEV CCI ADO ADX MACD_signal"
addmvas=BDTD
#addmvas="LD TMlpANN BDTD MLP MLPBFGS MLPBNN"
backtestmva=BDTD


#./bin/maketuple --spx_file ./db/SPX.csv --begin_date "2001-01-01" --end_date "2010-01-01" --day_shift 7

#./bin/mva --input_file outputFile_SPX.root --tree_path data --output_file outputFile_SPX_withMVA.root -e -t  --var $variables --mva $addmvas

./bin/mvabacktest --spx_file ./db/SPX.csv --begin_date "2010-01-01" --end_date "2014-12-01" --mva_type $backtestmva --cut_min -0.01 --cut_max 0.1 --var $variables
