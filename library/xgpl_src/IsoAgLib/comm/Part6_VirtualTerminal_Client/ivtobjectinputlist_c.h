/*
  ivtobjectinputlist_c.h

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
#ifndef IVTOBJECTINPUTLIST_C_H
#define IVTOBJECTINPUTLIST_C_H

#include "impl/vtobjectinputlist_c.h"


#ifdef CONFIG_USE_VTOBJECT_inputlist

namespace IsoAgLib {

class iVtObjectInputList_c : public __IsoAgLib::vtObjectInputList_c
{
private:
	iVtObjectInputList_c() = delete;

public:

	explicit iVtObjectInputList_c(
			    iVtClientObjectPool_c* pool,
			    ObjectID ID = autoID,
	  		    uint16_t width = 100,
				uint16_t height = 25,
				iVtObjectNumberVariable_c *variableReference = nullptr,
				uint8_t value = 0,
				uint8_t options = 0);

	~iVtObjectInputList_c() override;

  static uint16_t objectType() { return VT_OBJECT_TYPE_INPUT_LIST; }


  iVtObject_c* getListItem(uint8_t xth) { return vtObjectInputList_c::getListItem (xth); }

  uint8_t getNumberOfListItems() { return vtObjectInputList_c::getNumberOfListItems(); }

  void setValue(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=true) {
    vtObjectInputList_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setItem(uint8_t aui8_index, iVtObject_c* apc_object, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setItem (aui8_index, apc_object, b_enableReplaceOfCmd);
  }

  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setVariableReference(iVtObjectNumberVariable_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setVariableReference(newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectInputList_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  bool enable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false); //TODO
  bool disable(bool b_updateObject= false, bool b_enableReplaceOfCmd=false); //TODO

  bool getEnabled(); //TODO

  bool select(uint8_t selectOrActivate) { return vtObject_c::select(selectOrActivate); }

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES  
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectInputList_c::updateObjectType(); }
  */
  
  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectInputList_c::updateWidth(b_SendRequest);
  }
  
  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectInputList_c::updateHeight(b_SendRequest);
  }

  iVtObjectNumberVariable_c* updateVariableReference(bool b_SendRequest=false) {
    return vtObjectInputList_c::updateVariableReference(b_SendRequest);
  }

  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateValue(bool b_SendRequest=false) {
    return vtObjectInputList_c::getValue(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectInputList_c::updateOptions(b_SendRequest);
  }
  */
#endif

  virtual uint16_t getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_inputlist

#endif //IVTOBJECTINPUTLIST_C_H
