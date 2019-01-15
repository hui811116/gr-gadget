/* -*- c++ -*- */

#define GADGET_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "gadget_swig_doc.i"

%{
#include "gadget/payload_sink.h"
#include "gadget/burst_gate_cc.h"
#include "gadget/periodic_sender.h"
#include "gadget/packet_monitor.h"
#include "gadget/burst_tagger_cc.h"
#include "gadget/null_msg_sink.h"
%}


%include "gadget/payload_sink.h"
GR_SWIG_BLOCK_MAGIC2(gadget, payload_sink);
%include "gadget/burst_gate_cc.h"
GR_SWIG_BLOCK_MAGIC2(gadget, burst_gate_cc);
%include "gadget/periodic_sender.h"
GR_SWIG_BLOCK_MAGIC2(gadget, periodic_sender);
%include "gadget/packet_monitor.h"
GR_SWIG_BLOCK_MAGIC2(gadget, packet_monitor);
%include "gadget/burst_tagger_cc.h"
GR_SWIG_BLOCK_MAGIC2(gadget, burst_tagger_cc);
%include "gadget/null_msg_sink.h"
GR_SWIG_BLOCK_MAGIC2(gadget, null_msg_sink);
