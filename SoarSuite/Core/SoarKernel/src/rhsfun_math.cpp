#include <portability.h>

/*************************************************************************
 * PLEASE SEE THE FILE "license.txt" (INCLUDED WITH THIS SOFTWARE PACKAGE)
 * FOR LICENSE AND COPYRIGHT INFORMATION. 
 *************************************************************************/

/*************************************************************************
 *
 *  file:  rhsfun_math.cpp
 *
 * =======================================================================
 *  Support routines for doing math in the RHS of productions.
 *  Need more comments here.  Nothing in soarkernel.h either.
 *  
 *  
 * =======================================================================
 */

#include <stdlib.h>

#include "rhsfun_math.h"
#include "symtab.h"
#include "kernel.h"
#include "mem.h"
#include "print.h"
#include "lexer.h"
#include "rhsfun.h"
#include "soar_rand.h"

#include <math.h>



/* --------------------------------------------------------------------
                                Plus

   Takes any number of int_constant or float_constant arguments, and
   returns their sum.
-------------------------------------------------------------------- */

Symbol *plus_rhs_function_code (agent* thisAgent, list *args, void* /*user_data*/) {
  Bool float_found;
  int64_t i;
  double f = 0;
  Symbol *arg;
  cons *c;

  for (c=args; c!=NIL; c=c->rest) {
    arg = static_cast<symbol_union *>(c->first);
    if ((arg->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE) &&
        (arg->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE)) {
      print_with_symbols (thisAgent, "Error: non-number (%y) passed to + function\n",
                          arg);
      return NIL;
    }
  }

  i = 0;
  float_found = FALSE;
  while (args) {
    arg = static_cast<symbol_union *>(args->first);
    if (arg->common.symbol_type==INT_CONSTANT_SYMBOL_TYPE) {
      if (float_found) f += arg->ic.value;
      else i += arg->ic.value;
    } else {
      if (float_found) f += arg->fc.value;
      else { float_found = TRUE; f = arg->fc.value + i; }
    }
    args = args->rest;
  }
  if (float_found) return make_float_constant (thisAgent, f);
  return make_int_constant (thisAgent, i);
}

/* --------------------------------------------------------------------
                                Times

   Takes any number of int_constant or float_constant arguments, and
   returns their product.
-------------------------------------------------------------------- */

Symbol *times_rhs_function_code (agent* thisAgent, list *args, void* /*user_data*/) {
  Bool float_found;
  int64_t i;
  double f = 0;
  Symbol *arg;
  cons *c;
  
  for (c=args; c!=NIL; c=c->rest) {
    arg = static_cast<symbol_union *>(c->first);
    if ((arg->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE) &&
        (arg->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE)) {
      print_with_symbols (thisAgent, "Error: non-number (%y) passed to * function\n",
                          arg);
      return NIL;
    }
  }

  i = 1;
  float_found = FALSE;
  while (args) {
    arg = static_cast<symbol_union *>(args->first);
    if (arg->common.symbol_type==INT_CONSTANT_SYMBOL_TYPE) {
      if (float_found) f *= arg->ic.value;
      else i *= arg->ic.value;
    } else {
      if (float_found) f *= arg->fc.value;
      else { float_found = TRUE; f = arg->fc.value * i; }
    }
    args = args->rest;
  }
  if (float_found) return make_float_constant (thisAgent, f);
  return make_int_constant (thisAgent, i);
}

/* --------------------------------------------------------------------
                                Minus

   Takes one or more int_constant or float_constant arguments.
   If 0 arguments, returns NIL (error).
   If 1 argument (x), returns -x.
   If >=2 arguments (x, y1, ..., yk), returns x - y1 - ... - yk.
-------------------------------------------------------------------- */

Symbol *minus_rhs_function_code (agent* thisAgent, list *args, void* /*user_data*/) {
  Symbol *arg;
  double f = 0;  /* For gcc -Wall */
  int64_t i = 0;   /* For gcc -Wall */
  cons *c;
  Bool float_found;

  if (!args) {
    print (thisAgent, "Error: '-' function called with no arguments\n");
    return NIL;
  }
  
  for (c=args; c!=NIL; c=c->rest) {
    arg = static_cast<symbol_union *>(c->first);
    if ((arg->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE) &&
        (arg->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE)) {
      print_with_symbols (thisAgent, "Error: non-number (%y) passed to - function\n",
                          arg);
      return NIL;
    }
  }

  if (! args->rest) {
    /* --- only one argument --- */
    arg = static_cast<symbol_union *>(args->first);
    if (arg->common.symbol_type==INT_CONSTANT_SYMBOL_TYPE)
      return make_int_constant (thisAgent, - arg->ic.value);
    return make_float_constant (thisAgent, - arg->fc.value);
  }

  /* --- two or more arguments --- */
  arg = static_cast<symbol_union *>(args->first);
  float_found = FALSE;
  if (arg->common.symbol_type==INT_CONSTANT_SYMBOL_TYPE) i = arg->ic.value;
  else { float_found = TRUE; f = arg->fc.value; }
  for (c=args->rest; c!=NIL; c=c->rest) {
    arg = static_cast<symbol_union *>(c->first);
    if (arg->common.symbol_type==INT_CONSTANT_SYMBOL_TYPE) {
      if (float_found) f -= arg->ic.value;
      else i -= arg->ic.value;
    } else {
      if (float_found) f -= arg->fc.value;
      else { float_found = TRUE; f = i - arg->fc.value; }
    }
  }
 
  if (float_found) return make_float_constant (thisAgent, f);
  return make_int_constant (thisAgent, i);
}

/* --------------------------------------------------------------------
                     Floating-Point Division

   Takes one or more int_constant or float_constant arguments.
   If 0 arguments, returns NIL (error).
   If 1 argument (x), returns 1/x.
   If >=2 arguments (x, y1, ..., yk), returns x / y1 / ... / yk.
-------------------------------------------------------------------- */

