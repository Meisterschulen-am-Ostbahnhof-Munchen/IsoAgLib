/*
  vtobjectmacro_c.h

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTMACRO_C_H
#define VTOBJECTMACRO_C_H

#include "vtobject_c.h"


namespace __IsoAgLib {

class vtObjectMacro_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectMacro_s;

	// Pointer to the internal implementation
	iVtObjectMacro_s* vtObject_a;
	//TODO
	//std::unique_ptr<iVtObjectMacro_s> vtObject_a;

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  vtObjectMacro_c(iVtObjectMacro_s* vtObjectMacroSROM , int ai_multitonInst);

  iVtObjectMacro_s* get_vtObjectMacro_a();

  vtObjectMacro_c();

  uint32_t fitTerminal() const;

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 28; }
   */

  void saveReceivedAttribute (uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/);;
#endif
};

} // __IsoAgLib

#endif
