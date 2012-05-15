#ifndef __AFEB_teststand_Legend_h__
#define __AFEB_teststand_Legend_h__

#include "TLegend.h"

#endif
namespace AFEB { namespace teststand {

    class Legend : public TLegend {
    public:
      Legend();
      int getColor( const int i ) const;
      int getStyle( const int i ) const;
    private:
      enum LineStyle_t { solid=1, dotted=3, dashed=5, dottedDashed=7 };
      enum Color_t { black=1, red=2, blue=4, green=8 };
      static const int nStyles_ = 4;
      static const int nColors_ = 4;
      static const int styles_[nStyles_];
      static const int colors_[nColors_];
    };

  }
}
