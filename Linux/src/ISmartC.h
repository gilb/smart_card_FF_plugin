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

#ifndef __gen_ISmartC_h__
#define __gen_ISmartC_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_nsIServiceManager_h__
#include "nsIServiceManager.h"
#endif

#ifndef __gen_nsIComponentRegistrar_h__
#include "nsIComponentRegistrar.h"
#endif

#ifndef __gen_nsIURI_h__
#include "nsIURI.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIURI; /* forward declaration */

class nsACString; /* forward declaration */

class nsAUTF8String; /* forward declaration */


/* starting interface:    ISmartC */
#define ISMARTC_IID_STR "a3f0812b-950a-406b-b76c-6264bccf2f1b"

#define ISMARTC_IID \
  {0xa3f0812b, 0x950a, 0x406b, \
    { 0xb7, 0x6c, 0x62, 0x64, 0xbc, 0xcf, 0x2f, 0x1b }}

class NS_NO_VTABLE NS_SCRIPTABLE ISmartC : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(ISMARTC_IID)

  /* void SendAPDU (in PRUint32 ct, [array, size_is (ct)] in PRInt32 valueArray, out unsigned long count, [array, size_is (count), retval] out long retv); */
  NS_SCRIPTABLE NS_IMETHOD SendAPDU(PRUint32 ct, PRInt32 *valueArray, PRUint32 *count NS_OUTPARAM, PRInt32 **retv NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(ISmartC, ISMARTC_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_ISMARTC \
  NS_SCRIPTABLE NS_IMETHOD SendAPDU(PRUint32 ct, PRInt32 *valueArray, PRUint32 *count NS_OUTPARAM, PRInt32 **retv NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_ISMARTC(_to) \
  NS_SCRIPTABLE NS_IMETHOD SendAPDU(PRUint32 ct, PRInt32 *valueArray, PRUint32 *count NS_OUTPARAM, PRInt32 **retv NS_OUTPARAM) { return _to SendAPDU(ct, valueArray, count, retv); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_ISMARTC(_to) \
  NS_SCRIPTABLE NS_IMETHOD SendAPDU(PRUint32 ct, PRInt32 *valueArray, PRUint32 *count NS_OUTPARAM, PRInt32 **retv NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->SendAPDU(ct, valueArray, count, retv); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public ISmartC
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_ISMARTC

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, ISmartC)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* void SendAPDU (in PRUint32 ct, [array, size_is (ct)] in PRInt32 valueArray, out unsigned long count, [array, size_is (count), retval] out long retv); */
NS_IMETHODIMP _MYCLASS_::SendAPDU(PRUint32 ct, PRInt32 *valueArray, PRUint32 *count NS_OUTPARAM, PRInt32 **retv NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_ISmartC_h__ */
