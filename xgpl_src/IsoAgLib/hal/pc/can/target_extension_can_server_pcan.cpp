/***************************************************************************
              target_extension_can_server_A1_binary.cpp -
                    can server communicates with clients through message queues
                    read/write operation for /dev/wecan<bus_no>
                    message forwarding to other clients
                    #define SIMULATE_BUS_MODE for operation without can device (operation based on message forwarding)
                    use "can_server_a1 --help" for input parameters (log, replay mode)

                             -------------------
    begin                : Tue Oct 2 2001
    copyright            : (C) 1999 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
 ***************************************************************************/


/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

// do not use can bus, operation is based on message forwarding in server
#if !defined( SYSTEM_MCC ) && !defined( PCAN_MSCAN_MINOR_BASE )
	#define SIMULATE_BUS_MODE 1
#endif

#include "can_target_extensions.h"
#include <cstring>
#include <cstdio>
#include <cctype>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include <pthread.h>
#include <linux/version.h>
#include <linux/types.h>

#include "can_server.h"
#include "can_msq.h"

#ifdef USE_PCAN_LIB
#include <libpcan.h>
#endif

using namespace __HAL;

#ifdef USE_PCAN_LIB
HANDLE driverHandle;
#endif

/////////////////////////////////////////////////////////////////////////
// Globals
/** if the following define is active, the can_server writes important logging info to the given path.
 *  comment this define out to eliminate this */
#ifdef WIN32
  #define CAN_SERVER_LOG_PATH ".\\can_server.log"
#elif defined( SYSTEM_PCAN )
  #define CAN_SERVER_LOG_PATH "/sd0/settings/can_server.log"
#else
  #define CAN_SERVER_LOG_PATH "./can_server.log"
#endif

////////////////////////////////////////////////////////////////////////////////////////

#define MSGTYPE_EXTENDED        0x02            /* extended frame */
#define MSGTYPE_STANDARD        0x00            /* standard frame */

/* ioctl request codes */
#define CAN_MAGIC_NUMBER        'z'
#define MYSEQ_START             0x80

//****************************************************************************
// DEFINES

//****************************************************************************
// compatibilty defines
#ifndef USE_PCAN_LIB
#  if defined(DWORD) || defined(WORD) || defined(BYTE)
#    error "double define for DWORD, WORD, BYTE found"
#  endif
#endif

#ifdef __KERNEL__
#define DWORD  u32
#define WORD   u16
#define BYTE   u8
#else
#define DWORD  __u32
#define WORD   __u16
#define BYTE   __u8
#endif

#ifndef USE_PCAN_LIB
//****************************************************************************
// ioctls control codes
#define PCAN_INIT           _IOWR(CAN_MAGIC_NUMBER, MYSEQ_START,     TPCANInit)
#define PCAN_WRITE_MSG      _IOW (CAN_MAGIC_NUMBER, MYSEQ_START + 1, TPCANMsg)
#define PCAN_READ_MSG       _IOR (CAN_MAGIC_NUMBER, MYSEQ_START + 2, TPCANRdMsg)
#define PCAN_GET_STATUS     _IOR (CAN_MAGIC_NUMBER, MYSEQ_START + 3, TPSTATUS)
#define PCAN_DIAG           _IOR (CAN_MAGIC_NUMBER, MYSEQ_START + 4, TPDIAG)
#define PCAN_BTR0BTR1       _IOWR(CAN_MAGIC_NUMBER, MYSEQ_START + 5, TPBTR0BTR1)
#define PCAN_GET_EXT_STATUS _IOR (CAN_MAGIC_NUMBER, MYSEQ_START + 6, TPEXTENDEDSTATUS)
#endif

// device nodes minor base. Must be the same as defined in the driver (file pcan_mpc5200.c).
#ifndef PCAN_MSCAN_MINOR_BASE
	#define PCAN_MSCAN_MINOR_BASE     40
#endif


