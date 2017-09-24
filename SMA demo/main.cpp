/*
    A trivial SMA implementation. Lets the user specify time periods for short
    and medium-term moving averages. The strategy goes long when the short SMA
    crosses above the medium SMA, and goes short when short SMA crosses below
    the medium SMA.
*/

#include <iostream>
#include "finmodelling.h"

unsigned int short_term = 10, medium_term = 26;
const Decimal BOOK_SIZE = 1000000;

void generateSignal(Matrix &M);
void generatePNL(Matrix &M);

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


    addColumn( data, eodBalance( getColumn(data, 9), BOOK_SIZE, "BM_EOD_BAL" ), -1);
    generatePNL(data);

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

void generatePNL(Matrix &M)
{
    addColumn( M, newColumn(M, "PNL_TEMP"), -1 );
    addColumn( M, newColumn(M, "PNL"), -1 );
    M[medium_term+1][10] = BOOK_SIZE;

    for( unsigned int k = medium_term+2 ; k < M.size()-1 ; k++ )
    {
        if( toDec( M[k][7] ) * toDec( M[k+1][7] ) == -1 )
            M[k][10] = M[k][9];
        else
            M[k][10] = M[k-1][10];

        M[k][11] = toDec( M[k][10] ) - toDec( M[k-1][10] );
    }

    deleteColumn(M, 10);
}
