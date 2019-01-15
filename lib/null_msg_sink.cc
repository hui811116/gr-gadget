/* -*- c++ -*- */
/* 
 * Copyright 2019 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gadget/null_msg_sink.h>
#include <gnuradio/block_detail.h>

namespace gr {
  namespace gadget {

    class null_msg_sink_impl : public null_msg_sink
    {
    public:
    	null_msg_sink_impl() : block("null_msg_sink",
    		gr::io_signature::make(0,0,0),
    		gr::io_signature::make(0,0,0)),
    		d_null_port(pmt::mp("in"))
    	{
    		message_port_register_in(d_null_port);
    		set_msg_handler(d_null_port,boost::bind(&null_msg_sink_impl::msg_in,this,_1));
    	}
    	~null_msg_sink_impl()
    	{

    	}
    	void msg_in(pmt::pmt_t msg)
    	{
    		return;
    	}
    private:
    	const pmt::pmt_t d_null_port;
    };

    null_msg_sink::sptr
    null_msg_sink::make()
    {
    	return gnuradio::get_initial_sptr(new null_msg_sink_impl());
    }

  } /* namespace gadget */
} /* namespace gr */

