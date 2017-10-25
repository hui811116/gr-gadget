INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_GADGET gadget)

FIND_PATH(
    GADGET_INCLUDE_DIRS
    NAMES gadget/api.h
    HINTS $ENV{GADGET_DIR}/include
        ${PC_GADGET_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GADGET_LIBRARIES
    NAMES gnuradio-gadget
    HINTS $ENV{GADGET_DIR}/lib
        ${PC_GADGET_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GADGET DEFAULT_MSG GADGET_LIBRARIES GADGET_INCLUDE_DIRS)
MARK_AS_ADVANCED(GADGET_LIBRARIES GADGET_INCLUDE_DIRS)

