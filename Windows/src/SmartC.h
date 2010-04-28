/*
* SmartC : Firefox plugin for smart cards.
* Copyright (C) 2010  Gilles Bernabe.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SMARTC_H_
#define _SMARTC_H_

#include "ISMartC.h"

#include <stdio.h>
#include <tchar.h>
#include <winscard.h>
#include <string>
using namespace std;

#pragma comment(lib, "winscard.lib")

#define _SMARTCARD_ SmartC
#define SMARTC_CONTRACTID "@SmartCAuth.com/SmartCPlugin/SmartC;1"
#define SMARTC_CLASSNAME "SmartC Firefox Plugin"
#define SMARTC_CID  { 0xb49e9510, 0x5f9d, 0x4ea4, { 0xa6, 0x0e, 0x71, 0xca, 0x51, 0x9d, 0xe1, 0x0b } }

/* Header file */
class _SMARTCARD_ : public ISmartC
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_ISMARTC

  _SMARTCARD_();

private:
  ~_SMARTCARD_();

protected:
  /* additional members */
};

#endif //_SMARTC_H_
