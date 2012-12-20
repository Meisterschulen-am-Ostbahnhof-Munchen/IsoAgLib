/*
  can_driver_sys.cpp: socketcan driver implementation

  (C) Copyright 2011 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <linux/version.h>
#include "IsoAgLib/hal/generic_utils/can/canfifo_c.h"

#include <map>

#include <IsoAgLib/hal/pc/system/system.h>
#include <IsoAgLib/util/iassert.h>

#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

#ifndef DEF_CAN_NETDEV_PREFIX
#define DEF_CAN_NETDEV_PREFIX "can"
#endif

#if DEBUG_CAN
#define DEBUG_PRINT(...) \
  fprintf(stderr, "%s(%d): ", __FUNCTION__, __LINE__ ); \
  fprintf(stderr, __VA_ARGS__)
#else
#  define DEBUG_PRINT(...)
#endif

/* use this define to get a very verbose output */
//#define DEBUG_CAN_VERBOSE 1

namespace __HAL {
#ifdef USE_MUTUAL_EXCLUSION
  static int breakWaitPipeFd[2] = { -1, -1 };
  static int breakWaitPipeInitCounter = 0;

  void canInitBreakWait() {
    if( breakWaitPipeInitCounter == 0 )
    {
      /* open break waitUntilCanReceiveOrTimeout socket */
      if( pipe2( breakWaitPipeFd, O_NONBLOCK ) != 0 ) {
        perror("pipe");
      }
    }
    ++breakWaitPipeInitCounter;
  }

  void canCloseBreakWait() {
    isoaglib_assert( breakWaitPipeInitCounter >	0 );

    --breakWaitPipeInitCounter;
    if( breakWaitPipeInitCounter == 0 )
    {
      (void)close(breakWaitPipeFd[0]);
      (void)close(breakWaitPipeFd[1]);
    }
  }

  void canClearBreakWaitFd( fd_set& rfds ) {
    if( FD_ISSET( breakWaitPipeFd[0], &rfds ) ) {
      static char buff[256];
      while( 0 < read( breakWaitPipeFd[0], buff, 256 ) ) { }
    }
  }
#endif

}

namespace HAL {

  /** representation of a single can bus instance */
  struct canBus_s {
    canBus_s() :
      mb_initialized( false ),
      mi_fd( -1 ) {}
    bool mb_initialized;   /* initialization flag */
    int mi_fd;            /* socket fd */
  };


  /** information about each channel available */
  static canBus_s g_bus[ HAL_CAN_MAX_BUS_NR + 1 ];
  /** maximum fd */
  static int g_fdMax = 0;
  /** fd set with opened fd for receive path */
  static fd_set g_rfds;


  /** recalculate maximum fd and set fd mask for select call */
  void recalcFd() {
    FD_ZERO( &g_rfds );
    for ( uint8_t i = 0; i < HAL_CAN_MAX_BUS_NR; ++i ) {
      if ( g_bus[ i ].mb_initialized ) {
        FD_SET( g_bus[ i ].mi_fd, &g_rfds );
        if ( g_fdMax < g_bus[ i ].mi_fd ) {
          g_fdMax = g_bus[ i ].mi_fd;
        }
      }
    }
#ifdef USE_MUTUAL_EXCLUSION
    FD_SET( __HAL::breakWaitPipeFd[0], &g_rfds );
    if( __HAL::breakWaitPipeFd[0] > g_fdMax )
      g_fdMax = __HAL::breakWaitPipeFd[0];
#endif
  }


  /** check bus number for range */
  bool checkBusNumber( uint8_t ui8_bus ) {
    return ( HAL_CAN_MAX_BUS_NR >= ui8_bus );
  }

