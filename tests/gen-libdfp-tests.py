#!/usr/bin/env python

import sys
from optparse import OptionParser

# Decribes the evaluated function
class Function:
  def __init__ (self):
    self.name = None
    self.args = []
    self.ret = None

  def ret_init_type (self):
    return self.ret.argtype

  def ret_type (self):
    return self.ret.type

  def ret_field (self):
    if self.ret.decfield:
      return "." + self.ret.decfield
    return ""

  def ret_printf (self):
    return self.ret.printf

  def arg_type (self, i):
    return self.args[i].argtype;


# Describes an operation to perform
class Operation:
  def __init__(self):
    self.args = []
    self.ret = None

class Type:
  def __init__(self, name, type, tname = "", suffix = "", decfield = "",
               maxvalue = "", minvalue = "", subnormal = "", maxexp = "",
               minexp = "", printf = "", argtype = ""):
    self.name = name
    self.type = type
    self.tname = tname
    self.suffix = suffix
    self.decfield = decfield
    self.maxvalue = maxvalue
    self.minvalue = minvalue
    self.subnormal = subnormal
    
    self.minexp = minexp
    self.maxexp = maxexp
    self.printf = printf
    if not argtype:
      self.argtype = type
    else:
      self.argtype = argtype

class BoolType(Type):
  def parse_arg (self, arg):
    if arg == "True":
      return 1
    elif arg == "False": 
      return 0
    return arg;

class DecimalType(Type):
  SPECIAL_ARGS = {
    "Inf"     : "plus_infty",
    "-Inf"    : "minus_infty",
    "sNaN"    : "snan_value",
    "-sNaN"   : "snan_value",
    "NaN"     : "qnan_value",
    "-NaN"    : "qnan_value"
  }

  def parse_arg (self, arg):
    # sNAN value
    if arg == "sNaN":
      return "{ .ieee_nan = {.c = 0x1f,.signaling_nan = 1 } }"
    # Infinity or NaN value
    if arg in self.SPECIAL_ARGS:
      return "{ .%s = %s }" % (DECIMAL.decfield, self.SPECIAL_ARGS[arg])
    # Replace <number>E[+-]DEC_[MAX|MIN]_EXP by a expected number, i.e,
    # 1E-DEC_MIN_EXP -> 1E-383
    if "DEC_MAX_EXP" in arg:
       ret = arg.replace ("DEC_MAX_EXP", DECIMAL.maxexp)
       if '.' not in arg:
         if 'E' not in arg:
           ret += DECIMAL.suffix
       return "{ .%s = %s }" % (DECIMAL.decfield, ret)
    if "DEC_MIN_EXP" in arg:
       ret = arg.replace ("DEC_MIN_EXP", DECIMAL.minexp)
       if '.' not in arg:
         if 'E' not in arg:
           ret += DECIMAL.suffix
       return "{ .%s = %s }" % (DECIMAL.decfield, ret)
    # Macro fox max, min, tiny values
    if "DEC_MAX" in arg:
       ret = arg.replace ("DEC_MAX", DECIMAL.maxvalue);
       return "{ .%s = %s }" % (DECIMAL.decfield, ret)
    if "DEC_MIN" in arg:
       ret = arg.replace ("DEC_MIN", DECIMAL.minvalue);
       return "{ .%s = %s }" % (DECIMAL.decfield, ret)
    if "DEC_SUBNORMAL_MIN" in arg:
       ret = arg.replace ("DEC_SUBNORMAL_MIN", DECIMAL.subnormal);
       return "{ .%s = %s }" % (DECIMAL.decfield, ret)
    # Normal value
    if '.' not in arg:
      if 'E' not in arg:
        return "{ .%s = %s }" % (DECIMAL.decfield, arg)
    ret = arg + DECIMAL.suffix
    return "{ .%s = %s }" % (DECIMAL.decfield, ret)


DecimalTypes = {
  "bool"      : BoolType ("bool", "_Bool", printf = "%d"),

  "decimal32" : DecimalType ("decimal32",
                             "_Decimal32",
                             "32",
                             "DF",
                             "sd",
                             "DEC32_MAX",
                             "DEC32_MIN",
                             "DEC32_SUBNORMAL_MIN",
                             "97",
                             "94",
                             "%.7HgDF",
                             "union ieee754r_Decimal32"),

  "decimal64" : DecimalType ("decimal64",
                             "_Decimal64",
                             "64",
                             "DD",
                             "dd",
                             "DEC64_MAX",
                             "DEC64_MIN",
                             "DEC64_SUBNORMAL_MIN",
                             "385",
                             "382",
                             "%.16DgDD",
                             "union ieee754r_Decimal64"),

  "decimal128" : DecimalType ("decimal128",
                              "_Decimal128",
                              "128",
                              "DL",
                              "td",
                              "DEC128_MAX",
                              "DEC128_MIN",
                              "DEC128_SUBNORMAL_MIN",
                              "6145",
                              "6142",
                              "%.34DDgDL",
                              "union ieee754r_Decimal128")
}

DECIMAL = None


