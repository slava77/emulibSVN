#include<vector>

template<typename T>
inline std::vector<std::vector< T > * > * convert_2d_array_to_2d_vector(T * array, int dimension1_size, int dimension2_size) {
  std::vector<std::vector< T > * > * matrix = new std::vector<std::vector< T > * >();

  for(int i = 0; i < dimension1_size; ++i) {
    std::vector< T > * row = new std::vector< T >();
    for(int j = 0; j < dimension2_size; ++j) {
      row->push_back(array[i * dimension2_size + j]);
    }
    matrix->push_back(row);
  }

  return matrix;
}

template<typename T>
inline std::vector<std::vector< T > > twod_array_to_vectors(T * array, int dimension1_size, int dimension2_size) {
  std::vector<std::vector< T > > matrix = std::vector<std::vector< T > >();

  for(int i = 0; i < dimension1_size; ++i) {
    std::vector< T > row = std::vector< T >();
    for(int j = 0; j < dimension2_size; ++j) {
      row.push_back(array[i * dimension2_size + j]);
    }
    matrix.push_back(row);
  }

  return matrix;
}

template<typename T>
inline void write_all_histograms_in_2d_vector(std::vector<std::vector< T > > * histogram_matrix) {
  for(typename std::vector<std::vector< T > >::iterator row = histogram_matrix->begin();
      row != histogram_matrix->end();
      ++row) {
    for(typename std::vector< T >::iterator histogram = row->begin();
	histogram != row->end();
	++histogram) {
      (*histogram)->Write();
    }
  }
}

template<typename T>
inline void write_all_histograms_in_2d_vector_no_pointers(std::vector<std::vector< T > > histogram_matrix) {
  for(typename std::vector<std::vector< T > >::iterator row = histogram_matrix.begin();
      row != histogram_matrix.end();
      ++row) {
    for(typename std::vector< T >::iterator histogram = row->begin();
	histogram != row->end();
	++histogram) {
      histogram->Write();
    }
  }
}

template<typename T>
inline void delete_1d_vector_of_pointers(std::vector< T * > * v) {
  for(typename std::vector< T * >::iterator it = v->begin();
      it != v->end();
      ++it) {
    delete (*it);
  }

  delete v;
}

template<typename T>
inline void delete_2d_vector_of_pointers(std::vector<std::vector< T * > * > * matrix) {
  for(typename std::vector<std::vector< T * > * >::iterator row = matrix->begin();
      row != matrix->end();
      ++matrix) {
    delete_1d_vector_of_pointerS(*row);
  }

  delete matrix;
}
