/*
  iidentitem_c.h: object for managing identity/ies of the ECU

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IIDENT_ITEM_H
#define IIDENT_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/identitem_c.h"
#include "iisoitem_c.h"
#include "iisoname_c.h"

namespace __IsoAgLib {
  class IsoTerminal_c;
  class DevPropertyHandler_c;
  class ProprietaryMessageHandler_c;
  class ProprietaryMessageClient_c;
}

// Begin Namespace IsoAgLib
namespace IsoAgLib {
// predeclare iIsoTerminal_c, as this class is allowed to access private ( hidden )
// elements of this class exclusively ( see friend declaration at end of this file )
class iIsoTerminal_c;
class iProprietaryMessageHandler_c;
class iProprietaryMessageClient_c;

/**
  class for identity/ies (Control Function(s)) which are managed by the actual ECU;
  new instances start in prepare address claim state and stay there for a random and serialNo dependent time;
  then they are inserted as announcing member in the monitoring list and start announcing;

  If the IsoAgLib shall not start immediately with address claim on definition of the variable
  ( e.g. if some data must be read for definition of local identity ),
  the address claim can be started later with explicit call of iIdentItem_c::start( ).

  The IsoAgLib backend is responsible for answering all requests like
  RequestForClaimedSourceAdress ( ISO 11783 )

  The application just has to call the main trigger function Scheduler_c::timeEvent().

  After the monitor list report a completed address claim the status changes to active/claimed address;

  @short member ident item of this Control Function 
  @author Dipl.-Inform. Achim Spangler
  */
class iIdentItem_c : private __IsoAgLib::IdentItem_c  {
public:
  /**
    default constructor, which can optionally start address claim for this identity,
    if enough information is provided (EEPROM-address)
    @param aui16_saEepromAdr
  */
  iIdentItem_c (uint16_t aui16_saEepromAdr = 0xFFFF)
    : IdentItem_c (aui16_saEepromAdr) {}


  /**
    constructor for ISO identity, which starts address claim for this identity
    @param aui8_indGroup        select the industry group, 2 == agriculture
    @param aui8_devClass	device class
    @param aui8_devClassInst	deveice class instance
    @param ab_func              function code of the member (25 = network interconnect)
    @param aui16_manufCode      11bit manufactor code
    @param aui32_serNo          21bit serial number
    @param aui8_preferredSa       preselected source adress (SA) of the ISO item (fixed SA or last time
                                SA for self conf ISO device) (default 254 for free self-conf)
    @param aui16_eepromAdr    EEPROM adress, where the used source adress is stored for self_conf members
                                (default 0xFFFF for NO EEPROM store)
    @param ab_funcInst          function instance of this member (default 0)
    @param ab_ecuInst           ECU instance of this member (default 0)
    @param ab_selfConf          true -> this member as a self configurable source adress
    @param ai8_slaveCount       amount of attached slave devices; default -1 == no master state;
                                in case an address claim for the slave devices shall be sent by this ECU, they
                                must get their own IdentItem_c instance ( then with default value -1 for ai8_slaveCount )
    @param apc_slaveIsoNameList pointer to list of iIsoName_c values, where the slave devices are defined.
                                IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
  */
  iIdentItem_c(
    uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst, uint8_t ab_func,
    uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t aui8_preferredSa = 254, uint16_t aui16_eepromAdr = 0xFFFF,
    uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0, bool ab_selfConf = true
    #ifdef USE_WORKING_SET
    ,int8_t ai8_slaveCount = -1, const iIsoName_c* apc_slaveIsoNameList = NULL
    #endif
  ) : IdentItem_c (aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode, aui32_serNo,
                   aui8_preferredSa, aui16_eepromAdr, ab_funcInst, ab_ecuInst, ab_selfConf
                 #ifdef USE_WORKING_SET
                   ,ai8_slaveCount, apc_slaveIsoNameList
                 #endif
                  ) {}

  /** init function for later start of address claim of an ISO identity (this can be only called once upon a default-constructed object)
      @param aui8_indGroup        select the industry group, 2 == agriculture
      @param aui8_devClass	  device class
      @param aui8_devClassInst	  deveice class instance
       @param ab_func             function code of the member (25 = network interconnect)
      @param aui16_manufCode      11bit manufactor code
      @param aui32_serNo          21bit serial number
      @param aui8_preferredSa     preferred source adress (SA) of the ISO item (fixed SA or last time
                                  SA for self conf ISO device) (default 254 for no special wish)
      @param aui16_saEepromAdr    EEPROM adress, where the used IsoName / SA / flags are stored
                                  (default 0xFFFF for NO EEPROM store)
      @param ab_funcInst          function instance of this member (default 0)
      @param ab_ecuInst           ECU instance of this member (default 0)
      @param ab_selfConf          defaults to true -> this member as a self configurable source adress
      @param ai8_slaveCount       amount of attached slave devices; default -1 == no master state;
                                  in case an address claim for the slave devices shall be sent by this ECU, they
                                  must get their own IdentItem_c instance ( then with default value -1 for ai8_slaveCount )
      @param apc_slaveIsoNameList pointer to list of IsoName_c values, where the slave devices are defined.
                                  IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    */
  void init(
    uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst, uint8_t ab_func, uint16_t aui16_manufCode,
    uint32_t aui32_serNo, uint8_t aui8_preferredSa, uint16_t aui16_saEepromAdr, uint8_t ab_funcInst = 0, uint8_t ab_ecuInst = 0, bool ab_selfConf = true
    #ifdef USE_WORKING_SET
    ,int8_t ai8_slaveCount = -1, const iIsoName_c* apc_slaveIsoNameList = NULL
    #endif
    )
  { IdentItem_c::init (aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode,
                       aui32_serNo, aui8_preferredSa, aui16_saEepromAdr, ab_funcInst, ab_ecuInst, ab_selfConf
                       #ifdef USE_WORKING_SET
                       ,ai8_slaveCount, apc_slaveIsoNameList
                       #endif
                       );
  }


