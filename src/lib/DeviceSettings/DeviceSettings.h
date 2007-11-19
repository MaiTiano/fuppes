/***************************************************************************
 *            DeviceSettings.h
 * 
 *  FUPPES - Free UPnP Entertainment Service
 *
 *  Copyright (C) 2007 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
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

#ifndef _DEVICESETTINGS_H
#define _DEVICESETTINGS_H

#include <string>
#include <list>
#include <map>

#include "../ContentDirectory/ContentDatabase.h"

struct CImageSettings {
  
  friend class CConfigFile;
  
  CImageSettings();
  CImageSettings(CImageSettings* pImageSettings);
  
  std::string  sExt;
  std::string  sMimeType;  
  
  // dcraw
  bool    bDcraw;
  std::string  sDcrawParams;

  std::string Extension() { return sExt; }
  std::string MimeType() { return sMimeType; }
  
  bool Enabled() { return bEnabled; }
  
  // ImageMagick
  bool bGreater;
	bool bLess;
	int  nWidth;
	int  nHeight;
	enum { resize, scale } nResizeMethod; // resize = better quality (lower) | scale = lower quality (faster)

  bool Greater() { return bGreater; }
  bool Less() { return bLess; }
  int  Width() { return nWidth; }
  int  Height() { return nHeight; }
  
  private:
    bool bEnabled;
};

typedef struct {
  bool bShowChildCountInTitle;
  int  nMaxFileNameLength;
} DisplaySettings_t;

typedef enum TRANSCODING_TYPE {
  TT_NONE,
  TT_RENAME,
  TT_THREADED_DECODER_ENCODER,
  TT_TRANSCODER,
  TT_THREADED_TRANSCODER  
} TRANSCODING_TYPE;

typedef enum TRANSCODER_TYPE {
  TTYP_NONE,
  TTYP_IMAGE_MAGICK,
  TTYP_FFMPEG
} TRANSCODER_TYPE;

typedef enum ENCODER_TYPE {
  ET_NONE,
  ET_LAME,
  ET_TWOLAME,
  ET_WAV,
  ET_PCM
} ENCODER_TYPE;

typedef enum DECODER_TYPE {
  DT_NONE,
  DT_OGG_VORBIS,
  DT_FLAC,
  DT_MUSEPACK,
  DT_FAAD
} DECODER_TYPE;

typedef enum TRANSCODING_HTTP_RESPONSE {
  RESPONSE_STREAM,
  RESPONSE_CHUNKED
} TRANSCODING_HTTP_RESPONSE;

/*struct CFFmpegSettings {
  
  private:
    std::string   sVcodec;
    std::string   sAcodec;
    int           nVBitRate;
    int           nVFrameRate;
    int           nABitRate;
    int           nASampleRate;
};*/

struct CTranscodingSettings {
  
    friend class CConfigFile;
  
    CTranscodingSettings();  
    CTranscodingSettings(CTranscodingSettings* pTranscodingSettings);
  
    std::string   sExt;
    std::string   sMimeType;
    std::string   sDLNA;
  
    /*std::string   sDecoder;     // vorbis | flac | mpc
    std::string   sEncoder;     // lame | twolame | pcm | wav
    std::string   sTranscoder;  // ffmpeg*/
  
    std::string   sOutParams;
  
    std::string MimeType() { return sMimeType; }
    std::string DLNA() { return sDLNA; }
    bool Enabled() { return bEnabled; }
  
    unsigned int AudioBitRate() { return nAudioBitRate; }
    unsigned int AudioSampleRate() { return nAudioSampleRate; }
    unsigned int VideoBitRate() { return nVideoBitRate; }
  
    std::string  AudioCodec(std::string p_sACodec = "");
    std::string  VideoCodec(std::string p_sVCodec = "");
  
    std::string  FFmpegParams() { return sFFmpegParams; }
  
    int LameQuality() { return nLameQuality; }
  
    std::string  Extension() { return sExt; }
    TRANSCODING_HTTP_RESPONSE   TranscodingHTTPResponse() { return nTranscodingResponse; }
  
    int ReleaseDelay() { return nReleaseDelay; }
  
    //std::list<CFFmpegSettings*>   pFFmpegSettings;
  
    TRANSCODING_TYPE TranscodingType() { return nTranscodingType; }
    TRANSCODER_TYPE TranscoderType() { return nTranscoderType; }
    DECODER_TYPE    DecoderType() { return nDecoderType; }
    ENCODER_TYPE    EncoderType() { return nEncoderType; }
  
    bool  DoTranscode(std::string p_sACodec, std::string p_sVCodec);
  
  private:
    bool          bEnabled;
  
    TRANSCODING_HTTP_RESPONSE   nTranscodingResponse;
    TRANSCODING_TYPE            nTranscodingType;
    TRANSCODER_TYPE             nTranscoderType;
    DECODER_TYPE                nDecoderType;
    ENCODER_TYPE                nEncoderType;
    int                         nReleaseDelay;
  
    unsigned int  nAudioBitRate;
    unsigned int  nAudioSampleRate;
  
    unsigned int  nVideoBitRate;
  
    int           nLameQuality;
  
