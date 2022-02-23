/*
  vtobjectpicturegraphic_c.cpp

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

#include "vtobjectpicturegraphic_c.h"

#ifdef CONFIG_USE_VTOBJECT_picturegraphic
#include "../ivtobjectbutton_c.h"
#include "../ivtobjectmacro_c.h"
#include "vtclient_c.h"


namespace __IsoAgLib {


enum vtObjectPictureGraphic_c::AttributeID:uint8_t
{
	Width               = 1,
	Options             = 2,
	TransparencyColour  = 3,
};






struct vtObjectPictureGraphic_c::iVtObjectPictureGraphic_s: iVtObjectwMacro_s {
	uint16_t width;
	uint16_t actualWidth;
	uint16_t actualHeight;
	IsoAgLib::PictureGraphicFormat format;
	IsoAgLib::iVtObjectPictureGraphicOptions options;
	IsoAgLib::Colour transparencyColour;
	uint32_t numberOfBytesInRawData0;
	const uint8_t *rawData0;
	uint32_t numberOfBytesInRawData1;
	const uint8_t *rawData1;
	uint32_t numberOfBytesInRawData2;
	const uint8_t *rawData2;
	uint8_t numberOfFixedBitmapsToFollow;
	const repeat_rawData_rawBytes_actWidth_actHeight_formatoptions_s *fixedBitmapsToFollow;
	iVtObjectPictureGraphic_s(
			IsoAgLib::ObjectID ID,
			uint16_t width,
			uint16_t actualWidth,
			uint16_t actualHeight,
			IsoAgLib::PictureGraphicFormat format,
			IsoAgLib::iVtObjectPictureGraphicOptions options,
			IsoAgLib::Colour transparencyColour,
			uint32_t numberOfBytesInRawData0,
			const uint8_t *rawData0,
			uint32_t numberOfBytesInRawData1,
			const uint8_t *rawData1,
			uint32_t numberOfBytesInRawData2,
			const uint8_t *rawData2,
			uint8_t numberOfFixedBitmapsToFollow,
			const repeat_rawData_rawBytes_actWidth_actHeight_formatoptions_s *fixedBitmapsToFollow)
    : iVtObject_s(ID)
	, iVtObjectwMacro_s(ID)
	, width(width)
	, actualWidth(actualWidth)
	, actualHeight(actualHeight)
	, format(format)
	, options(options)
	, transparencyColour(transparencyColour)
	, numberOfBytesInRawData0(numberOfBytesInRawData0)
	, rawData0(rawData0)
	, numberOfBytesInRawData1(numberOfBytesInRawData1)
	, rawData1(rawData1)
	, numberOfBytesInRawData2(numberOfBytesInRawData2)
	, rawData2(rawData2)
	, numberOfFixedBitmapsToFollow(numberOfFixedBitmapsToFollow)
	, fixedBitmapsToFollow(fixedBitmapsToFollow) {
	}


};


#if 0
#define helperForDifferentSizes(a,b,c,f,g,h,x,y,z,optionander) \
        if (vtDimension < 400) { \
          numberOfBytesInRawData = vtObject_a->a; \
          rawData = vtObject_a->x; \
          options = (vtObject_a->f & 0x03) + ((vtObject_a->f & optionander) ? 0x04 : 0x00); \
        } else if (vtDimension < 480) { \
          numberOfBytesInRawData = vtObject_a->b; \
          rawData = vtObject_a->y; \
          options = (vtObject_a->g & 0x03) + ((vtObject_a->g & optionander) ? 0x04 : 0x00); \
        } else { \
          numberOfBytesInRawData = vtObject_a->c; \
          rawData = vtObject_a->z; \
          options = (vtObject_a->h & 0x03) + ((vtObject_a->h & optionander) ? 0x04 : 0x00); \
        }

#define helperForDifferentSizesSizeOnly(a,b,c) \
        if (vtDimension < 400) { \
          numberOfBytesInRawData = vtObject_a->a; \
        } else if (vtDimension < 480) { \
          numberOfBytesInRawData = vtObject_a->b; \
        } else { \
          numberOfBytesInRawData = vtObject_a->c; \
        }
//#else
#define helperForDifferentSizes(a,b,f,g,x,y,optionander) \
        if ( ( vtDimension < 480) || ( optionander == 0x04 ) ) { \
          numberOfBytesInRawData = vtObject_a->a; \
          rawData = vtObject_a->x; \
          options = (vtObject_a->f & 0x03) + ((vtObject_a->f & optionander) ? 0x04 : 0x00); \
        } else { \
          numberOfBytesInRawData = vtObject_a->b; \
          rawData = vtObject_a->y; \
          options = (vtObject_a->g & 0x03) + ((vtObject_a->g & optionander) ? 0x04 : 0x00); \
        }

#define helperForDifferentSizesSizeOnly(a,b,col) \
        if ( ( vtDimension < 480) || ( col == 0x0 ) ) { \
          numberOfBytesInRawData = vtObject_a->a; \
        } else { \
          numberOfBytesInRawData = vtObject_a->b; \
        }
#endif

#define min(a,b) (a<b)?a:b

#define MACRO_helperForDifferentSizesSizeOnly(a) \
          numberOfBytesInRawData = vtObject_a->a;

#define MACRO_helperForDifferentSizes(a,f,x,optionander) \
          MACRO_helperForDifferentSizesSizeOnly(a) \
          rawData = vtObject_a->x; \
          options = (vtObject_a->f & 0x03) + ((vtObject_a->f & optionander) ? 0x04 : 0x00); /* get the right RLE 1/4/8 bit to bit 2 when streaming! */

