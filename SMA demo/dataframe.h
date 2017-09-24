#include <iomanip>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>


typedef boost::multiprecision::cpp_dec_float_50 Decimal;
typedef boost::variant <std::string, Decimal> VarField;

using Row = std::vector <VarField>;
using Column = std::vector <VarField>;
using Matrix = std::vector <Row>;


Decimal toDec(VarField x)
{
    return boost::get <Decimal> (x);
}


int maxCols(const Matrix &M)
{
    unsigned int x = 0;

    for(const Row row : M)
    {
        row.size() > x ? x = row.size() : x = x;
    }

    return x;
}


Matrix readCSV(const std::string file_name, const char separator, std::initializer_list <int> ovrride)
{
    //  ovrride is an array containing numbers of columns that aren't forced into float data type
    //  Refer to this: https://stackoverflow.com/questions/17848192/error-no-matching-function-for-call-to-beginint-c


    Matrix result;
    std::string row, item;

    std::ifstream in(file_name);

    while( getline(in, row) )
    {
        Row R;
        std::stringstream ss(row);
        int k = 0;
        bool exists;

        while( getline(ss, item, separator) )
        {

            if( exists = std::any_of( std::begin(ovrride), std::end(ovrride), [&](int i)
            {
                return i == k + 1;
            } ) )
                R.push_back(item);
            else
            {
                try
                {
                    R.push_back( stod(item) );
                }
                catch(...)
                {
                    R.push_back(item);
                }
            }

            k++;
        }
        result.push_back(R);
    }

    in.close();
    return result;
}


void writeCSV(const char* file_name, const Matrix &M, const char separator, const int precision)
{
    std::ofstream out(file_name);
    out << std::fixed;
    out << std::setprecision(precision);

    for(Row row : M)
    {
        unsigned int k = 0;
        for (VarField s:row)
        {
            k++;
            k == row.size() ? out << s : out << s << separator;
        }
        out << '\n';
    }

    out.close();
}


Column getColumn(const Matrix &M, unsigned int col)
{
    Column x;  ////////// pending column no. sanity check
    for( unsigned int k = 0 ; k < M.size() ; k++)    x.push_back(M[k][col - 1]);

    return x;
}


Row getRow(const Matrix &M, unsigned int row)
{
    Row x;  /////////////// pending row sanity check
    for( unsigned int k = 0 ; k < M[row - 1].size() ; k++ )
        x.push_back(M[row - 1][k]);

    return x;
}


void deleteRow(Matrix &M, unsigned int row)
{
    if( row < M.size() && row >= 0 )    M.erase(M.begin() + row);
}


void deleteColumn(Matrix &M, unsigned int col)
{
    for(Row &row : M)
        if( col < row.size() && col >= 0 )  row.erase(row.begin() + col);
}


void addColumn(Matrix &M, Column column, unsigned int col)
{
    //  Add a new column after the (col)-th column.
    //  e.g. col=6 means that the new column will be inserted
    //  after the 6th column.

    unsigned int max_cols = maxCols(M);
    unsigned int n, m = 0;

    //  Ensure that column index doesn't exceed number of columns
    col > max_cols || col < 0 ? n = max_cols : n = col;

    for(Row &row : M)
    {
        //  If length of row < number of columns,
        if( row.size() < n )
        {
            //  procedurally increase row size by adding blanks
            for( unsigned int k=0 ; k < (n - row.size()) ; k++)
                row.push_back("");

            //  then add placeholder at designated column no.
            m < column.size() ? row.push_back( column[m] ) : row.push_back("");
        }
        else
            if( m < column.size() )
                row.insert(row.begin() + n, column[m]);
            else
                row.insert(row.begin() + n, "");

        m++;
    }
}


void printVector(const std::vector <VarField> &V)
{
    for( unsigned int i = 0 ; i < V.size() ; i++ )   std::cout << V[i] << ' ';
}


void printMatrix(const Matrix &M)
{
    for(Row row : M)
    {
        for (VarField s : row)    std::cout << std::setw( 12 ) << std::left << s << " ";
        std::cout << '\n';
    }
}


Column newColumn(const Matrix &M, std::string header)
{
    Column x;
    x.push_back(header);
    for(Row row : M)
        x.push_back("");

    return x;
}


void printPart(const Matrix &M, unsigned int start_row, unsigned int end_row)
{
    for( unsigned int k = start_row - 1 ; k < end_row ; k++ )
    {
        for ( VarField s : getRow(M, k+1) )
            std::cout << std::setw( 12 ) << std::left << s << " ";
        std::cout << '\n';
    }
}
