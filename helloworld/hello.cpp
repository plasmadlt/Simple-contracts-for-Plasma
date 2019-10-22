#include "hello.hpp"

#include <ion/print.hpp>

void hello::hi( name user )
{
  ion::require_auth(user);
  print( "Hello, ", user);
}

ION_DISPATCH( hello, (hi))
