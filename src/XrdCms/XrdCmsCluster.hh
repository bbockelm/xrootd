#ifndef __CMS_CLUSTER__H
#define __CMS_CLUSTER__H
/******************************************************************************/
/*                                                                            */
/*                      X r d C m s C l u s t e r . h h                       */
/*                                                                            */
/* (c) 2007 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
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

#include <stdlib.h>
#include <string.h>
#include <strings.h>
  
#include "XrdCms/XrdCmsTypes.hh"
#include "XrdOuc/XrdOucTList.hh"
#include "XrdSys/XrdSysPthread.hh"

class XrdLink;
class XrdCmsDrop;
class XrdCmsNode;
class XrdCmsSelect;
class XrdCmsPrefNodes;
class XrdCmsPref;

namespace XrdCms
{
struct CmsRRHdr;
}
 
/******************************************************************************/
/*                          O p t i o n   F l a g s                           */
/******************************************************************************/

namespace XrdCms
{

// Flags passed to Add()
//
static const int CMS_noStage =  1;
static const int CMS_Suspend =  2;
static const int CMS_Perm    =  4;
static const int CMS_isMan   =  8;
static const int CMS_Lost    = 16;
static const int CMS_isPeer  = 32;
static const int CMS_isProxy = 64;
static const int CMS_noSpace =128;

// Class passed to Space()
//
class SpaceData
{
public:

long long Total;    // Total space
int       wMinF;    // Free space minimum to select wFree node
int       wFree;    // Free space for nodes providing r/w access
int       wNum;     // Number of      nodes providing r/w access
int       wUtil;    // Average utilization
int       sFree;    // Free space for nodes providing staging
int       sNum;     // Number of      nodes providing staging
int       sUtil;    // Average utilization

          SpaceData() : Total(0), wMinF(0),
                        wFree(0), wNum(0), wUtil(0),
                        sFree(0), sNum(0), sUtil(0) {}
         ~SpaceData() {}
};
}
  
/******************************************************************************/
/*                   C l a s s   X r d C m s C l u s t e r                    */
/******************************************************************************/
  
// This a single-instance global class
//
class XrdCmsBaseFR;
class XrdCmsSelected;

class XrdCmsCluster
{
public:
friend class XrdCmsDrop;

int             NodeCnt;       // Number of active nodes

// Called to add a new node to the cluster. Status values are defined above.
//
XrdCmsNode     *Add(XrdLink *lp, int dport, int Status,
                    int sport, const char *theNID);

// Sends a message to all nodes matching smask (three forms for convenience)
//
SMask_t         Broadcast(SMask_t, const struct iovec *, int, int tot=0);

SMask_t         Broadcast(SMask_t smask, XrdCms::CmsRRHdr &Hdr,
                          char *Data,    int Dlen=0);

SMask_t         Broadcast(SMask_t smask, XrdCms::CmsRRHdr &Hdr,
                          void *Data,    int Dlen);

// Sends a message to a single node in a round-robbin fashion.
//
int             Broadsend(SMask_t smask, XrdCms::CmsRRHdr &Hdr,
                          void *Data,    int Dlen);

// Returns the node mask matching the given IP address
//
SMask_t         getMask(unsigned int IPv4adr);

// Returns the node mask matching the given cluster ID
//
SMask_t         getMask(const char *Cid);

// Extracts out node information. Opts are one or more of CmsLSOpts
//
enum            CmsLSOpts {LS_All = 0x0001, LS_IPO  = 0x0002};

XrdCmsSelected *List(SMask_t mask, CmsLSOpts opts);

// Returns the location of a file
//
int             Locate(XrdCmsSelect &Sel);

// Always run as a separate thread to monitor subscribed node performance
//
void           *MonPerf();

// Alwats run as a separate thread to maintain the node reference count
//
void           *MonRefs();

// Return total number of redirect references (sloppy as we don't lock it)
//
long long       Refs() {return SelWcnt+SelWtot+SelRcnt+SelRtot;}

// Called to remove a node from the cluster
//
void            Remove(const char *reason, XrdCmsNode *theNode, int immed=0);

// Called to reset the node reference counts for nodes matching smask
//
void            ResetRef(SMask_t smask);

// Called to select the best possible node to serve a file (two forms)
//
int             Select(XrdCmsSelect &Sel, XrdCmsPref *Pref=NULL);

int             Select(int isrw, int isMulti, SMask_t pmask, int &port,
                       char *hbuff, int &hlen);

// Called to get cluster space (for managers and supervisors only)
//
void            Space(XrdCms::SpaceData &sData, SMask_t smask);

// Called to return statistics
//
int             Stats(char *bfr, int bln); // Server
int             Statt(char *bfr, int bln); // Manager

// Called to fill in the PrefNodes structure from our internal Nodes array.
//
int             FillInPrefs(XrdCmsPrefNodes & node_prefs);

                XrdCmsCluster();
               ~XrdCmsCluster() {} // This object should never be deleted

private:
int         Assign(const char *Cid);
XrdCmsNode *calcDelay(int nump, int numd, int numf, int numo,
                      int nums, int &delay, const char **reason);
int         Drop(int sent, int sinst, XrdCmsDrop *djp=0);
void        Record(char *path, const char *reason);
int         Multiple(SMask_t mVec);
enum        {eExists, eDups, eROfs, eNoRep, eNoEnt}; // Passed to SelFail
int         SelFail(XrdCmsSelect &Sel, int rc);
int         SelNode(XrdCmsSelect &Sel, SMask_t  pmask, SMask_t  amask, XrdCmsPref *prefs=NULL);
XrdCmsNode *SelbyCost(SMask_t, int &, int &, const char **, int);
XrdCmsNode *SelbyLoad(SMask_t, int &, int &, const char **, int);
XrdCmsNode *SelbyRef (SMask_t, int &, int &, const char **, int);
int         SelDFS(XrdCmsSelect &Sel, SMask_t amask,
                   SMask_t &pmask, SMask_t &smask, int isRW);
void        sendAList(XrdLink *lp);
void        setAltMan(int snum, unsigned int ipaddr, int port);

static const  int AltSize = 24; // Number of IP:Port characters per entry

XrdSysMutex   cidMutex;         // Protects to cid list
XrdOucTList  *cidFirst;         // Cluster ID to cluster number map

XrdSysMutex   XXMutex;          // Protects cluster summary state variables
XrdSysMutex   STMutex;          // Protects all node information  variables
XrdCmsNode   *NodeTab[STMax];   // Current  set of nodes

int           STHi;             // NodeTab high watermark
int           doReset;          // Must send reset event to Managers[resetMask]
long long     SelWcnt;          // Curr  number of r/w selections (successful)
long long     SelWtot;          // Total number of r/w selections (successful)
long long     SelRcnt;          // Curr  number of r/o selections (successful)
long long     SelRtot;          // Total number of r/o selections (successful)
long long     SelTcnt;          // Total number of all selections

// The following is a list of IP:Port tokens that identify supervisor nodes.
// The information is sent via the try request to redirect nodes; as needed.
// The list is alays rotated by one entry each time it is sent.
//
char          AltMans[STMax*AltSize]; // ||123.123.123.123:12345|| = 21
char         *AltMend;
int           AltMent;

// The foloowing three variables are protected by the STMutex
//
SMask_t       resetMask;        // Nodes to receive a reset event
SMask_t       peerHost;         // Nodes that are acting as peers
SMask_t       peerMask;         // Always ~peerHost
};

namespace XrdCms
{
extern    XrdCmsCluster Cluster;
}
#endif
