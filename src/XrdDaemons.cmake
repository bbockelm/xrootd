
include( XRootDCommon )

#-------------------------------------------------------------------------------
# xrdcp
#-------------------------------------------------------------------------------
add_executable(
  xrootd
  XrdXrootd/XrdXrootdAdmin.cc           XrdXrootd/XrdXrootdAdmin.hh
  XrdXrootd/XrdXrootdAio.cc             XrdXrootd/XrdXrootdAio.hh
  XrdXrootd/XrdXrootdBridge.cc          XrdXrootd/XrdXrootdBridge.hh
  XrdXrootd/XrdXrootdCallBack.cc        XrdXrootd/XrdXrootdCallBack.hh
  XrdXrootd/XrdXrootdConfig.cc
  XrdXrootd/XrdXrootdFile.cc            XrdXrootd/XrdXrootdFile.hh
                                        XrdXrootd/XrdXrootdFileLock.hh
  XrdXrootd/XrdXrootdFileLock1.cc       XrdXrootd/XrdXrootdFileLock1.hh
                                        XrdXrootd/XrdXrootdFileStats.hh
  XrdXrootd/XrdXrootdJob.cc             XrdXrootd/XrdXrootdJob.hh
  XrdXrootd/XrdXrootdLoadLib.cc
                                        XrdXrootd/XrdXrootdMonData.hh
  XrdXrootd/XrdXrootdMonFile.cc         XrdXrootd/XrdXrootdMonFile.hh
  XrdXrootd/XrdXrootdMonFMap.cc         XrdXrootd/XrdXrootdMonFMap.hh
  XrdXrootd/XrdXrootdMonitor.cc         XrdXrootd/XrdXrootdMonitor.hh

  XrdXrootd/XrdXrootdPio.cc             XrdXrootd/XrdXrootdPio.hh
  XrdXrootd/XrdXrootdPrepare.cc         XrdXrootd/XrdXrootdPrepare.hh
  XrdXrootd/XrdXrootdProtocol.cc        XrdXrootd/XrdXrootdProtocol.hh
  XrdXrootd/XrdXrootdResponse.cc        XrdXrootd/XrdXrootdResponse.hh
                                        XrdXrootd/XrdXrootdStat.icc
  XrdXrootd/XrdXrootdStats.cc           XrdXrootd/XrdXrootdStats.hh
  XrdXrootd/XrdXrootdTransit.cc         XrdXrootd/XrdXrootdTransit.hh
  XrdXrootd/XrdXrootdTransPend.cc       XrdXrootd/XrdXrootdTransPend.hh
  XrdXrootd/XrdXrootdTransSend.cc       XrdXrootd/XrdXrootdTransSend.hh
  XrdXrootd/XrdXrootdXeq.cc
  XrdXrootd/XrdXrootdXeqAio.cc
                                        XrdXrootd/XrdXrootdTrace.hh
                                        XrdXrootd/XrdXrootdXPath.hh
                                        XrdXrootd/XrdXrootdReqID.hh )

target_link_libraries(
  xrootd
  XrdMain
  XrdOfs
  XrdUtils
  dl
  pthread
  ${EXTRA_LIBS}
  ${SOCKET_LIBRARY} )

#-------------------------------------------------------------------------------
# cmsd
#-------------------------------------------------------------------------------
add_executable(
  cmsd
  XrdCms/XrdCmsAdmin.cc           XrdCms/XrdCmsAdmin.hh
  XrdCms/XrdCmsBaseFS.cc          XrdCms/XrdCmsBaseFS.hh
  XrdCms/XrdCmsCache.cc           XrdCms/XrdCmsCache.hh
  XrdCms/XrdCmsCluster.cc         XrdCms/XrdCmsCluster.hh
  XrdCms/XrdCmsConfig.cc          XrdCms/XrdCmsConfig.hh
  XrdCms/XrdCmsJob.cc             XrdCms/XrdCmsJob.hh
  XrdCms/XrdCmsKey.cc             XrdCms/XrdCmsKey.hh
  XrdCms/XrdCmsManager.cc         XrdCms/XrdCmsManager.hh
  XrdCms/XrdCmsManList.cc         XrdCms/XrdCmsManList.hh
  XrdCms/XrdCmsManTree.cc         XrdCms/XrdCmsManTree.hh
  XrdCms/XrdCmsMeter.cc           XrdCms/XrdCmsMeter.hh
  XrdCms/XrdCmsNash.cc            XrdCms/XrdCmsNash.hh
  XrdCms/XrdCmsNode.cc            XrdCms/XrdCmsNode.hh
  XrdCms/XrdCmsPList.cc           XrdCms/XrdCmsPList.hh
  XrdCms/XrdCmsPrepare.cc         XrdCms/XrdCmsPrepare.hh
  XrdCms/XrdCmsPrepArgs.cc        XrdCms/XrdCmsPrepArgs.hh
  XrdCms/XrdCmsProtocol.cc        XrdCms/XrdCmsProtocol.hh
  XrdCms/XrdCmsRouting.cc         XrdCms/XrdCmsRouting.hh
  XrdCms/XrdCmsRRQ.cc             XrdCms/XrdCmsRRQ.hh
                                  XrdCms/XrdCmsSelect.hh
  XrdCms/XrdCmsState.cc           XrdCms/XrdCmsState.hh
  XrdCms/XrdCmsSupervisor.cc      XrdCms/XrdCmsSupervisor.hh
  XrdCms/XrdCmsPrefNodes.cc       XrdCms/XrdCmsPrefNodes.hh
  XrdCms/XrdCmsPref.cc            XrdCms/XrdCmsPref.hh
                                  XrdCms/XrdCmsTrace.hh )
target_link_libraries(
  cmsd
  XrdServer
  XrdMain
  XrdUtils
  pthread
  ${EXTRA_LIBS}
  ${SOCKET_LIBRARY} )

#-------------------------------------------------------------------------------
# Install
#-------------------------------------------------------------------------------
install(
  TARGETS xrootd cmsd
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

install(
  FILES
  ${PROJECT_SOURCE_DIR}/docs/man/cmsd.8
  ${PROJECT_SOURCE_DIR}/docs/man/xrootd.8
  DESTINATION ${CMAKE_INSTALL_MANDIR}/man8 )
