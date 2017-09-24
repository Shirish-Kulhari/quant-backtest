/*
    A trivial SMA implementation. Lets the user specify time periods for short
    and medium-term moving averages. The strategy goes long when the short SMA
    crosses above the medium SMA, and goes short when short SMA crosses below
    the medium SMA.
*/

#include <iostream>
#include "dataframe.h"

unsigned int short_term = 10, medium_term = 26;
const Decimal BOOK_SIZE = 1000000;


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

    {
        //  Generate the short-term SMA column

        addColumn(data, "SHORT_SMA", "", 6);

        for( unsigned int k = short_term ; k < data.size() ; k++ )
        {
            Decimal sum = 0;

            for( unsigned int c = k+1-short_term ; c <= k ; c++ )
            {
                sum = sum + boost::get <Decimal> (data[c][4]);
            }
            sum = sum/short_term;
            data[k][5] = sum;
        }
    }

    {
        //  Generate the medium-term SMA column

        addColumn(data, "MEDIUM_SMA", "", 6);

        for( unsigned int k = medium_term ; k < data.size() ; k++ )
        {
            Decimal sum = 0;

            for( unsigned int c = k+1-medium_term ; c <= k ; c++ )
            {
                sum = sum + boost::get <Decimal> (data[c][4]);
            }
            sum = sum/medium_term;
            data[k][6] = sum;
        }
    }

    {
        //  Generate the short/long signals

        addColumn(data, "SIGNAL", "", 7);

        for( unsigned int k = medium_term+2 ; k < data.size() ; k++ )
        {
            if( data[k-2][5] < data[k-2][6] && data[k-1][5] > data[k-1][6] )
                data[k][7] = 1;
            else if( data[k-2][5] > data[k-2][6] && data[k-1][5] < data[k-1][6] )
                data[k][7] = -1;
            else
                data[k][7] = data[k-1][7];
        }
    }

    printPart(data, 1, 20);
    writeCSV("output.csv", data, ',', 6);
}