  /** initialize a can channel with given parameter */
  bool canInit( unsigned channel, unsigned ) {
    DEBUG_PRINT( "%s: %d\n", __FUNCTION__, channel );

    /* argument check */
    if ( HAL_CAN_MAX_BUS_NR < channel ) {
      return HAL_RANGE_ERR;
    }

    /* check for already initialized bus */
    if ( g_bus[ channel ].mb_initialized ) {
      DEBUG_PRINT( "%s: bus already initialized\n", __FUNCTION__ );
      return true;
    }

    /*  generate device name */
    char fname[ IFNAMSIZ ];
    sprintf( fname, "%s%u", DEF_CAN_NETDEV_PREFIX, channel );

    /* create a socket */
    int fd = socket( PF_CAN, SOCK_RAW, CAN_RAW );
    if ( fd < 0 ) {
      perror( "socket" );
      return false;
    }

    /* get the interface index */
    struct ifreq ifr;
    strncpy( ifr.ifr_name, fname, IFNAMSIZ );
    if ( -1 == ioctl( fd, SIOCGIFINDEX, &ifr ) ) {
      perror( "SIOCGIFINDEX" );
      return false;
    }

    /* bind to the socket */
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if ( -1 == bind( fd, ( struct sockaddr* ) & addr, sizeof( addr ) ) ) {
      perror( "bind" );
      return false;
    }

    /* enable loopback (ensure, because it may not be default in the future) */
    int loopback = 1; /* 0 = disabled, 1 = enabled (default) */
    if ( -1 == setsockopt( fd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof( loopback ) ) ) {
      perror( "enabling loopback" );
      return false;
    }

    /* disable reception of own sent messages */
    int recv_own_msgs = 0; /* 0 = disabled (default), 1 = enabled */
    if ( -1 == setsockopt( fd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof( recv_own_msgs ) ) ) {
      perror( "setting local echo option" );
      return false;
    }

    /* set error mask */
    can_err_mask_t err_mask = ( CAN_ERR_MASK );
    if ( -1 == setsockopt( fd, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof( err_mask ) ) ) {
      perror( "setting error mask" );
      return false;
    }

    g_bus[ channel ].mi_fd = fd;
    g_bus[ channel ].mb_initialized = true;

#ifdef USE_MUTUAL_EXCLUSION
    __HAL::canInitBreakWait();
#endif

    recalcFd();

    return true;
  };


  /** close channel */
  bool canClose( unsigned channel ) {
    DEBUG_PRINT( "\n" );

    isoaglib_assert( checkBusNumber( channel ) );

    /* close bus */
    if ( -1 == close( g_bus[ channel ].mi_fd ) ) {
      perror( "close" );
      return false;
    }

    g_bus[ channel ].mb_initialized = false;
    g_bus[ channel ].mi_fd = -1;

#ifdef USE_MUTUAL_EXCLUSION
    __HAL::canCloseBreakWait();
#endif

    recalcFd();

    return true;
  };


  /**
    block till data is available on any opened channel or timeout occours
    @param ui16_timeout timeout in ms
  */
  bool canRxWait( unsigned timeout_ms ) {
    //DEBUG_PRINT("\n");

    static struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = timeout_ms * 1000;

    fd_set rfds = g_rfds;
    const int rc = select( g_fdMax + 1, &rfds, 0, 0, &timeout );

#ifdef USE_MUTUAL_EXCLUSION
    __HAL::canClearBreakWaitFd( rfds );
#endif

    return ( rc > 0 );
  };


#ifdef USE_MUTUAL_EXCLUSION
  void canRxWaitBreak() {
    if( write( __HAL::breakWaitPipeFd[1], "\0", 1 ) != 1 ) {
      perror("write");
    }
  }
#endif


  /** print errTest if errMask & data is true; return errMask & data */
  bool printIfError( uint32_t ui32_data, uint32_t ui32_errMask, const char* carr_errText ) {
    if ( ui32_data & ui32_errMask ) {
      fprintf( stderr, "%d: %s ", getTime(), carr_errText );
      return true;
    }
    return false;
  }


