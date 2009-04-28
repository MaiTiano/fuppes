/***************************************************************************
 *            FileAlterationMonitor.cpp
 *
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007-2008 Ulrich Völkel <fuppes@ulrich-voelkel.de>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "FileAlterationMonitor.h"

#ifdef HAVE_INOTIFY
#include <sys/ioctl.h>
#include <sys/inotify.h>
#endif

#include <iostream>
using namespace std;

CFileAlterationMgr* CFileAlterationMgr::m_Instance = 0;

CFileAlterationMgr* CFileAlterationMgr::Shared()
{
  if(m_Instance == 0) {
    m_Instance = new CFileAlterationMgr();
  }
  return m_Instance;  
}

CFileAlterationMonitor* CFileAlterationMgr::CreateMonitor(IFileAlterationMonitor* pEventHandler)
{
  CFileAlterationMonitor* pResult = NULL;
  
  #if defined(HAVE_INOTIFY)
  pResult = new CInotifyMonitor(pEventHandler);
  #endif
  
  // if no real monitor is available we return a
  // dummy monitor that does nothing but the
  // content database does not need to care wether
  // fam is available or not
  if(pResult == NULL) {
    pResult = new CDummyMonitor(pEventHandler);
  }
  
  return pResult;
}

#ifdef HAVE_INOTIFY

fuppesThreadCallback WatchLoop(void* arg);

CInotifyMonitor::CInotifyMonitor(IFileAlterationMonitor* pEventHandler):
  CFileAlterationMonitor(pEventHandler)
{
  m_pInotify = new Inotify();
  m_monitorThread = (fuppesThread)NULL;
  m_active = true;
}

CInotifyMonitor::~CInotifyMonitor()
{
 /* std::list<InotifyWatch*>::iterator iter;  
  for(iter = m_lWatches.begin(); iter != m_lWatches.end(); iter++) {
    //inotify_rm_watch(m_nInotifyFd, iter);
  }*/

  fuppesThreadCancel(m_monitorThread);
  fuppesThreadClose(m_monitorThread);
  
  delete m_pInotify;
}
  
bool CInotifyMonitor::addWatch(std::string path)
{
  appendTrailingSlash(&path);
  
	//p_sDirectory = p_sDirectory.substr(0, p_sDirectory.length()-1);
	//cout << "create watch: " << path << endl;
  
  if(m_watches.find(path) != m_watches.end()) {
    //cout << "watch already exists: " << path << endl;
    return false;
  }
  
  try {     // IN_UNMOUNT
		InotifyWatch* pWatch = new InotifyWatch(path, IN_CREATE | IN_DELETE | IN_MOVE | IN_CLOSE_WRITE); // IN_MODIFY 
    m_pInotify->Add(pWatch);
    m_watches[path] = pWatch;
  }
  catch(InotifyException &ex) {
    cout << "exception: " << ex.GetMessage() << endl;
  }
  
  if(!m_monitorThread) {
    fuppesThreadStart(m_monitorThread, WatchLoop);
  }

  return true;
}
  
void CInotifyMonitor::removeWatch(std::string path)
{
  appendTrailingSlash(&path);
  //cout << "remove watch: " << path << endl;
  
  std::map<std::string, InotifyWatch*>::iterator iter;
  if((iter = m_watches.find(path)) == m_watches.end()) {
    //cout << "watch not found: " << path << endl;
    return;
  }
  
  m_pInotify->Remove(iter->second);
  delete iter->second;  
  m_watches.erase(iter);  
}

void CInotifyMonitor::moveWatch(std::string fromPath, std::string toPath)
{
  appendTrailingSlash(&fromPath);
  appendTrailingSlash(&toPath);

  //cout << "move watch: " << fromPath << " to: " << toPath << endl;
  
  removeWatch(fromPath);
  addWatch(toPath);
}

