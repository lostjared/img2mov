AC_PREREQ([2.69])
AC_INIT(img2mov, 1.0, lostjared@lostsidedead.com)
AC_CONFIG_HEADERS(config.h)
AC_CANONICAL_SYSTEM
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_INIT_AUTOMAKE

PKG_CHECK_MODULES([OPENCV], [opencv5],
    [AC_MSG_NOTICE([OpenCV 5 found])],
    [PKG_CHECK_MODULES([OPENCV], [opencv4],
        [AC_MSG_NOTICE([OpenCV 4 found])],
        [AC_MSG_ERROR([OpenCV 4 or 5 development files were not found])])])

CV_INCLUDES=$OPENCV_CFLAGS
CV_LIBS=$OPENCV_LIBS


AC_SUBST(CV_INCLUDES)
AC_SUBST(CV_LIBS)


AC_OUTPUT(Makefile src/Makefile)