  /** deliver pointer to IsoItem_c in IsoMonitor_c
      @return NULL -> either no ISO item or not yet registered in IsoMonitor_c
    */
  iIsoItem_c* getIsoItem( void ) const { return static_cast<iIsoItem_c*>(IdentItem_c::getIsoItem()); }

  /** check if the ident has claimed address */
  bool isClaimedAddress( void ) const { return IdentItem_c::isClaimedAddress(); }

  /**
    retreive the actual ISOName value of this ident item
    @return ISOName code of this ident item instance
  */
  const iIsoName_c& isoName() const {return IdentItem_c::isoName().toConstIisoName_c(); }

  /**
    retrieve a POINTER to the ISOName of this ident item
    you can ONLY modify this item if an address is not yet claimed!
    i.e. in this case you'd get a NULL-pointer
    ATTENTION: So ALWAYS check for a NULL pointer return value before accessing!!
    @return NULL if this ident item has already claimed an address on the ISOBUS
            or
            a POINTER to the ISOName of this ident item!
   */
  iIsoName_c* modifyableIsoNameBeforeAddressClaimed() { if (!isClaimedAddress()) return &modifyableIsoName().toIisoName_c();
                                                        else return NULL; }


  /**
    Set ECU Identification fields, needed during the diagnostic procedure
    @return true if the fields were okay
            false if one field was too long (> 200 chars) or had '*' in it.
  */
  bool setEcuIdentification( const STL_NAMESPACE::string& astr_partNbr, const STL_NAMESPACE::string& astr_serialNbr, const STL_NAMESPACE::string& astr_location, const STL_NAMESPACE::string& astr_type, const STL_NAMESPACE::string& astr_manufacturerName )
  {
    return IdentItem_c::setEcuIdentification (astr_partNbr, astr_serialNbr, astr_location, astr_type, astr_manufacturerName);
  }

  /** Set SW Identification fields, needed during the diagnostic procedure
      @param astr_swId SwIdentification according to ISO 11783-12.
                       End fields with *, even the last one (and even if it is only one)
      @return true if the SW Identification was okay
              false if it was too long (> 200 chars) or empty
   */
  bool setSwIdentification( const STL_NAMESPACE::string& astr_swId)
  {
    return IdentItem_c::setSwIdentification (astr_swId);
  }

  //! Setter for the different certification message fields
  //! Parameter:
  //! @param ui16_year Certification year as in ISO 11783-7 A.29.1, must be between 2000 and 2061
  //! @param a_revision Certification revision as in ISO 11783-7 A.29.2
  //! @param a_laboratoryType Certification laboratory type as in ISO 11783-7 A.29.3
  //! @param aui16_laboratoryId Certification laboratory ID (11 bits wide) as in ISO 11783-7 A.29.4
  //! @param acrc_certificationBitMask Compliance certification type bitfield as in ISO 11783-7 A.29.5 till A.29.17
  //! @param aui16_referenceNumber Compliance certification reference number as in ISO 11783-7 A.29.18
  //! @return true all parameters were okay in range
  //!         false at least one parameter was wrong
  bool setCertificationData( uint16_t ui16_year, IsoAgLib::CertificationRevision_t a_revision, IsoAgLib::CertificationLabType_t a_laboratoryType, uint16_t aui16_laboratoryId,
                               const IsoAgLib::CertificationBitMask_t& acrc_certificationBitMask, uint16_t aui16_referenceNumber )
  {
    return IdentItem_c::setCertificationData( ui16_year , a_revision, a_laboratoryType, aui16_laboratoryId,
                                              acrc_certificationBitMask, aui16_referenceNumber );
  }

  /// Using the singletonVecKey from internal class
  MULTITON_PAR_BASE_DEF(IdentItem_c)

private:
  friend class iIsoMonitor_c;
  friend class iFsManager_c;
  friend class iIsoTerminal_c;
  friend class iVtClientServerCommunication_c;
  friend class iDevPropertyHandler_c;
  friend class __IsoAgLib::IsoTerminal_c;
  friend class iProprietaryMessageHandler_c;
  friend class iProprietaryMessageClient_c;
  friend class __IsoAgLib::ProprietaryMessageHandler_c;
  friend class __IsoAgLib::ProprietaryMessageClient_c;
};

} // IsoAgLib
#endif
