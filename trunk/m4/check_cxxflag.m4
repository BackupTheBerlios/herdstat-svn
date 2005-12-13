AC_DEFUN([LHS_CHECK_CXXFLAG], [
	  lhs_save_CXXFLAGS=$CXXFLAGS
	  CXXFLAGS="$CXXFLAGS $1"
	  AC_MSG_CHECKING([if $CXX supports the $1 flag])
	  AC_COMPILE_IFELSE([
#include <string>
std::string s;
	],
	[lhs_cxxflag_success=yes],
	[lhs_cxxflag_success=no])
	AC_MSG_RESULT($lhs_cxxflag_success)
	CXXFLAGS="$lhs_save_CXXFLAGS"
	if test "x$lhs_cxxflag_success" = "xyes" ; then
		CXXFLAGS="$CXXFLAGS $1"
	fi
	])

