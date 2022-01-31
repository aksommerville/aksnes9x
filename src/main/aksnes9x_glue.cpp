#include "aksnes9x.h"
#include <stdio.h>

/* Shutdown.
 */

void S9xExit() {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
}

/* Logging.
 */

void S9xMessage(int type,int messageid,char const *message) {
  printf("[%d,%d] %s\n",type,messageid,message);
}

/* Configuration.
 */

void S9xParseArg(char **argv,int &i,int argc) {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
}

/* Input.
 */
 
// (padid) 0..7. (btn) eg SNES_UP_MASK.
s9xcommand_t s9xjoycmd(int padid,int btn) {
  return (s9xcommand_t){
    .type=S9xButtonJoypad,
    .button={
      .joypad={
        .idx=(uint8_t)padid,
        .buttons=(uint16_t)btn,
      },
    },
  };
}

void S9xParsePortConfig(ConfigFile &conf,int pass) {

  // Each port has 4 joysticks connected to it, even/odd.
  S9xSetController(0,CTL_JOYPAD,0,2,4,6);
  S9xSetController(1,CTL_JOYPAD,1,3,5,7);
  
  // First joystick, use the keyboard.
  S9xMapButton(AKSNES9X_BTNID_KEY(0x0007004f),s9xjoycmd(0,SNES_RIGHT_MASK),0); // right
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070050),s9xjoycmd(0,SNES_LEFT_MASK),0); // left
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070052),s9xjoycmd(0,SNES_UP_MASK),0); // up
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070051),s9xjoycmd(0,SNES_DOWN_MASK),0); // down
  S9xMapButton(AKSNES9X_BTNID_KEY(0x0007001d),s9xjoycmd(0,SNES_B_MASK),0); // z
  S9xMapButton(AKSNES9X_BTNID_KEY(0x0007001b),s9xjoycmd(0,SNES_Y_MASK),0); // x
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070004),s9xjoycmd(0,SNES_A_MASK),0); // a
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070016),s9xjoycmd(0,SNES_X_MASK),0); // s
  S9xMapButton(AKSNES9X_BTNID_KEY(0x0007002b),s9xjoycmd(0,SNES_SELECT_MASK),0); // tab
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070028),s9xjoycmd(0,SNES_START_MASK),0); // enter
  S9xMapButton(AKSNES9X_BTNID_KEY(0x000700e1),s9xjoycmd(0,SNES_TL_MASK),0); // left shift
  S9xMapButton(AKSNES9X_BTNID_KEY(0x000700e5),s9xjoycmd(0,SNES_TR_MASK),0); // right shift
  
  // Second joystick, numeric keypad. Doesn't sound very convenient.
  S9xMapButton(AKSNES9X_BTNID_KEY(0x0007005e),s9xjoycmd(1,SNES_RIGHT_MASK),0); // 6
  S9xMapButton(AKSNES9X_BTNID_KEY(0x0007005c),s9xjoycmd(1,SNES_LEFT_MASK),0); // 4
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070060),s9xjoycmd(1,SNES_UP_MASK),0); // 8
  S9xMapButton(AKSNES9X_BTNID_KEY(0x0007005d),s9xjoycmd(1,SNES_DOWN_MASK),0); // 5
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070062),s9xjoycmd(1,SNES_B_MASK),0); // 0
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070058),s9xjoycmd(1,SNES_Y_MASK),0); // enter
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070063),s9xjoycmd(1,SNES_A_MASK),0); // dot
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070059),s9xjoycmd(1,SNES_X_MASK),0); // 1
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070055),s9xjoycmd(1,SNES_SELECT_MASK),0); // star
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070056),s9xjoycmd(1,SNES_START_MASK),0); // dash
  S9xMapButton(AKSNES9X_BTNID_KEY(0x0007005f),s9xjoycmd(1,SNES_TL_MASK),0); // 7
  S9xMapButton(AKSNES9X_BTNID_KEY(0x00070061),s9xjoycmd(1,SNES_TR_MASK),0); // 9
}

// These won't be called because we have asked not to be polled:
void S9xHandlePortCommand(s9xcommand_t cmd,short data1,short data2) {}
bool S9xPollAxis(unsigned int id,short *value) { return 1; }
bool S9xPollButton(unsigned int id,bool *pressed) { return 1; }
bool S9xPollPointer(unsigned int id,short *x,short *y) { return 1; }

/* Update.
 */

uint8_t S9xInitUpdate() {
  return 1;
}

uint8_t S9xContinueUpdate(int width,int height) {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
  return 1;//TODO is this a "continue" or a "success"?
}

uint8_t S9xDeinitUpdate(int width,int height) {
  return 1;
}

void S9xSyncSpeed() {
}

/* Sound.
 */

uint8_t S9xOpenSoundDevice() {
  // We do this separately, no worries.
  return 1;
}

void S9xToggleSoundChannel(int c) {
}

/* Video.
 */

void S9xSetPalette() {
  // Framebuffer is RGB565; I don't know what this is for.
}

/* Snapshot.
 */

uint8_t S9xOpenSnapshotFile(char const *fname,unsigned char readonly,Stream **file) {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
  return 0;
}

void S9xCloseSnapshotFile(Stream *) {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
}

void S9xAutoSaveSRAM() {
  aksnes9x_save_sram();
}

