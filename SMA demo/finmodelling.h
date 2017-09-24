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


Column eodBalance(const Column returns_column, Decimal book_size, const std::string header)
{
    Column x = returns_column;
    x[0] = header;
    x[1] = book_size;
    for( unsigned int i = 2 ; i < x.size() ; i++ )
        x[i] = "";
    VarField blank = "";

    for( unsigned int k = 2 ; k < returns_column.size() ; k++ )
    {
        try
        {
            x[k] = toDec( x[k-1] ) * (1 + toDec( returns_column[k] ));
        }
        catch(...)
        {
            x[k] = x[k-1];
        }
    }

    return x;
}
