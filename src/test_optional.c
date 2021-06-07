#include <stdio.h>
#include "libfoo.h"

int main()
{
  printf("foo_version_string = \"%s\"\n",foo_version_string);
  printf("foo_optional() = \"%s\"\n",foo_optional());
  return 0;
}