  /** print human readable error code on error frame reception */
  int16_t handleErrorFrame( const struct can_frame& frame ) {
    // check dlc
    if ( CAN_ERR_DLC != frame.can_dlc ) {
      fprintf( stderr, "handleErrorFrame: wrong dlc: %d!\n", frame.can_dlc );
      /*  this is something weird - check your can driver */
      return HAL_UNKNOWN_ERR;
    }

    ( void ) printIfError( frame.can_id, CAN_ERR_TX_TIMEOUT,            "TX timeout (by netdevice driver)" );
    ( void ) printIfError( frame.can_id, CAN_ERR_ACK,                   "received no ACK on transmission" );
    ( void ) printIfError( frame.can_id, CAN_ERR_BUSOFF,                "bus off" );
    ( void ) printIfError( frame.can_id, CAN_ERR_BUSERROR,              "bus error (may flood!)" );
    ( void ) printIfError( frame.can_id, CAN_ERR_RESTARTED,             "controller restarted" );

    /* arbitration lost in bit ... / data[0] */
    if ( printIfError( frame.can_id, CAN_ERR_LOSTARB,                  "lost arbitration / data[0]: " ) ) {
      ( void ) printIfError( frame.can_id, CAN_ERR_LOSTARB_UNSPEC,      "unspecified\n" );
    }
    /* else bit number in bitstream */

    /* error status of CAN-controller / data[1] */
    if ( printIfError( frame.can_id, CAN_ERR_CRTL,                     "controller problems / data[1]:" ) ) {
      ( void ) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_UNSPEC,      "unspecified\n" );
      ( void ) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_RX_OVERFLOW, "RX buffer overflow\n" );
      ( void ) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_TX_OVERFLOW, "TX buffer overflow\n" );
      ( void ) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_RX_WARNING,  "reached warning level for RX errors\n" );
      ( void ) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_TX_WARNING,  "reached warning level for TX errors\n" );
      ( void ) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_RX_PASSIVE,  "reached error passive status RX\n" );
      ( void ) printIfError( frame.data[ 1 ], CAN_ERR_CRTL_TX_PASSIVE,  "reached error passive status TX\n" );
    }
    /* (at least one error counter exceeds */
    /* the protocol-defined level of 127)  */

    /* error in CAN protocol (type) / data[2] */
    if ( printIfError( frame.can_id, CAN_ERR_PROT,                     "protocol violations / data[2..3]: " ) ) {
      ( void ) printIfError( frame.data[ 2 ], CAN_ERR_PROT_UNSPEC,      "unspecified\n" );
      ( void ) printIfError( frame.data[ 2 ], CAN_ERR_PROT_BIT,         "single bit error\n" );
      ( void ) printIfError( frame.data[ 2 ], CAN_ERR_PROT_FORM,        "frame format error\n" );
      ( void ) printIfError( frame.data[ 2 ], CAN_ERR_PROT_STUFF,       "bit stuffing error\n" );
      ( void ) printIfError( frame.data[ 2 ], CAN_ERR_PROT_BIT0,        "unable to send dominant bit\n" );
      ( void ) printIfError( frame.data[ 2 ], CAN_ERR_PROT_BIT1,        "unable to send recessive bit\n" );
      ( void ) printIfError( frame.data[ 2 ], CAN_ERR_PROT_OVERLOAD,    "bus overload\n" );
      ( void ) printIfError( frame.data[ 2 ], CAN_ERR_PROT_ACTIVE,      "active error announcement\n" );
      ( void ) printIfError( frame.data[ 2 ], CAN_ERR_PROT_TX,          "error occured on transmission\n" );
      /* error in CAN protocol (location) / data[3] */
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_UNSPEC,  "unspecified\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_SOF,     "start of frame\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ID28_21, "ID bits 28 - 21 (SFF: 10 - 3)\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ID20_18, "ID bits 20 - 18 (SFF: 2 - 0 \n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_SRTR,    "substitute RTR (SFF: RTR)\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_IDE,     "identifier extension\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ID17_13, "ID bits 17-13\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ID12_05, "ID bits 12-5\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ID04_00, "ID bits 4-0\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_RTR,     "RTR\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_RES1,    "reserved bit 1\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_RES0,    "reserved bit 0\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_DLC,     "data length code\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_DATA,    "data section\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_CRC_SEQ, "CRC sequence\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_CRC_DEL, "CRC delimiter\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ACK,     "ACK slot\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_ACK_DEL, "ACK delimiter\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_EOF,     "end of frame\n" );
      ( void ) printIfError( frame.data[ 3 ], CAN_ERR_PROT_LOC_INTERM,  "intermission\n" );
    }

    /* error status of CAN-transceiver / data[4] */
    /* CANH CANL */
    if ( printIfError( frame.can_id, CAN_ERR_TRX,                                "transceiver status  / data[4]: " ) ) {
      bool printed = false;
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_UNSPEC,             "CAN_ERR_TRX_UNSPEC\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANH_NO_WIRE,       "CAN_ERR_TRX_CANH_NO_WIRE\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANH_SHORT_TO_BAT,  "CAN_ERR_TRX_CANH_SHORT_TO_BAT\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANH_SHORT_TO_VCC,  "CAN_ERR_TRX_CANH_SHORT_TO_VCC\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANH_SHORT_TO_GND,  "CAN_ERR_TRX_CANH_SHORT_TO_GND\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANL_NO_WIRE,       "CAN_ERR_TRX_CANL_NO_WIRE\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_BAT,  "CAN_ERR_TRX_CANL_SHORT_TO_BAT\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_VCC,  "CAN_ERR_TRX_CANL_SHORT_TO_VCC\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_GND,  "CAN_ERR_TRX_CANL_SHORT_TO_GND\n" );
      printed |= printIfError( frame.data[ 4 ], CAN_ERR_TRX_CANL_SHORT_TO_CANH, "CAN_ERR_TRX_CANL_SHORT_TO_CANH\n" );
      if ( ! printed ) {
        fprintf( stderr, "\n" );
      }
    }

    return HAL_UNKNOWN_ERR;
  }


  /** retrive a pending msg from bus and store in tReceive */
  void canRxPoll( unsigned channel ) {
    static struct can_frame frame;
    bzero( &frame, sizeof( struct can_frame ) );

    isoaglib_assert( checkBusNumber( channel ) );

    while( recv( g_bus[ channel ].mi_fd, ( char* ) &frame, sizeof( struct can_frame ), MSG_DONTWAIT ) != -1 ) {
      /* check for err frame */
      if ( ( CAN_ERR_FLAG & frame.can_id ) == CAN_ERR_FLAG ) {
        DEBUG_PRINT( "%s: error frame!\n", __FUNCTION__ );
        handleErrorFrame( frame );
        return;
      }

      const bool ext = ( ( frame.can_id & CAN_EFF_FLAG ) == CAN_EFF_FLAG );
      __IsoAgLib::CanPkg_c pkg(
          frame.can_id & ( ext ? CAN_EFF_MASK : CAN_SFF_MASK ),
          ext,
          frame.can_dlc,
          getTime() );

      memcpy( pkg.getUint8DataPointer(), frame.data, frame.can_dlc );

      CanFifos_c::get( channel ).push( pkg );
    }
  };



  bool canTxSend( unsigned channel, const __IsoAgLib::CanPkg_c& msg ) {

#if DEBUG_CAN && DEBUG_CAN_VERBOSE
    DEBUG_PRINT( "%012d tx: %x#", getTime(), iframe.id );
    for ( unsigned i = 0; i < iframe.dlc; ++i ) {
      fprintf( stderr, "%02x", iframe.data[ i ] );
    }
    fprintf( stderr, "\n" );
#endif

    static struct can_frame frame;
    bzero( &frame, sizeof( struct can_frame ) );

    frame.can_id = msg.ident();
    frame.can_dlc = msg.getLen();
    if( msg.identType() == __IsoAgLib::Ident_c::ExtendedIdent ) {
      frame.can_id |= CAN_EFF_FLAG;
    }

    memcpy( frame.data, msg.getUint8DataConstPointer(), frame.can_dlc );

    if ( -1 == send( g_bus[ channel ].mi_fd, ( char* ) & frame, sizeof( struct can_frame ), MSG_DONTWAIT ) ) {
      perror( "send" );
      return false;
    }

    return true;
  };


  bool canState( unsigned, canState_t& state ) {
    state = e_canNoError;
    return true;
  }


  int canTxQueueFree( unsigned ) {
    return -1;
  }

} // end namespace __HAL

// eof
