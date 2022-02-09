/*
  vtobjectoutputnumber_c.h

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
#ifndef VTOBJECTOUTPUTNUMBER_C_H
#define VTOBJECTOUTPUTNUMBER_C_H

#include <IsoAgLib/isoaglib_config.h>

#ifdef USE_VTOBJECT_outputnumber


#include "../ivtobjectfontattributes_c.h"
#include "../ivtobject_c.h"
#include "vtobject_c.h"
#include "vtclient_c.h"
#include "vtclientconnection_c.h"


namespace __IsoAgLib {

class vtObjectOutputNumber_c : public vtObject_c
{
private:
	// Internal implementation class
	struct iVtObjectOutputNumber_s;

	// Pointer to the internal implementation
	iVtObjectOutputNumber_s* vtObject_a;
	//TODO
	//std::unique_ptr<iVtObjectOutputNumber_s> vtObject_a;

public:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);
  IsoAgLib::ObjectID getID() const;

  vtObjectOutputNumber_c(iVtObjectOutputNumber_s* vtObjectOutputNumberSROMs , int ai_multitonInst);

  iVtObjectOutputNumber_s* get_vtObjectOutputNumber_a();

  vtObjectOutputNumber_c();
  uint32_t fitTerminal() const;
  virtual void setValue(uint32_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true);
  void setOriginSKM(bool b_SKM);
  void setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn);

    // //////////////////////////////////
  // All special Attribute-Set methods
   virtual void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setVariableReference(IsoAgLib::iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setOffset(int32_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setScale(float newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setNumberOfDecimals(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setFormat(bool newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setHorizontalJustification(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
    virtual void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
    // ///////////////////////// getter for attributes
    virtual /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 12; }
   */

  uint16_t updateWidth(bool b_SendRequest=false);
  virtual uint16_t updateHeight(bool b_SendRequest=false);
  virtual uint8_t updateBackgroundColour(bool b_SendRequest=false);
  virtual uint16_t updateFontAttributes(bool b_SendRequest=false);
  virtual uint8_t updateOptions(bool b_SendRequest=false);
  virtual uint16_t updateVariableReference(bool b_SendRequest=false);
  virtual int32_t updateOffset(bool b_SendRequest=false);
  virtual float updateScale(bool b_SendRequest=false);
  virtual uint8_t updateNumberOfDecimals(bool b_SendRequest=false);
  virtual uint8_t updateFormat(bool b_SendRequest=false);
  virtual uint8_t updateJustification(bool b_SendRequest=false);

  /** that attribute is in parentheses in the spec, so commented out here
  uint32_t updateValue(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // __IsoAgLib

#endif //USE_VTOBJECT_outputnumber

#endif //VTOBJECTOUTPUTNUMBER_C_H
