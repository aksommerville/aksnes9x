#include "aksnes9x.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

/* Globals.
 */
 
static struct {
  char *rompath;
  int rompathc;
  char *userpath;
  int userpathc;
  char storage[1024];
} aksnes9x_rom_path={0};

/* mkdir on a path's dirname recursively.
 */
 
static int aksnes9x_ensure_directory_exists(const char *path) {
  char dirpath[1024];
  int rslashp=-1,pathc=0;
  for (;path[pathc];pathc++) {
    if (path[pathc]=='/') rslashp=pathc;
  }
  if (rslashp<0) return -1; // Relative paths not allowed.
  if (!rslashp) return 0; // Let's just assume that the root directory exists.
  if (rslashp>=sizeof(dirpath)) return -1;
  memcpy(dirpath,path,rslashp);
  dirpath[rslashp]=0;
  
  struct stat st;
  if (stat(dirpath,&st)<0) {
    if (errno!=ENOENT) return -1;
    if (aksnes9x_ensure_directory_exists(dirpath)<0) return -1;
    if (mkdir(dirpath,0775)<0) return -1;
    return 0;
  } else {
    if (!S_ISDIR(st.st_mode)) return -1;
    return 0;
  }
}

/* Compose a path in temporary storage.
 */
 
static const char *aksnes9x_compose_path(
  const char *subdirname, // eg "snapshot" or "sram"
  const char *sfx, // eg ".snap"
  int create
) {
  if (!subdirname) subdirname="";
  if (!sfx) sfx="";
  if (aksnes9x_rom_path.rompathc<1) return 0;
  if (aksnes9x_rom_path.userpathc<1) return 0;
  
  const char *rombase=aksnes9x_rom_path.rompath;
  int rombasec=0,p=0;
  for (;p<aksnes9x_rom_path.rompathc;p++) {
    if (aksnes9x_rom_path.rompath[p]=='/') {
      rombase=aksnes9x_rom_path.rompath+p+1;
      rombasec=0;
    } else {
      rombasec++;
    }
  }
  if (!rombasec) return 0;
  
  int pathc=snprintf(
    aksnes9x_rom_path.storage,sizeof(aksnes9x_rom_path.storage),
    "%.*s/%s/%.*s%s",
    aksnes9x_rom_path.userpathc,aksnes9x_rom_path.userpath,
    subdirname,
    rombasec,rombase,
    sfx
  );
  if ((pathc<1)||(pathc>=sizeof(aksnes9x_rom_path.storage))) return 0;
  
  if (create) {
    aksnes9x_ensure_directory_exists(aksnes9x_rom_path.storage);
  }
  
  return aksnes9x_rom_path.storage;
}

/* Store path to current ROM file.
 */

int aksnes9x_set_rom_path(const char *path) {
  int pathc=0;
  if (path) while (path[pathc]) pathc++;
  char *nv=0;
  int nc=0;
  if (pathc) {
    if (!(nv=malloc(pathc+1))) return -1;
    memcpy(nv,path,pathc);
    nv[pathc]=0;
  }
  if (aksnes9x_rom_path.rompath) free(aksnes9x_rom_path.rompath);
  aksnes9x_rom_path.rompath=nv;
  aksnes9x_rom_path.rompathc=pathc;
  return 0;
}

/* Get the path to the current ROM file.
 */
 
int aksnes9x_get_rom_path(void **dstpp) {
  if (!dstpp) *(void**)dstpp=aksnes9x_rom_path.rompath;
  return aksnes9x_rom_path.rompathc;
}

/* Store path to directory for user's transient data.
 */

int aksnes9x_set_user_path(const char *path,const char *suffix) {
  int pathc=0;
  if (path) while (path[pathc]) pathc++;
  int suffixc=0;
  if (suffix) while (suffix[suffixc]) suffixc++;
  int dstc=(pathc&&suffixc)?(pathc+suffixc+1):(pathc+suffixc);
  char *nv=0;
  int nc=0;
  if (dstc) {
    if (!(nv=malloc(dstc+1))) return -1;
    memcpy(nv,path,pathc);
    if (pathc&&suffixc) {
      nv[pathc]='/';
      memcpy(nv+pathc+1,suffix,suffixc);
    } else {
      memcpy(nv+pathc,suffix,suffixc);
    }
    nv[dstc]=0;
  }
  if (aksnes9x_rom_path.userpath) free(aksnes9x_rom_path.userpath);
  aksnes9x_rom_path.userpath=nv;
  aksnes9x_rom_path.userpathc=dstc;
  return 0;
}

/* Get the path to the user's transient data directory.
 */
 
int aksnes9x_get_user_path(void **dstpp) {
  if (!dstpp) *(void**)dstpp=aksnes9x_rom_path.userpath;
  return aksnes9x_rom_path.userpathc;
}

/* Get paths for specific transient data.
 */
 
const char *aksnes9x_get_sram_path(int create) {
  return aksnes9x_compose_path("sram",".srm",create);
}

const char *aksnes9x_get_snapshot_path(int create) {
  return aksnes9x_compose_path("snapshot",".snap",create);
}

const char *aksnes9x_get_fresh_screenshot_path(int create) {
  char suffix[32];
  struct tm *tm=localtime(0);
  if (tm) {
    snprintf(suffix,sizeof(suffix),
      ".%04d%02d%02d%02d%02d%02d.png",
      1900+tm->tm_year,1+tm->tm_mon,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec
    );
  } else {
    strcpy(suffix,".png");
  }
  return aksnes9x_compose_path("screenshot",suffix,create);
}