Symbol *fp_divide_rhs_function_code (agent* thisAgent, list *args, void* /*user_data*/) {
  Symbol *arg;
  double f;
  cons *c;

  if (!args) {
    print (thisAgent, "Error: '/' function called with no arguments\n");
    return NIL;
  }
  
  for (c=args; c!=NIL; c=c->rest) {
    arg = static_cast<symbol_union *>(c->first);
    if ((arg->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE) &&
        (arg->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE)) {
      print_with_symbols (thisAgent, "Error: non-number (%y) passed to / function\n",
                          arg);
      return NIL;
    }
  }

  if (! args->rest) {
    /* --- only one argument --- */
    arg = static_cast<symbol_union *>(args->first);
    if (arg->common.symbol_type==INT_CONSTANT_SYMBOL_TYPE) f = static_cast<double>(arg->ic.value);
    else f = arg->fc.value;
    if (f != 0.0) return make_float_constant (thisAgent, 1.0 / f);
    print (thisAgent, "Error: attempt to divide ('/') by zero.\n");
    return NIL;
  }

  /* --- two or more arguments --- */
  arg = static_cast<symbol_union *>(args->first);
  if (arg->common.symbol_type==INT_CONSTANT_SYMBOL_TYPE) f = static_cast<double>(arg->ic.value);
  else f = arg->fc.value;
  for (c=args->rest; c!=NIL; c=c->rest) {
    arg = static_cast<symbol_union *>(c->first);
    if (arg->common.symbol_type==INT_CONSTANT_SYMBOL_TYPE) {
      if (arg->ic.value) f /= arg->ic.value;
      else { print (thisAgent, "Error: attempt to divide ('/') by zero.\n"); return NIL; }
    } else {
      if (arg->fc.value != 0.0) f /= arg->fc.value;
      else { print (thisAgent, "Error: attempt to divide ('/') by zero.\n"); return NIL; }
    }
  }
  return make_float_constant (thisAgent, f);
}

/* --------------------------------------------------------------------
                     Integer Division (Quotient)

   Takes two int_constant arguments, and returns their quotient.
-------------------------------------------------------------------- */

Symbol *div_rhs_function_code (agent* thisAgent, list *args, void* /*user_data*/) {
  Symbol *arg1, *arg2;

  arg1 = static_cast<symbol_union *>(args->first);
  arg2 = static_cast<symbol_union *>(args->rest->first);
  
  if (arg1->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE) {
    print_with_symbols (thisAgent, "Error: non-integer (%y) passed to div function\n",
                        arg1);
    return NIL;
  }
  if (arg2->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE) {
    print_with_symbols (thisAgent, "Error: non-integer (%y) passed to div function\n",
                        arg2);
    return NIL;
  }

  if (arg2->ic.value == 0) {
    print (thisAgent, "Error: attempt to divide ('div') by zero.\n");
    return NIL;
  }
  
  return make_int_constant (thisAgent, arg1->ic.value / arg2->ic.value);
 /* Warning: ANSI doesn't say precisely what happens if one or both of the
    two args is negative. */
}

/* --------------------------------------------------------------------
                          Integer Modulus

   Takes two int_constant arguments (x,y) and returns (x mod y), i.e.,
   the remainder after dividing x by y.
-------------------------------------------------------------------- */

Symbol *mod_rhs_function_code (agent* thisAgent, list *args, void* /*user_data*/) {
  Symbol *arg1, *arg2;

  arg1 = static_cast<symbol_union *>(args->first);
  arg2 = static_cast<symbol_union *>(args->rest->first);
  
  if (arg1->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE) {
    print_with_symbols (thisAgent, "Error: non-integer (%y) passed to mod function\n",
                        arg1);
    return NIL;
  }
  if (arg2->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE) {
    print_with_symbols (thisAgent, "Error: non-integer (%y) passed to mod function\n",
                        arg2);
    return NIL;
  }

  if (arg2->ic.value == 0) {
    print (thisAgent, "Error: attempt to divide ('mod') by zero.\n");
    return NIL;
  }
  
  return make_int_constant (thisAgent, arg1->ic.value % arg2->ic.value);
 /* Warning:  ANSI guarantees this does the right thing if both args are
    positive.  If one or both is negative, it only guarantees that
    (a/b)*b + a%b == a. */
}

/*
 * SIN_RHS_FUNCTION_CODE
 *
 * Returns as a float the sine of an angle measured in radians.
 */
Symbol *sin_rhs_function_code(agent* thisAgent, list *args, void* /*user_data*/)
{
    Symbol *arg;
    double  arg_value;

    if (!args) {
	print(thisAgent, "Error: 'sin' function called with no arguments\n");
	return NIL;
    }

    arg = static_cast<symbol_union *>(args->first);
    if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE)
	arg_value = arg->fc.value;
    else if (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE)
	arg_value = static_cast<double>(arg->ic.value) ;
    else {
	print_with_symbols(thisAgent, "Error: 'sin' function called with non-numeric argument %y\n", arg);
	return NIL;
    }

    return make_float_constant(thisAgent, sin(arg_value));
}


/*
 * COS_RHS_FUNCTION_CODE
 *
 * Returns as a float the cosine of an angle measured in radians.
 */
Symbol *cos_rhs_function_code(agent* thisAgent, list *args, void* /*user_data*/)
{
    Symbol *arg;
    double  arg_value;

    if (!args) {
	print(thisAgent, "Error: 'cos' function called with no arguments\n");
	return NIL;
    }

    arg = static_cast<symbol_union *>(args->first);
    if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE)
	arg_value = arg->fc.value;
    else if (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE)
	arg_value = static_cast<double>(arg->ic.value) ;
    else {
	print_with_symbols(thisAgent, "Error: 'cos' function called with non-numeric argument %y\n", arg);
	return NIL;
    }
    return make_float_constant(thisAgent, cos(arg_value));
}


