/*
  ivtobjectinputboolean_c.h

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
#ifndef IVTOBJECTINPUTBOOLEAN_C_H
#define IVTOBJECTINPUTBOOLEAN_C_H

#include "impl/vtobjectinputboolean_c.h"


#ifdef CONFIG_USE_VTOBJECT_inputboolean

#include "ivtobjectfontattributes_c.h"
#include "ivtobjectnumbervariable_c.h"

namespace IsoAgLib {

class iVtObjectInputBoolean_c : public __IsoAgLib::vtObjectInputBoolean_c
{

private:
	iVtObjectInputBoolean_c() = delete;

public:

  ~iVtObjectInputBoolean_c() override;

  iVtObjectInputBoolean_c(
			iVtClientObjectPool_c* pool,
			ObjectID ID = autoID,
			Colour backgroundColour = WHITE,
			uint16_t width = 20,
			iVtObjectFontAttributes_c *foregroundColour = nullptr,
			iVtObjectNumberVariable_c *variableReference = nullptr,
			uint8_t value = 0,
			uint8_t enabled = 1);


  static uint16_t objectType();


  void setValue(bool newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) override;
  void setBackgroundColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setForegroundColour(iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  void setVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) override;
  bool enable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  bool disable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false) override;
  bool getEnabled() override;
  bool select(uint8_t selectOrActivate) override;

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES  
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputBoolean_c::updateObjectType(); }
  */
  
  Colour updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::updateBackgroundColour(b_SendRequest);
  }
  
  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::updateWidth(b_SendRequest);
  }

  iVtObjectFontAttributes_c* updateForegroundColour(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::updateForegroundColour(b_SendRequest);
  }

  iVtObjectNumberVariable_c* updateVariableReference(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::updateVariableReference(b_SendRequest);
  }

  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateValue(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::getValue(b_SendRequest);
  }

  uint8_t updateEnabled(bool b_SendRequest=false) {
    return vtObjectInputBoolean_c::updateEnabled(b_SendRequest);
  }
  */
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_inputboolean

#endif //IVTOBJECTINPUTBOOLEAN_C_H
