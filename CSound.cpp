#include "CSound.h"

bool CSound::IsValid (int idx, int ent_channel)
{
	if (idx == 0)
		return false;

	if (idx == -1)
		return false;

	if (ent_channel == 0)
		return false;

	return true;
}

bool CSound::IsLocal (int idx)
{
	if (idx != _LocalIndex)
		return false;

	return true;
}

bool CSound::IsVisible (Vector sound_origin)
{
	pmtrace_t tr;

	g_pEngine->pEventAPI->EV_SetTraceHull (2);
	g_pEngine->pEventAPI->EV_PlayerTrace (_EyePos, sound_origin, PM_WORLD_ONLY, -1, &tr);

	return (bool)(tr.fraction == 1.f);
}

int CSound::ResolveIndex (int idx, Vector sound_origin)
{
	if (ResolveSound.size ())
	{
		for (ResolveInfo resolve : ResolveSound)
		{
			if (resolve.m_iOrigIndex == idx)
				return resolve.m_iResolvIndex;
		}
	}

	for (int i = 1; i <= g_pEngine->GetMaxClients (); ++i)
	{
		cl_entity_s* ent = g_pEngine->GetEntityByIndex (i);

		float fDist = sound_origin.Distance (ent->origin);

		if (fDist < 16.f)
		{
			ResolveInfo resolve;
			resolve.m_iOrigIndex = idx;
			resolve.m_iResolvIndex = i;

			ResolveSound.push_back (resolve);

			return i;
		}
	}

	return -1;
}

std::deque<SoundInfo>& CSound::GetSound ()
{
	return Sound;
}

void CSound::NewFrame (SoundFrame frame)
{
	_LocalIndex = frame.m_iLocalIndex;
	_EyePos = frame.m_vEyePos;
	_Alive = frame.m_IsAlive;
}

void CSound::Push (int idx, int ent_channel, char* sound_file, float* origin, SoundFlags flags)
{
	if (!IsValid (idx, ent_channel))
		return;

	cl_entity_s* pLocal = g_pEngine->GetLocalPlayer ();

	if (!_Alive && pLocal->curstate.iuser1 == OBS_IN_EYE && pLocal->curstate.iuser2 == idx)
		return;

	if (!(flags & SndFlags_LocalIndex) && IsLocal (idx))
		return;

	if (flags & SndFlags_OnlyPlayerSounds && !strstr (sound_file, "player"))
		return;

	Vector sound_origin = origin;

	if (sound_origin.IsZero ())
		return;

	if (flags & SndFlags_OnlyInvisible && IsVisible (sound_origin))
		return;

	if (flags & SndFlags_ResolveIndex && idx > MAX_CLIENTS)
	{
		int resolv_idx = ResolveIndex (idx, sound_origin);

		if (resolv_idx != -1 && resolv_idx > 0 && resolv_idx <= MAX_CLIENTS)
			idx = resolv_idx;
	}

	SoundInfo sound_info;
	sound_info.m_vOrigin = sound_origin;
	sound_info.m_iIndex = idx;
	sound_info.m_iChannel = ent_channel;
	sound_info.m_szSoundFile = sound_file;
	sound_info.m_dwTime = GetTickCount ();

	Sound.push_back (sound_info);
}

void CSound::Update (DWORD max_valid_time)
{
	while (Sound.size () && (GetTickCount () - Sound.front ().m_dwTime) >= max_valid_time)
		Sound.pop_front ();
}

void CSound::Clear ()
{
	ResolveSound.clear ();
	Sound.clear ();
}

std::unique_ptr<CSound> m_pSound = std::make_unique<CSound> ();