#ifndef USE_PCAN_LIB
//****************************************************************************
// structures to communicate via ioctls
typedef struct
{
  WORD wBTR0BTR1;        // merged BTR0 and BTR1 register of the SJA100
  BYTE ucCANMsgType;     // 11 or 29 bits - put MSGTYPE_... in here
  BYTE ucListenOnly;     // listen only mode when != 0
} TPCANInit;             // for PCAN_INIT

typedef struct
{
  DWORD dwBitRate;       // in + out, bitrate in bits per second
  WORD  wBTR0BTR1;       // out only: the result
} TPBTR0BTR1;

typedef struct
{
  DWORD ID;              // 11/29 bit code
  BYTE  MSGTYPE;         // bits of MSGTYPE_*
  BYTE  LEN;             // count of data bytes (0..8)
  BYTE  DATA[8];         // data bytes, up to 8
} TPCANMsg;              // for PCAN_WRITE_MSG

typedef struct
{
  TPCANMsg Msg;          // the above message
  DWORD    dwTime;       // a timestamp in msec, read only
} TPCANRdMsg;            // for PCAN_READ_MSG

typedef struct 
{
  WORD  wErrorFlag;      // same as in TPDIAG, is cleared in driver after access
  int   nLastError;      // is cleared in driver after access
  int   nPendingReads;   // count of unread telegrams
  int   nPendingWrites;  // count of unsent telegrams
} TPEXTENDEDSTATUS;      // for PCAN_GET_ESTATUS
#endif


/////////////////////////////////////////////////////////////////////////
// Local Data

static bool  canBusIsOpen[cui32_maxCanBusCnt];

bool ca_GetcanBusIsOpen_1 (int busId)
{
  return canBusIsOpen[busId];
}



