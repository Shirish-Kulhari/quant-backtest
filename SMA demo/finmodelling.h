#include "dataframe.h"

Matrix sma(Matrix &M, unsigned int period, std::string header)
{
    //  Generate SMA of a price time series

    addColumn(M, header, "", -1);
    unsigned int max_cols = maxCols(M);
    for( unsigned int k = period ; k < M.size() ; k++ )
    {
        Decimal sum = 0;
        for( unsigned int c = k+1-period ; c <= k ; c++ )
        {
            sum = sum + toDec( M[c][4] );
        }
        sum = sum/period;
        M[k][max_cols - 1] = sum;
    }

    return M;
}
