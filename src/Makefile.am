lib_LTLIBRARIES = libmediascan.la

if LINUXBSD

libmediascan_la_sources_linuxbsd = audio.c buffer.c mediascan.c mediascan_unix.c mediascan_linux.c progress.c result.c error.c video.c util.c \
  image.c image_jpeg.c image_png.c image_bmp.c image_gif.c thumb.c thread.c database.c \
  tag.c tag_item.c \
  libdlna/audio_aac.c libdlna/audio_ac3.c libdlna/audio_amr.c libdlna/audio_atrac3.c \
  libdlna/audio_g726.c libdlna/audio_lpcm.c libdlna/audio_mp1.c libdlna/audio_mp2.c libdlna/audio_mp3.c \
  libdlna/audio_wma.c libdlna/av_mpeg1.c libdlna/av_mpeg2.c libdlna/av_mpeg4_part10.c \
  libdlna/av_mpeg4_part2.c libdlna/av_wmv9.c libdlna/containers.c libdlna/profiles.c \
  jenkins/lookup3.c

endif

if SUN

libmediascan_la_sources_sun = audio.c buffer.c mediascan.c mediascan_unix.c mediascan_sun.c progress.c result.c error.c video.c util.c \
  image.c image_jpeg.c image_png.c image_bmp.c image_gif.c thumb.c thread.c database.c \
  tag.c tag_item.c \
  libdlna/audio_aac.c libdlna/audio_ac3.c libdlna/audio_amr.c libdlna/audio_atrac3.c \
  libdlna/audio_g726.c libdlna/audio_lpcm.c libdlna/audio_mp1.c libdlna/audio_mp2.c libdlna/audio_mp3.c \
  libdlna/audio_wma.c libdlna/av_mpeg1.c libdlna/av_mpeg2.c libdlna/av_mpeg4_part10.c \
  libdlna/av_mpeg4_part2.c libdlna/av_wmv9.c libdlna/containers.c libdlna/profiles.c \
  jenkins/lookup3.c

endif

if DARWIN

libmediascan_la_sources_darwin = audio.c buffer.c mediascan.c mediascan_unix.c progress.c result.c error.c video.c util.c \
  image.c image_jpeg.c image_png.c image_bmp.c image_gif.c thumb.c thread.c database.c mediascan_macos.m NSString+SymlinksAndAliases.m \
  tag.c tag_item.c \
  libdlna/audio_aac.c libdlna/audio_ac3.c libdlna/audio_amr.c libdlna/audio_atrac3.c \
  libdlna/audio_g726.c libdlna/audio_lpcm.c libdlna/audio_mp1.c libdlna/audio_mp2.c libdlna/audio_mp3.c \
  libdlna/audio_wma.c libdlna/av_mpeg1.c libdlna/av_mpeg2.c libdlna/av_mpeg4_part10.c \
  libdlna/av_mpeg4_part2.c libdlna/av_wmv9.c libdlna/containers.c libdlna/profiles.c \
  jenkins/lookup3.c

libmediascan_la_OBJCFLAGS = -Wall -I$(top_srcdir)/include

endif

libmediascan_la_SOURCES = $(libmediascan_la_sources_linuxbsd) $(libmediascan_la_sources_sun) $(libmediascan_la_sources_darwin)

libmediascan_la_CFLAGS = -Wall -I$(top_srcdir)/include

libmediascan_la_LDFLAGS = -version-info 0:0:0

# XXX only include in dist, not install
include_HEADERS = audio.h buffer.h common.h error.h mediascan.h progress.h fixed.h queue.h \
  image.h image_jpeg.h image_png.h image_gif.h image_bmp.h result.h thumb.h thread.h util.h video.h \
  database.h tag.h tag_item.h \
  libdlna/containers.h libdlna/dlna.h libdlna/dlna_internals.h libdlna/profiles.h \
  NSString+SymlinksAndAliases.h
