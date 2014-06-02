/*
 *  $Id: ccl_get.c,v 1.4 2004/04/15 17:48:34 sbooth Exp $
 *
 *  Copyright (C) 2004 Stephen F. Booth
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ccl.h"

const char* 
ccl_get(const struct ccl_t *data, 
	const char *key)
{
  const struct ccl_pair_t 	*pair;
  struct ccl_pair_t 		temp;

  if(data == 0 || key == 0)
    return 0;

  temp.key = (char*) key;
  temp.value = 0;

  pair = (const struct ccl_pair_t*) bst_find(data->table, &temp);
  
  return pair == 0 ? 0 : pair->value;
}