/*
 * SQRT_RHS_FUNCTION_CODE
 *
 * Returns as a float the square root of its argument (integer or float).
 */
Symbol *sqrt_rhs_function_code(agent* thisAgent, list *args, void* /*user_data*/)
{
    Symbol *arg;
    double  arg_value;

    if (!args) {
	print(thisAgent, "Error: 'sqrt' function called with no arguments\n");
	return NIL;
    }

    arg = static_cast<symbol_union *>(args->first);
    if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE)
	arg_value = arg->fc.value;
    else if (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE)
	arg_value = static_cast<double>(arg->ic.value);
    else {
	print_with_symbols(thisAgent, "Error: 'sqrt' function called with non-numeric argument %y\n", arg);
	return NIL;
    }
    return make_float_constant(thisAgent, sqrt(arg_value));
}


/*
 * ATAN2_RHS_FUNCTION_CODE
 *
 * Returns as a float in radians the arctangent of (first_arg/second_arg)
 * which are floats or integers.
 */
Symbol *atan2_rhs_function_code(agent* thisAgent, list *args, void* /*user_data*/)
{
    Symbol *arg;
    cons *c;
    double  numer_value,
           denom_value;

    if (!args) {
	print(thisAgent, "Error: 'atan2' function called with no arguments\n");
	return NIL;
    }

    for (c=args; c!=NIL; c=c->rest) {
	arg = static_cast<symbol_union *>(c->first);
	if (   (arg->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE)
	    && (arg->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE)) {
	    print_with_symbols (thisAgent, "Error: non-number (%y) passed to atan2\n",
				arg);
	    return NIL;
	}
    }

    if (!args->rest) {
	print(thisAgent, "Error: 'atan2' function called with only one argument\n");
	return NIL;
    }

    arg = static_cast<symbol_union *>(args->first);
    if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE)
	numer_value = arg->fc.value;
    else
	numer_value = static_cast<double>(arg->ic.value) ;

    c = args->rest;
    if (c->rest) {
	print(thisAgent, "Error: 'atan2' function called with more than two arguments.\n");
	return NIL;
    }
    arg = static_cast<symbol_union *>(c->first);
    if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE)
	denom_value = arg->fc.value;
    else
	denom_value = static_cast<double>(arg->ic.value) ;

    return make_float_constant(thisAgent, atan2(numer_value, denom_value));
}


/*
 * ABS_RHS_FUNCTION_CODE
 *
 * Returns the absolute value of a float as a float, of an int as an int.
 */
Symbol *abs_rhs_function_code(agent* thisAgent, list *args, void* /*user_data*/)
{
    Symbol *arg,
           *return_value;

    if (!args) {
	print(thisAgent, "Error: 'abs' function called with no arguments\n");
	return NIL;
    }

    arg = static_cast<symbol_union *>(args->first);
    if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE)
	return_value = make_float_constant(thisAgent, fabs(arg->fc.value));
    else if (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE)
	return_value = make_int_constant(thisAgent, (arg->ic.value<0) ? -arg->ic.value : arg->ic.value);
    else {
	print_with_symbols(thisAgent, "Error: 'abs' function called with non-numeric argument %y\n", arg);
	return NIL;
    }
    return return_value;
}


/* --------------------------------------------------------------------
                         int

   Casts the given symbol into an integer.  If the symbol is a sym
   constant, a conversion is done.  If the symbol is a float, then
   the integer portion is returned.
-------------------------------------------------------------------- */

Symbol *int_rhs_function_code (agent* thisAgent, list *args, void* /*user_data*/) {
  Symbol * sym;

  if (!args) {
    print (thisAgent, "Error: 'int' function called with no arguments.\n");
    return NIL;
  }

  if (args->rest) {
    print (thisAgent, "Error: 'int' takes exactly 1 argument.\n");
    return NIL;
  }

  sym = static_cast<Symbol *>(args->first);
  if (sym->common.symbol_type == VARIABLE_SYMBOL_TYPE) {
    print_with_symbols (thisAgent, "Error: variable (%y) passed to 'int' RHS function.\n",
			sym);
    return NIL;
  } else if (sym->common.symbol_type == IDENTIFIER_SYMBOL_TYPE) {
    print_with_symbols (thisAgent, "Error: identifier (%y) passed to 'int' RHS function.\n",
			sym);
    return NIL;
  } else if (sym->common.symbol_type == SYM_CONSTANT_SYMBOL_TYPE) {
    int64_t int_val;

    errno = 0;
    int_val = strtol(symbol_to_string (thisAgent, sym, FALSE, NIL, 0), NULL, 10);
    if (errno) {
      print (thisAgent, "Error: bad integer (%y) given to 'int' RHS function\n",
	     sym);
      return NIL;
    }
    return make_int_constant (thisAgent, int_val);
  } else if (sym->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) {
    symbol_add_ref(sym) ;
    return sym;
  } else if (sym->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE) {
    double int_part;
    modf(sym->fc.value, &int_part);
    return make_int_constant(thisAgent, static_cast<int64_t>(int_part) );
  }

  print (thisAgent, "Error: unknown symbol type (%y) given to 'int' RHS function\n",
	 sym);
  return NIL;
}


/* --------------------------------------------------------------------
                         float

   Casts the given symbol into an float.  If the symbol is a sym
   constant, a conversion is done.  If the symbol is an int, then
   the integer portion is converted.
-------------------------------------------------------------------- */

