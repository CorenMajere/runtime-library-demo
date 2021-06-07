#include <stdio.h>
#include "libfoo.h"

int main()
{
  printf("foo_version_string = \"%s\"\n",foo_version_string);
  printf("foo() = \"%s\"\n",foo());
  return 0;
}
