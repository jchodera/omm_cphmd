/** This header contains the definitions of the exceptions that will be used for
  * error handling in this program.
  */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

namespace Amber {

class AmberParmError : public std::runtime_error {
    public:
        AmberParmError(std::string const& s) :
            std::runtime_error(s) {}
};

};
#endif /* EXCEPTIONS_H */