Symbol *float_rhs_function_code (agent* thisAgent, list *args, void* /*user_data*/) {
  Symbol * sym;

  if (!args) {
    print (thisAgent, "Error: 'float' function called with no arguments.\n");
    return NIL;
  }

  if (args->rest) {
    print (thisAgent, "Error: 'float' takes exactly 1 argument.\n");
    return NIL;
  }

  sym = static_cast<Symbol *>(args->first);
  if (sym->common.symbol_type == VARIABLE_SYMBOL_TYPE) {
    print_with_symbols (thisAgent, "Error: variable (%y) passed to 'float' RHS function.\n",
			sym);
    return NIL;
  } else if (sym->common.symbol_type == IDENTIFIER_SYMBOL_TYPE) {
    print_with_symbols (thisAgent, "Error: identifier (%y) passed to 'float' RHS function.\n",
			sym);
    return NIL;
  } else if (sym->common.symbol_type == SYM_CONSTANT_SYMBOL_TYPE) {
    double float_val;

    errno = 0;
    float_val = strtod(symbol_to_string (thisAgent, sym, FALSE, NIL, 0), NULL);
    if (errno) {
      print (thisAgent, "Error: bad float (%y) given to 'float' RHS function\n",
	     sym);
      return NIL;
    }
    return make_float_constant (thisAgent, float_val);
  } else if (sym->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE) {
    symbol_add_ref(sym) ;
    return sym;
  } else if (sym->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) {
    return make_float_constant(thisAgent, static_cast<double>(sym->ic.value) );
  }

  print (thisAgent, "Error: unknown symbol type (%y) given to 'float' RHS function\n",
	 sym);
  return NIL;
}

/* voigtjr 6/12/2007: added these built in functions on laird's request
these are straight out of the <8.6 kernel */

/***********************************************************
These RHS functions are used in the Quake-Soar agent.  They
are modified versions of the routines taken from TacAir-Soar.
*************************************************************/

/* "Normalizes" an integral heading to be between -180 and +180 */
int64_t normalize_heading_int(int64_t n)
{
    /* we need to make sure that -180 < value <= 180 so we modify */
    /*  the original rounded value using the fact that for heading, */
    /*  for any integer value of x, the following holds:            */
    /*            heading1 = x*360 + heading2                      */
    while (n <= -180)
        n += 360;
    while (n > 180)
        n -= 360;

    return n;
}

/* "Normalizes" a floating point heading to be between -180.0 and +180.0 */
double normalize_heading_float(double n)
{
    /* we need to make sure that -180 < value <= 180 so we modify */
    /*  the original rounded value using the fact that for heading, */
    /*  for any integer value of x, the following holds:            */
    /*            heading1 = x*360 + heading2                      */
    while (n <= -180.0)
        n += 360.0;
    while (n > 180.0)
        n -= 360.0;

    return n;
}

int64_t round_off_heading_int(int64_t n, int64_t m)
{
    int64_t unbounded_rounded;

    /* need to round the first (i_n) to the nearest second (i_m) */
    if (n < 0)
        unbounded_rounded = m * ((n - (m / 2L)) / m);
    else
        unbounded_rounded = m * ((n + (m / 2L)) / m);

    return unbounded_rounded;
}

double round_off_heading_float(double n, double m)
{
    double n_10, m_10, unbounded_rounded;
    double ip;
    double ip2;

    /* OK.  Both n and m can have tenths, so multiply by 10 and treat
       as integers */
    modf((n * 10.0), &ip);
    n_10 = ip;
    modf((m * 10.0), &ip);
    m_10 = ip;

    if (n_10 < 0.0) {

        modf((m_10 / 2.0), &ip2);
        modf(((n_10 - ip2) / m_10), &ip);
        unbounded_rounded = (m_10 * ip);
    } else {

        modf((m_10 / 2.0), &ip2);
        modf(((n_10 + ip2) / m_10), &ip);
        unbounded_rounded = (m_10 * ip);
    }

    /* Divide by 10 to get tenths back and return */
    return unbounded_rounded / 10.0;
}

Symbol *round_off_heading_air_rhs_function_code(agent* thisAgent, list *args, void* /*user_data*/)
{
    Symbol *arg;
    double n = 0, f_m = 0;
    int64_t i_m = 0;
    cons *c;
    bool float_found = FALSE;

    if (!args) {
        print(thisAgent, "Error: 'round_off_heading' function called with no arguments\n");
        return NIL;
    }

    if (!args->rest) {
        /* --- only one argument --- */
        print(thisAgent, "Error: 'round_off_heading' function called with only one argument.\n");
        return NIL;
    }

    /* --- two or more arguments --- */
    arg = static_cast<Symbol *>(args->first);
    if (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE)
        n = static_cast<double>(arg->ic.value) ;
    else if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE) {
        n = arg->fc.value;
    }

    c = args->rest;
    if (c->rest) {
        /* --- more than two arguments --- */
        print(thisAgent, "Error: 'round_off_heading' function called with more than two arguments.\n");
        return NIL;
    }
    arg = static_cast<Symbol *>(c->first);
    if (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE)
        i_m = arg->ic.value;
    else if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE) {
        float_found = TRUE;
        f_m = arg->fc.value;
    }

    /* Now, deal with the arguments based on type and return result */
    if (float_found)
        return make_float_constant(thisAgent, normalize_heading_float(round_off_heading_float(n, f_m)));
    else
        return make_int_constant(thisAgent, normalize_heading_int(round_off_heading_int(static_cast<int64_t>(n) , i_m)));

}

/* code for round_off_heading */

