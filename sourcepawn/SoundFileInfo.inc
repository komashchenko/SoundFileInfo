#if defined _SoundFileInfo_included
  #endinput
#endif
#define _SoundFileInfo_included



enum SF_Data
{
	SF_Title = 0,
	SF_Artist,
	SF_Album,
	SF_Comment,
	SF_Genre
}

methodmap SoundFile < Handle
{
	/**
	 * Создает новый экземпляр SoundFile.
	 * Когда он больше вам не нужен вы должны удалить его через CloseHandle() или delete.
	 *	-
	 * @param sPath			Путь к звуковому файлу, относительно папки sound.
	 *	-
	 * @return				Возвращает SoundFile.
	 *	-
	 * @error 				Невозможно открыть файл или неизвестное разрешение файла (не .wav или .mp3).
	*/
	public native SoundFile(const char[] sPath);
	
	
	
	/**
	 * Получает продолжительность трека.
	 *	-
	 * @return				Продолжительность, если не удалось получить вернет -1.0.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native float GetDuration();
	
	/**
	 * Получает BitRate трека.
	 *	-
	 * @return				BitRate, если не удалось получить вернет -1.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native int GetBitRate();
	
	/**
	 * Получает SamplingRate трека.
	 *	-
	 * @return				SamplingRate, если не удалось получить вернет -1.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native int GetSamplingRate();
	
	/**
	 * Получает количество аудиоканалов трека.
	 *	-
	 * @return				Количество аудиоканалов, если не удалось получить вернет -1.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native int GetChannels();
	
	/**
	 * Получает номер трека.
	 *	-
	 * @return				Номер, если не удалось получить или не установлен вернет -1.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native int GetTrack();
	
	/**
	 * Получает год трека.
	 *	-
	 * @return				Год, если не удалось получить или не установлен вернет -1.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native int GetYear();
	
	
	
	/**
	 * Проверяет наличие указанных данных.
	 *	-
	 * @param sfd			Проверяемые данные (Смотрите SF_Data).
	 *	-
	 * @return				true если данные есть, false если не удалось получить или данных нет.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native bool IsSet(SF_Data sfd);
	
	/**
	 * Получает название трека.
	 *	-
	 * @param sBuf			Строка в которую будет записано название.
	 * @param iMaxlength	Максимальная длина sBuf.
	 *	-
	 * @return				true если название получено, false если не удалось получить.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native bool GetTitle(char[] sBuf, int iMaxlength);
	
	/**
	 * Получает исполнителя трека.
	 *	-
	 * @param sBuf			Строка в которую будет записан исполнитель.
	 * @param iMaxlength	Максимальная длина sBuf.
	 *	-
	 * @return				true если исполнитель получен, false если не удалось получить.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native bool GetArtist(char[] sBuf, int iMaxlength);
	
	/**
	 * Получает альбом трека.
	 *	-
	 * @param sBuf			Строка в которую будет записан альбом.
	 * @param iMaxlength	Максимальная длина sBuf.
	 *	-
	 * @return				true если альбом получен, false если не удалось получить.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native bool GetAlbum(char[] sBuf, int iMaxlength);
	
	/**
	 * Получает комментарий к треку.
	 *	-
	 * @param sBuf			Строка в которую будет записан комментарий.
	 * @param iMaxlength	Максимальная длина sBuf.
	 *	-
	 * @return				true если комментарий получен, false если не удалось получить.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native bool GetComment(char[] sBuf, int iMaxlength);
	
	/**
	 * Получает жанр трека.
	 *	-
	 * @param sBuf			Строка в которую будет записан жанр.
	 * @param iMaxlength	Максимальная длина sBuf.
	 *	-
	 * @return				true если жанр получен, false если не удалось получить.
	 *	-
	 * @error				Невалидный SoundFile.
	*/
	public native bool GetGenre(char[] sBuf, int iMaxlength);
};



public Extension __ext_SoundFileInfo = 
{
	name = "SoundFileInfo",
	file = "SoundFileInfo.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public __ext_SoundFileInfo_SetNTVOptional()
{
	MarkNativeAsOptional("SoundFile.SoundFile");
	MarkNativeAsOptional("SoundFile.GetDuration");
	MarkNativeAsOptional("SoundFile.GetBitRate");
	MarkNativeAsOptional("SoundFile.GetSamplingRate");
	MarkNativeAsOptional("SoundFile.GetChannels");
	MarkNativeAsOptional("SoundFile.GetTrack");
	MarkNativeAsOptional("SoundFile.GetYear");
	MarkNativeAsOptional("SoundFile.IsSet");
	MarkNativeAsOptional("SoundFile.GetTitle");
	MarkNativeAsOptional("SoundFile.GetArtist");
	MarkNativeAsOptional("SoundFile.GetAlbum");
	MarkNativeAsOptional("SoundFile.GetComment");
	MarkNativeAsOptional("SoundFile.GetGenre");
}
#endif