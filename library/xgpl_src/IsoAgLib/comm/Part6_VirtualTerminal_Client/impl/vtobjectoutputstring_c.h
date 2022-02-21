/*
  vtobjectoutputstring_c.h

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
#ifndef VTOBJECTOUTPUTSTRING_C_H
#define VTOBJECTOUTPUTSTRING_C_H

#include <IsoAgLib/isoaglib_config.h>


#ifdef CONFIG_USE_VTOBJECT_outputstring


#include <memory> // PImpl

#include "../ivtobjectstring_c.h"
#include "vtclientconnection_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {

class vtObjectOutputString_c : public IsoAgLib::iVtObjectString_c
{
private:
	enum AttributeID:uint8_t;
	// Internal implementation class
	struct iVtObjectOutputString_s;

	// Pointer to the internal implementation
	std::unique_ptr<iVtObjectOutputString_s> vtObject_a;

    vtObjectOutputString_c(iVtObjectOutputString_s* vtObjectOutputStringSROM , multiton ai_multitonInst);

public:
  uint16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset) override;
  IsoAgLib::ObjectID getID() const override;


  vtObjectOutputString_c() = delete;

  vtObjectOutputString_c(
		  	multiton ai_multitonInst,
		  	IsoAgLib::ObjectID ID,
			uint16_t width,
			uint16_t height,
			IsoAgLib::Colour backgroundColour,
			IsoAgLib::iVtObjectFontAttributes_c *fontAttributes,
			IsoAgLib::iVtObjectStringOptions options,
			IsoAgLib::iVtObjectStringVariable_c *variableReference,
			IsoAgLib::Justification justification,
			char *value);





  ~vtObjectOutputString_c() override;
  uint32_t fitTerminal() const override;
  void setOriginSKM(bool b_SKM) override;
#ifdef CONFIG_USE_VTOBJECT_button
  void setOriginBTN(IsoAgLib::iVtObjectButton_c* p_btn) override;
#endif //CONFIG_USE_VTOBJECT_button
  const char* getString() override;
  virtual void setValueCopy    (char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#ifdef USE_VT_UNICODE_SUPPORT

        virtual void setValueCopyUTF8(const char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setValueCopyUTF16(const char* newValue, uint16_t length, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
#endif

        virtual void setValueRef(char* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
  // //////////////////////////////////
  // All special Attribute-Set methods
        virtual void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setBackgroundColour(IsoAgLib::Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setFontAttributes(IsoAgLib::iVtObjectFontAttributes_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setOptions(IsoAgLib::iVtObjectStringOptions newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setVariableReference(IsoAgLib::iVtObjectStringVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setJustification(IsoAgLib::Justification newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);
        virtual void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 11; }
  */

    virtual uint16_t updateWidth(bool b_SendRequest=false);
  virtual uint16_t updateHeight(bool b_SendRequest=false);
  virtual IsoAgLib::Colour updateBackgroundColour(bool b_SendRequest=false);
  virtual IsoAgLib::iVtObjectFontAttributes_c* updateFontAttributes(bool b_SendRequest=false);
  virtual IsoAgLib::iVtObjectStringOptions updateOptions(bool b_SendRequest=false);
  virtual IsoAgLib::iVtObjectStringVariable_c*  updateVariableReference(bool b_SendRequest=false);
  virtual IsoAgLib::Justification updateJustification(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue) override;
#endif
};

} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_outputstring

#endif //VTOBJECTOUTPUTSTRING_C_H
