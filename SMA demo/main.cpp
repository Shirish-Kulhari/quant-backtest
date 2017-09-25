/**********************************************************************************\
|   A simple SMA implementation. Lets the user specify time periods for short     |
|   and medium-term moving averages. The strategy goes long when the short SMA     |
|   crosses above the medium SMA, and goes short when short SMA crosses below      |
|   the medium SMA.                                                                |
\**********************************************************************************/

#include <iostream>
#include "finmodelling.h"

unsigned int short_term = 10, medium_term = 26;
const Decimal BOOK_SIZE = 1000000;

void generateSignal(Matrix &M);
void generateReinvPNL(Matrix &M, const Decimal book_size, const unsigned int bal_col, const std::string header);
void generateFixedInvPNL(Matrix &M, const Decimal book_size, const unsigned int bal_col, const std::string header);
Column eodReinvBalance(const Column returns_column, Decimal book_size, const std::string header);
Column eodFixedInvBalance(const Column returns_column, const Column signal_column, Decimal book_size, const std::string header);


int main(void)
{
    Matrix data = readCSV("input.csv", ',', {1});
    {
        std::cout << "Do you want to specify custom SMA periods? (y / any other key)  ";
        char choice = std::cin.get();
        if(choice == 'y' || choice == 'Y')
        {
            std::cout << "\nEnter the duration for short-term SMA : ";
            std::cin >> short_term;
            std::cout << "\nEnter the duration for medium-term SMA : ";
            std::cin >> medium_term;
        }
    }
    Column price_column = getColumn(data, 5);
    addColumn( data, sma(price_column, short_term, 4, "SHORT_SMA"), -1 );
    addColumn( data, sma(price_column, medium_term, 4, "MEDIUM_SMA"), -1 );

    generateSignal(data);

    addColumn( data, returns(price_column, "BM_RET"), -1 );

    for( unsigned int k = 1 ; k < medium_term+2 ; k++ )
        data[k][8] = "";


    addColumn( data, eodReinvBalance( getColumn(data, 9), BOOK_SIZE, "BM_EOD_BAL" ), -1);
    generateReinvPNL(data, BOOK_SIZE, 10, "BM_PNL");

    {
        addColumn( data, newColumn(data, "RETURNS"), -1 );
        for( unsigned int k2 = medium_term+2 ; k2 < data.size() ; k2++ )
            data[k2][11] = toDec( data[k2][7] ) * toDec( data[k2][8] );
    }

    addColumn( data, eodReinvBalance( getColumn(data, 12), BOOK_SIZE, "EOD_BAL" ), -1);
    generateReinvPNL(data, BOOK_SIZE, 13, "PNL");

    addColumn( data, eodFixedInvBalance( getColumn(data, 9), getColumn(data, 8), BOOK_SIZE, "BM_EOD_BAL" ), -1);
    generateFixedInvPNL(data, BOOK_SIZE, 15, "BM_PNL");

    addColumn( data, eodFixedInvBalance( getColumn(data, 12), getColumn(data, 8), BOOK_SIZE, "BM_EOD_BAL" ), -1);
    generateFixedInvPNL(data, BOOK_SIZE, 17, "PNL");

    writeCSV("output.csv", data, ',', 6);
}


void generateSignal(Matrix &M)
{
    addColumn( M, newColumn(M, "SIGNAL"), -1 );

    for( unsigned int k = medium_term+2 ; k < M.size() ; k++ )
    {
        if( M[k-2][5] < M[k-2][6] && M[k-1][5] > M[k-1][6] )
            M[k][7] = 1;
        else if( M[k-2][5] > M[k-2][6] && M[k-1][5] < M[k-1][6] )
            M[k][7] = -1;
        else
            M[k][7] = M[k-1][7];
    }
}

void generateReinvPNL(Matrix &M, const Decimal book_size, const unsigned int bal_col, const std::string header)
{
    addColumn( M, newColumn(M, ""), -1 );
    addColumn( M, newColumn(M, header), -1 );

    unsigned int max_cols = maxCols(M);

    M[medium_term+1][max_cols - 2] = book_size;

    for( unsigned int k = medium_term+2 ; k < M.size()-1 ; k++ )
    {
        if( toDec( M[k][7] ) * toDec( M[k+1][7] ) == -1 )
            M[k][max_cols - 2] = M[k][bal_col - 1];
        else
            M[k][max_cols - 2] = M[k-1][max_cols - 2];

        M[k][max_cols - 1] = toDec( M[k][max_cols - 2] ) - toDec( M[k-1][max_cols - 2] );
    }

    deleteColumn(M, max_cols - 2);
}


void generateFixedInvPNL(Matrix &M, const Decimal book_size, const unsigned int bal_col, const std::string header)
{
    addColumn( M, newColumn(M, header), -1 );

    unsigned int max_cols = maxCols(M);

    for( unsigned int k = medium_term+2 ; k < M.size()-1 ; k++ )
    {
        if( toDec( M[k][7] ) != toDec( M[k+1][7] ) )
            M[k][max_cols - 1] = toDec( M[k][bal_col - 1] ) - book_size;
        else
            M[k][max_cols - 1] = 0;
    }
}


Column eodReinvBalance(const Column returns_column, Decimal book_size, const std::string header)
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


Column eodFixedInvBalance(const Column returns_column, const Column signal_column, Decimal book_size, const std::string header)
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
            if(toDec( signal_column[k] ) == toDec( signal_column[k-1] ))
                x[k] = toDec( x[k-1] ) * (1 + toDec( returns_column[k] ));
            else
                x[k] = book_size * (1 + toDec( returns_column[k] ));
        }
        catch(...)
        {
            x[k] = x[k-1];
        }

        if( k == medium_term+2 )
            x[k] = toDec( x[k-1] ) * (1 + toDec( returns_column[k] ));
    }

    return x;
}
