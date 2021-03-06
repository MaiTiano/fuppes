/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/***************************************************************************
 *            DeviceIdentificationMgr.cpp
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007 Ulrich Völkel <u-voelkel@users.sourceforge.net>
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
 
#include "DeviceIdentificationMgr.h"
#include "../Log.h"
#include "../SharedConfig.h"
#include "../Configuration/DeviceMapping.h"
#include "../Common/RegEx.h"
#include "MacAddressTable.h"
#include <iostream>

using namespace fuppes;
using namespace std;
 
CDeviceIdentificationMgr* CDeviceIdentificationMgr::m_pInstance = 0;

CDeviceIdentificationMgr* CDeviceIdentificationMgr::Shared()
{
  if(m_pInstance == 0)
	  m_pInstance = new CDeviceIdentificationMgr();
	return m_pInstance;
}

CDeviceIdentificationMgr::CDeviceIdentificationMgr()
{
  m_pDefaultSettings = new CDeviceSettings("default");
}

CDeviceIdentificationMgr::~CDeviceIdentificationMgr()
{

  for(m_SettingsIt = m_Settings.begin();
      m_SettingsIt != m_Settings.end();
      ++m_SettingsIt) {
    delete *m_SettingsIt;
  }
  m_Settings.clear();
  
  delete m_pDefaultSettings;
}

void ReplaceDescriptionVars(std::string* p_sValue)
{
	string sValue = *p_sValue;
	string::size_type pos;

	// version (%v)
	while((pos = sValue.find("%v")) != string::npos) {
		sValue = sValue.replace(pos, 2, CSharedConfig::Shared()->GetAppVersion());
	}
	// short name (%s)
	while((pos = sValue.find("%s")) != string::npos) {
		sValue = sValue.replace(pos, 2, CSharedConfig::Shared()->GetAppName());
	}	
	// hostname (%h)
	while((pos = sValue.find("%h")) != string::npos) {
		sValue = sValue.replace(pos, 2, CSharedConfig::Shared()->networkSettings->GetHostname());
	}
  // ip address (%i)
	while((pos = sValue.find("%i")) != string::npos) {
		sValue = sValue.replace(pos, 2, CSharedConfig::Shared()->networkSettings->GetIPv4Address());
	}
		
  *p_sValue = sValue;
}

void CDeviceIdentificationMgr::Initialize()
{
	ReplaceDescriptionVars(&m_pDefaultSettings->MediaServerSettings()->FriendlyName);
	ReplaceDescriptionVars(&m_pDefaultSettings->MediaServerSettings()->ModelName);
	ReplaceDescriptionVars(&m_pDefaultSettings->MediaServerSettings()->ModelNumber);
	ReplaceDescriptionVars(&m_pDefaultSettings->MediaServerSettings()->ModelDescription);
	ReplaceDescriptionVars(&m_pDefaultSettings->MediaServerSettings()->SerialNumber);	
		
	CDeviceSettings* pSettings;
	for(m_SettingsIt = m_Settings.begin(); 
			m_SettingsIt != m_Settings.end(); 
			++m_SettingsIt)
	{
	  pSettings = *m_SettingsIt;

		ReplaceDescriptionVars(&pSettings->MediaServerSettings()->FriendlyName);
		ReplaceDescriptionVars(&pSettings->MediaServerSettings()->ModelName);
		ReplaceDescriptionVars(&pSettings->MediaServerSettings()->ModelNumber);
		ReplaceDescriptionVars(&pSettings->MediaServerSettings()->ModelDescription);
		ReplaceDescriptionVars(&pSettings->MediaServerSettings()->SerialNumber);
	}	
}

void CDeviceIdentificationMgr::IdentifyDevice(CHTTPMessage* pDeviceMessage)
{  
  assert(pDeviceMessage != NULL);
  DeviceMapping* devmap = CSharedConfig::Shared()->deviceMapping;

  string mac;
  bool foundMatch = false;
  // search the MAC Adresses first
  for(vector<struct mapping>::const_iterator it = devmap->macAddrs.begin(); !foundMatch && it != devmap->macAddrs.end(); ++it) {

    if(!MacAddressTable::mac(pDeviceMessage->GetRemoteIPAddress(), mac))
      continue;

    if (it->value.compare(mac) == 0) {
      pDeviceMessage->DeviceSettings(it->device);
      pDeviceMessage->setVirtualFolderLayout(it->vfolder);
      foundMatch = true;
    }
  }

  // Then search the IP Adresses
  for(vector<struct mapping>::const_iterator it = devmap->ipAddrs.begin(); !foundMatch && it != devmap->ipAddrs.end(); ++it) {
    if (it->value.compare(pDeviceMessage->GetRemoteIPAddress()) == 0) {
      pDeviceMessage->DeviceSettings(it->device);
      pDeviceMessage->setVirtualFolderLayout(it->vfolder);
      foundMatch = true;
    }
  }

  
  // ... just use the default device
	if(!foundMatch) {
  	pDeviceMessage->DeviceSettings(m_pDefaultSettings);

    // check if the default vfolder layout is enabled
    StringList enabled = CSharedConfig::Shared()->virtualFolders()->getEnabledFolders();
    if(enabled.indexOf("default") >= 0)    
      pDeviceMessage->setVirtualFolderLayout("default");
    else
      pDeviceMessage->setVirtualFolderLayout("");
  }

  
  // check if the requests http header contains a layout request
  // this is nothing that any real upnp rendere should have but
  // the fuppes webinterface uses it and maybe some future config gui
  // could also use this
  RegEx rxVirtualLayout("VIRTUAL-LAYOUT: *(\\w+)", PCRE_CASELESS);
  if(rxVirtualLayout.search(pDeviceMessage->GetHeader())) {
    string layout = rxVirtualLayout.match(1);

    if(layout.compare("none") == 0) {
        pDeviceMessage->setVirtualFolderLayout("");
    }
    else {    
      StringList enabled = CSharedConfig::Shared()->virtualFolders()->getEnabledFolders();
      if(enabled.indexOf(layout) >= 0)
        pDeviceMessage->setVirtualFolderLayout(layout);
    }
  }


  
  
  Log::log(Log::config, Log::extended, __FILE__, __LINE__,
    "HTTP Request using device settings \"%s\"\n\tip: %s\n\tuser agent: %s\n\tmac address: %s",
    pDeviceMessage->DeviceSettings()->m_sDeviceName.c_str(),
    pDeviceMessage->GetRemoteIPAddress().c_str(),
    pDeviceMessage->m_sUserAgent.c_str(),
    mac.c_str());
}


CDeviceSettings* CDeviceIdentificationMgr::GetSettingsForInitialization(std::string p_sDeviceName)
{
  // check if device exists
  CDeviceSettings* pSettings = NULL;
  
  if(p_sDeviceName.compare("default") == 0) {
    return m_pDefaultSettings;
  }
  
	for(m_SettingsIt = m_Settings.begin(); m_SettingsIt != m_Settings.end(); ++m_SettingsIt)	{    
    if((*m_SettingsIt)->m_sDeviceName.compare(p_sDeviceName) == 0) {
      pSettings = *m_SettingsIt;
      break;
    }
  }
  
  // no in does not - create new default settings
  if(!pSettings) {
    pSettings = new CDeviceSettings(p_sDeviceName, m_pDefaultSettings);
    m_Settings.push_back(pSettings);
  } 
    
  return pSettings;
}

void CDeviceIdentificationMgr::PrintSetting(CDeviceSettings* pSettings, std::string* p_sOut) {
  
  CFileSettings* pFileSet;
  stringstream sTmp;
  stringstream result;
  
  if(!p_sOut) {  
    cout << "device: " << pSettings->m_sDeviceName << endl;
    cout << "  release delay: " << pSettings->nDefaultReleaseDelay << endl;
    cout << "  file_settings: " << endl;
  }
  else {
  /*
    *p_sOut += "<table>";
    *p_sOut += "<tr><th colspan=\"2\">device: " + pSettings->m_sDeviceName + "</th></tr>";
    
    sTmp << pSettings->nDefaultReleaseDelay << endl;
    *p_sOut += "<tr><td>release delay</td>";
    *p_sOut += "<td>" + sTmp.str() + "</td></tr>";
    sTmp.str("");
    
    *p_sOut += "<tr><th colspan=\"2\">file settings</th></tr>";
*/

    result << "<h2>" << pSettings->m_sDeviceName << "</h2>";

    result << "<table>" <<
      "<thead>" <<
        "<tr>" <<
          "<th colspan=\"3\">" << "settings" << "</th>" <<
          "<th colspan=\"4\">" << "transcoding" << "</th>" <<
        "</tr>" <<
        "<tr>" <<
          "<th>ext</th>" <<
          "<th>mime/type</th>" <<
          "<th>objectType</th>" <<
          "<th>ext</th>" <<
          "<th>mime/type</th>" <<
          "<th></th>" <<
          "<th></th>" <<
        "</tr>" <<
      "</thead>" <<
      "<tbody>";
    
  }
  
  for(pSettings->m_FileSettingsIterator = pSettings->m_FileSettings.begin();
      pSettings->m_FileSettingsIterator != pSettings->m_FileSettings.end();
      pSettings->m_FileSettingsIterator++) {
          
    pFileSet = pSettings->m_FileSettingsIterator->second;
    
    if(!p_sOut) {        
      cout << "    ext: " << pSettings->m_FileSettingsIterator->first << endl; 
      //cout << "    dlna: " << pFileSet->DLNA() << endl;
      cout << "    mime-type: " << pFileSet->MimeType() << endl;
      cout << "    upnp-type: " << pFileSet->ObjectType() << endl;
    }
    else { 
/*
      *p_sOut += "<tr><th colspan=\"2\">ext: " + pSettings->m_FileSettingsIterator->first + " </th></tr>";
      *p_sOut += "<tr><td>dlna</td>";
      *p_sOut += "<td>" + pFileSet->DLNA() + "</td></tr>";
      *p_sOut += "<tr><td>mime/type</td>";
      *p_sOut += "<td>" + pFileSet->MimeType() + "</td></tr>";
      *p_sOut += "<tr><td>upnp type</td>";
      *p_sOut += "<td>" + pFileSet->ObjectTypeAsStr() + "</td></tr>";
*/
      
      result << "<tr>";
      result << "<td style=\"background-color: #ccffcc;\">" << pSettings->m_FileSettingsIterator->first << "</td>";
      result << "<td style=\"background-color: #ccffcc;\">" << pFileSet->originalMimeType() << "</td>";
      result << "<td style=\"background-color: #ccffcc;\">" << pFileSet->ObjectTypeAsStr() << "</td>";
    }


    if(pFileSet->pTranscodingSettings) {
      if(!p_sOut) {
        cout << "  transcode: " << endl;
        cout << "    ext: " << pFileSet->pTranscodingSettings->sExt << endl;
        //cout << "    dlna: " << pFileSet->pTranscodingSettings->sDLNA << endl;
        cout << "    mime/type: " << pFileSet->MimeType() << endl;      
        cout << "    release delay: " << pFileSet->ReleaseDelay() << endl;
      }
      else {
        /*
         *p_sOut += "<tr><th colspan=\"2\">transcode</th></tr>";
        *p_sOut += "<tr><td>ext</td>";
        *p_sOut += "<td>" + pFileSet->pTranscodingSettings->sExt + "</td></tr>";  
        *p_sOut += "<tr><td>dlna</td>";
        *p_sOut += "<td>" + pFileSet->pTranscodingSettings->DLNA() + "</td></tr>";
        *p_sOut += "<tr><td>mime/type</td>";
        *p_sOut += "<td>" + pFileSet->pTranscodingSettings->MimeType() + "</td></tr>";
        
        sTmp << pFileSet->ReleaseDelay();
        *p_sOut += "<tr><td>release delay</td>";
        *p_sOut += "<td>" + sTmp.str() + "</td></tr>";
        sTmp.str("");
        */
        
        result << "<td style=\"background-color: #ffcccc;\">" << pFileSet->pTranscodingSettings->sExt << "</td>";
        result << "<td style=\"background-color: #ffcccc;\">" << pFileSet->MimeType() << "</td>";
        result << "<td style=\"background-color: #ffcccc;\">" << "" << "</td>";
        result << "<td style=\"background-color: #ffcccc;\">" << "" << "</td>";
        //result << "<td>" << pFileSet->ReleaseDelay() << "</td>";        
      }
    }
    else if(pFileSet->pImageSettings) {
      if(!p_sOut) {
        cout << "  resize: " << endl;
        cout << "    height: " << pFileSet->pImageSettings->nHeight << endl;
        cout << "    width: " << pFileSet->pImageSettings->nWidth << endl;
        cout << "    greater: " << pFileSet->pImageSettings->bGreater << endl;
        cout << "    less: " << pFileSet->pImageSettings->bLess << endl;
      }
      else {
        /*
        *p_sOut += "<tr><th colspan=\"2\">resize</th></tr>";
        sTmp << pFileSet->pImageSettings->nHeight;
        *p_sOut += "<tr><td>height</td>";
        *p_sOut += "<td>" + sTmp.str() + "</td></tr>";
        sTmp.str("");
        sTmp << pFileSet->pImageSettings->nWidth;
        *p_sOut += "<tr><td>width</td>";
        *p_sOut += "<td>" + sTmp.str() + "</td></tr>";
        sTmp.str("");
        
        *p_sOut += "<tr><td>greater</td>";
        if(pFileSet->pImageSettings->bGreater)
          *p_sOut += "<td>true</td></tr>";
        else
          *p_sOut += "<td>false</td></tr>";
        
        *p_sOut += "<tr><td>less</td>";
        if(pFileSet->pImageSettings->bLess)
          *p_sOut += "<td>true</td></tr>";
        else
          *p_sOut += "<td>false</td></tr>";
        */

        result << "<td style=\"background-color: #ffcccc;\">" << pFileSet->pImageSettings->sExt << "</td>";
        result << "<td style=\"background-color: #ffcccc;\">" << pFileSet->MimeType() << "</td>";
        result << "<td style=\"background-color: #ffcccc;\">" << pFileSet->pImageSettings->nWidth << "</td>";
        result << "<td style=\"background-color: #ffcccc;\">" << pFileSet->pImageSettings->nHeight << "</td>";
      }
    }
    else {
      if(!p_sOut) {
        cout << "  no transcoding/resizing" << endl;
      }
      else {
        //*p_sOut += "<tr><td colspan=\"2\">no transcoding/resizing</td></tr>";
        result << "<td colspan=\"4\" style=\"background-color: #ffcccc;\">n/a</td>";
      }
    }
          
    if(!p_sOut) {
      cout << endl;
    }
    else {
      result << "</tr>";
    }
        
  } // for
  
  if(p_sOut) {
    //*p_sOut += "</table>";
    result << "</tbody></table>";
    *p_sOut = result.str();
  }
  
}

void CDeviceIdentificationMgr::PrintSettings(std::string* p_sOut)
{
  CDeviceSettings* pSettings;
  
  if(!p_sOut) {
    cout << "device settings" << endl;
  }


  string result;  
  PrintSetting(m_pDefaultSettings, &result);
  *p_sOut += result;
  
  for(m_SettingsIt = m_Settings.begin(); 
      m_SettingsIt != m_Settings.end(); 
      ++m_SettingsIt)	{    

    pSettings = *m_SettingsIt;
      
    PrintSetting(pSettings, &result);    
    *p_sOut += result;
  }
  
}
