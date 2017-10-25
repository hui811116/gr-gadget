/* -*- c++ -*- */

#define GADGET_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "gadget_swig_doc.i"

%{
#include "gadget/payload_sink.h"
%}


%include "gadget/payload_sink.h"
GR_SWIG_BLOCK_MAGIC2(gadget, payload_sink);

