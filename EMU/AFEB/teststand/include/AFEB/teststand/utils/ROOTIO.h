#include "TMatrixTBase.h"
#include <iomanip>
#include <iostream>

using namespace std;

/// Insertion operator for all \e ROOT matrix classes derived from \c TMatrixTBase.
///
/// @param os ostream object
/// @param m any \e ROOT matrix class derived from \c TMatrixTBase
///
/// @return ostream object
///
template <typename T>
ostream& operator<<( ostream& os, const TMatrixTBase<T>& m ){
  for ( int iRow=0; iRow<m.GetNrows(); ++iRow ){
    os << "[ ";
    for ( int iCol=0; iCol<m.GetNcols(); ++iCol ){
      os << right << noshowpos << showpoint << scientific << setprecision(5) << setw(12) << m( iRow, iCol ) << ( iCol+1 == m.GetNcols() ? "" : ", " );
    }
    os << " ]";
    if ( iRow+1 < m.GetNrows() ) os << endl;
  }
  return os;
}