fuppesThreadCallback WatchLoop(void* arg)    
{
  CInotifyMonitor* pInotify = (CInotifyMonitor*)arg;
  InotifyEvent event;
  
  
  std::string eventPath;
  int         movedFromCookie = 0;
  std::string movedFromPath;
  std::string movedFromFile;
  bool        movedFromIsDir = false;
  
  CFileAlterationEvent   famEvent;
  // path, event
  std::map<std::string, CFileAlterationEvent*>            events;
  std::map<std::string, CFileAlterationEvent*>::iterator  eventsIter;  
  
  while(true) {
  
    //cout << "wait for events" << endl;
    
    try {    
      pInotify->m_pInotify->WaitForEvents();
    }
    catch(InotifyException &ex) {
      cout << "exception" << ex.GetMessage() << endl;
    }
    
    //cout << "got " << pInotify->m_pInotify->GetEventCount() << " events" << endl;
    
    while(pInotify->m_pInotify->GetEvent(&event)) {
      
      if(event.IsType(IN_IGNORED)) {
        cout << "inotify: IN_IGNORED" << endl;        
        /*string sDump;
        event.DumpTypes(sDump);
        cout << "cookie: " << event.GetCookie() << " mask: " << sDump << endl;*/        
        continue;
      } 
      
      FAM_EVENT_TYPE  type;
      eventPath = event.GetWatch()->GetPath() + event.GetName();            
      
			// check if we have a pending MOVED_FROM event and
      // throw a delete if the current event is no MOVED_TO.
      // I assume that a MOVED_FROM is directly followed
      // by a MOVED_TO if moved inside watched dirs.
      // I hope this assumption is correct!?
      if(movedFromCookie > 0 && !event.IsType(IN_MOVED_TO)) {       
        famEvent.m_type  = FAM_DELETE;
        famEvent.m_path  = movedFromPath;
        famEvent.m_file  = movedFromFile;        
        famEvent.m_isDir = movedFromIsDir;
        if(movedFromIsDir) {          
    			pInotify->removeWatch(movedFromPath + movedFromFile);          
        }
        pInotify->FamEvent(&famEvent);        
        movedFromCookie = 0;
      }
      
      
      // IN_CREATE
			if(event.IsType(IN_CREATE)) {        
        //cout << "object created: " << eventPath << " [NEW]" << endl;        

        // directories just send a CREATE event so we can
        // throw the event right now
        if(event.IsType(IN_ISDIR)) {
   				pInotify->addWatch(eventPath);
          
          famEvent.m_type  = FAM_CREATE;
          famEvent.m_isDir = true;
          famEvent.m_path  = event.GetWatch()->GetPath();
          famEvent.m_file  = event.GetName();            
                 
          pInotify->FamEvent(&famEvent);
        }
        // the file's CREATE event is always followed by a
        // CLOSE event. therefore we queue it and wait for CLOSE
        else {
          events[eventPath] = new CFileAlterationEvent();
          events[eventPath]->m_type  = FAM_CREATE;
          events[eventPath]->m_isDir = false;
          events[eventPath]->m_path  = event.GetWatch()->GetPath();
          events[eventPath]->m_file  = event.GetName();
        }        
        
			} // IN_CREATE
      
      // IN_DELETE
      else if(event.IsType(IN_DELETE)) {
        //cout << "object deleted: " << eventPath << endl;
        
        famEvent.m_type = FAM_DELETE;
        famEvent.m_path = event.GetWatch()->GetPath();
        famEvent.m_file = event.GetName();        
        if(event.IsType(IN_ISDIR)) {
          famEvent.m_isDir = true;
    			pInotify->removeWatch(eventPath);          
        }

        pInotify->FamEvent(&famEvent);
      } // IN_DELETE        

      // IN_CLOSE_WRITE
      else if(event.IsType(IN_CLOSE_WRITE)) {
        //cout << "object closed: " << eventPath << " cookie: " << event.GetCookie() << endl;
        
        // check if there is a create event and throw it ...
        eventsIter = events.find(eventPath);
        if(eventsIter != events.end()) {
          
          CFileAlterationEvent* evt = eventsIter->second;
          pInotify->FamEvent(evt);
          delete evt;
          events.erase(eventsIter); 
        }
        // ... else we have a (file) modify event
        else {
          famEvent.m_type  = FAM_MODIFY;
          famEvent.m_isDir = false;
          famEvent.m_path  = event.GetWatch()->GetPath();
          famEvent.m_file  = event.GetName();
          pInotify->FamEvent(&famEvent);
        }
        
      } // IN_CLOSE_WRITE
      
      
      // IN_MOVED_FROM
      else if(event.IsType(IN_MOVED_FROM)) {        
        //cout << "object moved from: " << eventPath << " [NEW]" << endl;

        movedFromCookie = event.GetCookie();
        movedFromPath   = event.GetWatch()->GetPath();
        movedFromFile   = event.GetName();
        movedFromIsDir  = event.IsType(IN_ISDIR);
      } // IN_MOVED_FROM
      
      // IN_MOVED_TO
      else if(event.IsType(IN_MOVED_TO)) {

        //cout << "object moved to: " << eventPath << " [NEW]" << endl;
        
        if(event.GetCookie() == movedFromCookie) {
          //cout << "moved already watched object from : " << movedFromPath + movedFromFile << " to: " << eventPath << " [MOVE]" << endl;            

          famEvent.m_type  = FAM_MOVE;
          famEvent.m_isDir = event.IsType(IN_ISDIR);
          famEvent.m_path  = event.GetWatch()->GetPath();
          famEvent.m_file  = event.GetName();
          famEvent.m_oldPath  = movedFromPath;
          famEvent.m_oldFile  = movedFromFile;          
          pInotify->FamEvent(&famEvent);          

          movedFromCookie = 0;
        }
        else {
          //cout << "new object moved in to: " << eventPath << " [NEW]" << endl;
          
          famEvent.m_type = (event.IsType(IN_ISDIR) ? (FAM_CREATE | FAM_MOVE) : FAM_CREATE);          
          famEvent.m_isDir = event.IsType(IN_ISDIR);
          famEvent.m_path  = event.GetWatch()->GetPath();
          famEvent.m_file  = event.GetName();                 
                     
          if(event.IsType(IN_ISDIR)) {
            pInotify->addWatch(eventPath); 
          }
          pInotify->FamEvent(&famEvent); 
        }

      }  // IN_MOVED_TO   
      

    } // while getEvents
    
  }
  
  fuppesThreadExit();
}
  
