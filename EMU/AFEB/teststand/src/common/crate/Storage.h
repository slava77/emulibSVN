//---------------------------------------------------------------------------
#ifndef StorageH
#define StorageH
//---------------------------------------------------------------------------

#include <vector>
#include <string>
#include <fstream.h>
//---------------------------------------------------------------------------

namespace storage_block {
//---------------------------------------------------------------------------
using std::vector;
using std::string;
//---------------------------------------------------------------------------

static const int max_columns = 16;
static const int def_precision = 2;

template < typename Type1, typename Type2 >
struct struct_template {
  Type1 first;
  Type2 columns[ max_columns ]; };

//------------- CLASS STORAGE -----------------------------------------------

template < typename Type1, typename Type2 >
class Storage
{
public:

  Storage ( void ) : prec(def_precision), vec_array(0) {};
  ~Storage(){};

  void put ( const Type1& first_col, const Type2* cols );
  bool get ( Type1& first_col, Type2* cols );
  bool get ( Type1& first_col, Type2* cols , int row );
  bool get ( Type1& first_col, Type2& col , int row , int ch );

  int rows ( void ) { return vec_array.size(); }
  void erase_end ( int _rows );
  void erase ( void )
    { vec_array.erase( vec_array.begin(), vec_array.end() ); }

  bool LoadFile ( char* file_name );
  bool SaveFile ( char* file_name );
  bool SaveFile ( void )
    { return SaveFile ( sFileName.c_str() ); }

  void precision ( int _prec ) { prec = _prec; } 

private:

  int prec;
  string sFileName;
  struct_template < Type1, Type2 > str_row;
  vector < struct_template < Type1, Type2 > > vec_array;

};
//---------------------------------------------------------------------------


//---------- REALISATION OF THE STORAGE CLASS -------------------------------

template < typename Type1, typename Type2 >
void Storage<Type1,Type2>::put ( const Type1& first_col, const Type2* cols )
{
  str_row.first = first_col;
  for ( int col = 0; col < max_columns; ++col )
    str_row.columns[col] = cols[col];
  vec_array.push_back ( str_row );
}
//---------------------------------------------------------------------------

template < typename Type1, typename Type2 >
bool Storage<Type1,Type2>::get ( Type1& first_col, Type2* cols )
{
  if ( vec_array.empty() ) return false;
  str_row = vec_array.back();
  first_col = str_row.first;
  for ( int col = 0; col < max_columns; ++col )
    cols[col] = str_row.columns[col];
  return true;
}
//---------------------------------------------------------------------------

template < typename Type1, typename Type2 >
bool Storage<Type1,Type2>::get
  ( Type1& first_col, Type2* cols, int row )
{
  if ( ( row < 0 )
    || ( row >= static_cast<int>(vec_array.size()) )
    || vec_array.empty() )
    return false;
  first_col = vec_array[row].first;
  for ( int col = 0; col < max_columns; ++col )
    cols[col] = vec_array[row].columns[col];
  return true;
}
//---------------------------------------------------------------------------

template < typename Type1, typename Type2 >
bool Storage<Type1,Type2>::get
  ( Type1& first_col, Type2& col, int row , int ch )
{
  if ( ( row < 0 )
    || ( row >= static_cast<int>(vec_array.size()) )
    || vec_array.empty() )
    return false;
  first_col = vec_array[row].first;
  col = vec_array[row].columns[ch];
  return true;
}
//---------------------------------------------------------------------------

template < typename Type1, typename Type2 >
void Storage<Type1,Type2>::erase_end ( int _rows )
{
  if ( _rows >= vec_array.size() ) _rows = vec_array.size();
  for ( int row = 0; row < _rows; ++row )
    vec_array.erase( vec_array.end() );
}

//---------------------------------------------------------------------------

template < typename Type1, typename Type2 >
bool Storage<Type1,Type2>::LoadFile ( char* file_name )
{
  if ( file_name[0] == 0 ) return false;
  sFileName = file_name;
  ifstream file ( file_name );
  if ( !file ) return false;

  int col;
  float temp;
  erase();
  while ( !file.eof() )
  {
    file >> temp;
    str_row.first = static_cast<Type1>( temp );
    for ( col = 0; col < max_columns; ++col )
    {
      if ( file.eof() ) break;
      file >> temp;
      str_row.columns[col] = static_cast<Type2>( temp );
    }
    vec_array.push_back ( str_row );
  }
  vec_array.pop_back();

  file.close();
  return vec_array.empty() ? false : true;
}
//---------------------------------------------------------------------------

template < typename Type1, typename Type2 >
bool Storage<Type1,Type2>::SaveFile ( char* file_name )
{
  if ( file_name[0] == 0 ) return false;
  if ( vec_array.empty() ) return false;

  ofstream file ( file_name );
  if ( !file ) return false;

  int col;
  file << std::fixed;
  file.precision ( prec );
  for ( int row = 0, nrows = vec_array.size(); row < nrows; ++row )
  {
    file << vec_array[row].first << "\t";
    for ( col = 0; col < max_columns; ++col )
      file << vec_array[row].columns[col] << "\t";
    file << std::endl;
  }

  file.close();
  return true;
}
//---------------------------------------------------------------------------

} //end namespace storage_block
using namespace storage_block;

//---------------------------------------------------------------------------
#endif