#define MACRO_calculate_ui8_graphicType \
          ui8_graphicType = ( min (getVtClientInstance4Comm().getClientByID (s_properties.clientId).getVtServerInst().getVtCapabilities().hwGraphicType, vtObject_a->format) ); \
          /* If 16-color bitmap is not specified, take the 2-color version. -That's the only exception! */ \
          if ((ui8_graphicType == 1) && (vtObject_a->rawData1 == NULL)) ui8_graphicType = 0;

#define MACRO_CheckFixedBitmapsLoop_start \
    /* See if we have colorDepth of VT */ \
    uint8_t vtDepth = getVtClientInstance4Comm().getClientByID (s_properties.clientId).getVtServerInst().getVtCapabilities().hwGraphicType; \
    /* Check for 100%-matching fixedBitmaps first */ \
    bool b_foundFixedBitmap = false; \
    for (int fixNr=0; fixNr<vtObject_a->numberOfFixedBitmapsToFollow; fixNr++) { \
      /* Matching means only match in WIDTH, not (yet) HEIGHT - and in Colourdepth! */ \
      if ( (width == vtObject_a->fixedBitmapsToFollow [fixNr].actualWidth) \
      && (vtObject_a->fixedBitmapsToFollow [fixNr].formatoptions >> 6 == vtDepth) ) {
// here user can insert code into the loop.
#define MACRO_CheckFixedBitmapsLoop_end \
        b_foundFixedBitmap = true; \
        break; \
      } \
    }
//TODO !! revert this Change !
#define MACRO_calculateRequestedSize \
  uint16_t width; \
  if ((s_properties.flags & FLAG_ORIGIN_SKM)) { \
    width = (((uint32_t) vtObject_a->width * factorM)/factorD); \
  } else { \
    width = (((uint32_t) vtObject_a->width * vtDimension) /opDimension); \
  }