#endif // HAVE_INOTIFY


#ifdef WIN32
fuppesThreadCallback WatchLoop(void* arg);

CWindowsFileMonitor::CWindowsFileMonitor(IFileAlterationMonitor* pEventHandler)
:CFileAlterationMonitor(pEventHandler)
{
  m_monitorThread = (fuppesThread)NULL;
  m_active = true;
}

CWindowsFileMonitor::~CWindowsFileMonitor()
{
  fuppesThreadCancel(m_monitorThread);
  fuppesThreadClose(m_monitorThread);
}
  
bool CWindowsFileMonitor::addWatch(std::string path)
{
  appendTrailingSlash(&path);
  
	//p_sDirectory = p_sDirectory.substr(0, p_sDirectory.length()-1);
	cout << "create watch: " << path << endl;
  
 /* if(m_watches.find(path) != m_watches.end()) {
    cout << "watch already exists: " << path << endl;
    return false;
  }
  
  try {     // IN_UNMOUNT
		InotifyWatch* pWatch = new InotifyWatch(path, IN_CREATE | IN_DELETE | IN_MOVE | IN_CLOSE_WRITE); // IN_MODIFY 
    m_pInotify->Add(pWatch);
    m_watches[path] = pWatch;
  }
  catch(InotifyException &ex) {
    cout << "exception: " << ex.GetMessage() << endl;
  }*/
  
  if(!m_monitorThread) {
    fuppesThreadStart(m_monitorThread, WatchLoop);
	}
  return true;
}
  
void CWindowsFileMonitor::removeWatch(std::string path)
{
  appendTrailingSlash(&path);
  cout << "remove watch: " << path << endl;
  
/*  std::map<std::string, InotifyWatch*>::iterator iter;
  if((iter = m_watches.find(path)) == m_watches.end()) {
    cout << "watch not found: " << path << endl;
    return;
  }
  
  m_pInotify->Remove(iter->second);
  delete iter->second;  
  m_watches.erase(iter);  */
}

void CWindowsFileMonitor::moveWatch(std::string fromPath, std::string toPath)
{
  appendTrailingSlash(&fromPath);
  appendTrailingSlash(&toPath);

  cout << "move watch: " << fromPath << " to: " << toPath << endl;
  
  removeWatch(fromPath);
  addWatch(toPath);
}

fuppesThreadCallback WatchLoop(void* arg)    
{
  CWindowsFileMonitor* monitor = (CWindowsFileMonitor*)arg;
	
	
	fuppesThreadExit();
}

#endif
