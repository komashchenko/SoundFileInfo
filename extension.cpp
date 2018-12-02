#include "extension.h"
#include <string.h>
#include <tag.h>
#include <wavfile.h>
#include <mpegfile.h>

SoundFileInfo		g_SoundFileInfo;
SMEXT_LINK(&g_SoundFileInfo);

SoundFileHandler	g_SoundFileHandler;
HandleType_t		g_htSoundFile;

void cp1251_to_utf8(char* out, const char* in);



bool SoundFileInfo::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	g_htSoundFile = g_pHandleSys->CreateType("SoundFile", &g_SoundFileHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);
	sharesys->AddNatives(myself, g_SoundFileNatives);
	g_pShareSys->RegisterLibrary(myself, "SoundFileInfo");
	
	return true;
}

void SoundFileInfo::SDK_OnUnload()
{
	g_pHandleSys->RemoveType(g_htSoundFile, myself->GetIdentity());
}

void SoundFileHandler::OnHandleDestroy(HandleType_t type, void* object)
{
	delete (TagLib::File*)object;
}



static cell_t SoundFile_SoundFile(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = NULL;
	char sPathFull[PLATFORM_MAX_PATH];
	char* sPath;
	pContext->LocalToString(params[1], &sPath);
	
	int iLen = strlen(sPath);
	if(iLen > 4)
	{
		g_pSM->BuildPath(Path_Game, sPathFull, sizeof(sPathFull), "sound/%s", sPath);
		if(stricmp(sPath+iLen-4, ".wav") == 0)
		{
			SoundFile = new TagLib::RIFF::WAV::File(sPathFull);
		}
		else if(stricmp(sPath+iLen-4, ".mp3") == 0)
		{
			SoundFile = new TagLib::MPEG::File(sPathFull);
		}
		else
		{
			return pContext->ThrowNativeError("File has an unknown extension.");
		}
	}
	else
	{
		return pContext->ThrowNativeError("File has an unknown extension.");
	}
	
	if(!SoundFile->isValid())
	{
		delete SoundFile;
		return pContext->ThrowNativeError("Unable to open '%s' for reading.", sPathFull);
	}
	
	Handle_t hndl = g_pHandleSys->CreateHandle(g_htSoundFile, SoundFile, pContext->GetIdentity(), myself->GetIdentity(), NULL);
	if (hndl == BAD_HANDLE)
	{
		delete SoundFile;
	}
	
	return hndl;
}

TagLib::File* GetTagLibFileFromHandle(IPluginContext* pContext, cell_t handle)
{
	HandleError err;
	HandleSecurity sec(pContext->GetIdentity(), myself->GetIdentity());
	
	TagLib::File* SoundFile = NULL;
	Handle_t hndl = static_cast<Handle_t>(handle);
	if ((err=handlesys->ReadHandle(hndl, g_htSoundFile, &sec, (void**)&SoundFile)) != HandleError_None)
	{
		pContext->ThrowNativeError("Invalid SoundFile handle %x (error %d)", hndl, err);
	}
	
	return SoundFile;
}

bool TagLibStringToLocal(IPluginContext* pContext, cell_t local_addr, size_t maxbytes, TagLib::String tlStr)
{
	if(!tlStr.isEmpty())
	{
		if(tlStr.isLatin1())
		{
			const char* str = tlStr.toCString(false);
			char* str_buf = new char[strlen(str)*2+1];
			cp1251_to_utf8(str_buf, str);
			pContext->StringToLocalUTF8(local_addr, maxbytes, str_buf, NULL);
			delete[] str_buf;
		}
		else
		{
			pContext->StringToLocalUTF8(local_addr, maxbytes, tlStr.toCString(true), NULL);
		}
		
		return true;
	}
	
	return false;
}

static cell_t SoundFile_GetDuration(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::AudioProperties *AudioProperties = SoundFile->audioProperties();
		if(AudioProperties)
		{
			return sp_ftoc(AudioProperties->lengthInMilliseconds() / 1000.f);
		}
	}
	
	return sp_ftoc(-1.f);
}

static cell_t SoundFile_GetBitRate(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::AudioProperties *AudioProperties = SoundFile->audioProperties();
		if(AudioProperties)
		{
			return AudioProperties->bitrate();
		}
	}
	
	return -1;
}

static cell_t SoundFile_GetSamplingRate(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::AudioProperties *AudioProperties = SoundFile->audioProperties();
		if(AudioProperties)
		{
			return AudioProperties->sampleRate();
		}
	}
	
	return -1;
}

static cell_t SoundFile_GetChannels(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::AudioProperties *AudioProperties = SoundFile->audioProperties();
		if(AudioProperties)
		{
			return AudioProperties->channels();
		}
	}
	
	return -1;
}

static cell_t SoundFile_GetTrack(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::Tag *tag = SoundFile->tag();
		if(tag)
		{
			unsigned int track = tag->track();
			if(track != 0)
			{
				return track;
			}
		}
	}
	
	return -1;
}

static cell_t SoundFile_GetYear(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::Tag *tag = SoundFile->tag();
		if(tag)
		{
			unsigned int year = tag->year();
			if(year != 0)
			{
				return year;
			}
		}
	}
	
	return -1;
}

