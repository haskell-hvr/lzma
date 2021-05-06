/*
 * FFI wrappers for `lzma-streams`
 *
 * Copyright (c) 2014, Herbert Valerio Riedel <hvr@gnu.org>
 *
 * This code is BSD3 licensed, see ../LICENSE file for details
 *
 */

#include <stdio.h>
#include <string.h>
#include <lzma.h>
#include <HsFFI.h>

HsInt
hs_lzma_init_decoder(lzma_stream *ls, HsBool autolzma, uint64_t memlimit, uint32_t flags)
{
  /* recommended super-portable initialization */
  const lzma_stream ls_init = LZMA_STREAM_INIT;
  *ls = ls_init;

  const lzma_ret ret = (autolzma ? lzma_auto_decoder : lzma_stream_decoder)(ls, memlimit, flags);

  return ret;
}

HsInt
hs_lzma_init_encoder(lzma_stream *ls, uint32_t preset, HsInt check, HsInt threads)
{
  /* recommended super-portable initialization */
  const lzma_stream ls_init = LZMA_STREAM_INIT;
  *ls = ls_init;

  lzma_mt mt = {
    .threads = threads,
    // Use the default preset (6) for LZMA2.
    // To use a preset, filters must be set to NULL.
    .preset = preset,
    // Use CRC64 for integrity checking. See also
    // 01_compress_easy.c about choosing the integrity check.
    .check = check,
  };
  const lzma_ret ret = lzma_stream_encoder_mt(ls, &mt);

  return ret;
}

void
hs_lzma_done(lzma_stream *ls)
{
  lzma_end(ls);
}

HsInt
hs_lzma_run(lzma_stream *const ls, const HsInt action,
            const uint8_t ibuf[], const HsInt ibuf_len,
            uint8_t obuf[], const HsInt obuf_len)
{
  ls->next_in = ibuf;
  ls->avail_in = ibuf_len;
  ls->next_out = obuf;
  ls->avail_out = obuf_len;

  // paranoia
  memset(obuf, 0, obuf_len);
  
  const lzma_ret ret = lzma_code(ls, action);

  // paranoia
  ls->next_in = NULL;
  ls->next_out = NULL;

  return ret;
}
