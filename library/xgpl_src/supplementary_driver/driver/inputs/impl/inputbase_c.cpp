/*
  inputbase_c.cpp - implementation file for InputBase_c

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "inputbase_c.h"
#include "inputs_c.h"


namespace __IsoAgLib {

InputBase_c::InputBase_c( uint8_t aui8_channelNr, IsoAgLib::iInput_c::inputType_t ren_inputType )
  : ui8_channelNr( aui8_channelNr )
  , en_inputType( ren_inputType )
{ 
}


void
InputBase_c::init( uint8_t aui8_channelNr, IsoAgLib::iInput_c::inputType_t ren_inputType )
{
  ui8_channelNr = aui8_channelNr;
  en_inputType = ren_inputType;
}


} // __IsoAgLib
