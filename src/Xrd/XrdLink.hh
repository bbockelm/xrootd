#ifndef __XRD_LINK_H__
#define __XRD_LINK_H__
/******************************************************************************/
/*                                                                            */
/*                            X r d L i n k . h h                             */
/*                                                                            */
/* (c) 2004 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/*                                                                            */
/* This file is part of the XRootD software suite.                            */
/*                                                                            */
/* XRootD is free software: you can redistribute it and/or modify it under    */
/* the terms of the GNU Lesser General Public License as published by the     */
/* Free Software Foundation, either version 3 of the License, or (at your     */
/* option) any later version.                                                 */
/*                                                                            */
/* XRootD is distributed in the hope that it will be useful, but WITHOUT      */
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      */
/* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public       */
/* License for more details.                                                  */
/*                                                                            */
/* You should have received a copy of the GNU Lesser General Public License   */
/* along with XRootD in a file called COPYING.LESSER (LGPL license) and file  */
/* COPYING (GPL license).  If not, see <http://www.gnu.org/licenses/>.        */
/*                                                                            */
/* The copyright holder's institutional names and contributor's names may not */
/* be used to endorse or promote products derived from this software without  */
/* specific prior written permission of the institution or contributor.       */
/******************************************************************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

#include "XrdSys/XrdSysPthread.hh"

#include "Xrd/XrdJob.hh"
#include "Xrd/XrdLinkMatch.hh"
#include "Xrd/XrdProtocol.hh"
  
/******************************************************************************/
/*                       X r d L i n k   O p t i o n s                        */
/******************************************************************************/
  
#define XRDLINK_RDLOCK  0x0001
#define XRDLINK_NOCLOSE 0x0002

/******************************************************************************/
/*                      C l a s s   D e f i n i t i o n                       */
/******************************************************************************/
  
class XrdInet;
class XrdNetBuffer;
class XrdNetPeer;
class XrdPoll;
class XrdOucTrace;
class XrdScheduler;
class XrdSysError;

class XrdLink : XrdJob
{
public:
friend class XrdLinkScan;
friend class XrdPoll;
friend class XrdPollPoll;
friend class XrdPollDev;
friend class XrdPollE;

static XrdLink *Alloc(XrdNetPeer &Peer, int opts=0);

void          Bind() {}                // Obsolete
void          Bind(pthread_t tid) {}   // Obsolete

int           Client(char *buff, int blen);

int           Close(int defer=0);

void          DoIt();

void          Enable();

int           FDnum() {return FD;}

static XrdLink *fd2link(int fd)
                {if (fd < 0) fd = -fd; 
                 return (fd <= LTLast && LinkBat[fd] ? LinkTab[fd] : 0);
                }

static XrdLink *fd2link(int fd, unsigned int inst)
                {if (fd < 0) fd = -fd; 
                 if (fd <= LTLast && LinkBat[fd] && LinkTab[fd]
                 && LinkTab[fd]->Instance == inst) return LinkTab[fd];
                 return (XrdLink *)0;
                }

static XrdLink *Find(int &curr, XrdLinkMatch *who=0);

