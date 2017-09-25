#include "dataframe.h"


Column sma(const Column column, const unsigned int period, const unsigned int pricecol, const std::string header)
{
    //  Generate SMA of a price time series
    Column x = column;
    x[0] = header;
    for( unsigned int i = 1 ; i < x.size() ; i++ )
        x[i] = "";

    for( unsigned int k = period ; k < column.size() ; k++ )
    {
        Decimal sum = 0;
        for( unsigned int c = k+1-period ; c <= k ; c++ )
        {
            sum = sum + toDec( column[c] );
        }
        sum = sum/period;
        x[k] = sum;
    }

    return x;
}


Column returns(const Column column, const std::string header)
{
    Column x = column;
    x[0] = header;
    for( unsigned int i = 1 ; i < x.size() ; i++ )
        x[i] = "";

    for( unsigned int k = 2 ; k < column.size() ; k++ )
        x[k] = toDec( column[k] ) / toDec( column[k-1] ) - 1;

    return x;
}
