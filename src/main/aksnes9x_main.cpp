/* aksnes9x_main.cpp
 *
 * Parameters:
 *   --hardware=normal|multitap
 */

#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

#include "aksnes9x.h"
#include "snes9x/apu/apu.h"
#include "snes9x/memmap.h"
#include "snes9x/display.h"
#include "snes9x/cheats.h"
#include "snes9x/controls.h"
#include "snes9x/snes9x.h"

extern "C" {
  #include "emuhost.h"
}

static int akps_open_file(const char *path);

static int aksnes9x_loaded=0;
static void *aksnes9x_fb=0;
static int16_t *aksnes9x_audiov=0;
static int aksnes9x_audioa=0;
static int aksnes9x_hardware=0;

/* Establish input map.
 * emuhost and snes9x both provide input mapping helpers, so we definitely don't need to.
 */
 
static int aksnes9x_map_input() {
  for (int playerid=1;playerid<=5;playerid++) {
    S9xMapButton((playerid<<16)|EH_BTN_UP,s9xjoycmd(playerid-1,SNES_UP_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_DOWN,s9xjoycmd(playerid-1,SNES_DOWN_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_LEFT,s9xjoycmd(playerid-1,SNES_LEFT_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_RIGHT,s9xjoycmd(playerid-1,SNES_RIGHT_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_SOUTH,s9xjoycmd(playerid-1,SNES_B_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_WEST,s9xjoycmd(playerid-1,SNES_Y_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_EAST,s9xjoycmd(playerid-1,SNES_A_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_NORTH,s9xjoycmd(playerid-1,SNES_X_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_AUX1,s9xjoycmd(playerid-1,SNES_START_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_AUX2,s9xjoycmd(playerid-1,SNES_SELECT_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_L1,s9xjoycmd(playerid-1,SNES_TL_MASK),0);
    S9xMapButton((playerid<<16)|EH_BTN_R1,s9xjoycmd(playerid-1,SNES_TR_MASK),0);
  }
  return 0;
}

/* Load/Save SRAM.
 */
 
static int aksnes9x_load_sram() {
  const char *path=aksnes9x_get_sram_path(0);
  if (!path) return -1;
  if (!Memory.LoadSRAM(path)) return -1;
  return 0;
}

int aksnes9x_save_sram() {
  const char *path=aksnes9x_get_sram_path(1);
  if (!path) return -1;
  if (!Memory.SaveSRAM(path)) return -1;
  printf("%s: Saved SRAM\n",path);
  return 0;
}

/* Command-line options.
 */
  
static int aksnes9x_main_option(const char *k,int kc,const char *v,int vc) {
  printf("Unexpected option '%.*s' = '%.*s'\n",kc,k,vc,v);
  return -1;
}

/* Logging.
 */

static void aksnes9x_main_log(int level,const char *msg,int msgc) {
  printf("[%d] %.*s\n",level,msgc,msg);
}

/* Receive input event.
 */

static int aksnes9x_main_event(void *userdata,int playerid,int btnid,int value,int state) {
  if ((playerid<1)||(playerid>5)) return 0;
  if (btnid&0xffff0000) return 0;
  S9xReportButton((playerid<<16)|btnid,value);
  return 0;
}

/* Set the global snes9x settings.
 * I have no idea what most of it means.
 */
 
static void aksnes9x_default_settings() {
  /*
  Settings.SuperFX=1;
  Settings.DSP=1;
  Settings.SA1=1;
  Settings.C4=1;
  Settings.SDD1=1;
  Settings.SPC7110=1;
  Settings.SPC7110RTC=1;
  Settings.OBC1=1;
  Settings.SETA=1;
  Settings.SRTC=1;
  Settings.BS=0;
  Settings.BSXItself=0;
  Settings.BSXBootup=0;
  Settings.MSU1=1;
  Settings.MouseMaster=0;
  Settings.SuperScopeMaster=0;
  Settings.JustifierMaster=0;
  Settings.MacsRifleMaster=0;
  */
  Settings.MultiPlayer5Master=1;
  
  Settings.SoundSync=0;
  Settings.SixteenBitSound=1;
  Settings.SoundPlaybackRate=44100;
  Settings.SoundInputRate=32040;
  Settings.Stereo=1;
  Settings.ReverseStereo=0;
  Settings.Mute=0;
  Settings.DynamicRateControl=0;
  Settings.DynamicRateLimit=0; /* Multiplied by 1000 */
  Settings.InterpolationMethod=0;
  
  Settings.SupportHiRes=0; // If enabled, VM has the option to emit 512 columns instead of 256. Unclear how it signals that. Anyway, emuhost expects a fixed width.
  Settings.Transparency=1;
  Settings.BG_Forced=0;
  Settings.DisableGraphicWindows=0;
  
  Settings.SkipFrames=0;
  Settings.TurboSkipFrames=0;
  Settings.AutoMaxSkipFrames=0;
  Settings.TurboMode=0;
  Settings.HighSpeedSeek=0;
  Settings.FrameAdvance=0;
  Settings.Rewinding=0;
  
#if 0
	bool8	TraceDMA;
	bool8	TraceHDMA;
	bool8	TraceVRAM;
	bool8	TraceUnknownRegisters;
	bool8	TraceDSP;
	bool8	TraceHCEvent;
	bool8	TraceSMP;
	
	bool8	ForceLoROM;
	bool8	ForceHiROM;
	bool8	ForceHeader;
	bool8	ForceNoHeader;
	bool8	ForceInterleaved;
	bool8	ForceInterleaved2;
	bool8	ForceInterleaveGD24;
	bool8	ForceNotInterleaved;
	bool8	ForcePAL;
	bool8	ForceNTSC;
	bool8	PAL;
	uint32	FrameTimePAL;
	uint32	FrameTimeNTSC;
	uint32	FrameTime;

	bool8	DisplayTime;
	bool8	DisplayFrameRate;
	bool8	DisplayWatchedAddresses;
	bool8	DisplayPressedKeys;
	bool8	DisplayMovieFrame;
	bool8	AutoDisplayMessages;
	uint32	InitialInfoStringTimeout;
	uint16	DisplayColor;
	bool8	BilinearFilter;

	bool8	Multi;
	char	CartAName[PATH_MAX + 1];
	char	CartBName[PATH_MAX + 1];

	bool8	DisableGameSpecificHacks;
	bool8	BlockInvalidVRAMAccessMaster;
	bool8	BlockInvalidVRAMAccess;
	int32	HDMATimingHack;

	bool8	ForcedPause;
	bool8	Paused;
	bool8	StopEmulation;

	bool8	NetPlay;
	bool8	NetPlayServer;
	char	ServerName[128];
	int		Port;

	bool8	MovieTruncate;
	bool8	MovieNotifyIgnored;
	bool8	WrongMovieStateProtection;
	bool8	DumpStreams;
	int		DumpStreamsMaxFrames;

	bool8	TakeScreenshot;
	int8	StretchScreenshots;
	bool8	SnapshotScreenshots;
	char    InitialSnapshotFilename[PATH_MAX + 1];
	bool8	FastSavestates;

	bool8	ApplyCheats;
	bool8	NoPatch;
	bool8	IgnorePatchChecksum;
	bool8	IsPatched;
	int32	AutoSaveDelay;
	bool8	DontSaveOopsSnapshot;
	bool8	UpAndDown;

	bool8	OpenGLEnable;

    bool8   SeparateEchoBuffer;
	uint32	SuperFXClockMultiplier;
    int OverclockMode;
	int	OneClockCycle;
	int	OneSlowClockCycle;
	int	TwoClockCycles;
	int	MaxSpriteTilesPerLine;
	#endif
}

/* Init.
 */

static int _aksnes9x_load_file(const char *path) {

  const char *HOME=getenv("HOME");
  if (HOME&&HOME[0]) {
    if (aksnes9x_set_user_path(HOME,".snes9x")<0) return -1;
  } else {
    printf("No home directory. Saving and such will not work.\n");
  }
  
  if (aksnes9x_map_input()<0) return -1;
  
  S9xLoadConfigFiles(0,0);
  Settings.AutoSaveDelay=10; // Save SRAM 10 seconds after noticing it's dirty.
  if (!Memory.Init() || !S9xInitAPU()) return -1;
  aksnes9x_default_settings();
  S9xInitSound(20);
  S9xSetSoundMute(TRUE);
  
  // Multitap by default. Seems most 2-player games don't mind.
  // (but not all; eg Tetris Attack)
  switch (aksnes9x_hardware) {
    case CTL_JOYPAD: {
        S9xSetController(0,CTL_JOYPAD,0,0,0,0);
        S9xSetController(1,CTL_JOYPAD,1,1,1,1);
        S9xReportControllers();
      } break;
    case CTL_MP5: default: {
        S9xSetController(0,CTL_JOYPAD,0,0,0,0);
        S9xSetController(1,CTL_MP5,1,2,3,4);
        S9xReportControllers();
      } break;
  }

  GFX.Screen=(uint16*)aksnes9x_fb;
  GFX.Pitch=SNES_WIDTH*2;
  if (!S9xGraphicsInit()) return -1;
  
  aksnes9x_save_sram();
  if (path&&path[0]) {
    printf("Open ROM: %s\n",path);
    aksnes9x_set_rom_path(path);
    if (Memory.LoadROM(path)) {
      
      S9xDeleteCheats();
      S9xCheatsEnable();
      aksnes9x_load_sram();
     
      S9xSetSoundMute(FALSE);
     
      aksnes9x_loaded=1;
      
    } else {
      printf("...failed\n");
      aksnes9x_loaded=0;
      aksnes9x_set_rom_path(0);
    }
  }

  return 0;
}

/* Parameter.
 */
 
static int _aksnes9x_configure(const char *k,int kc,const char *v,int vc,int vn) {
  
  if ((kc==8)&&!memcmp(k,"hardware",8)) {
    if ((vc==6)&&!memcmp(v,"normal",6)) { aksnes9x_hardware=CTL_JOYPAD; return 1; }
    if ((vc==8)&&!memcmp(v,"multitap",8)) { aksnes9x_hardware=CTL_MP5; return 1; }
  }
  
  return 0;
}

/* Update.
 */
 
static int _aksnes9x_update(int partial) {
  
  if (!aksnes9x_loaded) {
    fprintf(stderr,"aksnes9x: No ROM loaded\n");
    return -1;
  }
  
  int playerid=1; for (;playerid<=5;playerid++) {
    uint16_t status=eh_input_get(playerid);
    uint16_t mask=0x2000;
    for (;mask;mask>>=1) {
      S9xReportButton((playerid<<16)|mask,status&mask);
    }
  }
  
  int samplec=S9xGetSampleCount();
  if (samplec>aksnes9x_audioa) {
    int na=(samplec+1024)&~1023;
    void *nv=realloc(aksnes9x_audiov,na<<1);
    if (!nv) return -1;
    aksnes9x_audiov=(int16_t*)nv;
    aksnes9x_audioa=na;
  }
  S9xMixSamples((uint8*)aksnes9x_audiov,samplec);
  
  S9xMainLoop();

  eh_video_write(aksnes9x_fb);
  eh_audio_write(aksnes9x_audiov,samplec>>1);
  
  return 0;
}

/* Main.
 */

int main(int argc,char **argv) {
  struct eh_delegate delegate={
    .name="aksnes9x",
    .iconrgba=0,//TODO
    .iconw=0,
    .iconh=0,
    .video_width=SNES_WIDTH,
    .video_height=241,
    .video_format=EH_VIDEO_FORMAT_RGB16,
    .rmask=0xf800,
    .gmask=0x07e0,
    .bmask=0x001f,
    .video_rate=60,
    .audio_rate=44100,
    .audio_chanc=2,
    .audio_format=EH_AUDIO_FORMAT_S16N,
    .playerc=5,
    .configure=_aksnes9x_configure,
    .load_file=_aksnes9x_load_file,
    .update=_aksnes9x_update,
    //.reset=_aksnes9x_reset,
  };
  if (!(aksnes9x_fb=calloc(delegate.video_width*2,delegate.video_height))) return -1;
  int status=eh_main(argc,argv,&delegate);
  if (status) return status;
  S9xAutoSaveSRAM();
  return 0;
}
