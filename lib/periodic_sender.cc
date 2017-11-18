/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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
#include <gadget/periodic_sender.h>
#include <gnuradio/block_detail.h>
#include <fstream>

namespace gr {
  namespace gadget {
  	#define SEQLEN 4
  	#define MAXLEN 123
    class periodic_sender_impl : public periodic_sender
    {
    public:
    	periodic_sender_impl(
    		float period_ms, 
    		const std::string& filename,
    		int nsend): block("periodic_sender",
    		gr::io_signature::make(0,0,0),
    		gr::io_signature::make(0,0,0)),
    		d_fb_in(pmt::mp("fb_in")),
    		d_pdu_out(pmt::mp("pdu_out"))
    	{
    		message_port_register_in(d_fb_in);
    		set_msg_handler(d_fb_in,boost::bind(&periodic_sender_impl::msg_in,this,_1));
    		message_port_register_out(d_pdu_out);
    		if(!read_data(filename)){
    			throw std::invalid_argument("File openning failure...abort");
    		}
    		d_file_size = d_data_src.size();
    		d_seqno = 0;
    		if(nsend<0){
    			throw std::invalid_argument("Sending number should be positive");
    		}else if(nsend==0){
    			d_idle = false;
    		}else{
    			d_idle = true;
    		}
    		d_nsend = nsend;
    		d_update = false;
    	}
    	~periodic_sender_impl(){}
    	void msg_in(pmt::pmt_t msg)
    	{
    		pmt::pmt_t k = pmt::car(msg);
    		pmt::pmt_t v = pmt::cdr(msg);
    	}
    	void reset(bool trigger)
    	{
    		if(trigger && !d_update){
    			d_update = true;
    			d_send_all.notify_one();
    		}
    	}
    	bool start()
    	{
    		d_finished = false;
    		d_thread = boost::shared_ptr<gr::thread::thread>
    			(new gr::thread::thread(&periodic_sender_impl::run,this));
    		return block::start();
    	}
    	bool stop()
    	{
    		d_finished = true;
    		d_thread->interrupt();
    		d_thread->join();
    		return block::stop();
    	}
    private:
    	void generate_pkt()
    	{
    		gr::thread::scoped_lock guard(d_mutex);
    		const uint8_t* u8_seq = (const uint8_t*) &d_seqno;
          	d_buf[0] = u8_seq[1];
          	d_buf[1] = u8_seq[0];
          	d_buf[2] = u8_seq[1];
          	d_buf[3] = u8_seq[0];
          	memcpy(d_buf+SEQLEN,d_data_src[d_seqno].data(),sizeof(char)*d_data_src[d_seqno].size());
          	pmt::pmt_t blob = pmt::make_blob(d_buf,SEQLEN+d_data_src[d_seqno].size());
          	d_cur_msg = pmt::cons(pmt::PMT_NIL,blob);
    	}
    	void run()
    	{
    		while(true){
    			if(d_idle){
    				gr::thread::scoped_lock lock(d_mutex);
    				d_send_all.wait(lock);
    				lock.unlock();
    			}else{
    				gr::thread::scoped_lock lock(d_mutex);
    				generate_pkt();
    				message_port_pub(d_pdu_out,d_cur_msg);
    				d_send_all.timed_wait(lock,boost::posix_time::milliseconds(d_period));
    				lock.unlock();	
    			}
    			//boost::this_thread::sleep(boost::posix_time::milliseconds(d_period));
    			if(d_finished){
    				return;
    			}else if(d_update){
    				d_seqno = 0;
    				d_send_cnt =0;
    				d_update = false;
    				d_idle = false;
    			}else{
    				d_seqno = (d_seqno==0xffff)? 0 : d_seqno+1;
    				d_seqno %= d_file_size;
    				d_send_cnt++;
    				if(d_send_cnt==d_nsend){
    					d_send_cnt =0;
    					d_idle = (d_nsend==0)? false : true;
    				}
    			}
    			
    		}
    	}
    	bool read_data(const std::string& filename)
    	{
    		gr::thread::scoped_lock guard(d_mutex);
          	std::string str,line;
          	d_file.open(filename.c_str(),std::fstream::in);
          	if(!d_file.is_open()){
            	return false;
          	}
          	while(getline(d_file,line,'\n')){
            	std::istringstream temp(line);
            	std::vector<uint8_t> u8;
            	while(getline(temp,str,',')){
              	int tmp = std::atoi(str.c_str());
              	if(u8.size()>MAXLEN){
                	throw std::runtime_error("message to be transmitted exceed the maximum payload length");
              	}
              	u8.push_back((uint8_t)tmp);
            	}
            	d_data_src.push_back(u8);
          	}
          	d_file.close();
          	return true;
    	}
    	const pmt::pmt_t d_fb_in;
    	const pmt::pmt_t d_pdu_out;
    	pmt::pmt_t d_cur_msg;
    	gr::thread::mutex d_mutex;
    	gr::thread::condition_variable d_send_all;
    	uint16_t d_seqno;
    	float d_period;
    	std::vector<std::vector<uint8_t> > d_data_src;
    	boost::shared_ptr<gr::thread::thread> d_thread;
    	boost::posix_time::ptime d_sys_time;
    	bool d_finished;
    	bool d_update;
    	bool d_idle;
    	std::fstream d_file;
    	int d_file_size;
    	int d_nsend;
    	int d_send_cnt;
    	uint8_t d_buf[256];
    };

    periodic_sender::sptr
    periodic_sender::make(float period_ms, const std::string& filename, int nsend)
    {
    	return gnuradio::get_initial_sptr(new periodic_sender_impl(period_ms,filename,nsend));
    }

  } /* namespace gadget */
} /* namespace gr */

