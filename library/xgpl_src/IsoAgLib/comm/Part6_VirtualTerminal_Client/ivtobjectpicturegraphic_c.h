/*
  ivtobjectpicturegraphic_c.h

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
#ifndef IVTOBJECTPICTUREGRAPHIC_C_H
#define IVTOBJECTPICTUREGRAPHIC_C_H

#include "impl/vtobjectpicturegraphic_c.h"

#ifdef CONFIG_USE_VTOBJECT_picturegraphic

namespace IsoAgLib {


class iVtObjectPictureGraphic_c : public __IsoAgLib::vtObjectPictureGraphic_c
{
public:
  static ObjectType objectType() { return VT_OBJECT_TYPE_PICTURE_GRAPHIC; }


  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPictureGraphic_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPictureGraphic_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setTransparencyColour(Colour newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPictureGraphic_c::setTransparencyColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  void setRawData0 (uint8_t* newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth=0xFFFF, uint16_t aui16_actHeight=0xFFFF, uint16_t aui16_width=0xFFFF) {
    vtObjectPictureGraphic_c::setRawData0 (newValue, aui32_size, ab_rle, aui16_actWidth, aui16_actHeight, aui16_width);
  }
  void setRawData1 (uint8_t* newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth=0xFFFF, uint16_t aui16_actHeight=0xFFFF, uint16_t aui16_width=0xFFFF) {
    vtObjectPictureGraphic_c::setRawData1 (newValue, aui32_size, ab_rle, aui16_actWidth, aui16_actHeight, aui16_width);
  }
  void setRawData2 (uint8_t* newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth=0xFFFF, uint16_t aui16_actHeight=0xFFFF, uint16_t aui16_width=0xFFFF) {
    vtObjectPictureGraphic_c::setRawData2 (newValue, aui32_size, ab_rle, aui16_actWidth, aui16_actHeight, aui16_width);
  }
#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectPictureGraphic_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateWidth(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateOptions(b_SendRequest);
  }

  uint8_t updateTransparencyColour(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateTransparencyColour(b_SendRequest);
  }

  /** these attributes are in parentheses in the spec, so commented out here
  uint16_t updateActualWidth(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateActualWidth(b_SendRequest);
  }

  uint16_t updateActualHeight(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateActualHeight(b_SendRequest);
  }

  uint8_t updateFormat(bool b_SendRequest=false) {
    return vtObjectPictureGraphic_c::updateFormat(b_SendRequest);
  }
  */
#endif

  virtual ObjectType getObjectType() const { return objectType(); }
};

} // IsoAgLib

#endif //CONFIG_USE_VTOBJECT_picturegraphic

#endif //IVTOBJECTPICTUREGRAPHIC_C_H
