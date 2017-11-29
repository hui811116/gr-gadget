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
#include "burst_gate_cc_impl.h"

namespace gr {
  namespace gadget {

    burst_gate_cc::sptr
    burst_gate_cc::make(const std::string& tagname, int mult)
    {
      return gnuradio::get_initial_sptr
        (new burst_gate_cc_impl(tagname, mult));
    }

    /*
     * The private constructor
     */
    burst_gate_cc_impl::burst_gate_cc_impl(const std::string& tagname, int mult)
      : gr::block("burst_gate_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
              d_sob(pmt::intern("tx_sob")),
              d_eob(pmt::intern("tx_eob")),
              d_name(pmt::intern(alias()))
    {
      if(mult<=0){
        throw std::invalid_argument("Multiplier should be positive");
      }
      d_mult = mult;
      if(tagname.empty()){
        throw std::invalid_argument("tag name should be specified");
      }
      d_tag = pmt::intern(tagname);
      d_state = false;
    }

    /*
     * Our virtual destructor.
     */
    burst_gate_cc_impl::~burst_gate_cc_impl()
    {
    }
    void
    burst_gate_cc_impl::add_sob(int offset)
    {
      if(!d_state){
        d_state = true;
        add_item_tag(0,nitems_written(0)+offset,d_sob,pmt::PMT_T,d_name);
      }
    }
    void
    burst_gate_cc_impl::add_eob(int offset)
    {
      add_item_tag(0,nitems_written(0)+offset-1,d_eob,pmt::PMT_T,d_name);
      d_state = false;
    }
    void
    burst_gate_cc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    burst_gate_cc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      int samps = std::min(ninput_items[0],noutput_items);
      std::vector<tag_t> tags;
      if(d_count==0){
        get_tags_in_window(tags,0,0,samps,d_tag);
        if(!tags.empty()){
          int offset = tags[0].offset- nitems_read(0);
          if(offset == 0){
            add_sob(0);
            d_count = pmt::to_long(tags[0].value) * d_mult;
          }else{
            consume_each(offset);
            return 0;
          }
        }  
      }
      if(d_count){
        int nout = std::min(d_count,samps);
        memcpy(out,in,sizeof(gr_complex)*nout);
        d_count -= nout;
        if(d_count ==0){
          if(nout == ninput_items[0]){
            add_eob(nout);
          }
        }
        consume_each(nout);
        return nout;
      }else{
        consume_each (samps);
        return 0;  
      }
    }

  } /* namespace gadget */
} /* namespace gr */