/* --------------------------------------------------------------------
                                round_off

 Takes two numbers and returns the first rounded to the nearest second.
-------------------------------------------------------------------- */
Symbol *round_off_air_rhs_function_code(agent* thisAgent, list *args, void* /*user_data*/)
{
    Symbol *arg;
    double n = 0, f_m = 0;
    int64_t i_m = 0;
    cons *c;
    bool float_found = FALSE;

    if (!args) {
        print(thisAgent, "Error: 'round_off' function called with no arguments\n");
        return NIL;
    }

    if (!args->rest) {
        /* --- only one argument --- */
        print(thisAgent, "Error: 'round_off' function called with only one argument.\n");
        return NIL;
    }

    /* --- two or more arguments --- */
    arg = static_cast<Symbol *>(args->first);
    if (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE)
        n = static_cast<double>(arg->ic.value) ;
    else if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE) {
        n = arg->fc.value;
    }

    c = args->rest;
    if (c->rest) {
        /* --- more than two arguments --- */
        print(thisAgent, "Error: 'round_off' function called with more than two arguments.\n");
        return NIL;
    }
    arg = static_cast<Symbol *>(c->first);
    if (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE)
        i_m = arg->ic.value;
    else if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE) {
        float_found = TRUE;
        f_m = arg->fc.value;
    }

    /* Now, deal with the arguments based on type and return result */
    if (float_found)
        return make_float_constant(thisAgent, round_off_heading_float(n, f_m));
    else
        return make_int_constant(thisAgent, round_off_heading_int(static_cast<int64_t>(n), i_m));
}

#define PI 3.141592653589
#define PI_OVER_TWO (PI/2)
#define TWO_PI (PI*2)
#define RAD_TO_DEG(X) ((X*180)/PI)
#define X 0
#define Y 1
#define Z 2

void vector_from_to_position(double pos1[3], double pos2[3], double vector[3])
{
    vector[X] = pos2[X] - pos1[X];
    vector[Y] = pos2[Y] - pos1[Y];
    vector[Z] = pos2[Z] - pos1[Z];
}

void vec2_norm(double v[3], double r[3], int abort)
{
    double mag, mag2;
    mag2 = v[X] * v[X] + v[Y] * v[Y];
    mag = sqrt(mag2);
    if (!abort && (mag < 0.01)) {
        r[0] = 1.0;
        r[1] = 0.0;
        return;
    }
    r[0] = v[0] / mag;
    r[1] = v[1] / mag;
}

double convert_to_soar_angle(double heading_in_rads)
{
    double heading;

    /* Not only correct, but more efficient! */
    heading = heading_in_rads - PI_OVER_TWO;
    if (heading < 0.0)
        heading += TWO_PI;
    heading = TWO_PI - heading;
    if (heading > PI)
        heading -= TWO_PI;

    return heading;
}

void hrl_xydof_to_heading(double xydof[3], double * output)
{
    double heading_in_rads;

    heading_in_rads = convert_to_soar_angle(atan2(xydof[Y], xydof[X]));

    (*output) = heading_in_rads;
}

int64_t air_soar_round_off_angle(int64_t n, int64_t m)
{
    int64_t unbounded_rounded, bounded_rounded;

    /* need to round the first (n) to the nearest second (m) */
    if (n < 0)
        unbounded_rounded = m * ((n - (m / 2L)) / m);
    else
        unbounded_rounded = m * ((n + (m / 2L)) / m);

    /* we need to make sure that -180 < value <= 180. */

	// FIXME replace this code with the much faster mod2pi in ed olson's linalg library
    bounded_rounded = (unbounded_rounded % 360);
    if (bounded_rounded > 180)
        bounded_rounded -= 360;
    if (bounded_rounded <= -180)
        bounded_rounded += 360;

    return bounded_rounded;
}

double bracket_rad_to_deg(double var)
{
    return static_cast<double>(air_soar_round_off_angle(static_cast<int64_t>(RAD_TO_DEG(var)), 1)) ;
}

int64_t convert(double flo)
{
    return static_cast<int64_t>(flo);
}

int64_t heading_to_point(int64_t current_x, int64_t current_y, int64_t x, int64_t y)
{
    double plane_pos[3], waypoint_pos[3], dir[3];
    double heading;

    plane_pos[0] = static_cast<double>(current_x) ;
    plane_pos[1] = static_cast<double>(current_y) ;
    plane_pos[2] = 0;

    waypoint_pos[0] = static_cast<double>(x) ;
    waypoint_pos[1] = static_cast<double>(y) ;
    waypoint_pos[2] = 0;

    vector_from_to_position(plane_pos, waypoint_pos, dir);
    vec2_norm(dir, dir, FALSE);
    hrl_xydof_to_heading(dir, &heading);

    return convert(bracket_rad_to_deg(heading));
}

/* --------------------------------------------------------------------
                                compute-heading

 Takes 4 args and returns integer heading from x1,y1 to x2,y2
-------------------------------------------------------------------- */

Symbol *compute_heading_rhs_function_code(agent* thisAgent, list *args, void* /*user_data*/)
{
    Symbol *arg;
    int64_t current_x, current_y;
    int64_t waypoint_x, waypoint_y;
    int count;
    cons *c;

    if (!args) {
        print(thisAgent, "Error: 'compute-heading' function called with no arguments\n");
        return NIL;
    }

    for (c = args; c != NIL; c = c->rest) {
        arg = static_cast<Symbol *>(c->first);
        if ((arg->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE) &&
            (arg->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE)) {
            print_with_symbols(thisAgent, "Error: non-number (%y) passed to - compute-heading\n", arg);
            return NIL;
        }
    }

    count = 1;

    for (c = args->rest; c != NIL; c = c->rest) {
        arg = static_cast<Symbol *>(c->first);
        if ((arg->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE) &&
            (arg->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE)) {
            print_with_symbols(thisAgent, "Error: non-number (%y) passed to compute-heading function.\n", arg);
            return NIL;
        } else {
            count++;
        }
    }

    if (count != 4) {
        print(thisAgent, "Error: 'compute-heading' takes exactly 4 arguments.\n");
        return NIL;
    }

    arg = static_cast<Symbol *>(args->first);
    current_x = (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) ? arg->ic.value : static_cast<int64_t>(arg->fc.value);

    arg = static_cast<Symbol *>(args->rest->first);
    current_y = (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) ? arg->ic.value : static_cast<int64_t>(arg->fc.value);

    arg = static_cast<Symbol *>(args->rest->rest->first);
    waypoint_x = (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) ? arg->ic.value : static_cast<int64_t>(arg->fc.value);

    arg = static_cast<Symbol *>(args->rest->rest->rest->first);
    waypoint_y = (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) ? arg->ic.value : static_cast<int64_t>(arg->fc.value);

    return make_int_constant(thisAgent, heading_to_point(current_x, current_y, waypoint_x, waypoint_y));
}