uint16_t
vtObjectPictureGraphic_c::stream(uint8_t* destMemory, uint16_t maxBytes, objRange_t sourceOffset)
{
    uint16_t curBytes=0; /* current bytes written */;
    MACRO_scaleLocalVars;
    MACRO_scaleSKLocalVars;

    uint8_t ui8_graphicType = 0;

    const uint8_t* rawData = NULL;
    uint32_t numberOfBytesInRawData = 0;
    uint16_t actualWidth = 0;
    uint16_t actualHeight = 0;
    uint8_t options = 0;

    MACRO_calculateRequestedSize

    MACRO_CheckFixedBitmapsLoop_start
        rawData = vtObject_a->fixedBitmapsToFollow [fixNr].rawData;
        numberOfBytesInRawData = vtObject_a->fixedBitmapsToFollow [fixNr].numberOfBytesInRawData;
        actualWidth = vtObject_a->fixedBitmapsToFollow [fixNr].actualWidth;
        actualHeight = vtObject_a->fixedBitmapsToFollow [fixNr].actualHeight;
        /* format is bit 8+7, options is bit 2-0 (NO RLE1/4/8 stuff here!! */
        options = vtObject_a->fixedBitmapsToFollow [fixNr].formatoptions & 0x7;
        ui8_graphicType = vtDepth;
    MACRO_CheckFixedBitmapsLoop_end

    if (!b_foundFixedBitmap) {
      // See what we have as standard...
      actualWidth = vtObject_a->actualWidth;
      actualHeight = vtObject_a->actualHeight;
      MACRO_calculate_ui8_graphicType
      switch (ui8_graphicType) {
        case 2:  MACRO_helperForDifferentSizes (numberOfBytesInRawData2, options.options, rawData2, 0x10) break;
        case 1:  MACRO_helperForDifferentSizes (numberOfBytesInRawData1, options.options, rawData1, 0x08) break;
        case 0:
        default: MACRO_helperForDifferentSizes (numberOfBytesInRawData0, options.options, rawData0, 0x04) break;
      }
    }

    // Get a ref to the vtClient, so that we can convert colours by calling getUserConvertedColor() over and over
    VtClientConnection_c& vtClient = getVtClientInstance4Comm().getClientByID(s_properties.clientId);
    const uint32_t pgheaderSize = 17;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = VT_OBJECT_TYPE_PICTURE_GRAPHIC; // Object Type = Picture Graphic
      destMemory [3] = width & 0xFF; // calculated above
      destMemory [4] = width >> 8;   // dependent on FLAG_ORIGIN_SKM
      destMemory [5] = actualWidth & 0xFF;
      destMemory [6] = actualWidth >> 8;
      destMemory [7] = actualHeight & 0xFF;
      destMemory [8] = actualHeight >> 8;
      destMemory [9] = ui8_graphicType;
      destMemory [10] = options;
      destMemory [11] = vtClient.getUserConvertedColor(vtObject_a->transparencyColour, this, IsoAgLib::TransparencyColour);
      destMemory [12] = (numberOfBytesInRawData) & 0xFF;
      destMemory [13] = (numberOfBytesInRawData >> 8) & 0xFF;
      destMemory [14] = (numberOfBytesInRawData >> 16) & 0xFF;
      destMemory [15] = (numberOfBytesInRawData >> 24) & 0xFF;
      destMemory [16] = vtObject_a->numberOfMacrosToFollow;

      sourceOffset += pgheaderSize;
      curBytes += pgheaderSize;
    }

