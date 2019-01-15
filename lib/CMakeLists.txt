# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND gadget_sources
    payload_sink.cc
    burst_gate_cc_impl.cc
    periodic_sender.cc
    packet_monitor.cc
    burst_tagger_cc_impl.cc
    null_msg_sink.cc )

set(gadget_sources "${gadget_sources}" PARENT_SCOPE)
if(NOT gadget_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT gadget_sources)

add_library(gnuradio-gadget SHARED ${gadget_sources})
target_link_libraries(gnuradio-gadget ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-gadget PROPERTIES DEFINE_SYMBOL "gnuradio_gadget_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-gadget PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-gadget RUNTIME_COMPONENT "gadget_runtime" DEVEL_COMPONENT "gadget_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_gadget_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_gadget.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gadget.cc
)

add_executable(test-gadget ${test_gadget_sources})

target_link_libraries(
  test-gadget
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-gadget
)

GR_ADD_TEST(test_gadget test-gadget)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

