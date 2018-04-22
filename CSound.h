#pragma once

#include "DLLMain.h"

typedef int SoundFlags;

enum SoundFlags_
{
	SndFlags_None = 1 << 1,
	SndFlags_LocalIndex = 1 << 2,
	SndFlags_OnlyPlayerSounds = 1 << 3,
	SndFlags_OnlyInvisible = 1 << 4,
	SndFlags_ResolveIndex = 1 << 5,
};

struct SoundFrame
{
	Vector		m_vEyePos;
	int			m_iLocalIndex;
	bool		m_IsAlive;
};

struct SoundInfo
{
	Vector		m_vOrigin;
	DWORD		m_dwTime;
	char*		m_szSoundFile;
	int			m_iIndex;
	int			m_iChannel;
};

struct ResolveInfo
{
	int			m_iOrigIndex;
	int			m_iResolvIndex;
};

class CSound
{
private:
	std::deque<SoundInfo> Sound;
	std::deque<ResolveInfo> ResolveSound;

	Vector		_EyePos;
	int			_LocalIndex;
	bool		_Alive;

	bool		IsValid (int idx, int ent_channel);
	bool		IsLocal (int idx);
	bool		IsVisible (Vector sound_origin);

	int			ResolveIndex (int idx,Vector sound_origin);

public:
	std::deque<SoundInfo>& GetSound ();

	void		NewFrame (SoundFrame frame);
	void		Push (int idx, int ent_channel, char* sound_file, float* origin, SoundFlags flags = SndFlags_None);
	void		Update (DWORD max_valid_time = 1000);

	void		Clear ();
};

extern std::unique_ptr<CSound> m_pSound;