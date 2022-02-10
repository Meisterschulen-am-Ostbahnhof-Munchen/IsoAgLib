/*
  vtobjectstringvariable_c.cpp

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

#include "../ivtobjectstringvariable_c.h"
#include "vtobjectstringvariable_c.h"

#ifdef CONFIG_USE_VTOBJECT_stringvariable

#include "vtclient_c.h"



namespace __IsoAgLib {



struct vtObjectStringVariable_c::iVtObjectStringVariable_s : iVtObject_s {
    uint16_t length;
    char* value;
    iVtObjectStringVariable_s(
    		IsoAgLib::ObjectID ID,
  		    uint16_t length,
			char *value)
    : iVtObject_s(ID)
    , length(length)
    , value(value)
    {}
};


int16_t
vtObjectStringVariable_c::stream(uint8_t* destMemory,
                                 uint16_t maxBytes,
                                 objRange_t sourceOffset)
{
  uint16_t curBytes=0; /* current bytes written */;

  if (sourceOffset == 0) { // dump out constant sized stuff
    destMemory [0] = vtObject_a->ID & 0xFF;
    destMemory [1] = vtObject_a->ID >> 8;
    destMemory [2] = 22; // Object Type = Integer Variable
    destMemory [3] = vtObject_a->length & 0xFF;
    destMemory [4] = vtObject_a->length >> 8;
    curBytes += 5;
    sourceOffset += 5;
  }

  while ((sourceOffset >= 5U) && (sourceOffset < (5U+vtObject_a->length)) && ((curBytes+1) <= maxBytes)) {
    destMemory [curBytes] = vtObject_a->value [sourceOffset-5];
    curBytes++;
    sourceOffset++;
  }

  return curBytes;
}


vtObjectStringVariable_c::~vtObjectStringVariable_c() = default;

IsoAgLib::ObjectID vtObjectStringVariable_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}

uint32_t
vtObjectStringVariable_c::fitTerminal() const
{
  return 5 + vtObject_a->length;
}

#ifdef USE_VT_UNICODE_SUPPORT
// Operation : setValueCopyUTF8
//! @param newValue:
//! @param b_updateObject:
void
vtObjectStringVariable_c::setValueCopyUTF8 (const char* newValue, uint8_t aui8_fontType, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  const uint16_t cui16_strLen = (uint16_t)CNAMESPACE::strlen (newValue);
  char* pc_iso8859 = new char [cui16_strLen+1];

  convertStringUnicodeTo8859 (newValue, cui16_strLen, pc_iso8859, aui8_fontType);

  setValueCopy (pc_iso8859, b_updateObject, b_enableReplaceOfCmd);
  delete[] pc_iso8859;
}


void
vtObjectStringVariable_c::setValueCopyUTF16 (const char* newValue, uint16_t length, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  int copyLen = ( length <= vtObject_a->length) ? length : vtObject_a->length;

  // UTF-16 string must be a multiple of 2 bytes long.
  if( copyLen % 2 )
   copyLen--;

  if (b_updateObject) {
    // check if not already RAM string buffer?
    if (!(s_properties.flags & FLAG_STRING_IN_RAM)) {
      s_properties.flags |= FLAG_STRING_IN_RAM;
      // create new String buffer with same length as original one, as the size can't be changed !!
      char *newStringBuffer = new char [vtObject_a->length+1];
      saveValueP (MACRO_getStructOffset(get_vtObjectStringVariable_a(), value), sizeof(iVtObjectStringVariable_s), (IsoAgLib::iVtObject_c*) newStringBuffer);
    }
    char *dest = vtObject_a->value;
    const char *src = newValue;

    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    spacePadBomUTF16( dest, copyLen, vtObject_a->length );
  }

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValueUTF16 (getID(), newValue, copyLen, b_enableReplaceOfCmd);

}
#endif


void
vtObjectStringVariable_c::setValueCopy(char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    // check if not already RAM string buffer?
    char *dest = vtObject_a->value;
    const char *src = newValue;
    int copyLen = (CNAMESPACE::strlen (newValue) <= vtObject_a->length) ? CNAMESPACE::strlen (newValue) : vtObject_a->length;
    int i=0; for (; i<copyLen; i++) *dest++ = *src++;
    for (; i<vtObject_a->length; i++) *dest++ = ' ';
    *dest = 0x00; // 0-termiante!
  }

  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValue (this, newValue, vtObject_a->length, b_enableReplaceOfCmd);
}


void
vtObjectStringVariable_c::setValueRef(char* newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    // delete RAM_String first, before we lose the pointer!
      delete (vtObject_a->value);

    vtObject_a->value = newValue;
  }

  uint16_t ui16_tempLen = 0;
  if (newValue != NULL ) ui16_tempLen = uint16_t( (CNAMESPACE::strlen (newValue) <= vtObject_a->length) ? CNAMESPACE::strlen (newValue) : vtObject_a->length );
  __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).commandHandler().sendCommandChangeStringValueRef( this, newValue, ui16_tempLen, b_enableReplaceOfCmd);
}


const char*
vtObjectStringVariable_c::getString()
{
  return vtObject_a->value;
}

    vtObjectStringVariable_c::vtObjectStringVariable_c(
            vtObjectStringVariable_c::iVtObjectStringVariable_s *vtObjectStringVariableSROM, int ai_multitonInst)
            :iVtObjectString_c(ai_multitonInst)
    		,vtObject_a(vtObjectStringVariableSROM)
    {}

    vtObjectStringVariable_c::vtObjectStringVariable_c(
    		int ai_multitonInst,
    		IsoAgLib::ObjectID ID,
    		char *value)
    	:vtObjectStringVariable_c(
    			new iVtObjectStringVariable_s(
    					ID,
    					strlen(value),
    					value),
    			ai_multitonInst)
    {
    }


#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
    void vtObjectStringVariable_c::saveReceivedAttribute(uint8_t, uint8_t *) {}
#endif
} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_stringvariable