static cell_t SoundFile_IsSet(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		enum SF_Data
		{
			SF_Title = 0,
			SF_Artist,
			SF_Album,
			SF_Comment,
			SF_Genre,
			SF_MAX
		};
		
		if(!(0 <= params[2] && params[2] < SF_MAX))
		{
			return pContext->ThrowNativeError("Unknown SF_Data value.");
		}
		
		TagLib::Tag *tag = SoundFile->tag();
		if(tag && !tag->isEmpty())
		{
			TagLib::String sValue;
			
			switch(params[2])
			{
				case SF_Title:
				{
					sValue = tag->title();
					break;
				}
				case SF_Artist:
				{
					sValue = tag->artist();
					break;
				}
				case SF_Album:
				{
					sValue = tag->album();
					break;
				}
				case SF_Comment:
				{
					sValue = tag->comment();
					break;
				}
				case SF_Genre:
				{
					sValue = tag->genre();
					break;
				}
			}
			
			if(!sValue.isEmpty())
			{
				return 1;
			}
		}
	}
	
	return 0;
}

static cell_t SoundFile_GetTitle(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::Tag *tag = SoundFile->tag();
		if(tag)
		{
			return TagLibStringToLocal(pContext, params[2], params[3], tag->title());
		}
	}
	
	return 0;
}

static cell_t SoundFile_GetArtist(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::Tag *tag = SoundFile->tag();
		if(tag)
		{
			return TagLibStringToLocal(pContext, params[2], params[3], tag->artist());
		}
	}
	
	return 0;
}

static cell_t SoundFile_GetAlbum(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::Tag *tag = SoundFile->tag();
		if(tag)
		{
			return TagLibStringToLocal(pContext, params[2], params[3], tag->album());
		}
	}
	
	return 0;
}

static cell_t SoundFile_GetComment(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::Tag *tag = SoundFile->tag();
		if(tag)
		{
			return TagLibStringToLocal(pContext, params[2], params[3], tag->comment());
		}
	}
	
	return 0;
}

static cell_t SoundFile_GetGenre(IPluginContext* pContext, const cell_t* params)
{
	TagLib::File* SoundFile = GetTagLibFileFromHandle(pContext, params[1]);
	if(SoundFile)
	{
		TagLib::Tag *tag = SoundFile->tag();
		if(tag)
		{
			return TagLibStringToLocal(pContext, params[2], params[3], tag->genre());
		}
	}
	
	return 0;
}

const sp_nativeinfo_t g_SoundFileNatives[] =
{
	{ "SoundFile.SoundFile",		SoundFile_SoundFile },
	{ "SoundFile.GetDuration",		SoundFile_GetDuration },
	{ "SoundFile.GetBitRate",		SoundFile_GetBitRate },
	{ "SoundFile.GetSamplingRate",	SoundFile_GetSamplingRate },
	{ "SoundFile.GetChannels",		SoundFile_GetChannels },
	{ "SoundFile.GetTrack",			SoundFile_GetTrack },
	{ "SoundFile.GetYear",			SoundFile_GetYear },
	{ "SoundFile.IsSet",			SoundFile_IsSet },
	{ "SoundFile.GetTitle",			SoundFile_GetTitle },
	{ "SoundFile.GetArtist",		SoundFile_GetArtist },
	{ "SoundFile.GetAlbum",			SoundFile_GetAlbum },
	{ "SoundFile.GetComment",		SoundFile_GetComment },
	{ "SoundFile.GetGenre",			SoundFile_GetGenre },
	{ NULL,							NULL }
};



//https://www.linux.org.ru/forum/development/3968525
void cp1251_to_utf8(char* out, const char* in)
{
	static const int table[128] = 
	{                    
		0x82D0,0x83D0,0x9A80E2,0x93D1,0x9E80E2,0xA680E2,0xA080E2,0xA180E2,
		0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,    
		0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,
		0,0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,               
		0xA0C2,0x8ED0,0x9ED1,0x88D0,0xA4C2,0x90D2,0xA6C2,0xA7C2,              
		0x81D0,0xA9C2,0x84D0,0xABC2,0xACC2,0xADC2,0xAEC2,0x87D0,              
		0xB0C2,0xB1C2,0x86D0,0x96D1,0x91D2,0xB5C2,0xB6C2,0xB7C2,              
		0x91D1,0x9684E2,0x94D1,0xBBC2,0x98D1,0x85D0,0x95D1,0x97D1,            
		0x90D0,0x91D0,0x92D0,0x93D0,0x94D0,0x95D0,0x96D0,0x97D0,
		0x98D0,0x99D0,0x9AD0,0x9BD0,0x9CD0,0x9DD0,0x9ED0,0x9FD0,
		0xA0D0,0xA1D0,0xA2D0,0xA3D0,0xA4D0,0xA5D0,0xA6D0,0xA7D0,
		0xA8D0,0xA9D0,0xAAD0,0xABD0,0xACD0,0xADD0,0xAED0,0xAFD0,
		0xB0D0,0xB1D0,0xB2D0,0xB3D0,0xB4D0,0xB5D0,0xB6D0,0xB7D0,
		0xB8D0,0xB9D0,0xBAD0,0xBBD0,0xBCD0,0xBDD0,0xBED0,0xBFD0,
		0x80D1,0x81D1,0x82D1,0x83D1,0x84D1,0x85D1,0x86D1,0x87D1,
		0x88D1,0x89D1,0x8AD1,0x8BD1,0x8CD1,0x8DD1,0x8ED1,0x8FD1
	};
	while (*in)
	{
		if (*in & 0x80)
		{
			int v = table[(int)(0x7f & *in++)];
			if (!v)
			{
                continue;
			}
			*out++ = (char)v;
			*out++ = (char)(v >> 8);
			if (v >>= 16)
			{
				*out++ = (char)v;
			}
		}
		else
		{
			*out++ = *in++;
		}
	}
	*out = 0;
}