#ifdef CONFIG_VT_CLIENT_PICTURE_GRAPHIC_COLOUR_CONVERSION
    // Colour Operation is for b/w picture graphics only!
    // let's get color 0 (black) and color 1 (white) converted by the application
    // and decide then what to do with these colors in case of a b/w terminal (graphicType==0)
    Colour colourOperation = 0;
    enum ColourOperation_e
    {
                    // W B (White, Black) - 1 if that colour changed its value during the conversion
          NoChange  // 0 0 = 0 - No change (leave all bits alone)
        , SetToZero // 1 0 = 1 - 0 (set all bits to 0=black)
        , SetToOne  // 0 1 = 2 - 1 (set all bits to 1=white)
        , Toggle    // 1 1 = 3 - XOR (toggle all bits with XOR)
    };

    if( ui8_graphicType == 0 )
    {
        for( Colour colourCode=0; colourCode<2;++colourCode )
        {
            uint8_t convertedColour = vtClient.getUserConvertedColor( colourCode, this, IsoAgLib::PictureGraphicColour );

            if( convertedColour != colourCode )
            {
                // If the colour toggled (from 1 to 0 or 0 to 1 which are the only possibilities for 1-bit color)
                // then set that colour's bit (see table under ColourOperation_e)
                // Black is bit 0, White is bit 1 (that's the reason for the shift below)
                colourOperation |= (1 << colourCode);
            }
        }
    }
#endif

    while ((sourceOffset >= pgheaderSize) && (sourceOffset < (pgheaderSize+numberOfBytesInRawData)) && ((curBytes+1) <= maxBytes))
    {
#ifdef CONFIG_VT_CLIENT_PICTURE_GRAPHIC_COLOUR_CONVERSION
        if( sourceOffset < (pgheaderSize + (vtObject_a->numberOfMacrosToFollow << 1)) )
        {
            // Copy over the macros
            // 2 bytes for each macro defined, so the end of the macros is
            // sourceOffset + (pgheaderSize + (vtObject_a->numberOfMacrosToFollow << 1))
            destMemory [curBytes] = rawData [sourceOffset-pgheaderSize];
        }
        else
        {
            // If this is a RunLength Encoded 2-byte pair (count, colour), and this is not the colour byte
            if ((options & 0x04) && (sourceOffset & 0x01))
            {
                // just return the byte without conversion
                destMemory[curBytes] = rawData[sourceOffset - pgheaderSize];
            }
            else
            {
                // Otherwise, either it is not RLE encoded
                // or this is the colour (2nd) byte of the 2-byte pair
                // So, yes, we need to convert the color

                // Try to do colour conversion on the bitmap if necessary (during upload)
                switch (ui8_graphicType)
                {
                case 2: // 8-bit
                    destMemory[curBytes] = vtClient.getUserConvertedColor(rawData[sourceOffset - pgheaderSize], this, IsoAgLib::PictureGraphicColour);
                    break;

                case 1: // 4-bit - Convert only 4 bits at a time (2 function calls)
                    // Convert 2 color codes (YES, convert color)
                    destMemory[curBytes] =
                        (
                            vtClient.getUserConvertedColor(rawData[sourceOffset - pgheaderSize] & 0x0F, this, IsoAgLib::PictureGraphicColour)			// Colour in Bottom 4 bits
                        |
                            (vtClient.getUserConvertedColor((rawData[sourceOffset - pgheaderSize] & 0xF0) >> 4, this, IsoAgLib::PictureGraphicColour)	// Colour in Top 4 bits
                             << 4)
                        );
                    break;

                case 0: // 1-bit - Convert all 8-bits at once
                    switch (colourOperation)
                    {
                    case NoChange:
                        destMemory[curBytes] = rawData[sourceOffset - pgheaderSize];
                        break;

                    case SetToZero:
                        destMemory[curBytes] = 0;
                        break;

                    case SetToOne:
                        destMemory[curBytes] = 0xFF;
                        break;

                    case Toggle:
                        destMemory[curBytes] = rawData[sourceOffset - pgheaderSize] ^ 0xFF;
                        break;
                    }
                    break;

                default:
                    isoaglib_assert( !"Error - Unknown ui8_graphicType, wrong object pool data!" );
                    break;
                }
            }
        }
#else
      destMemory [curBytes] = rawData [sourceOffset-pgheaderSize];
#endif

      curBytes++;
      sourceOffset++;
    }

    MACRO_streamEventMacro(pgheaderSize+numberOfBytesInRawData);

    return curBytes;
}



