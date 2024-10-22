/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef PSXF_GUARD_AUDIO_H
#define PSXF_GUARD_AUDIO_H

#include "psx.h"

//XA enumerations
typedef enum
{
	XA_Menu,   //MENU.XA
	XA_GameOvr,   //GAMEOVR.XA
	XA_1, //1
	XA_2, //2
	XA_3, //3
	XA_4, //4
	XA_5, //5
	XA_6, //6
	XA_7, //7
	XA_8, //8
	XA_9, //9
	XA_10, //10
	XA_11, //11
	XA_12, //12
	XA_13, //13
	XA_14, //14
	XA_15, //15
	XA_16, //16
	XA_Cutscene, //Cutscene

	XA_Max,
} XA_File;

typedef enum
{
	//MENU.XA
	XA_GettinFreaky, //Gettin' Freaky
	XA_GameOver,     //Game Over
	//GAMEOVR.XA
	XA_GameOverJ, //Game Over Jorsawsee
	XA_GameOverP, //Game Over Pico
	XA_GameOverH, //Game Over Henry
	//1.XA
	XA_SussusMoogus, //Sussus Moogus
	XA_Sabotage, //Sabotage
	//2.XA
	XA_Meltdown, //Meltdown
	XA_SussusToogus, //Sussus Toogus
	//3.XA
	XA_LightsDown, //Lights Down
	XA_Reactor, //Reactor
	//4.XA
	XA_Ejected, //Ejected
	XA_Mando, //Mando
	//5.XA
	XA_Dlow, //Dlow
	XA_Oversight, //Oversight
	//6.XA
	XA_Danger, //Danger
	XA_DoubleKill, //Double Kill
	//7.XA
	XA_Defeat, //Defeat
	XA_Finale, //Finale
	//8.XA
	XA_IdentityCrisis, //Identity Crisis
	//9.XA
	XA_Ashes, //Ashes
	XA_Magmatic, //Magmatic
	//10.XA
	XA_BoilingPoint, //Boiling Point
	XA_Delusion, //Delusion
	//11.XA
	XA_Blackout, //Blackout
	XA_Neurotic, //Neurotic
	//12.XA
	XA_Heartbeat, //Heartbeat
	XA_Pinkwave, //Pinkwave
	//13.XA
	XA_Pretender, //Pretender
	XA_SaucesMoogus, //Sauces Moogus
	//14.XA
	XA_O2, //O2
	XA_VotingTime, //Voting Time
	//15.XA
	XA_Turbulence, //Turbulence
	XA_Victory, //Victory
	//16.XA
	XA_Roomcode, //Roomcode
	//Cutscene.XA
	XA_ArmedCutscene, //Armed Cutscene
	
	XA_TrackMax,
} XA_Track;

//Audio functions
u32 Audio_GetLength(XA_Track lengthtrack);
void Audio_Init(void);
void Audio_Quit(void);
void Audio_Reset(void);
void Audio_PlayXA_Track(XA_Track track, u8 volume, u8 channel, boolean loop);
void Audio_SeekXA_Track(XA_Track track);
void Audio_PauseXA(void);
void Audio_ResumeXA(void);
void Audio_StopXA(void);
void Audio_ChannelXA(u8 channel);
s32 Audio_TellXA_Sector(void);
s32 Audio_TellXA_Milli(void);
boolean Audio_PlayingXA(void);
void Audio_WaitPlayXA(void);
void Audio_ProcessXA(void);
void findFreeChannel(void);
u32 Audio_LoadVAGData(u32 *sound, u32 sound_size);
void AudioPlayVAG(int channel, u32 addr);
void Audio_PlaySoundOnChannel(u32 addr, u32 channel, int volume);
void Audio_PlaySound(u32 addr, int volume);
u32 VAG_IsPlaying(u32 channel);
void Audio_ClearAlloc(void);

#endif