       int    getIOStats(long long &inbytes, long long &outbytes,
                              int  &numstall,     int  &numtardy)
                        { inbytes = BytesIn + BytesInTot;
                         outbytes = BytesOut+BytesOutTot;
                         numstall = stallCnt + stallCntTot;
                         numtardy = tardyCnt + tardyCntTot;
                         return InUse;
                        }

static int    getName(int &curr, char *bname, int blen, XrdLinkMatch *who=0);

XrdProtocol  *getProtocol() {return Protocol;} // opmutex must be locked

void          Hold(int lk) {(lk ? opMutex.Lock() : opMutex.UnLock());}

char         *ID;      // This is referenced a lot

static   void Init(XrdSysError *eP, XrdOucTrace *tP, XrdScheduler *sP)
                  {XrdLog = eP; XrdTrace = tP; XrdSched = sP;}

static   void Init(XrdInet *iP) {XrdNetTCP = iP;}

unsigned int  Inst() {return Instance;}

int           isFlawed() {return Etext != 0;}

int           isInstance(unsigned int inst)
                        {return FD >= 0 && Instance == inst;}

const char   *Name(sockaddr *ipaddr=0)
                     {if (ipaddr) memcpy(ipaddr, &InetAddr, sizeof(sockaddr));
                      return (const char *)Lname;
                     }

const char   *Host(sockaddr *ipaddr=0)
                     {if (ipaddr) memcpy(ipaddr, &InetAddr, sizeof(sockaddr));
                      return (const char *)HostName;
                     }

int           Peek(char *buff, int blen, int timeout=-1);

int           Recv(char *buff, int blen);
int           Recv(char *buff, int blen, int timeout);

int           RecvAll(char *buff, int blen, int timeout=-1);

int           Send(const char *buff, int blen);
int           Send(const struct iovec *iov, int iocnt, int bytes=0);

struct sfVec {union {char *buffer;    // ->Data if fdnum < 0
                     off_t offset;    // File offset      of data
                    };
              int   sendsz;           // Length of data at offset
              int   fdnum;            // File descriptor for data
             };
static const int sfMax = 8;

static int    sfOK;                   // True if Send(sfVec) enabled

int           Send(const struct sfVec *sdP, int sdn); // Iff sfOK > 0

void          Serialize();                              // ASYNC Mode

int           setEtext(const char *text);

void          setID(const char *userid, int procid);

static void   setKWT(int wkSec, int kwSec);

XrdProtocol  *setProtocol(XrdProtocol *pp);

void          setRef(int cnt);                          // ASYNC Mode

static int    Setup(int maxfd, int idlewait);

static int    Stats(char *buff, int blen, int do_sync=0);

       void   syncStats(int *ctime=0);

       int    Terminate(const XrdLink *owner, int fdnum, unsigned int inst);

time_t        timeCon() {return conTime;}

int           UseCnt() {return InUse;}

void          armBridge() {isBridged = 1;}
int           hasBridge() {return isBridged;}

              XrdLink();
             ~XrdLink() {}  // Is never deleted!

private:

void   Reset();
int    sendData(const char *Buff, int Blen);

static XrdSysError  *XrdLog;
static XrdOucTrace  *XrdTrace;
static XrdScheduler *XrdSched;
static XrdInet      *XrdNetTCP;

static XrdSysMutex   LTMutex;    // For the LinkTab only LTMutex->IOMutex allowed
static XrdLink     **LinkTab;
static char         *LinkBat;
static unsigned int  LinkAlloc;
static int           LTLast;
static const char   *TraceID;
static int           devNull;
static short         killWait;
static short         waitKill;

// Statistical area (global and local)
//
static long long    LinkBytesIn;
static long long    LinkBytesOut;
static long long    LinkConTime;
static long long    LinkCountTot;
static int          LinkCount;
static int          LinkCountMax;
static int          LinkTimeOuts;
static int          LinkStalls;
static int          LinkSfIntr;
       long long        BytesIn;
       long long        BytesInTot;
       long long        BytesOut;
       long long        BytesOutTot;
       int              stallCnt;
       int              stallCntTot;
       int              tardyCnt;
       int              tardyCntTot;
       int              SfIntr;
static XrdSysMutex  statsMutex;

// Identification section
//
struct sockaddr     InetAddr;
char                Uname[24];  // Uname and Lname must be adjacent!
char                Lname[232];
char               *HostName;
int                 HNlen;
pthread_t           TID;

XrdSysMutex         opMutex;
XrdSysMutex         rdMutex;
XrdSysMutex         wrMutex;
XrdSysSemaphore     IOSemaphore;
XrdSysCondVar      *KillcvP;        // Protected by opMutex!
XrdLink            *Next;
XrdNetBuffer       *udpbuff;
XrdProtocol        *Protocol;
XrdProtocol        *ProtoAlt;
XrdPoll            *Poller;
struct pollfd      *PollEnt;
char               *Etext;
int                 FD;
unsigned int        Instance;
time_t              conTime;
int                 InUse;
int                 doPost;
char                LockReads;
char                KeepFD;
char                isEnabled;
char                isIdle;
char                inQ;
char                isBridged;
char                KillCnt;        // Protected by opMutex!
static const char   KillMax =   60;
static const char   KillMsk = 0x7f;
static const char   KillXwt = 0x80;
};
#endif