/* --------------------------------------------------------------------
                                compute-range

 Takes 4 args and returns integer range from x1,y1 to x2,y2
-------------------------------------------------------------------- */

Symbol *compute_range_rhs_function_code(agent* thisAgent, list *args, void* /*user_data*/)
{
    Symbol *arg;
    double current_x, current_y;
    double waypoint_x, waypoint_y;
    int count;
    cons *c;

    if (!args) {
        print(thisAgent, "Error: 'compute-range' function called with no arguments\n");
        return NIL;
    }

    for (c = args; c != NIL; c = c->rest) {
        arg = static_cast<Symbol *>(c->first);
        if ((arg->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE)
            && (arg->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE)) {
            print_with_symbols(thisAgent, "Error: non-number (%y) passed to - compute-range\n", arg);
            return NIL;
        }
    }

    count = 1;

    for (c = args->rest; c != NIL; c = c->rest) {
        arg = static_cast<Symbol *>(c->first);
        if ((arg->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE)
            && (arg->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE)) {
            print_with_symbols(thisAgent, "Error: non-number (%y) passed to compute-range function.\n", arg);
            return NIL;
        } else {
            count++;
        }
    }

    if (count != 4) {
        print(thisAgent, "Error: 'compute-range' takes exactly 4 arguments.\n");
        return NIL;
    }

    arg = static_cast<Symbol *>(args->first);
    current_x = (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) ? static_cast<double>(arg->ic.value) : arg->fc.value;

    arg = static_cast<Symbol *>(args->rest->first);
    current_y = (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) ? static_cast<double>(arg->ic.value) : arg->fc.value;

    arg = static_cast<Symbol *>(args->rest->rest->first);
    waypoint_x = (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) ? static_cast<double>(arg->ic.value) : arg->fc.value;

    arg = static_cast<Symbol *>(args->rest->rest->rest->first);
    waypoint_y = (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) ? static_cast<double>(arg->ic.value) : arg->fc.value;

    return make_int_constant(thisAgent, static_cast<int64_t>(sqrt((current_x - waypoint_x)
                                             * (current_x - waypoint_x)
                                             + (current_y - waypoint_y)
                                             * (current_y - waypoint_y))));
}

/* --------------------------------------------------------------------
								rand-float

 Takes an optional integer argument.
 Returns [0,1.0] of no argument, or if argument is not positive.
 Returns [0,n] if argument is positive.
-------------------------------------------------------------------- */
Symbol* rand_float_rhs_function_code(agent* thisAgent, list* args, void* /*user_data*/)
{
	double n = 0;
	if (args) 
	{
	    cons* c = args;
	    Symbol* arg = static_cast<Symbol*>(c->first);
		if (arg) {
			if (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) {
				n = static_cast<double>(arg->ic.value);
			} else if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE) {
				n = arg->fc.value;
			} else {
	            print_with_symbols(thisAgent, "Error: non-number (%y) passed to - rand-float\n", arg);
				return NIL;
			}
		} else {
			// assume default behavior (no arg)
			// possibly warn? when can this happen?
		}
	}

	if (n > 0) {
		return make_float_constant(thisAgent, SoarRand(n));
	} 
	return make_float_constant(thisAgent, SoarRand());
}

/* --------------------------------------------------------------------
								rand-int

 Takes an optional integer argument.
 Returns [-2^31,2^31-1] of no argument, or if argument is not positive.
 Returns [0,n] if argument is positive.
-------------------------------------------------------------------- */
Symbol* rand_int_rhs_function_code(agent* thisAgent, list* args, void* /*user_data*/)
{
	int64_t n = 0;
	if (args) 
	{
	    cons* c = args;
	    Symbol* arg = static_cast<Symbol*>(c->first);
		if (arg) {
			if (arg->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE) {
				n = arg->ic.value;
			} else if (arg->common.symbol_type == FLOAT_CONSTANT_SYMBOL_TYPE) {
				n = static_cast<int64_t>(arg->fc.value);
			} else {
	            print_with_symbols(thisAgent, "Error: non-number (%y) passed to - rand-int\n", arg);
				return NIL;
			}
		} else {
			// assume default behavior (no arg)
			// possibly warn? when can this happen?
		}
	}

	if (n > 0) {
		return make_int_constant(thisAgent, static_cast<int64_t>(SoarRandInt(static_cast<uint32_t>(n))));
	} 
	return make_int_constant(thisAgent, SoarRandInt());
}

inline double _dice_zero_tolerance( double in )
{
	return ( ( fabs( in ) <= 0.00001 )?( 0 ):( in ) );
}

// http://www.brpreiss.com/books/opus4/html/page467.html
uint64_t _dice_binom( uint64_t n, uint64_t m )
{
	uint64_t* b = new uint64_t[ n+1 ];
	uint64_t i, j, ret;

	b[0] = 1;
	for ( i=1; i<=n; ++i )
	{
		b[i] = 1;
		for ( j=(i-1); j>0; --j )
		{
			b[j] += b[j-1];
		}
	}
	ret = b[m];
	delete[] b;

	return ret;
}

