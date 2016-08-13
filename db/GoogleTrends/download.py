#!/usr/bin/python

from pyGTrends import pyGTrends
import csv, datetime, time, getpass, sys
import re

google_username = raw_input("Google username: ")
google_password = getpass.getpass("Google password: ")

def read_csv_data( data ):
    """
        Reads CSV from given path and Return list of dict with Mapping
    """
    csv_reader = csv.reader( data )
    # Read the column names from the first line of the file
    fields = csv_reader.next()
    data_lines = []
    for row in csv_reader:
        items = dict(zip(fields, row))
        data_lines.append(items)
    return data_lines

def progressbar(it, prefix = "", size = 60):
    count = len(it)
    def _show(_i):
        x = int(size*_i/count)
        sys.stdout.write("%s[%s%s] %i/%i\r" % (prefix, "#"*x, "."*(size-x), _i, count))
        sys.stdout.flush()
    
    _show(0)
    for i, item in enumerate(it):
        yield item
        _show(i+1)
    sys.stdout.write("\n")
    sys.stdout.flush()


def getGTData( search_query = "debt", date="all", geo="all", scale="1", position = "end" ) :
    
    connector = pyGTrends( google_username, google_password )
    connector.download_report( ( search_query ) 
			   , date = date
                           , geo = geo
                           , scale = scale )

    #connector.writer()

    #print connector.getData()
    #d = DictReader( connector.getData().split('\n'))
    #print  csv.DictReader( connector.csv().split('\n'))
    #print  connector.csv( section='Main', as_list=True )

    data = connector.csv( section='Main' ).split('\n')
    csv_reader = csv.reader( data )
    # example using City to cache search terms
    #csv_reader = csv.reader( connector.csv( section='City' ).split('\n') )
 
    #for row in csv_reader:
    #    print row

    #data1 = read_csv_data( data )
    #print data1

    csv_reader = csv.reader( data )
    #fields = csv_reader.next() # Read the column names

    # remove all whitespaces
    search_query = search_query.strip() 
    search_query = " ".join( search_query.split() )
    search_query = search_query.replace(" ", "") 
        
    with open( search_query + '_google_report.csv', 'w') as csv_out:
        positionInWeek = { "start" : 0, "end" : 1 }
        separator = " - "
        csv_writer = csv.writer( csv_out )
        #print "LOOPING ON ENTRIES"
        for count, row in enumerate( csv_reader ):
	    if separator not in row[0] : 
                csv_writer.writerow( row )
                continue

            date = row[0].split( separator )[ positionInWeek[ position ] ] 

	    # we want to remove any whitespaces from the value entry since we are not interested in blank data points
            val = re.sub(r'\s+', '', row[1] )
            if len(val) < 1 :
                #print "We will skip Null value at date %s" % str(date)
                continue
            #row = str(date)+','+str(val)
            if count == 0:
                csv_writer.writerow( row )
            else:
                csv_writer.writerow( [ str(date) ] + [ str(val) ])
    print "File saved: %s " % ( search_query + '_google_report.csv' )

def getGoogleTrendData( search_queries = ["debt"], date="all", geo="all", scale="1" ) :

    for search_term in progressbar( search_queries, "Downloading: ", 40 ):
        getGTData(search_query = search_term, date = date, geo = geo, scale = scale )
	time.sleep(2)  # Delay for x seconds    
    return True


if __name__=="__main__":

    list_of_queries = ["hedge fund", "dividend", "earnings", "ftse", "inflation", "markets",
                       "bonds", "debt", "financial markets", "gains", "investment", "growth",
		       "derivatives", "crisis", "unemployment", "banking", "SPX", "economy",
                       "stocks", "economics", "money", "invest", "bubble", "terrorist", "crude oil",
	               "wheat", "gold", "war", "holiday", "inflation", "tourism", "happiness", "depression" ]

    # Remove duplicate entries in the list if there are any...
    list_of_queries = list( set( list_of_queries ) )
    if getGoogleTrendData( search_queries = list_of_queries, date="all", geo="US", scale="1" ) :
        print "Google Trend Data aquired."
        