IsoAgLib::ObjectID vtObjectPictureGraphic_c::getID() const {
	isoaglib_assert(vtObject_a);
	return vtObject_a->ID;
}


uint32_t
vtObjectPictureGraphic_c::fitTerminal() const
{
  MACRO_scaleLocalVars;
  MACRO_scaleSKLocalVars;

  uint32_t numberOfBytesInRawData = 0;
  uint8_t ui8_graphicType;

  MACRO_calculateRequestedSize

  MACRO_CheckFixedBitmapsLoop_start
      numberOfBytesInRawData = vtObject_a->fixedBitmapsToFollow [fixNr].numberOfBytesInRawData;
  MACRO_CheckFixedBitmapsLoop_end

  if (!b_foundFixedBitmap) {
    MACRO_calculate_ui8_graphicType
    switch (ui8_graphicType) {
      case 2:  MACRO_helperForDifferentSizesSizeOnly (numberOfBytesInRawData2) break;
      case 1:  MACRO_helperForDifferentSizesSizeOnly (numberOfBytesInRawData1) break;
      case 0:
      default: MACRO_helperForDifferentSizesSizeOnly (numberOfBytesInRawData0) break;
    }
  }

  return 17+numberOfBytesInRawData+vtObject_a->numberOfMacrosToFollow*2;
}

#ifdef CONFIG_USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectPictureGraphic_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
	  getAttribute(Width);
  return vtObject_a->width;
}

IsoAgLib::iVtObjectPictureGraphicOptions
vtObjectPictureGraphic_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(Options);
  return vtObject_a->options;
}

IsoAgLib::Colour
vtObjectPictureGraphic_c::updateTransparencyColour(bool b_SendRequest)
{
  if (b_SendRequest)
    getAttribute(TransparencyColour);
  return vtObject_a->transparencyColour;
}

vtObjectPictureGraphic_c::~vtObjectPictureGraphic_c() = default;

/** these attributes are in parentheses in the spec, so commented out here
uint16_t
vtObjectPictureGraphic_c::updateActualWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), actualWidth), sizeof(iVtObjectPictureGraphic_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), actualWidth), sizeof(iVtObjectPictureGraphic_s));
}

uint16_t
vtObjectPictureGraphic_c::updateActualHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), actualHeight), sizeof(iVtObjectPictureGraphic_s), 5);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), actualHeight), sizeof(iVtObjectPictureGraphic_s));
}

uint8_t
vtObjectPictureGraphic_c::updateFormat(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), format), sizeof(iVtObjectPictureGraphic_s), 6);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), format), sizeof(iVtObjectPictureGraphic_s));
}
*/