double _dice_prob_exact( int64_t dice, int64_t sides, int64_t count )
{
	// makes no sense
	if ( dice < 0 )
		return 0;
	if ( count < 0 )
		return 0;
	if ( sides < 1 )
		return 0;

	// if there are no dice, probability is zero unless count is also zero
	if ( dice == 0 )
	{
		if ( count == 0 )
			return 1;

		return 0;
	}

	if ( count > dice )
		return 0;

	double p1kd = pow( static_cast< double >( sides ), static_cast< double >( count ) );
	double p2nkn = pow( static_cast< double >( sides - 1 ), static_cast< double >( dice - count ) );
	double p2nkd = pow( static_cast< double >( sides ), static_cast< double >( dice - count ) );

	double result = static_cast< double >( _dice_binom( static_cast< uint64_t >( dice ), static_cast< uint64_t >( count ) ) );
	result *= ( static_cast< double >( 1.0 ) / p1kd );
	result *= ( p2nkn / p2nkd );

	return result;
}

double _dice_prob_atleast( int64_t dice, int64_t sides, int64_t count )
{
	// makes no sense
	if ( dice < 0 )
		return 0;
	if ( count < 0 )
		return 0;
	if ( sides < 1 )
		return 0;

	double result = 0.0;
	for ( int64_t i=0; ( count + i <= dice ); ++i )
	{
		result += _dice_prob_exact( dice, sides, count+i );
	}

	return result;
}

// Taken primarily from Jon Voigt's soar-dice project
// (compute-dice-probability dice sides count predicate) = 0..1
Symbol* dice_prob_rhs_function_code(agent* thisAgent, list* args, void* /*user_data*/)
{
	int64_t dice;
	int64_t sides;
	int64_t count;

	enum pred_type { eq, ne, lt, gt, le, ge, bad };
	pred_type pred = bad;

	// parse + validate
	{
		Symbol* temp_sym;

		// dice
		temp_sym = static_cast< Symbol* >( args->first );
		if ( ( temp_sym->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE ) &&
			 ( temp_sym->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE ) )
		{
			print_with_symbols( thisAgent, "Error: non-number (%y) passed as 'dice' to - compute-dice-probability\n", temp_sym );
			return NIL;
		}
		dice = ( ( temp_sym->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE )?( temp_sym->ic.value ):( static_cast< int64_t >( temp_sym->fc.value ) ) );

		// sides
		temp_sym = static_cast< Symbol* >( args->rest->first );
		if ( ( temp_sym->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE ) &&
			 ( temp_sym->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE ) )
		{
			print_with_symbols( thisAgent, "Error: non-number (%y) passed as 'sides' to - compute-dice-probability\n", temp_sym );
			return NIL;
		}
		sides = ( ( temp_sym->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE )?( temp_sym->ic.value ):( static_cast< int64_t >( temp_sym->fc.value ) ) );

		// count
		temp_sym = static_cast< Symbol* >( args->rest->rest->first );
		if ( ( temp_sym->common.symbol_type != INT_CONSTANT_SYMBOL_TYPE ) &&
			 ( temp_sym->common.symbol_type != FLOAT_CONSTANT_SYMBOL_TYPE ) )
		{
			print_with_symbols( thisAgent, "Error: non-number (%y) passed as 'count' to - compute-dice-probability\n", temp_sym );
			return NIL;
		}
		count = ( ( temp_sym->common.symbol_type == INT_CONSTANT_SYMBOL_TYPE )?( temp_sym->ic.value ):( static_cast< int64_t >( temp_sym->fc.value ) ) );

		// pred
		temp_sym = static_cast< Symbol* >( args->rest->rest->rest->first );
		if ( temp_sym->common.symbol_type != SYM_CONSTANT_SYMBOL_TYPE )
		{
			print_with_symbols( thisAgent, "Error: non-string (%y) passed as 'pred' to - compute-dice-probability\n", temp_sym );
			return NIL;
		}
		if ( strcmp( temp_sym->sc.name, "eq" ) == 0 )
		{
			pred = eq;
		}
		else if ( strcmp( temp_sym->sc.name, "ne" ) == 0 )
		{
			pred = ne;
		}
		else if ( strcmp( temp_sym->sc.name, "lt" ) == 0 )
		{
			pred = lt;
		}
		else if ( strcmp( temp_sym->sc.name, "gt" ) == 0 )
		{
			pred = gt;
		}
		else if ( strcmp( temp_sym->sc.name, "le" ) == 0 )
		{
			pred = le;
		}
		else if ( strcmp( temp_sym->sc.name, "ge" ) == 0 )
		{
			pred = ge;
		}
		if ( pred == bad )
		{
			print_with_symbols( thisAgent, "Error: invalid string (%y) passed as 'pred' to - compute-dice-probability\n", temp_sym );
			return NIL;
		}
	}

	double ret = 0;
	if ( pred == eq )
	{
		if ( ( count < 0 ) || ( count > dice ) )
		{
			ret = 0;
		}
		else
		{
			ret = _dice_zero_tolerance( _dice_prob_exact( dice, sides, count ) );
		}
	}
	else if ( pred == ne )
	{
		if ( ( count < 0 ) || ( count > dice ) )
		{
			ret = 1;
		}
		else
		{
			ret = _dice_zero_tolerance( 1 - _dice_prob_exact( dice, sides, count ) );
		}
	}
	else if ( pred == lt )
	{
		if ( count <= 0 )
		{
			ret = 0;
		}
		else if ( count > dice )
		{
			ret = 1;
		}
		else
		{
			ret = _dice_zero_tolerance( 1 - _dice_prob_atleast( dice, sides, count ) );
		}
	}
	else if ( pred == gt )
	{
		if ( count < 0 )
		{
			ret = 1;
		}
		else if ( count >= dice )
		{
			ret = 0;
		}
		else
		{
			ret = _dice_zero_tolerance( _dice_prob_atleast( dice, sides, count ) - _dice_prob_exact( dice, sides, count ) );
		}
	}
	else if ( pred == le )
	{
		if ( count < 0 )
		{
			ret = 0;
		}
		else if ( count >= dice )
		{
			ret = 1;
		}
		else
		{
			ret = _dice_zero_tolerance( ( 1 - _dice_prob_atleast( dice, sides, count ) ) + _dice_prob_exact( dice, sides, count ) );
		}
	}
	else if ( pred == ge )
	{
		if ( count <= 0 )
		{
			ret = 1;
		}
		else if ( count > dice )
		{
			ret = 0;
		}
		else
		{
			ret = _dice_zero_tolerance( _dice_prob_atleast( dice, sides, count ) );
		}
	}
	
	return make_float_constant(thisAgent, ret);
}