/* Unknown TODO
 */

void S9xExtraUsage() {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
}

const char *S9xStringInput(char const *msg) {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
  return msg;
}

const char *S9xChooseFilename(unsigned char readonly) {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
  return "";
}

const char *S9xChooseMovieFilename(unsigned char readonly) {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
  return "";
}

/* Path helpers.
 */

const char *S9xBasename(char const *path) {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
  return path;
}

const char *S9xGetFilename(char const *ex,s9x_getdirtype dirtype) {
  //TODO This gets called a lot during load.
  /*
  switch (dirtype) {
    case DEFAULT_DIR: printf("%s DEFAULT '%s'\n",__func__,ex); break;
    case HOME_DIR: printf("%s HOME '%s'\n",__func__,ex); break;
    case ROMFILENAME_DIR: printf("%s ROMFILENAME '%s'\n",__func__,ex); break;
    case ROM_DIR: printf("%s ROM '%s'\n",__func__,ex); break;
    case SRAM_DIR: printf("%s SRAM '%s'\n",__func__,ex); break;
    case SNAPSHOT_DIR: printf("%s SNAPSHOT '%s'\n",__func__,ex); break;
    case SCREENSHOT_DIR: printf("%s SCREENSHOT '%s'\n",__func__,ex); break;
    case SPC_DIR: printf("%s SPC '%s'\n",__func__,ex); break;
    case CHEAT_DIR: printf("%s CHEAT '%s'\n",__func__,ex); break;
    case PATCH_DIR: printf("%s PATCH '%s'\n",__func__,ex); break;
    case BIOS_DIR: printf("%s BIOS '%s'\n",__func__,ex); break;
    case LOG_DIR: printf("%s LOG '%s'\n",__func__,ex); break;
    case SAT_DIR: printf("%s SAT '%s'\n",__func__,ex); break;
    case LAST_DIR: printf("%s LAST '%s'\n",__func__,ex); break;
    default: printf("%s UNKNOWN '%s'\n",__func__,ex); break;
  }
  */
  return 0;
}

const char *S9xGetFilenameInc(char const *e,s9x_getdirtype dirtype) {
  //TODO
  printf("TODO: %s [%s:%d]\n",__func__,__FILE__,__LINE__);
  return e;
}

const char *S9xGetDirectory(s9x_getdirtype dirtype) {
  //TODO This gets called at startup. Does it have a purpose?
  return "";
}

/* Split path.
 */
 
void _splitpath(char const *path,char *drive,char *dir,char *stem,char *ext) {
  if (!path) return;
  
  // I'm assuming UNIX-style paths, kosher for MacOS and Linux.
  int pathc=0;
  while (path[pathc]) pathc++; // Measure input.
  while ((pathc>1)&&(path[pathc-1]=='/')) pathc--; // Strip trailing slashes. (don't touch [0])
  int rslashp=-1,rdotp=-1,i; // Locate rightmost slash and dot.
  for (i=pathc;i-->0;) {
    if (path[i]=='/') {
      rslashp=i;
      break;
    }
    if ((path[i]=='.')&&(rdotp<0)) {
      rdotp=i;
    }
  }
  
  // We don't do drive names.
  if (drive) drive[0]=0;
  
  // Directory is pretty straightforward, but be mindful of "/" alone.
  if (dir) {
    const char *src=path;
    int srcc=rslashp;
    if (srcc<0) srcc=0;
    else if (srcc==0) srcc=1; // "/"
    else if (srcc>_MAX_DIR) srcc=_MAX_DIR;
    memcpy(dir,src,srcc);
    dir[srcc]=0;
  }
  
  // They want basename split into stem and extension, gross. :P
  // I'm assuming that the dot should disappear, not be included in either.
  if (stem) {
    const char *src=path+rslashp+1; // safe, even if negative
    int srcc=(rdotp>rslashp)?(rdotp-rslashp-1):pathc-rslashp-1;
    if (srcc<0) srcc=0;
    else if (srcc>_MAX_FNAME) srcc=_MAX_FNAME;
    memcpy(stem,src,srcc);
    stem[srcc]=0;
  }
  
  if (ext) {
    if (rdotp<0) {
      ext[0]=0;
    } else {
      const char *src=path+rdotp+1;
      int srcc=pathc-rdotp-1;
      if (srcc<0) srcc=0;
      else if (srcc>_MAX_EXT) srcc=_MAX_EXT;
      memcpy(ext,src,srcc);
      ext[srcc]=0;
    }
  }
}

/* Join path.
 */

void _makepath(char *path,char const *drive,char const *dir,char const *stem,char const *ext) {
  if (!path) return;
  
  int drivec=0; if (drive) while (drive[drivec]) drivec++;
  int dirc=0; if (dir) while (dir[dirc]) dirc++;
  int stemc=0; if (stem) while (stem[stemc]) stemc++;
  int extc=0; if (ext) while (ext[extc]) extc++;
  
  snprintf(path,PATH_MAX,
    "%.*s%s%.*s%s%.*s%s%.*s", // say it ten times fast
    drivec,drive,drivec?":":"",
    dirc,dir,(dirc&&(stemc||extc))?"/":"",
    stemc,stem,
    extc?".":"",extc,ext
  );
}
