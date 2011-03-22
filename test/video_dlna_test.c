// Test all DLNA video profiles

#include <libmediascan.h>

#include "tap.h"
#include "common.h"
#include <math.h>
#include <unistd.h>
#include <stdint.h>

#define TEST_COUNT 33

// From ffmpeg utils.c:print_fps
static const char *
fps2str(double fps)
{
  static char str[10];
  uint64_t v = lrintf(fps*100);
  if     (v% 100      ) sprintf(str, "%3.2f", fps);
  else if(v%(100*1000)) sprintf(str, "%1.0f", fps);
  else                  sprintf(str, "%1.0fk", fps/1000); 
  return str;
}


static void my_result_callback(MediaScan *s, MediaScanResult *r) {
  char *file;
  if ((file = strrchr(r->path, '/')) == NULL)
    if ((file = strrchr(r->path, '\\')) == NULL)
      return;  
  file++;
  
  //ms_dump_result(r);
  
  /// MPEG1
  
  {
    if (!strcmp(file, "MPEG1.mpg")) {
      ok(r->type == TYPE_VIDEO, "MPEG1.mpg type is video ok");
      is(r->mime_type, "video/mpeg", "MPEG1.mpg MIME type video/mpeg ok");
      is(r->dlna_profile, "MPEG1", "MPEG1.mpg DLNA profile MPEG1 ok");
      ok(r->size == 51200, "MPEG1.mpg file size is 51200 ok");
      ok(r->bitrate == 1363969, "MPEG1.mpg bitrate is 1363969bps ok");
      ok(r->duration_ms == 300, "MPEG1.mpg duration is 0.3s ok");
      ok(r->video->width == 352, "MPEG1.mpg video width 352 ok");
      ok(r->video->height == 240, "MPEG1.mpg video height 240 ok");
      is(fps2str(r->video->fps), "29.97", "MPEG1.mpg framerate 29.97 ok");
    }
  }
  
  /// MPEG2
  
  // MPEG_PS_NTSC with LPCM audio
  // XXX LPCM audio format type not registered
  {
    if (!strcmp(file, "MPEG_PS_NTSC-lpcm.mpg")) {
      ok(r->type == TYPE_VIDEO, "MPEG_PS_NTSC-lpcm.mpg type is video ok");
      is(r->mime_type, "video/mpeg", "MPEG_PS_NTSC-lpcm.mpg MIME type video/mpeg ok");
      is(r->dlna_profile, "MPEG_PS_NTSC", "MPEG_PS_NTSC-lpcm.mpg DLNA profile ok");
      ok(r->video->width == 720, "MPEG_PS_NTSC-lpcm.mpg video width 720 ok");
      ok(r->video->height == 480, "MPEG_PS_NTSC-lpcm.mpg video height 480 ok");
      is(fps2str(r->video->fps), "29.97", "MPEG_PS_NTSC-lpcm.mpg framerate 29.97 ok");
    }
  }
  
  // MPEG_PS_NTSC with AC3 audio
  {
    if (!strcmp(file, "MPEG_PS_NTSC-ac3.mpg")) {
      ok(r->type == TYPE_VIDEO, "MPEG_PS_NTSC-ac3.mpg type is video ok");
      is(r->mime_type, "video/mpeg", "MPEG_PS_NTSC-ac3.mpg MIME type video/mpeg ok");
      is(r->dlna_profile, "MPEG_PS_NTSC", "MPEG_PS_NTSC-ac3.mpg DLNA profile ok");
      ok(r->video->width == 720, "MPEG_PS_NTSC-ac3.mpg video width 720 ok");
      ok(r->video->height == 480, "MPEG_PS_NTSC-ac3.mpg video height 480 ok");
      is(fps2str(r->video->fps), "29.97", "MPEG_PS_NTSC-ac3.mpg framerate 29.97 ok");
    }
  }
  
  // MPEG_PS_PAL with AC3 audio
  {
    if (!strcmp(file, "MPEG_PS_PAL-ac3.mpg")) {
      ok(r->type == TYPE_VIDEO, "MPEG_PS_PAL-ac3.mpg type is video ok");
      is(r->mime_type, "video/mpeg", "MPEG_PS_PAL-ac3.mpg MIME type video/mpeg ok");
      is(r->dlna_profile, "MPEG_PS_PAL", "MPEG_PS_PAL-ac3.mpg DLNA profile ok");
      ok(r->video->width == 720, "MPEG_PS_PAL-ac3.mpg video width 720 ok");
      ok(r->video->height == 576, "MPEG_PS_PAL-ac3.mpg video height 480 ok");
      is(fps2str(r->video->fps), "25", "MPEG_PS_PAL-ac3.mpg framerate 25 ok");
    }
  }
  
  // MPEG_TS_SD_NA_ISO
  {
    if (!strcmp(file, "MPEG_TS_SD_NA_ISO.ts")) {
      ok(r->type == TYPE_VIDEO, "MPEG_TS_SD_NA_ISO.ts type is video ok");
      is(r->mime_type, "video/mpeg", "MPEG_TS_SD_NA_ISO.ts MIME type video/mpeg ok");
      is(r->dlna_profile, "MPEG_TS_SD_NA_ISO", "MPEG_TS_SD_NA_ISO.ts DLNA profile ok");
      ok(r->video->width == 544, "MPEG_TS_SD_NA_ISO.ts video width 720 ok");
      ok(r->video->height == 480, "MPEG_TS_SD_NA_ISO.ts video height 480 ok");
      is(fps2str(r->video->fps), "29.97", "MPEG_TS_SD_NA_ISO.ts framerate 29.97 ok");
    }
  }
  
  /**
   * libdlna supports:
   * 
   * MPEG_PS_NTSC
   * MPEG_PS_NTSC_XAC3
   * MPEG_PS_PAL
   * MPEG_PS_PAL_XAC3
   * MPEG_TS_MP_LL_AAC
   * MPEG_TS_MP_LL_AAC_T
   * MPEG_TS_MP_LL_AAC_ISO
   * MPEG_TS_SD_EU
   * MPEG_TS_SD_EU_T
   * MPEG_TS_SD_EU_ISO
   * MPEG_TS_SD_NA
   * MPEG_TS_SD_NA_T
   * MPEG_TS_SD_NA_ISO
   * MPEG_TS_SD_NA_XAC3
   * MPEG_TS_SD_NA_XAC3_T
   * MPEG_TS_SD_NA_XAC3_ISO
   * MPEG_TS_HD_NA
   * MPEG_TS_HD_NA_T
   * MPEG_TS_HD_NA_ISO
   * MPEG_TS_HD_NA_XAC3
   * MPEG_TS_HD_NA_XAC3_T
   * MPEG_TS_HD_NA_XAC3_ISO
   * MPEG_ES_PAL
   * MPEG_ES_NTSC
   * MPEG_ES_PAL_XAC3
   * MPEG_ES_NTSC_XAC3
   *
   * Additional profiles not supported:
   *
   * DIRECTV_TS_SD
   * MPEG_PS_SD_DTS
   * MPEG_PS_HD_DTS
   * MPEG_PS_HD_DTSHD
   * MPEG_PS_HD_DTSHD_HRA
   * MPEG_PS_HD_DTSHD_MA
   * MPEG_TS_DTS_ISO
   * MPEG_TS_DTS_T
   * MPEG_TS_DTSHD_HRA_ISO
   * MPEG_TS_DTSHD_HRA_T
   * MPEG_TS_DTSHD_MA_ISO
   * MPEG_TS_DTSHD_MA_T
   * MPEG_TS_HD_50_L2_ISO
   * MPEG_TS_HD_50_L2_T
   * MPEG_TS_HD_X_50_L2_T
   * MPEG_TS_HD_X_50_L2_ISO
   * MPEG_TS_HD_60_L2_ISO
   * MPEG_TS_HD_60_L2_T
   * MPEG_TS_HD_X_60_L2_T
   * MPEG_TS_HD_X_60_L2_ISO
   * MPEG_TS_HD_NA_MPEG1_L2_ISO
   * MPEG_TS_HD_NA_MPEG1_L2_T
   * MPEG_TS_JP_T
   * MPEG_TS_SD_50_AC3_T
   * MPEG_TS_SD_50_L2_T
   * MPEG_TS_SD_60_AC3_T
   * MPEG_TS_SD_60_L2_T
   * MPEG_TS_SD_EU_AC3_ISO
   * MPEG_TS_SD_EU_AC3_T
   * MPEG_TS_SD_JP_MPEG1_L2_T
   * MPEG_TS_SD_NA_MPEG1_L2_ISO
   * MPEG_TS_SD_NA_MPEG1_L2_T
   */
}

static void my_error_callback(MediaScan *s, MediaScanError *error) {
  LOG_ERROR("[Error] %s (%s)\n", error->error_string, error->path);
}

int
main(int argc, char *argv[])
{
  plan(TEST_COUNT);
  
  ms_set_log_level(ERROR);
  
  // Get path to this binary
  char *bin = _findbin(argv[0]);
  char *dir = _abspath(bin, "../data/video/dlna"); // because binary is in .libs dir

  MediaScan *s = ms_create();
  ms_add_path(s, dir);
  ms_set_result_callback(s, my_result_callback);
  ms_set_error_callback(s, my_error_callback);
  ms_scan(s);    
  ms_destroy(s);
  
  free(dir);
  free(bin);
  
  return exit_status();
}
