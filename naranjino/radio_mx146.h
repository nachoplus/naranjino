/* naranjino copyright (C) 2012  Nacho mas
 * based on trackuino. copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __RADIO_MX146_H__
#define __RADIO_MX146_H__

#include "radio.h"

class RadioMx146 : public Radio {
  public:
    virtual void setup();
    virtual void ptt_on();
    virtual void ptt_off();
  private:
    void send_cmd(const char *cmd, int cmd_len, char *res, int res_len);
    void set_freq(unsigned long freq);
    int query_temp();
};

#endif
