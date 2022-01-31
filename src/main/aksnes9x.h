#ifndef AKSNES9X_H
#define AKSNES9X_H

#include <stdint.h>

#ifdef __cplusplus
  #include "snes9x/display.h" /* s9x_getdirtype */
  #include "snes9x/conffile.h" /* ConfigFile */
  #include "snes9x/controls.h" /* s9xcommand_t */
#endif

#ifdef __cplusplus
  extern "C" {
#endif

int aksnes9x_set_rom_path(const char *path);
int aksnes9x_get_rom_path(void **dstpp);

// For convenience, we can join two paths when setting.
int aksnes9x_set_user_path(const char *path,const char *suffix);
int aksnes9x_get_user_path(void **dstpp);

// Derived from rom_path.
// These strings will be overwritten by subsequent calls; don't hold on to them.
// Null if unavailable, never empty.
const char *aksnes9x_get_sram_path(int create);
const char *aksnes9x_get_snapshot_path(int create);
const char *aksnes9x_get_fresh_screenshot_path(int create);

int aksnes9x_save_sram();

#ifdef __cplusplus
  }
#endif

#define AKSNES9X_BTNID(bus,dev,btn) ((bus<<28)|(dev<<20)|(btn))
#define AKSNES9X_BTNID_KEY(keycode) AKSNES9X_BTNID(0,0,(keycode)&0x000fffff)
#define AKSNES9X_BTNID_HID(dev,btn) AKSNES9X_BTNID(1,(dev)&0xff,(btn)&0x000fffff)

#ifdef __cplusplus

s9xcommand_t s9xjoycmd(int padid,int btn);
void S9xParseArg(char **argv,int &i,int argc);
void S9xParsePortConfig(ConfigFile &conf,int pass);
void S9xHandlePortCommand(s9xcommand_t cmd,short data1,short data2);

void S9xMessage(int type,int messageid,char const *message);
bool S9xPollAxis(unsigned int id,short *value);
bool S9xPollButton(unsigned int id,bool *pressed);
bool S9xPollPointer(unsigned int id,short *x,short *y);
void S9xSyncSpeed();
void S9xExtraUsage();
uint8_t S9xInitUpdate();
uint8_t S9xContinueUpdate(int width,int height);
uint8_t S9xDeinitUpdate(int width,int height);
void S9xSetPalette();
const char *S9xBasename(char const *path);
const char *S9xGetFilename(char const *ex,s9x_getdirtype dirtype);
const char *S9xGetFilenameInc(char const *e,s9x_getdirtype dirtype);
const char *S9xGetDirectory(s9x_getdirtype dirtype);
uint8_t S9xOpenSnapshotFile(char const *fname,unsigned char readonly,Stream **file);
void S9xCloseSnapshotFile(Stream *);
const char *S9xStringInput(char const *msg);
void S9xAutoSaveSRAM();
const char *S9xChooseFilename(unsigned char readonly);
uint8_t S9xOpenSoundDevice();
void S9xToggleSoundChannel(int c);
const char *S9xChooseMovieFilename(unsigned char readonly);
void S9xExit();

void _splitpath(char const *path,char *drive,char *dir,char *stem,char *ext);
void _makepath(char *path,char const *drive,char const *dir,char const *stem,char const *ext);

#endif

#endif
