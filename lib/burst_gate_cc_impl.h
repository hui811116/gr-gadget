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

#ifndef INCLUDED_GADGET_BURST_GATE_CC_IMPL_H
#define INCLUDED_GADGET_BURST_GATE_CC_IMPL_H

#include <gadget/burst_gate_cc.h>

namespace gr {
  namespace gadget {

    class burst_gate_cc_impl : public burst_gate_cc
    {
     private:
      int d_mult;
      pmt::pmt_t d_tag;
      int d_count;
      const pmt::pmt_t d_sob;
      const pmt::pmt_t d_eob;
      const pmt::pmt_t d_name;
      void add_sob(int offset);
      void add_eob(int offset);

     public:
      burst_gate_cc_impl(const std::string& tagname, int mult);
      ~burst_gate_cc_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace gadget
} // namespace gr

#endif /* INCLUDED_GADGET_BURST_GATE_CC_IMPL_H */

