//-----------------------------------------------------------------------
// $Id: Singleton.h,v 3.0 2006/07/20 21:15:47 geurts Exp $
// $Log: Singleton.h,v $
// Revision 3.0  2006/07/20 21:15:47  geurts
// *** empty log message ***
//
// Revision 2.0  2005/04/12 08:07:03  geurts
// *** empty log message ***
//
//
//-----------------------------------------------------------------------
#ifndef SINGLETON_H
#define SINGLETON_H
// a templatedsingleton
// 
//
//  version     0.1   V.I. 98/10/29
//  version     0.2   V.I. 99/04/28
//     now autodestroy
//  version     0.3   V.I. 99/10/10
//     zero the pointer in destructor
//       some shared library can call the destructor
//       (and the constructor) many times....
//  version     0.4   V.I. 22/08/2000
//         added a Helper to set instance before calling the constructor
//         to allow call to instance() from the constructor itself


#include "OwnIt.h"

/** a templated singleton

    It inherits from the user class
    
   T can be constructed only with its default constructor.

*/
template<class T>
class Singleton;



template<class T>
class SingletonHelper {
  
  
protected:
  
  inline explicit SingletonHelper(Singleton<T> * it);
  
};


template<class T>
class Singleton : private SingletonHelper<T> , public T {
  
public:
  
  typedef Singleton<T> self;
  typedef OwnIt<self> pointer;
  
  inline static self * instance();
  
  inline static void deleteInstance();
  inline static void setInstance(self * it);
  
  
protected:
  
  static pointer & nakedInstance() {
    static pointer instance_;
    return instance_;
  };
  
protected:
  
  inline Singleton() : SingletonHelper<T>(this) {}
  
};


template<class T>
inline SingletonHelper<T>::SingletonHelper(Singleton<T> * it)  {
  Singleton<T>::setInstance(it);
}


#include<typeinfo>
using std::endl;

template<class T>
inline typename Singleton<T>::self * Singleton<T>::instance() {
  if (nakedInstance().get() == 0){
    self * p = new self(); // instance now set in constructor of helper...
     std::cout 
      << "constructed a Singleton for " 
      << typeid(T).name() << " at " << nakedInstance().get() 
      << " " << p << std::endl;
  }
  return nakedInstance().get();
};

template<class T>
inline void Singleton<T>::setInstance(typename Singleton<T>::self * it) {
    std::cout  
      << "setting a Singleton for " 
      << typeid(T).name() << " at " << it << std::endl;
    nakedInstance() = it;
}

template<class T>
inline void Singleton<T>::deleteInstance() {
  if (nakedInstance().get() != 0){
    std::cout 
      << "deleting a Singleton for " 
      << typeid(T).name() << " at " << nakedInstance().get() << std::endl;
    nakedInstance().reset();
  }
  
}

#endif // SINGLETON_H
