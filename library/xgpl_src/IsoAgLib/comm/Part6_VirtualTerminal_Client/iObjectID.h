/*
 * iObjectID.h
 *
 *  Created on: 04.02.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IOBJECTID_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IOBJECTID_H_

namespace IsoAgLib {


	enum ObjectID : uint16_t
	{
		autoID = 0,
		firstID = 257, /* Macro ObjID must be 0-255 !! */
		nullID = 0xFFFFU,
	};

}

#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IOBJECTID_H_ */
