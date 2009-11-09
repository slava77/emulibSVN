#ifndef __emuTA_Exception_h__
#define __emuTA_Exception_h__


#include "xcept/include/xcept/Exception.h"


namespace emuTA
{
    namespace exception
    {
        /**
         * Exception raised by the emuTA package.
         */
        class Exception: public xcept::Exception
        {
            public:

            Exception(std::string name,
                      std::string message,
                      std::string module,
                      int line,
                      std::string function)
            : xcept::Exception(name, message, module, line, function)
            {
            }

            Exception(std::string name,
                      std::string message,
                      std::string module,
                      int line,
                      std::string function,
                      xcept::Exception & e)
            : xcept::Exception(name, message, module, line, function, e)
            {
            }
        };
    }
}

#endif