/* ====================================================================

                  Initialize the Built-In RHS Math Functions

====================================================================
*/

void init_built_in_rhs_math_functions (agent* thisAgent)
{
	add_rhs_function (thisAgent, make_sym_constant (thisAgent, "+"), plus_rhs_function_code,
					-1, TRUE, FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant (thisAgent, "*"), times_rhs_function_code,
					-1, TRUE, FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant (thisAgent, "-"), minus_rhs_function_code,
					-1, TRUE, FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant (thisAgent, "/"), fp_divide_rhs_function_code,
					-1, TRUE, FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant (thisAgent, "div"), div_rhs_function_code,
					2, TRUE, FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant (thisAgent, "mod"), mod_rhs_function_code,
					2, TRUE, FALSE, 0);

	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "sin"),
			sin_rhs_function_code,
			1,
			TRUE,
			FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "cos"),
			cos_rhs_function_code,
			1,
			TRUE,
			FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "atan2"),
			atan2_rhs_function_code,
			2,
			TRUE,
			FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "sqrt"),
			sqrt_rhs_function_code,
			1,
			TRUE,
			FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "abs"),
			abs_rhs_function_code,
			1,
			TRUE,
			FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "int"),
			int_rhs_function_code,
			1,
			TRUE,
			FALSE, 0);
	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "float"),
			float_rhs_function_code,
			1,
			TRUE,
			FALSE, 0);

	/* voigtjr 6/12/2007: added these built in functions on laird's request
	these are straight out of the <8.6 kernel */
	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "round-off-heading"), 
		round_off_heading_air_rhs_function_code, 2, TRUE, FALSE, 0);

	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "round-off"), 
		round_off_air_rhs_function_code, 2, TRUE, FALSE, 0);

	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "compute-heading"), 
		compute_heading_rhs_function_code, 4, TRUE, FALSE, 0);

	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "compute-range"), 
		compute_range_rhs_function_code, 4, TRUE, FALSE, 0);

	// NLD: 11/11 (ditto voigtjr's motivation above)
	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "compute-dice-probability"),
		dice_prob_rhs_function_code, 4, TRUE, FALSE, 0);

	// Bug 800: implement rhs rand functions
	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "rand-int"), 
		rand_int_rhs_function_code, -1, TRUE, FALSE, 0);

	add_rhs_function (thisAgent, make_sym_constant(thisAgent, "rand-float"), 
		rand_float_rhs_function_code, -1, TRUE, FALSE, 0);

}

void remove_built_in_rhs_math_functions (agent* thisAgent)
{
	// DJP-FREE: These used to call make_sym_constant, but the symbols must already exist and if we call make here again we leak a reference.
	remove_rhs_function (thisAgent, find_sym_constant (thisAgent, "+"));
	remove_rhs_function (thisAgent, find_sym_constant (thisAgent, "*"));
	remove_rhs_function (thisAgent, find_sym_constant (thisAgent, "-"));
	remove_rhs_function (thisAgent, find_sym_constant (thisAgent, "/"));
	remove_rhs_function (thisAgent, find_sym_constant (thisAgent, "div"));
	remove_rhs_function (thisAgent, find_sym_constant (thisAgent, "mod"));
	remove_rhs_function (thisAgent, find_sym_constant(thisAgent, "sin"));
	remove_rhs_function (thisAgent, find_sym_constant(thisAgent, "cos"));
	remove_rhs_function (thisAgent, find_sym_constant(thisAgent, "atan2"));
	remove_rhs_function (thisAgent, find_sym_constant(thisAgent, "sqrt"));
	remove_rhs_function (thisAgent, find_sym_constant(thisAgent, "abs"));
	remove_rhs_function (thisAgent, find_sym_constant(thisAgent, "int"));
	remove_rhs_function (thisAgent, find_sym_constant(thisAgent, "float"));

	/* voigtjr 6/12/2007: added these built in functions on laird's request
	these are straight out of the <8.6 kernel */
	remove_rhs_function(thisAgent, find_sym_constant(thisAgent, "round-off-heading"));
	remove_rhs_function(thisAgent, find_sym_constant(thisAgent, "round-off"));
	remove_rhs_function(thisAgent, find_sym_constant(thisAgent, "compute-heading"));
	remove_rhs_function(thisAgent, find_sym_constant(thisAgent, "compute-range"));

	// NLD: 11/11
	remove_rhs_function(thisAgent, find_sym_constant(thisAgent, "compute-dice-probability"));

	// Bug 800: implement rand
	remove_rhs_function(thisAgent, find_sym_constant(thisAgent, "rand-int"));
	remove_rhs_function(thisAgent, find_sym_constant(thisAgent, "rand-float"));
}