void
vtObjectPictureGraphic_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case Width:              vtObject_a->width              = convertLittleEndianStringUi16(  pui8_attributeValue); break;
    case Options:            vtObject_a->options.options    = (IsoAgLib::iVtObjectPictureGraphicOptions_e)convertLittleEndianStringUi8(   pui8_attributeValue); break;
    case TransparencyColour: vtObject_a->transparencyColour = convertLittleEndianStringColour(pui8_attributeValue); break;
    /** these attributes are in parentheses in the spec, so commented out here
    case 4: vtObject_a->actualWidth), sizeof(iVtObjectPictureGraphic_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: vtObject_a->actualHeight), sizeof(iVtObjectPictureGraphic_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 6: saveValue8(MACRO_getStructOffset(get_vtObjectPictureGraphic_a(), format), sizeof(iVtObjectPictureGraphic_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
    vtObjectPictureGraphic_c::vtObjectPictureGraphic_c(
    		vtObjectPictureGraphic_c::iVtObjectPictureGraphic_s *vtObjectPictureGraphicSROM, multiton ai_multitonInst)
            :vtObject_c(ai_multitonInst)
    		,vtObject_a(vtObjectPictureGraphicSROM)
    {}



    void vtObjectPictureGraphic_c::setWidth(uint16_t newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
        MACRO_scaleLocalVars
        MACRO_scaleSKLocalVars

        uint32_t scaledDim = uint32_t( newValue );
      #ifndef USE_VT_CLIENT_OLD_UNSCALED_SIZE_COMMANDS
        MACRO_scaleDimension( scaledDim )
      #endif

    	if (b_updateObject)
    		vtObject_a->width = newValue;
        setAttribute (Width, scaledDim, b_enableReplaceOfCmd);
    }

    void vtObjectPictureGraphic_c::setOptions(IsoAgLib::iVtObjectPictureGraphicOptions newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	setAttribute ( Options, newValue.options & 0x7, b_enableReplaceOfCmd);
    }

    void
    vtObjectPictureGraphic_c::setTransparencyColour(IsoAgLib::Colour newValue, bool b_updateObject, bool b_enableReplaceOfCmd) {
    	if (b_updateObject)
    		vtObject_a->transparencyColour = newValue;
    	setAttribute ( TransparencyColour, getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserConvertedColor (newValue, this, IsoAgLib::TransparencyColour), b_enableReplaceOfCmd);
    }

    void
    vtObjectPictureGraphic_c::setRawData0(uint8_t *newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth,
                                          uint16_t aui16_actHeight, uint16_t aui16_width) { // normally it would be enough to just use saveValueP once, because the ram-struct is then created... but anyway...
    	vtObject_a->rawData0 = newValue;
        vtObject_a->numberOfBytesInRawData0 = aui32_size;
        vtObject_a->options.bits.rawData = ab_rle ? IsoAgLib::RawData::rle : IsoAgLib::RawData::rawData ;
        if (aui16_actWidth != 0xFFFF) vtObject_a->actualWidth  = aui16_actWidth;
        if (aui16_actHeight!= 0xFFFF) vtObject_a->actualHeight = aui16_actHeight;
        if (aui16_width    != 0xFFFF) vtObject_a->width        = aui16_width;
    }

    void
    vtObjectPictureGraphic_c::setRawData1(uint8_t *newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth,
                                          uint16_t aui16_actHeight, uint16_t aui16_width) { // normally it would be enough to just use saveValueP once, because the ram-struct is then created... but anyway...
    	vtObject_a->rawData1 = newValue;
        vtObject_a->numberOfBytesInRawData1 = aui32_size;
        vtObject_a->options.bits.rawData = ab_rle ? IsoAgLib::RawData::rle : IsoAgLib::RawData::rawData ;
        if (aui16_actWidth != 0xFFFF) vtObject_a->actualWidth  = aui16_actWidth;
        if (aui16_actHeight!= 0xFFFF) vtObject_a->actualHeight = aui16_actHeight;
        if (aui16_width    != 0xFFFF) vtObject_a->width       = aui16_width;
    }

    void
    vtObjectPictureGraphic_c::setRawData2(uint8_t *newValue, uint32_t aui32_size, bool ab_rle, uint16_t aui16_actWidth,
                                          uint16_t aui16_actHeight, uint16_t aui16_width) { // normally it would be enough to just use saveValueP once, because the ram-struct is then created... but anyway...
    	vtObject_a->rawData2 = newValue;
        vtObject_a->numberOfBytesInRawData2 = aui32_size;
        vtObject_a->options.bits.rawData = ab_rle ? IsoAgLib::RawData::rle : IsoAgLib::RawData::rawData ;
        if (aui16_actWidth != 0xFFFF) vtObject_a->actualWidth  = aui16_actWidth;
        if (aui16_actHeight!= 0xFFFF) vtObject_a->actualHeight = aui16_actHeight;
        if (aui16_width    != 0xFFFF) vtObject_a->width        = aui16_width;
    }


} // __IsoAgLib

#endif //CONFIG_USE_VTOBJECT_picturegraphic
