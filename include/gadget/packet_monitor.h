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


#ifndef INCLUDED_GADGET_PACKET_MONITOR_H
#define INCLUDED_GADGET_PACKET_MONITOR_H

#include <gadget/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace gadget {

    /*!
     * \brief <+description+>
     *
     */
    class GADGET_API packet_monitor : virtual public block
    {
    public:
      typedef boost::shared_ptr<packet_monitor> sptr;
      static sptr make();
      virtual int get_send() const =0;
      virtual int get_rx() const =0;
      virtual void call_reset(bool reset)=0;
    };

  } // namespace gadget
} // namespace gr

#endif /* INCLUDED_GADGET_PACKET_MONITOR_H */

