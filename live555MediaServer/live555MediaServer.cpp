/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2015, Live Networks, Inc.  All rights reserved
// LIVE555 Media Server
// main program

#include <BasicUsageEnvironment.hh>
#include "DynamicRTSPServer.hh"
#include "version.hh"
#include "RecvStream.hh"

int main(int argc, char** argv) {
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Create the RTSP server.  Try first with the default port number (554),
  // and then with the alternative port number (8554):
  RTSPServer* rtspServer;
  portNumBits rtspServerPortNum = 554;
  rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
  /*if (rtspServer == NULL) {
    rtspServerPortNum = 8554;
    rtspServer = DynamicRTSPServer::createNew(*env, rtspServerPortNum, authDB);
  }*/
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }
  
  int fsock = rtspServer->createMyServerSocket();
  if (fsock == -1){
    *env << "createMyServerSocket error\n";
     return 0;
  }
  if (CRecoFace::vedio_init() != 0)
  {
  	exit(2);
  }

  *env << "LIVE555 Media Server\n";
  *env << "\tversion " << MEDIA_SERVER_VERSION_STRING
       << " (LIVE555 Streaming Media library version "
       << LIVEMEDIA_LIBRARY_VERSION_STRING << ").\n";

  char* urlPrefix = rtspServer->rtspURLPrefix();
  *env << "Play streams from this server using the URL\n\t"
       << urlPrefix << "video\n";

  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  //if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
  //  *env << "(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling, or for HTTP live streaming (for indexed Transport Stream files only).)\n";
  //} else {
  //  *env << "(RTSP-over-HTTP tunneling is not available.)\n";
  //}

  env->taskScheduler().doEventLoop(); // does not return

  CRecoFace::vedio_release();
  
  return 0; // only to prevent compiler warning
}