/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_InitApi_1
// PURPOSE: To load and initialize the CAN driver and return the CAN API version
// RETURNS: API version greater than 0
//      0 on error (no CAN device available)
// NOTES: In this case, we will simply return an API version of 1.0  (0x0100)
//      Highversion is in highbyte, Lowversion is in lowbyte
//
// FUTURE:  This should be finished to actually check if /dev/wecan0 and
//      /dev/wecan1 are available and the drivers installed correctly or not
//
/////////////////////////////////////////////////////////////////////////
int ca_InitApi_1 ()
{
  for( uint32_t i=0; i<cui32_maxCanBusCnt; i++ )
    {
    canBusIsOpen[i] = false;
    }

  return 0x0100;
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_ResetCanCard_1
// PURPOSE: To reset the alreay initialized CAN driver (especially clear its
//      buffers and any error conditions)
// RETURNS: non-zero if CAN device was reset ok
//      0 on error
// NOTES: In this case, we will simply return 1
// FUTURE:  This should be finished to actually check the return values of fflush()
//
/////////////////////////////////////////////////////////////////////////
int ca_ResetCanCard_1(void)
{
  return 1;
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_InitCanCard_1
// PURPOSE: To initialize the specified CAN BUS to begin sending/receiving msgs
// PARAMS:  channel   // 0 for CANLPT/ICAN, else 1 for first BUS
//      msgnr   // msg-nr / 0 for CANLPT/ICAN
//      accode    // Acceptance Code to receive everything for ICAN
//      accmask   // Acceptance Mask to receive everything for ICAN
//      fullcanmask // filter array of int[16];
//      btr0    // BTR0
//      btr1    // BTR1
//      octrl   // reserved
//      typ     // typ 0 = 2 x 32 Bit, 1 = 4 x 16 Bit,
//            // 2 = 8 x 8 Bit, 3 = kein durchlass
//      extended  // reserved
// RETURNS: non-zero if CAN BUS was initialized ok
//      0 on error
// NOTES: In this case, we will simply return 1
//
// FUTURE:  This should be finished to actually use the parameters like baud
//      rate and extended IDs during the initialization.  Right now they
//      are hardcoded and the passed in params are ignored.
//      Should check return value of write() to make sure the baud rate
//      was set correctly before proceeding.
//
/////////////////////////////////////////////////////////////////////////
int ca_InitCanCard_1 (uint32_t channel, int wBitrate, server_c* pc_serverData)
{

  DEBUG_PRINT1("init can channel %d\n", channel);

  if( !canBusIsOpen[channel] ) {
    DEBUG_PRINT1("Opening CAN BUS channel=%d\n", channel);

    char fname[32];
#ifndef SIMULATE_BUS_MODE
    sprintf( fname, "/dev/pcan%u", PCAN_MSCAN_MINOR_BASE + channel );
#else
    sprintf( fname, "/dev/null");
#endif

    DEBUG_PRINT1("open( \"%s\", O_RDRWR)\n", fname);

#ifdef USE_PCAN_LIB
    //driverHandle = CAN_Open(HW_USB, 0);
    driverHandle = LINUX_CAN_Open(fname, O_RDWR | O_NONBLOCK);
    pc_serverData->can_device[channel] = LINUX_CAN_FileHandle(driverHandle);
    if ( driverHandle == NULL ) std::cerr << "Open CAN Fault" << std::endl;
#else
    pc_serverData->can_device[channel] = open(fname, O_RDWR | O_NONBLOCK);
    if (pc_serverData->can_device[channel] == -1) {
      DEBUG_PRINT1("Could not open CAN bus %d\n",channel);
      return 0;
    }
#endif
#ifndef SIMULATE_BUS_MODE
    ///////////////
    // pcan modification
    ///////////////
#ifdef USE_PCAN_LIB
    WORD useBtr = LINUX_CAN_BTR0BTR1(driverHandle, wBitrate*1000);
    if (CAN_Init(driverHandle, useBtr, 2) < 0) { std::cerr << "Init Problem" << std::endl;return 0;}
#else
    TPBTR0BTR1 ratix;
    TPCANInit init;

    // init wBitrate
    DEBUG_PRINT1("Init Bitrate with PCAN_BTR0BTR1 wBitrate =%d\n",wBitrate*1000);
    ratix.dwBitRate = wBitrate * 1000;
    ratix.wBTR0BTR1 = 0;
    if ((ioctl(pc_serverData->can_device[channel], PCAN_BTR0BTR1, &ratix)) < 0)
      return 0;


    // init CanMsgType (if extended Can Msg of not)
    DEBUG_PRINT1("Init CAN Driver with PCAN_INIT wBitrate =%x\n",ratix.wBTR0BTR1);
    //default value = extended
    init.wBTR0BTR1    = ratix.wBTR0BTR1;
    init.ucCANMsgType = MSGTYPE_EXTENDED;  // 11 or 29 bits
    init.ucListenOnly = 0;            // listen only mode when != 0
    if ((ioctl(pc_serverData->can_device[channel], PCAN_INIT, &init)) < 0)
      return 0;
#endif
    ////////////////
#endif

    canBusIsOpen[channel] = true;
    return 1;

  } else
    return 1; // already initialized and files are already open
}

void __HAL::updatePendingMsgs(server_c* pc_serverData, int8_t i8_bus)
{
  // get amount of waiting-to-be-sent-out can-msgs in can-controller
  static TPEXTENDEDSTATUS extstat;
  if (i8_bus < 0)
  { // update all buses!
    for (uint8_t ui8_bus=0; ui8_bus < cui32_maxCanBusCnt; ui8_bus++)
    {
      if (pc_serverData->i_pendingMsgs[ui8_bus] >= 5)
      { // we only need to update those who could change from >= 5 to < 5...
#if 0 //def USE_PCAN_LIB
        if (LINUX_CAN_Extended_Status(driverHandle, &(extstat.nPendingReads), &(extstat.nPendingWrites))) continue;
#else
        if ((ioctl(pc_serverData->can_device[ui8_bus], PCAN_GET_EXT_STATUS, &extstat)) < 0) continue;
#endif
        pc_serverData->i_pendingMsgs[ui8_bus] = extstat.nPendingWrites;
        DEBUG_PRINT1 ("peak-can's number of pending msgs is %d\n", pc_serverData->i_pendingMsgs[ui8_bus]);
      }
    }
  }
  else
  { // update just the given bus!
#if 0 //def USE_PCAN_LIB
    if (LINUX_CAN_Extended_Status(driverHandle, &(extstat.nPendingReads), &(extstat.nPendingWrites))) return;
#else
    if ((ioctl(pc_serverData->can_device[i8_bus], PCAN_GET_EXT_STATUS, &extstat)) < 0) return;
#endif
    pc_serverData->i_pendingMsgs[i8_bus] = extstat.nPendingWrites;
    DEBUG_PRINT1 ("peak-can's number of pending msgs is %d\n", pc_serverData->i_pendingMsgs[i8_bus]);
  }
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_TransmitCanCard_1
// PURPOSE: To send a msg on the specified CAN BUS
// PARAMS:  channel   // 0 for CANLPT/ICAN, else 1 for first BUS
//      extended  // extended Frame
//      ptSend    // can object
// RETURNS: non-zero if msg was sent ok
//      0 on error
// NOTES: In this case, we will simply return 1
//
/////////////////////////////////////////////////////////////////////////
int ca_TransmitCanCard_1(tSend* ptSend, uint8_t ui8_bus, server_c* pc_serverData)
{
// Always Transmit to this format:
//  the letter 'm', extended/standard ID, CAN ID, Data Length, data bytes, timestamp
//  m e 0x0cf00300 8  0xff 0xfe 0x0b 0xff 0xff 0xff 0xff 0xff       176600
//
// Input is in this format:
// typedef struct
// {
//   uint32_t dwId;                          /** Identifier */
//   uint8_t bXtd;                           /** Laenge Bit Identifier */
//   uint8_t bDlc;                           /** Anzahl der empfangenen Daten */
//   uint8_t abData[8];                      /** Datenpuffer */
// } tSend;

//  fprintf(stderr,"Transmitting data to channel %d\n", channel);


  TPCANMsg msg;
  msg.ID = ptSend->dwId;
  msg.MSGTYPE = ( ptSend->bXtd ? MSGTYPE_EXTENDED : MSGTYPE_STANDARD );
  msg.LEN = ptSend->bDlc;

  for( int i=0; i<msg.LEN; i++ )
    msg.DATA[i] = ptSend->abData[i];

#ifndef SIMULATE_BUS_MODE

// select call should not be necessary during write
#if 0
  fd_set wfds;
  int retval;
  int maxfd = pc_serverData->can_device[ui8_bus]+1;

  FD_ZERO(&wfds);
  FD_SET(pc_serverData->can_device[ui8_bus], &wfds);

  struct timeval tv;
  tv.tv_sec = 0;    // tv_sec and tv_usec= 0: timeout immediately (only check if data is waiting when on a timer)
  tv.tv_usec = 10000;  // wait not more than 10 msec for the state: nonblocking write
  retval = select(maxfd, NULL, &wfds, NULL, &tv);
  if(retval == -1)
  {
    DEBUG_PRINT("Error Occured in select\n");
    return 0;

  } else if(retval == 0)
  {
    DEBUG_PRINT("Error can't write Occured in select\n");
    return 0;
  } else
  {
    if(FD_ISSET(pc_serverData->can_device[channel], &wfds) != 1)
    {
      DEBUG_PRINT("Not selecting right thing\n");
      return 0;
    }
  }
#endif

  int ret = 0;

  if ((ui8_bus < HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus]) {
#if 0 //def USE_PCAN_LIB
    ret = CAN_Write(driverHandle, &msg);
#else
    ret = ioctl(pc_serverData->can_device[ui8_bus], PCAN_WRITE_MSG, &msg);
#endif


#ifdef DEBUG_IOCTL
    if (ret < 0) {
      perror("ca_TransmitCanCard_1 ioctl");

      // try to read
      TPCANRdMsg msg;
      ret = ioctl(pc_serverData->can_device[ui8_bus], PCAN_READ_MSG, &msg);
      printf("id 0x%x msg_type 0x%x len 0x%x data 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x time 0x%x\n", msg.Msg.ID, msg.Msg.MSGTYPE, msg.Msg.LEN, msg.Msg.DATA[0], msg.Msg.DATA[1], msg.Msg.DATA[2], msg.Msg.DATA[3], msg.Msg.DATA[4], msg.Msg.DATA[5], msg.Msg.DATA[6], msg.Msg.DATA[7], msg.dwTime);

      if (ret < 0) {
        perror("ioctl read after write");
      }
    }
#endif
  }

#else
  int ret = 0;
#endif

  if (ret < 0) {
    // perror("ca_TransmitCanCard_1 ioctl");
    /* nothing to read or interrupted system call */
  }
  /// NEW:
  else
  {
    updatePendingMsgs(pc_serverData, ui8_bus);
    int i_pendingMsgs = pc_serverData->i_pendingMsgs[ui8_bus];
    if ((i_pendingMsgs > 0) && (list_sendTimeStamps.size() >= (i_pendingMsgs)))
    { // something pending!
      std::list<int32_t>::iterator pc_iter = list_sendTimeStamps.begin();
      i_pendingMsgs--; // we're >0 at the beginning!
      while (i_pendingMsgs)
      {
        pc_iter++;
        i_pendingMsgs--;
      }
      const int ci_delay = getTime() - (*pc_iter);
      pc_iter++; // go to the one that's been already sent and remove from list!
      while (pc_iter != list_sendTimeStamps.end())
      { // remove all the timestamps of the already sent messages!
        pc_iter = list_sendTimeStamps.erase (pc_iter);
      }
      // do we have a new max for this bus?
      DEBUG_PRINT3 ("target_extension_can_server_pcan::ca_TransmitCanCard_1: SEND_DELAY WAS: %d    -- max for bus %d is: %d\n", ci_delay, ui8_bus, pc_serverData->i32_sendDelay[ui8_bus]);
      if (ci_delay > pc_serverData->i32_sendDelay[ui8_bus])
      { // yes we do, so set it!
        pc_serverData->i32_sendDelay[ui8_bus] = ci_delay;
        DEBUG_PRINT ("target_extension_can_server_pcan::ca_TransmitCanCard_1: reporting back HAL_NEW_SEND_DELAY");
        return HAL_NEW_SEND_DELAY;
      }
    }
  }
  return 1;
}

int ca_ReceiveCanCard_1(uint8_t ui8_bus, server_c* pc_serverData, CANmsg *ps_canMsg)
{

  TPCANRdMsg msg;
#ifdef USE_PCAN_LIB
  int ret = LINUX_CAN_Read(driverHandle, &msg);
#else
  int ret = ioctl(pc_serverData->can_device[ui8_bus], PCAN_READ_MSG, &msg);
#endif

  if (ret < 0)
    return ret;

  ps_canMsg->id = msg.Msg.ID;
  ps_canMsg->msg_type = msg.Msg.MSGTYPE;
  ps_canMsg->len = msg.Msg.LEN;
  ps_canMsg->time = msg.dwTime;

  memcpy( ps_canMsg->data, msg.Msg.DATA, msg.Msg.LEN );

  return ret;
}

int32_t getServerTimeFromClientTime( client_c& ref_receiveClient, int32_t ri32_clientTime )
{
  return ri32_clientTime + ref_receiveClient.i32_msecStartDeltaClientMinusServer;
}


void addSendTimeStampToList(client_c *ps_client, int32_t i32_sendTimeStamp)
{
  list_sendTimeStamps.push_front (getServerTimeFromClientTime (*ps_client, i32_sendTimeStamp));
}



