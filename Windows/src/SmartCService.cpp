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

#include "nsIGenericFactory.h"
#include "SmartC.h"

NS_GENERIC_FACTORY_CONSTRUCTOR(SmartC)

static nsModuleComponentInfo components[] =
{
    {
       SMARTC_CLASSNAME, 
       SMARTC_CID,
       SMARTC_CONTRACTID,
       SmartCConstructor,
    }
};

NS_IMPL_NSGETMODULE("SmartCService", components) 

