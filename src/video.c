


#include <libmediascan.h>

#ifdef WIN32
#include "mediascan_win32.h"
#endif


#ifdef _MSC_VER
#pragma warning( disable: 4244 )
#endif

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#ifdef _MSC_VER
#pragma warning( default: 4244 )
#endif


#include "common.h"
#include "image.h"
#include "video.h"
#include "error.h"
#include "util.h"
#include "libdlna/profiles.h"


MediaScanVideo *video_create(void) {
  MediaScanVideo *v = (MediaScanVideo *)calloc(sizeof(MediaScanVideo), 1);
  if (v == NULL) {
    ms_errno = MSENO_MEMERROR;
    FATAL("Out of memory for new MediaScanVideo object\n");
    return NULL;
  }

  LOG_MEM("new MediaScanVideo @ %p\n", v);
  return v;
}

MediaScanImage *video_create_image_from_frame(MediaScanVideo *v, MediaScanResult *r) {
  MediaScanImage *i = image_create();
  AVFormatContext *avf = (AVFormatContext *) r->_avf;
  av_codecs_t *codecs = (av_codecs_t *)v->_codecs;
  AVCodec *codec = (AVCodec *) v->_avc;
  AVFrame *frame = NULL;
  AVFrame *frame_rgb = NULL;
  AVPacket packet;
  struct SwsContext *swsc = NULL;
  int got_picture;
  int64_t duration_tb = ((double)avf->duration / AV_TIME_BASE) / av_q2d(codecs->vs->time_base);
  uint8_t *src;
  int x, y;
  int ofs = 0;

  if ((avcodec_open(codecs->vc, codec)) < 0) {
    LOG_ERROR("Couldn't open video codec %s for thumbnail creation\n", codec->name);
    goto err;
  }

  frame = avcodec_alloc_frame();
  if (!frame) {
    LOG_ERROR("Couldn't allocate a video frame\n");
    goto err;
  }

  av_init_packet(&packet);

  i->path = v->path;
  i->width = v->width;
  i->height = v->height;

  // XXX select best video frame, for example:
  // * Skip frames of all the same color (e.g. blank intro frames
  // * Use edge detection to skip blurry frames
  //   * http://code.google.com/p/fast-edge/
  //   * http://en.wikipedia.org/wiki/Canny_edge_detector 
  // * Use a frame some percentage into the video, what percentage?
  // * If really ambitious, use OpenCV for finding a frame with a face?

  // XXX other ways to seek if this fails
  // XXX for now, seek 10% into the video
  av_seek_frame(avf, codecs->vsid, (int)((double)duration_tb * 0.1), 0);

  for (;;) {
    if ((av_read_frame(avf, &packet)) < 0) {
      LOG_ERROR("Couldn't read video frame\n");
      goto err;
    }

    // Skip frame if it's not from the video stream
    if (packet.stream_index != codecs->vsid)
      continue;

    // Skip non-key-frames
    if (!(packet.flags & PKT_FLAG_KEY))
      continue;

    LOG_DEBUG
      ("Using video packet: pos %lld size %d, stream_index %d, duration %d\n",
       packet.pos, packet.size, packet.stream_index, packet.duration);

    if ((avcodec_decode_video2(codecs->vc, frame, &got_picture, &packet)) < 0) {
      LOG_ERROR("Error decoding video frame\n");
      goto err;
    }

    if (got_picture) {
      int rgb_bufsize;
      uint8_t *rgb_buffer = NULL;

      // use swscale to convert from source format to RGBA in our buffer with no resizing
      // XXX what scaler is fastest here when not actually resizing?
      swsc = sws_getContext(i->width, i->height, codecs->vc->pix_fmt,
                            i->width, i->height, PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
      if (!swsc) {
        LOG_ERROR("Unable to get swscale context\n");
        goto err;
      }

      frame_rgb = avcodec_alloc_frame();
      if (!frame_rgb) {
        LOG_ERROR("Couldn't allocate a video frame\n");
        goto err;
      }

      // XXX There is probably a way to get sws_scale to write directly to i->_pixbuf in our RGBA format

      rgb_bufsize = avpicture_get_size(PIX_FMT_RGB24, i->width, i->height);
      rgb_buffer = av_malloc(rgb_bufsize);
      if (!rgb_buffer) {
        LOG_ERROR("Couldn't allocate an RGB video buffer\n");
        goto err;
      }
      avpicture_fill((AVPicture *) frame_rgb, rgb_buffer, PIX_FMT_RGB24, i->width, i->height);

      // Convert image to RGB24
      sws_scale(swsc, frame->data, frame->linesize, 0, i->height, frame_rgb->data, frame_rgb->linesize);

      // Allocate space for our version of the image
      image_alloc_pixbuf(i, i->width, i->height);

      src = frame_rgb->data[0];
      ofs = 0;
      for (y = 0; y < i->height; y++) {
        for (x = 0; x < i->width * 3; x += 3) {
          i->_pixbuf[ofs++] = COL(src[x], src[x + 1], src[x + 2]);
        }
        src += i->width * 3;
      }

      // Free the frame
      av_free(rgb_buffer);

      // Done!
      goto out;
    }
  }

err:
  image_destroy(i);
  i = NULL;

out:
  sws_freeContext(swsc);
  av_free_packet(&packet);
  if (frame)
    av_free(frame);
  if (frame_rgb)
    av_free(frame_rgb);

  avcodec_close(codecs->vc);

  return i;
}

void video_add_thumbnail(MediaScanVideo *v, MediaScanImage *thumb) {
  if (v->nthumbnails < MAX_THUMBS)
    v->thumbnails[v->nthumbnails++] = thumb;
}

uint8_t *video_get_thumbnail(MediaScanVideo *v, int index, int *length) {
  uint8_t *ret = NULL;
  if (v->nthumbnails >= index) {
    MediaScanImage *thumb = v->thumbnails[index];
    Buffer *buf = (Buffer *)thumb->_dbuf;
    *length = buffer_len(buf);
    ret = (uint8_t *)buffer_ptr(buf);
  }
  else {
    *length = 0;
  }

  return ret;
}

void video_destroy(MediaScanVideo *v) {
  int x;

  // free thumbnails
  for (x = 0; x < v->nthumbnails; x++)
    image_destroy(v->thumbnails[x]);

  LOG_MEM("destroy MediaScanVideo @ %p\n", v);
  free(v);
}