def parse_file (filename):
  try:
    file = open (filename, "r")
    lines = file.readlines()
    i = 0;

    # Parse the header with function description
    func = Function()

    for l in range (i, len(lines)):
      if not lines[l].startswith ("#"):
        break

      fields = lines[l].split()
      if fields[1].startswith("name"):
        func.name = fields[2]
      if fields[1].startswith("arg"):
        func.args.append(DecimalTypes[fields[2] + DECIMAL.tname])
      elif fields[1].startswith("ret"):
        rettname = fields[2]
        if rettname == "decimal":
          rettname += DECIMAL.tname
        func.ret = DecimalTypes[rettname]
      i = i + 1

    # Parse the inputs and expected result
    operations = []
    expected_args = len (func.args)
    for l in range(i, len(lines)):
      if not lines[l].strip():
        continue
      
      fields = lines[l].split()
      # Check if number of arguments is the expected one
      if len(fields) - 1 is not expected_args:
        print ("warning: %s:%i: line %s not follow specified function" % \
          (filename, l, lines[l]))
        continue

      op = Operation()
      for oparg in range (0, expected_args):
        op.args.append(fields[oparg])
      op.ret = fields[len(fields)-1]

      operations.append(op)

    return (func, operations)

  except (IOError, OSError) as e:
    print ("error: open (%s) failed: %s\n" % (filename, e.strerror))
    exit (1)


def print_header ():
  print ("#ifndef __STDC_WANT_DEC_FP__")
  print ("# define __STDC_WANT_DEC_FP__")
  print ("#endif")
  print ("")
  print ("#include <stdio.h>")
  print ("#include <math.h>")
  print ("#include <float.h>")
  print ("#include <endian.h>")
  print ("#include \"test-common.h\"")
  print ("")
  print ("#define _WANT_VC 1")
  print ("#include \"scaffold.c\"")

def print_special_ctes ():
  print ("#ifndef DEC_INFINITY")
  print ("# define DEC_INFINITY __builtin_infd32()")
  print ("#endif");
  print ("#ifndef DEC_NAN")
  print ("# define DEC_NAN      __builtin_nand32(\"\")")
  print ("#endif");
  print ("#define   plus_infty     DEC_INFINITY")
  print ("#define   minus_infty    -DEC_INFINITY")
  print ("#define   qnan_value     DEC_NAN")
  print ("#define   snan_value     DEC_NAN")
  print ("")

def print_operations (func, operations):
  print ("typedef struct {")
  for i in range(0, len(func.args)):
    print ("  %s arg%i;" % (func.arg_type(i), i))
  print ("  %s e;" % func.ret_init_type())
  print ("} operations_t;")
  print ("")
  print ("static const operations_t operations[] = {")
  for op in operations:
    print ("  {"),
    for i in range(0, len(op.args)):
      print ("%s," % func.args[i].parse_arg(op.args[i])),
    print (" %s }," % func.ret.parse_arg(op.ret))
  print ("};")
  print ("static const int operations_size = \
    sizeof(operations)/sizeof(operations[0]);");
  print ("")


def print_func_call(func):
  print ("int main (void) {")
  print ("  int i;")
  print ("  for (i = 0; i < operations_size; ++i) {")

  # ret = function (arg1, ...)
  print ("    %s ret = %sd%s (" % (func.ret_type(), func.name, DECIMAL.tname)),
  for i in range(0, len(func.args)):
    print ("operations[i].arg%i.%s" % (i, DECIMAL.decfield)),
    if i is not len(func.args)-1:
      print (","),
  print (");")

  # printf ("function (arg1, ...)")
  print ("    printf (\"%sd%s (" % (func.name, DECIMAL.tname)),
  for i in range(0, len(func.args)):
    print ("%s" % (DECIMAL.printf)),
    if i is not len(func.args)-1:
      print (","),
  print (") = %s\", " % (func.ret_printf())),
  for i in range(0, len(func.args)):
    print ("operations[i].arg%i.%s" % (i, DECIMAL.decfield)),
    if i is not len(func.args)-1:
      print (","),
  print (", ret);")

  # _VC_P(f,l,x,y,fmt)
  print ("    _VC_P (__FILE__, __LINE__, operations[i].e%s, ret, \"%s\");" % \
          (func.ret_field(), func.ret_printf()));

  print ("  }")
  print ("")
  print ("  _REPORT ();")
  print ("")
  print ("  return fail;")
  print ("}")
    

def print_output (filename):
  (func, operations) = parse_file (filename)
  
  print_header ()
  print_special_ctes ()
  print_operations (func, operations)
  print_func_call (func)


if __name__ == "__main__":
  parser = OptionParser ()
  parser.add_option ("-f", "--file", dest="filename",
                     help="white output to FILE")
  parser.add_option ("-t", "--type", dest="dectype",
                     help="DECIMAL type to use")
  (options, args) = parser.parse_args()

  if options.filename:
    sys.stdout = open (options.filename, "w")

  if not args:
    print ("usage: %s <options> <input file>" % argv[0])
    exit (0)
  if not options.dectype:
    print ("error: you must specify a DECIMAL type: decimal[32|64|128]")
    exit (0)

  DECIMAL = DecimalTypes[options.dectype]

  print_output (args[0])
