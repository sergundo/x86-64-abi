#include <stdio.h>
#include <string.h>
#include "typelist.h"


/* This builds the tests for returning arguments.  */


/* Small helper function that prints spaces as '_'.  */
void
print_no_spaces (const char *str)
{
  const char *c = str;

  while (*c)
    {
      if (*c == ' ')
	printf ("_");
      else
	printf ("%c", *c);

      c++;
    }
}


/* Make top of the file.  */
void
make_file_start ()
{
  printf ("/* This is an autogenerated file. Do not edit.  */\n\n");
  printf ("#include \"defines.h\"\n");
  printf ("#include \"macros.h\"\n");
  printf ("#include \"args.h\"\n\n");
}

void
make_testing_callee (const char *type, const char *args, const char *ret)
{
  printf ("%s\nfun_test_returning_", type);
  print_no_spaces (type);
  /* We need to make sure that 'typeof' of this function (used in the
     WRAP* macros) does not contain an automatically determined 'const pure'
     specified.  If it does, then using this type as type for a
     function pointer inherits this specifier.  When calling such
     function pointer GCC can optimize away the call, it it thinks it's
     pure (even though overridden with 'snapshot*' meanwhile).  */
  printf (" (%s)\n{\n  volatile_var++;\n  return %s;\n}\n\n", args, ret);
}

void
make_testing_caller ()
{
  printf ("#define def_test_returning_type_xmm(fun, type, ret, reg) \\\n");
  printf ("  { type var = WRAP_RET (fun) (); \\\n");
  printf ("  assert (ret == (type) reg && ret == var); }\n");
}

void
make_test_functions ()
{
  int i;
  char args[100];
  char ret[100];

  /* Make scalar return test functions.  */
  for (i=0; i<typecount; i++)
    if ((testflags[i] & NO_AUTORETURN) == 0) {
      sprintf (args, "void");
      sprintf (ret, "%d", i+64);
      make_testing_callee (types[i], args, ret);
    }
}

void
make_test_scalar_returning ()
{
  int i;

  /* Print out the header of the function.  */
  printf ("int\nmain (void)\n{\n");

  /* Make the tests.  */
  for (i=0; i<typecount; i++) 
    if ((testflags[i] & NO_AUTORETURN) == 0) {
      printf ("  def_test_returning_type_xmm");
      printf ("(fun_test_returning_");
      print_no_spaces (types[i]);
      printf (", %s, %d, %s);\n", types[i], i+64, returns[i]);
    }

  printf ("  return 0;\n}\n");
}

int
main (int argc, char **argv)
{
  init_typelist ();

  make_file_start ();

  make_test_functions ();
  make_testing_caller ();
  make_test_scalar_returning ();

  return 0;
}