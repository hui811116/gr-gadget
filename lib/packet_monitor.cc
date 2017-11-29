/* -*- c++ -*- */
/* 
 * Copyright 2017 Teng-Hui Huang.
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
#include <gnuradio/block_detail.h>
#include <gadget/packet_monitor.h>


namespace gr {
  namespace gadget {

    class packet_monitor_impl : public packet_monitor
    {
    public:
    	packet_monitor_impl() : block("packet_monitor",
    		gr::io_signature::make(0,0,0),
    		gr::io_signature::make(0,0,0)),
    		d_send_port(pmt::mp("send_in")),
    		d_rx_port(pmt::mp("rx_in"))
    	{
    		message_port_register_in(d_send_port);
    		message_port_register_in(d_rx_port);
    		set_msg_handler(d_send_port,boost::bind(&packet_monitor_impl::pkt_in,this,_1));
    		set_msg_handler(d_rx_port,boost::bind(&packet_monitor_impl::pkt_out,this,_1));
    		d_send_cnt=0;
    		d_rx_cnt=0;
    	}
    	~packet_monitor_impl()
    	{

    	}
    	void call_reset(bool reset)
    	{
    		gr::thread::scoped_lock guard(d_mutex);
    		if(reset){
    			d_send_cnt=0;
    			d_rx_cnt=0;	
    		}
    	}
    	void pkt_in(pmt::pmt_t msg)
    	{
    		gr::thread::scoped_lock guard(d_mutex);
    		pmt::pmt_t k = pmt::car(msg);
    		pmt::pmt_t v = pmt::cdr(msg);
    		if(pmt::is_blob(v)){
    			size_t io(0);
    			const uint8_t* uvec = pmt::u8vector_elements(v,io);
    			if(io>0){
    				d_send_cnt++;
    			}
    		}
    	}
    	void pkt_out(pmt::pmt_t msg)
    	{
    		gr::thread::scoped_lock guard(d_mutex);
    		pmt::pmt_t k = pmt::car(msg);
    		pmt::pmt_t v = pmt::cdr(msg);
    		if(pmt::is_blob(v)){
    			size_t io(0);
    			const uint8_t* uvec = pmt::u8vector_elements(v,io);
    			if(io>0){
    				d_rx_cnt++;
    			}
    		}
    	}
    	int get_send() const
    	{
    		return d_send_cnt;
    	}
    	int get_rx() const
    	{
    		return d_rx_cnt;
    	}
    	
    private:
    	const pmt::pmt_t d_send_port;
    	const pmt::pmt_t d_rx_port;
    	int d_send_cnt;
    	int d_rx_cnt;
    	gr::thread::mutex d_mutex;
    	bool d_finished;
    };

    packet_monitor::sptr
    packet_monitor::make()
    {
    	return gnuradio::get_initial_sptr(new packet_monitor_impl());
    }

  } /* namespace gadget */
} /* namespace gr */

