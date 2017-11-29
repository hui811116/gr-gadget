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
#include <gadget/payload_sink.h>
#include <gnuradio/block_detail.h>
#include <fstream>
#include <iomanip>

namespace gr {
  namespace gadget {

    class payload_sink_impl : public payload_sink
    {
    public:
    	enum SYSTYPE{
    			LSA=0,
    			SNS=1,
    			PROU=2,
    			PWR=3
    	};
    	payload_sink_impl(const std::string& filename, int sys, bool append):block("payload_sink",
    		gr::io_signature::make(0,0,0),
    		gr::io_signature::make(0,0,0)),
    		d_pld_port(pmt::mp("msg_in"))
    	{
    		d_file = NULL;
        d_append = append;
        message_port_register_in(d_pld_port);
        set_msg_handler(d_pld_port,boost::bind(&payload_sink_impl::msg_in,this,_1));
        update_file(filename);
        switch(sys){
         	case LSA:
         	case SNS:
         	case PROU:
         	case PWR:
           		d_sys = sys;
         	break;
         	default:
           		throw std::runtime_error("Undefined system");
         	break;
        }
        d_acc_pkt =0;
        d_start_time = boost::posix_time::second_clock::local_time();
    	}
    	~payload_sink_impl(){
    		if(d_file->is_open()){
            	d_file->close();
        	}
          	delete d_file;
    	}
    	void update_file(const std::string& filename)
      {
        gr::thread::scoped_lock guard(d_mutex);
        if(d_file!=NULL && d_file->is_open()){
          d_file->close();
          delete d_file;
        }
        if(d_append){
          d_file = new std::fstream(filename.c_str(),std::fstream::out|std::fstream::app);
        }else{
          d_file = new std::fstream(filename.c_str(),std::fstream::out|std::fstream::trunc);
        }
        if(!d_file->is_open()){
          throw std::runtime_error("<FILE SINK>cannot open file...file exist or cannot be opened");
        }
        // record the time when file started!
        boost::posix_time::ptime current_time = boost::posix_time::second_clock::local_time();
        *d_file <<"\n[Event]\n<Time>\n"<< boost::posix_time::to_iso_string(current_time)
        <<"<Bytes>\n"<<std::dec<<0<<"\n<Packets>\n"<<0<<"\n[Event*]"<<std::endl;
      }
      int acc_packets() const
      {
        int acc = (int) d_acc_pkt;
        return acc;
      }
      
    private:
    	  void msg_in(pmt::pmt_t msg)
        {
          gr::thread::scoped_lock guard(d_mutex);
          // seq number can be stored here
          pmt::pmt_t k = pmt::car(msg); // pwr and seqno is hide in this key
          pmt::pmt_t v = pmt::cdr(msg);
          if(!pmt::is_blob(v) || !pmt::is_dict(k)){
            return;
          }
          uint16_t seq=0;
          float pwr=0;
          seq = pmt::to_long(pmt::dict_ref(k,pmt::intern("seqno"),pmt::from_long(0)));
          pwr = pmt::to_float(pmt::dict_ref(k,pmt::intern("pwr"),pmt::from_float(0)));
          size_t io(0);
          const uint8_t* uvec = pmt::u8vector_elements(v,io);
          switch(d_sys){
            case LSA:
                *d_file << "[LSA]"<<std::endl;
            break;
            case SNS:
                *d_file << "[SNS]"<<std::endl;
            break;
            case PROU:
                *d_file << "[PROU]"<<std::endl;
            break;
            case PWR:
            	*d_file << "[PWR]"<<std::endl;
            break;
            default:
              throw std::runtime_error("Undefined system");
            break;
          }
          *d_file<<"<seq>\n"<<std::dec<<seq<<std::endl
          <<"<pwr>\n"<<std::dec<<std::fixed<<std::setprecision(2)<<pwr<<std::endl
          <<"<size>\n"<<std::dec<<io<<std::endl
          <<"[Hex]"<<std::endl;
          int i=0;
          for(i=0;i<io;++i){
            *d_file<<std::setfill('0')<<std::setw(2)<<std::hex<<(int)uvec[i];
            if(i!=io-1){
              *d_file<<",";
            }
          }
          *d_file<<"\n[Hex*]"<<std::endl;
          switch(d_sys){
            case LSA:
                *d_file << "[LSA*]"<<std::endl;
            break;
            case SNS:
                *d_file << "[SNS*]"<<std::endl;
            break;
            case PROU:
                *d_file << "[PROU*]"<<std::endl;
            break;
            case PWR:
            	*d_file << "[PWR]"<<std::endl;
            break;
            default:
              throw std::runtime_error("Undefined system");
            break;
          }
          d_acc_pkt++;
        }
    	const pmt::pmt_t d_pld_port;
    	gr::thread::mutex d_mutex;
    	std::fstream* d_file;
    	boost::posix_time::ptime d_start_time;
    	int d_sys;
      unsigned int d_acc_pkt;
      bool d_append;
    };

    payload_sink::sptr
    payload_sink::make(const std::string& filename, int sys, bool append)
    {
    	return gnuradio::get_initial_sptr(new payload_sink_impl(filename,sys,append));
    }

  } /* namespace gadget */
} /* namespace gr */