    std::string     sACodecCondition;
    std::string     sVCodecCondition;
    std::string     sACodec;
    std::string     sVCodec;
  
    std::string     sFFmpegParams;
};

struct CFileSettings {
  
  friend class CConfigFile;
  
  CFileSettings();
  CFileSettings(CFileSettings* pFileSettings);
  
  std::string   MimeType(std::string p_sACodec = "", std::string p_sVCodec = "");
  std::string   DLNA();
  
  unsigned int  TargetAudioSampleRate();
  unsigned int  TargetAudioBitRate();
  
  std::string   Extension(std::string p_sACodec = "", std::string p_sVCodec = "");
  
  TRANSCODING_HTTP_RESPONSE   TranscodingHTTPResponse();

  
  bool Enabled() { return bEnabled; }  
  bool ExtractMetadata() { return bExtractMetadata; }
  
  CTranscodingSettings* pTranscodingSettings;
  CImageSettings*       pImageSettings;
  
  OBJECT_TYPE   ObjectType() { return nType; }
  std::string   ObjectTypeAsStr();
  
  int ReleaseDelay();
  
  private:
    bool  bEnabled;
    bool  bExtractMetadata;
  
    std::string   sExt;
    OBJECT_TYPE   nType;
    std::string   sMimeType;
    std::string   sDLNA;  
};

typedef std::map<std::string, CFileSettings*>::iterator FileSettingsIterator_t;

struct CMediaServerSettings
{
	std::string		FriendlyName;
	std::string 	Manufacturer;
	std::string 	ManufacturerURL;
	std::string 	ModelName;
	std::string 	ModelNumber;
	std::string 	ModelURL;
	std::string		ModelDescription;
	bool					UseModelDescription;
	std::string 	SerialNumber;
	bool					UseSerialNumber;
	std::string		UPC;
	bool					UseUPC;
	bool					UseDLNA;

	bool					UseURLBase;
	bool					UseXMSMediaReceiverRegistrar;
};

class CDeviceSettings
{
  friend class CConfigFile;
  friend class CDeviceIdentificationMgr;
  
  public:
	  CDeviceSettings(std::string p_sDeviceName);
    CDeviceSettings(std::string p_sDeviceName, CDeviceSettings* pSettings);
		
		bool HasUserAgent(std::string p_sUserAgent);
    bool HasIP(std::string p_sIPAddress);	
    /*std::list<std::string> m_slUserAgents;
		std::list<std::string> m_slIPAddresses;*/

    OBJECT_TYPE       ObjectType(std::string p_sExt);
    std::string       ObjectTypeAsStr(std::string p_sExt);

    bool              DoTranscode(std::string p_sExt, std::string p_sACodec = "", std::string p_sVCodec = "");
    TRANSCODING_TYPE  GetTranscodingType(std::string p_sExt);
    TRANSCODER_TYPE   GetTranscoderType(std::string p_sExt, std::string p_sACodec = "", std::string p_sVCodec = "");
    DECODER_TYPE      GetDecoderType(std::string p_sExt);
    ENCODER_TYPE      GetEncoderType(std::string p_sExt);
  
    std::string   MimeType(std::string p_sExt, std::string p_sACodec = "", std::string p_sVCodec = "");
    std::string   DLNA(std::string p_sExt);
  
    unsigned int  TargetAudioSampleRate(std::string p_sExt);
    unsigned int  TargetAudioBitRate(std::string p_sExt);
    
    bool          Exists(std::string p_sExt);
    std::string   Extension(std::string p_sExt, std::string p_sACodec = "", std::string p_sVCodec = "");
    TRANSCODING_HTTP_RESPONSE TranscodingHTTPResponse(std::string p_sExt);
  
    int ReleaseDelay(std::string p_sExt);
    
    DisplaySettings_t* DisplaySettings() { return &m_DisplaySettings; }
    CFileSettings* FileSettings(std::string p_sExt);
    void AddExt(CFileSettings* pFileSettings, std::string p_sExt);
    
		CMediaServerSettings* MediaServerSettings() { return &m_MediaServerSettings; }
		
    bool         EnableDeviceIcon() { return m_bEnableDeviceIcon; }  
    bool         Xbox360Support() { return m_bXBox360Support; }    
		bool         ShowPlaylistAsContainer() { return m_bShowPlaylistAsContainer; }		
    bool         DLNAEnabled() { return m_bDLNAEnabled; }
    
    std::string  VirtualFolderDevice() { return m_sVirtualFolderDevice; }
  
  private:
    std::string m_sDeviceName;
    std::string m_sVirtualFolderDevice;
    
    DisplaySettings_t m_DisplaySettings;
		CMediaServerSettings m_MediaServerSettings;
		
		bool m_bShowPlaylistAsContainer;
		bool m_bXBox360Support;
    bool m_bDLNAEnabled;  
    bool m_bEnableDeviceIcon;
    
    std::map<std::string, CFileSettings*> m_FileSettings;
    std::map<std::string, CFileSettings*>::iterator m_FileSettingsIterator;
  
    int nDefaultReleaseDelay;
    
    std::list<std::string> m_slUserAgents;
		std::list<std::string> m_slIPAddresses;
};

#endif // _DEVICESETTINGS_H
