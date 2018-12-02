#pragma semicolon 1
#include <SoundFileInfo>

public void OnPluginStart()
{
	RegConsoleCmd("sm_sndinfo", sm_sndinfo);
}

public Action sm_sndinfo(int iClient, int args)
{
	char sPath[PLATFORM_MAX_PATH], sPathFull[PLATFORM_MAX_PATH];
	
	GetCmdArg(1, sPath, sizeof(sPath));
	
	FormatEx(sPathFull, sizeof sPathFull, "sound/%s", sPath);
	if(!FileExists(sPathFull))
	{
		PrintToServer("File not found '%s'", sPathFull);
		return Plugin_Handled;
	}
	
	int iLen = strlen(sPath);
	if(!(iLen > 4 && (strcmp(sPath[iLen-4], ".wav", false) || strcmp(sPath[iLen-4], ".mp3", false))))
	{
		PrintToServer("File '%s' has an unknown extension.", sPathFull);
		return Plugin_Handled;
	}
	
	SoundFile soundfile = new SoundFile(sPath);
	
	if(soundfile == null)
	{
		PrintToServer("Invalid handle!");
		return Plugin_Handled;
	}
	
	PrintToServer("GetDuration - %f", soundfile.GetDuration());
	PrintToServer("GetBitRate - %d", soundfile.GetBitRate());
	PrintToServer("GetSamplingRate - %d", soundfile.GetSamplingRate());
	PrintToServer("GetGetChannels - %d", soundfile.GetChannels());
	PrintToServer("GetTrack - %d", soundfile.GetTrack());
	PrintToServer("GetYear - %d", soundfile.GetYear());
	
	PrintToServer("IsSet SF_Title - %d", soundfile.IsSet(SF_Title));
	PrintToServer("IsSet SF_Artist - %d", soundfile.IsSet(SF_Artist));
	PrintToServer("IsSet SF_Album - %d", soundfile.IsSet(SF_Album));
	PrintToServer("IsSet SF_Comment - %d", soundfile.IsSet(SF_Comment));
	PrintToServer("IsSet SF_Genre - %d", soundfile.IsSet(SF_Genre));
	
	char sBuf[64];
	if(soundfile.GetTitle(sBuf, sizeof sBuf))
	{
		PrintToServer("GetTitle - %s", sBuf);
	}
	if(soundfile.GetArtist(sBuf, sizeof sBuf))
	{
		PrintToServer("GetArtist - %s", sBuf);
	}
	if(soundfile.GetAlbum(sBuf, sizeof sBuf))
	{
		PrintToServer("GetAlbum - %s", sBuf);
	}
	if(soundfile.GetComment(sBuf, sizeof sBuf))
	{
		PrintToServer("GetComment - %s", sBuf);
	}
	if(soundfile.GetGenre(sBuf, sizeof sBuf))
	{
		PrintToServer("GetGenre - %s", sBuf);
	}
	
	delete soundfile;
	
	return Plugin_Handled;
}