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
#include "burst_tagger_cc_impl.h"

namespace gr {
  namespace gadget {

    burst_tagger_cc::sptr
    burst_tagger_cc::make(const std::string& tagname,int mult)
    {
      return gnuradio::get_initial_sptr
        (new burst_tagger_cc_impl(tagname,mult));
    }

    /*
     * The private constructor
     */
    burst_tagger_cc_impl::burst_tagger_cc_impl(const std::string& tagname,int mult)
      : gr::block("burst_tagger_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
              d_mult(mult),
              d_tagname(pmt::intern(tagname)),
              d_name(pmt::intern(alias())),
              d_sob(pmt::intern("tx_sob")),
              d_eob(pmt::intern("tx_eob"))
    {
      d_count =0;
      if(mult<=0 || tagname.empty()){
        throw std::invalid_argument("invalid multiplier, abort");
      }
      set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    burst_tagger_cc_impl::~burst_tagger_cc_impl()
    {
    }

    void
    burst_tagger_cc_impl::add_sob()
    {
      add_item_tag(0,nitems_written(0),d_sob,pmt::PMT_T,d_name);
    }

    void
    burst_tagger_cc_impl::add_eob(int offset)
    {
      add_item_tag(0,nitems_written(0)+offset-1,d_eob,pmt::PMT_T,d_name);
    }

    void
    burst_tagger_cc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      ninput_items_required[0] = noutput_items;
    }

    int
    burst_tagger_cc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      std::vector<tag_t> tags;
      int nin = std::min(ninput_items[0],noutput_items);
      int ncon =0;
      if(d_count ==0){
        get_tags_in_window(tags,0,0,nin,d_tagname);
        if(!tags.empty()){
          int offset = tags[0].offset - nitems_read(0);
          if(offset == 0){
            add_sob();
            d_count = pmt::to_long(tags[0].value) * d_mult;
          }else{
            consume_each(offset);
            return 0;
          }
        }
      }
      if(d_count){
        ncon = std::min(d_count,nin);
        d_count -= ncon;
        if(d_count==0){
          add_eob(ncon);
        }
        memcpy(out,in,sizeof(gr_complex)*ncon);
        consume_each(ncon);
        return ncon;
      }else{
        consume_each (nin);
        return 0;  
      }
    }

  } /* namespace gadget */
} /* namespace gr */

