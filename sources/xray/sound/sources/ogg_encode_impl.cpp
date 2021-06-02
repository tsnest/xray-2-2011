////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "ogg_utils.h"
#include <float.h>

namespace xray {
namespace sound {

namespace ogg_utils 
{

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

enum
{
	read_size = 1024,
};

struct input_format
{
	s32		id_data_len; /* Amount of data needed to id whether this can load the file */
	char	*format;
	char	*description;
};


struct wav_fmt
{
	s16		format;
	s16		channels;
	s32		samplerate;
	s32		bytespersec;
	s16		align;
	s16		samplesize;
	u32		mask;
};

struct wavfile{
	s16				channels;
	s32				channel_map;
	s16				samplesize;
	s32				totalsamples;
	s32				samplesread;
	fs_new::file_type	*f;
	s16				bigendian;
    s32				*channel_permute;
};


// resampler stuff

typedef float SAMPLE;

typedef struct
{
    u32 channels, infreq, outfreq, taps;
    float *table;
    SAMPLE *pool;

    /* dynamic bits */
    s32 poolfill;
    s32 offset;
} res_state;

enum res_parameter
{
    RES_END,
    RES_GAIN,    /* (double)1.0 */
    RES_CUTOFF,    /* (double)0.80 */ 
    RES_TAPS,    /* (int)45 */
    RES_BETA    /* (double)16.0 */
};

struct resampler
{
    res_state resampler_state;
    void *real_readdata;
    float **bufs;
    s32 channels;
    s32 bufsize;
    s32 done;
};

static double I_zero(double x)
{
    int n = 0;
    double u = 1.0,
        s = 1.0,
        t;

    do
    {
        n += 2;
        t = x / n;
        u *= t * t;
        s += u;
    } while (u > 1e-21 * s);

    return s;
}

static int hcf(int arg1, int arg2)
{
    int mult = 1;

    while (~(arg1 | arg2) & 1)
        arg1 >>= 1, arg2 >>= 1, mult <<= 1;

    while (arg1 > 0)
    {
        if (~(arg1 & arg2) & 1)
        {
            arg1 >>= (~arg1 & 1);
            arg2 >>= (~arg2 & 1);
        }
        else if (arg1 < arg2)
            arg2 = (arg2 - arg1) >> 1;
        else
            arg1 = (arg1 - arg2) >> 1;
    }

    return arg2 * mult;
}

static void filt_sinc(float *dest, int N, int step, double fc, double gain, int width)
{
    double s = fc / step;
    int mid, x;
    float *endpoint = dest + N,
        *base = dest,
        *origdest = dest;

    assert(width <= N);

    if ((N & 1) == 0)
    {
        *dest = 0.0;
        dest += width;
        if (dest >= endpoint)
            dest = ++base;
        N--;
    }

    mid = N / 2;
    x = -mid;

    while (N--)
    {
		*dest = static_cast<float>((x ? sin(x * xray::math::pi * s) / (x * xray::math::pi) * step : fc) * gain);
        x++;
        dest += width;
        if (dest >= endpoint)
            dest = ++base;
    }
    R_ASSERT(dest == origdest + width);
}

int res_push_check(res_state const * const state, size_t srclen)
{
    if (state->poolfill < static_cast<int>(state->taps))
        srclen -= state->taps - state->poolfill;

    return ((s32)srclen * state->outfreq - state->offset + state->infreq - 1) / state->infreq;
}

static SAMPLE sum(float const *scale, int count, SAMPLE const *source, SAMPLE const *trigger, SAMPLE const *reset, int srcstep)
{
    float total = 0.0;

    while (count--)
    {
        total += *source * *scale;

        if (source == trigger)
            source = reset, srcstep = 1;
        source -= srcstep;
        scale++;
    }

    return total;
}

static int push(res_state const * const state, SAMPLE *pool, int * const poolfill, int * const offset, SAMPLE *dest, int dststep, SAMPLE const *source, int srcstep, size_t srclen)
{
    SAMPLE    * const destbase = dest,
        *poolhead = pool + *poolfill,
        *poolend = pool + state->taps,
        *newpool = pool;
    SAMPLE const *refill, *base, *endpoint;
    int    lencheck;


    assert(state);
    assert(pool);
    assert(poolfill);
    assert(dest);
    assert(source);

    assert(state->poolfill != -1);

    lencheck = res_push_check(state, srclen);

    /* fill the pool before diving in */
    while (poolhead < poolend && srclen > 0)
    {
        *poolhead++ = *source;
        source += srcstep;
        srclen--;
    }

    if (srclen <= 0)
        return 0;

    base = source;
    endpoint = source + srclen * srcstep;

    while (source < endpoint)
    {
        *dest = sum(state->table + *offset * state->taps, state->taps, source, base, poolend, srcstep);
        dest += dststep;
        *offset += state->infreq;
        while (*offset >= static_cast<int>(state->outfreq))
        {
            *offset -= state->outfreq;
            source += srcstep;
        }
    }

    assert(dest == destbase + lencheck * dststep);

    /* pretend that source has that underrun data we're not going to get */
    srclen += (source - endpoint) / srcstep;

    /* if we didn't get enough to completely replace the pool, then shift things about a bit */
    if (srclen < state->taps)
    {
        refill = pool + srclen;
        while (refill < poolend)
            *newpool++ = *refill++;

        refill = source - srclen * srcstep;
    }
    else
        refill = source - state->taps * srcstep;

    /* pull in fresh pool data */
    while (refill < endpoint)
    {
        *newpool++ = *refill;
        refill += srcstep;
    }

    assert(newpool > pool);
    assert(newpool <= poolend);

    *poolfill = newpool - pool;

    return (dest - destbase) / dststep;
}

static void win_kaiser(float *dest, int N, double alpha, int width)
{
    double I_alpha, midsq;
    int x;
    float *endpoint = dest + N,
        *base = dest,
        *origdest = dest;

    assert(width <= N);

    if ((N & 1) == 0)
    {
        *dest = 0.0;
        dest += width;
        if (dest >= endpoint)
            dest = ++base;
        N--;
    }

    x = -(N / 2);
    midsq = (double)(x - 1) * (double)(x - 1);
    I_alpha = I_zero(alpha);

    while (N--)
    {
        *dest *= static_cast<float>(I_zero(alpha * sqrt(1.0 - ((double)x * (double)x) / midsq)) / I_alpha);
        x++;
        dest += width;
        if (dest >= endpoint)
            dest = ++base;
    }
    R_ASSERT(dest == origdest + width);
}

int res_push_max_input(res_state const * const state, size_t maxoutput)
{
    return maxoutput * state->infreq / state->outfreq;
}

int res_drain(res_state *state, SAMPLE **dstlist)
{
    SAMPLE *tail;
    int result = -1, poolfill = -1, offset = -1;

    assert(state);
    assert(dstlist);
    assert(state->poolfill >= 0);

    if ((tail = (SAMPLE*)_alloca(state->taps * sizeof(SAMPLE))) == NULL)
        return -1;

	xray::memory::zero(tail, state->taps * sizeof(SAMPLE));

    for (unsigned int i = 0; i < state->channels; i++)
    {
        poolfill = state->poolfill;
        offset = state->offset;
        result = push(state, state->pool + i * state->taps, &poolfill, &offset, dstlist[i], 1, tail, 1, state->taps / 2 - 1);
    }

    state->poolfill = -1;

    return result;
}

int res_push(res_state *state, SAMPLE **dstlist, SAMPLE const **srclist, size_t srclen)
{
    int result = -1, poolfill = -1, offset = -1;

    assert(state);
    assert(dstlist);
    assert(srclist);
    assert(state->poolfill >= 0);

    for (unsigned int i = 0; i < state->channels; i++)
    {
        poolfill = state->poolfill;
        offset = state->offset;
        result = push(state, state->pool + i * state->taps, &poolfill, &offset, dstlist[i], 1, srclist[i], 1, srclen);
    }
    state->poolfill = poolfill;
    state->offset = offset;

    return result;
}

// resampler stuff end

long wav_read(void *in, fs_new::synchronous_device_interface const & device, float **buffer, int samples)
{
    wavfile *f = (wavfile *)in;
    int sampbyte = f->samplesize / 8;
    signed char *buf = (signed char*)_alloca(samples*sampbyte*f->channels);
	xray::memory::zero(buf, samples*sampbyte*f->channels);
	long bytes_read = device->read(f->f, buf, samples*sampbyte*f->channels);
    int i,j;
    long realsamples;
    int *ch_permute = f->channel_permute;

    if(f->totalsamples && f->samplesread + 
            bytes_read/(sampbyte*f->channels) > f->totalsamples) {
        bytes_read = sampbyte*f->channels*(f->totalsamples - f->samplesread);
    }

    realsamples = bytes_read/(sampbyte*f->channels);
    f->samplesread += realsamples;

    if(f->samplesize==8)
    {
        unsigned char *bufu = (unsigned char *)buf;
        for(i = 0; i < realsamples; i++)
        {
            for(j=0; j < f->channels; j++)
            {
                buffer[j][i]=((int)(bufu[i*f->channels + ch_permute[j]])-128)/128.0f;
            }
        }
    }
    else if(f->samplesize==16)
    {
        if(!f->bigendian)
        {
            for(i = 0; i < realsamples; i++)
            {
                for(j=0; j < f->channels; j++)
                {
                    buffer[j][i] = ((buf[i*2*f->channels + 2*ch_permute[j] + 1]<<8) |
                                    (buf[i*2*f->channels + 2*ch_permute[j]] & 0xff))/32768.0f;
                }
            }
        }
        else
        {
            for(i = 0; i < realsamples; i++)
            {
                for(j=0; j < f->channels; j++)
                {
                    buffer[j][i]=((buf[i*2*f->channels + 2*ch_permute[j]]<<8) |
                                  (buf[i*2*f->channels + 2*ch_permute[j] + 1] & 0xff))/32768.0f;
                }
            }
        }
    }
    else if(f->samplesize==24) 
    {
        if(!f->bigendian) {
            for(i = 0; i < realsamples; i++)
            {
                for(j=0; j < f->channels; j++) 
                {
                    buffer[j][i] = ((buf[i*3*f->channels + 3*ch_permute[j] + 2] << 16) |
                      (((unsigned char *)buf)[i*3*f->channels + 3*ch_permute[j] + 1] << 8) |
                      (((unsigned char *)buf)[i*3*f->channels + 3*ch_permute[j]] & 0xff)) 
                        / 8388608.0f;

                }
            }
        }
        else 
		{
			LOG_ERROR("Big endian 24 bit PCM data is not currently supported, aborting.\n");
            return 0;
        }
    }
    else {
        LOG_ERROR("Internal error: attempt to read unsupported bitdepth %d\n", f->samplesize);
        return 0;
    }

    return realsamples;
}

static long read_resampled(void *d, fs_new::synchronous_device_interface const & device, float **buffer, int samples)
{
//	LOG_INFO("read resampled %f", buffer[0][0]);
    resampler *rs = (resampler*)d;
    long in_samples;
    int out_samples;

    in_samples = res_push_max_input(&rs->resampler_state, samples);
    if(in_samples > rs->bufsize)
        in_samples = rs->bufsize;

	in_samples = wav_read(rs->real_readdata, device, rs->bufs, in_samples);

    if(in_samples <= 0) {
        if(!rs->done) {
            rs->done = 1;
            out_samples = res_drain(&rs->resampler_state, buffer);
            return out_samples;
        }
        return 0;
    }

    out_samples = res_push(&rs->resampler_state, buffer, (float const **)rs->bufs, in_samples);

    if(out_samples <= 0) 
	{
		LOG_ERROR("BUG: Got zero samples from resampler: your file will be truncated. Please report this.\n");
    }

    return out_samples;
}

static u32 read_u32_le(unsigned char* buf)
{
	return (((buf)[3]<<24)|((buf)[2]<<16)|((buf)[1]<<8)|((buf)[0]&0xff));
}

static u16 read_u16_le(unsigned char* buf)
{
	return (((buf)[1]<<8)|((buf)[0]&0xff));
}


static int wav_permute_matrix[8][8] = 
{
  {0},              /* 1.0 mono   */
  {0,1},            /* 2.0 stereo */
  {0,2,1},          /* 3.0 channel ('wide') stereo */
  {0,1,2,3},        /* 4.0 discrete quadraphonic */
  {0,2,1,3,4},      /* 5.0 surround */
  {0,2,1,4,5,3},    /* 5.1 surround */
/*{0,2,1,4,5,6,3},     6.1 surround - WRONG*/
  {0,2,1,5,6,4,3},  /* 6.1 surround */
  {0,2,1,6,7,4,5,3} /* 7.1 surround (classic theater 8-track) */
};

static int seek_forward(fs_new::file_type *in, fs_new::synchronous_device_interface const & device, unsigned int length)
{
	if(device->seek(in, length, fs_new::seek_file_current))
	{
		/* Failed. Do it the hard way. */
		unsigned char buf[1024];
		unsigned int seek_needed = length;
		int seeked;
		while(seek_needed > 0)
		{
			seeked = device->read(in, buf, seek_needed>1024?1024:seek_needed);
			if(!seeked)
				return 0; /* Couldn't read more, can't read file */
			else
				seek_needed -= seeked;
		}
	}
	return 1;
}


int wav_id(unsigned char *buf, int len)
{
	unsigned int flen;
	
	if(len<12) return 0; /* Something screwed up */

	if(memcmp(buf, "RIFF", 4))
		return 0; /* Not wave */

	flen = read_u32_le(buf+4); /* We don't use this */

	if(memcmp(buf+8, "WAVE",4))
		return 0; /* RIFF, but not wave */

	return 1;
}

static int find_wav_chunk(fs_new::file_type *in, fs_new::synchronous_device_interface const & device, char *type, unsigned int *len)
{
	unsigned char buf[8];
	unsigned int result = 1;

	while(result)
	{
		if(device->read(in, buf, 8) < 8) /* Suck down a chunk specifier */
		{
			LOG_ERROR("Warning: Unexpected EOF in reading WAV header\n");
			result = 0;
			return 0; /* EOF before reaching the appropriate chunk */
		}

		if(memcmp(buf, type, 4))
		{
			*len = read_u32_le(buf+4);
			if(!seek_forward(in, device, *len))
			{
				result = 0;
				return 0;
			}

			buf[4] = 0;
			LOG_ERROR("Skipping chunk of type \"%s\", length %d\n", buf, *len);
		}
		else
		{
			*len = read_u32_le(buf+4);
			result = 0;
			return 1;
		}
	}
	return 0;
}

input_format *open_audio_file(fs_new::file_type *in, fs_new::synchronous_device_interface const & device)
{
	unsigned char *buf=NULL;
	int buf_size=12, buf_filled=0;
	int ret;

	buf = (unsigned char *)_alloca(buf_size);
	xray::memory::zero(buf, buf_size);

	ret = device->read(in, buf+buf_filled, buf_size-buf_filled);
	buf_filled += ret;



	if(wav_id(buf, buf_filled))
	{
		return NULL;
		///* ok, we now have something that can handle the file */
		//if(formats[j].open_func(in, opt, buf, buf_filled)) {
		//	free(buf);
		//	return &formats[j];
		//}
	}

	return NULL;
}

int oe_write_page(ogg_page *page, fs_new::file_type *fp, fs_new::synchronous_device_interface const & device)
{
    int written;
	written = device->write(fp, page->header, page->header_len);
	written += device->write(fp, page->body, page->body_len);

    return written;
}

int ogg_encode_impl(fs_new::file_type* input_file,
					fs_new::file_type* output_file,
					fs_new::synchronous_device_interface const & device,
					u16 channels,
					int new_rate,
					int new_bitrate)
{

	u32 old_float_point_control = 0;
	errno_t fp_error = _controlfp_s(&old_float_point_control, _PC_53,_MCW_PC );
	ASSERT			(fp_error == 0);

	void *readdata		= NULL;
	open_audio_file(input_file, device);

	wavfile *wav = (wavfile*)_alloca(sizeof(wavfile));
	xray::memory::zero(wav, sizeof(wavfile));
	/////////////////////////////////////////////
	// open wav
	////////////////////////////////////////////
	//{
		unsigned char buf[40];
		unsigned int len;
		int samplesize;
		wav_fmt format;
		int i;

		/* Ok. At this point, we know we have a WAV file. Now we have to detect
		 * whether we support the subtype, and we have to find the actual data
		 * We don't (for the wav reader) need to use the buffer we used to id this
		 * as a wav file (oldbuf)
		 */

		if(!find_wav_chunk(input_file, device, "fmt ", &len))
			return 0; /* EOF */

		if(len < 16) 
		{
			LOG_ERROR("Warning: Unrecognised format chunk in WAV header\n");
			return 0; /* Weird format chunk */
		}

		/* A common error is to have a format chunk that is not 16, 18 or 40 bytes
		 * in size.  This is incorrect, but not fatal, so we only warn about 
		 * it instead of refusing to work with the file.  Please, if you
		 * have a program that's creating format chunks of sizes other than
		 * 16, 18 or 40 bytes in size, report a bug to the author.
		 * (40 bytes accommodates WAVEFORMATEXTENSIBLE conforming files.)
		 */
		if (len!=16 && len!=18 && len!=40)
			LOG_ERROR("Warning: INVALID format chunk in wav header.\n Trying to read anyway (may not work)...\n");

	//    if(len>40)len=40;

		if (device->read(input_file, buf, len) < len) 
		{
			LOG_ERROR("Warning: Unexpected EOF in reading WAV header\n");
			return 0;
		}

		format.format =      read_u16_le(buf); 
		format.channels =    read_u16_le(buf+2); 
		format.samplerate =  read_u32_le(buf+4);
		format.bytespersec = read_u32_le(buf+8);
		format.align =       read_u16_le(buf+12);
		format.samplesize =  read_u16_le(buf+14);

		if(format.format == -2) /* WAVE_FORMAT_EXTENSIBLE */
		{
		  if(len<40)
		  {
			LOG_ERROR("ERROR: Extended WAV format header invalid (too small)\n");
			return 0;
		  }

		  format.mask = read_u32_le(buf+20);
		  /* warn the user if the format mask is not a supported/expected type */
		  switch(format.mask){
		  case 1539: /* 4.0 using side surround instead of back */
			LOG_ERROR("WARNING: WAV file uses side surround instead of rear for quadraphonic;\n"
					"remapping side speakers to rear in encoding.\n");
			break;
		  case 1551: /* 5.1 using side instead of rear */
			LOG_ERROR("WARNING: WAV file uses side surround instead of rear for 5.1;\n"
					"remapping side speakers to rear in encoding.\n");
			break;
		  case 319:  /* 6.1 using rear instead of side */
			LOG_ERROR("WARNING: WAV file uses rear surround instead of side for 6.1;\n"
					"remapping rear speakers to side in encoding.\n");
			break;
		  case 255:  /* 7.1 'Widescreen' */
			LOG_ERROR("WARNING: WAV file is a 7.1 'Widescreen' channel mapping;\n"
					"remapping speakers to Vorbis 7.1 format.\n");
			break;
		  case 0:    /* default/undeclared */
		  case 1:    /* mono */
		  case 3:    /* stereo */
		  case 51:   /* quad */
		  case 55:   /* 5.0 */
		  case 63:   /* 5.1 */
		  case 1807: /* 6.1 */
		  case 1599: /* 7.1 */
			break;
		  default:
			LOG_ERROR("ARNING: Unknown WAV surround channel mask: %d\n"
					"blindly mapping speakers using default SMPTE/ITU ordering.\n",	format.mask);
			break;
		  }
		  format.format = read_u16_le(buf+24);
		}

		samplesize = format.samplesize/8;


		if(!find_wav_chunk(input_file, device, "data", &len))
			return 0; /* EOF */

		if(format.align != format.channels * samplesize) {
			/* This is incorrect according to the spec. Warn loudly, then ignore
			 * this value.
			 */
			LOG_ERROR("Warning: WAV 'block alignment' value is incorrect, ignoring.\n" 
				"The software that created this file is incorrect.\n");
		}

		
		if(format.samplesize == samplesize*8 && 
				(format.samplesize == 32 || format.samplesize == 24 ||
				 format.samplesize == 16 || format.samplesize == 8)) 
		{
			/* OK, good - we have a supported format,
			 * now we want to find the size of the file
			 */

			wav->f = input_file;
			wav->samplesread = 0;
			wav->bigendian = 0;
			wav->channels = format.channels; /* This is in several places. The price
								of trying to abstract stuff. */
			wav->samplesize = format.samplesize;

			int total_samples_per_channel = 0; /* Give up, like raw format */

			wav->totalsamples = total_samples_per_channel;

			readdata = (void *)wav;

			wav->channel_permute = (int*)_alloca(wav->channels * sizeof(int));
			xray::memory::zero(wav->channel_permute, wav->channels * sizeof(int));
			if (wav->channels <= 8)
				/* Where we know the mappings, use them. */
				memcpy(wav->channel_permute, wav_permute_matrix[wav->channels-1], sizeof(int) * wav->channels);
			else
				/* Use a default 1-1 mapping */
				for (i=0; i < wav->channels; i++)
					wav->channel_permute[i] = i;
		}
		else 
		{
			LOG_ERROR("ERROR: Wav file is unsupported subformat (must be 8, 16, 24 or 32 bit PCM\n"
					"or floating point PCM\n");
			return 0;
		}
	//}

	resampler rs;// = (resampler*)calloc(1, sizeof(resampler));
	xray::memory::zero( &rs, sizeof(resampler));
    int c;

    rs.bufsize = 4096; /* Shrug */
    rs.real_readdata = readdata;
    rs.bufs = (float**)_alloca(sizeof(float *) * channels);
	xray::memory::zero(rs.bufs, sizeof(float *) * channels);
    rs.channels = channels;
    rs.done = 0;

	double beta = 16.0,
		cutoff = 0.80,
		gain = 1.0;
    int taps = 45;

    int factor;

	int outfreq = new_rate;
	int infreq = format.samplerate;

	if (&rs.resampler_state == NULL || channels <= 0 || outfreq <= 0 || infreq <= 0 || taps <= 0)
        return -1;


    factor = hcf(infreq, outfreq);
    outfreq /= factor;
    infreq /= factor;

	/* adjust to rational values for downsampling */
    if (outfreq < infreq)
    {
        /* push the cutoff frequency down to the output frequency */
        cutoff = cutoff * outfreq / infreq; 

        /* compensate for the sharper roll-off requirement
         * by using a bigger hammer */
        taps = taps * infreq/outfreq;
    }

	if ((rs.resampler_state.table = (float*)_alloca(outfreq * taps * sizeof(float))) == NULL)
		return -1;
	if ((rs.resampler_state.pool = (SAMPLE*)_alloca(channels * taps * sizeof(SAMPLE))) == NULL)
	{
		return -1;
	}

	xray::memory::zero(rs.resampler_state.table, outfreq * taps * sizeof(float));
	xray::memory::zero(rs.resampler_state.pool, channels * taps * sizeof(SAMPLE));

    rs.resampler_state.poolfill = taps / 2 + 1;
    rs.resampler_state.channels = channels;
    rs.resampler_state.outfreq = outfreq;
    rs.resampler_state.infreq = infreq;
    rs.resampler_state.taps = taps;
    rs.resampler_state.offset = 0;

	filt_sinc(rs.resampler_state.table, outfreq * taps, outfreq, cutoff, gain, taps);
    win_kaiser(rs.resampler_state.table, outfreq * taps, beta, taps);

    for(c=0; c < channels; c++)
	{
        rs.bufs[c] = (float*)_alloca(sizeof(float) * rs.bufsize);
		xray::memory::zero(rs.bufs[c], sizeof(float) * rs.bufsize);
	}

    readdata = &rs;

    ogg_stream_state os;
    ogg_page         og;
    ogg_packet       op;

    vorbis_dsp_state vd;
    vorbis_block     vb;
    vorbis_info      vi;

    long samplesdone=0;
    int eos;
    long bytes_written = 0, packetsdone=0;
    int ret=0;
    int result;

	if(channels > 255) 
	{
        LOG_ERROR("255 channels should be enough for anyone. (Sorry, but Vorbis doesn't support more)\n");
        return 1;
    }

    vorbis_info_init(&vi);
    /* Have vorbisenc choose a mode for us */

	if(vorbis_encode_setup_managed(&vi, channels, new_rate, 
                     -1,
					 new_bitrate*1000, 
                     -1))
	{
            LOG_ERROR("Mode initialisation failed: invalid parameters for bitrate\n");
            vorbis_info_clear(&vi);
            return 1;
    }


	vorbis_encode_setup_init(&vi);

    vorbis_analysis_init(&vd,&vi);
    vorbis_block_init(&vd,&vb);

	int serialno = rand();
	ogg_stream_init(&os, serialno);

	ogg_packet header_main;
	ogg_packet header_comments;
	ogg_packet header_codebooks;

	vorbis_comment vc;
	vorbis_comment_init(&vc);
	/* Build the packets */
	vorbis_analysis_headerout(&vd,&vc,&header_main,&header_comments,&header_codebooks);

	/* And stream them out */
	/* output the vorbis bos first, then the kate bos, then the fisbone packets */
	ogg_stream_packetin(&os,&header_main);
	result = ogg_stream_flush(&os, &og);
	while(result)
	{
		if(!result) break;
		ret = oe_write_page(&og, output_file, device);
		if(ret != og.header_len + og.body_len)
		{
			ret = 1;
		}
		result = ogg_stream_flush(&os, &og);
	}

	/* write the next Vorbis headers */
	ogg_stream_packetin(&os,&header_comments);
	ogg_stream_packetin(&os,&header_codebooks);

	result = ogg_stream_flush(&os, &og);
	while(result)
	{
		if(!result) break;
		ret = oe_write_page(&og, output_file, device);
		if(ret != og.header_len + og.body_len)
		{
			ret = 1;
		}
		result = ogg_stream_flush(&os, &og);
	}

    eos = 0;

	    /* Main encode loop - continue until end of file */
    while(!eos)
    {
        float **buffer = vorbis_analysis_buffer(&vd, read_size);
		//LOG_INFO("%f ", buffer[0][0]);
        long samples_read = read_resampled(readdata, device, 
                buffer, read_size);
		//LOG_INFO("samples read%d ", samples_read);
        if(samples_read ==0)
		{
            /* Tell the library that we wrote 0 bytes - signalling the end */
            vorbis_analysis_wrote(&vd,0);
		}
        else
        {
            samplesdone += samples_read;

            /* Tell the library how many samples (per channel) we wrote 
               into the supplied buffer */
            vorbis_analysis_wrote(&vd, samples_read);
        }

        /* While we can get enough data from the library to analyse, one
           block at a time... */
        while(vorbis_analysis_blockout(&vd,&vb)==1)
        {

            /* Do the main analysis, creating a packet */
            vorbis_analysis(&vb, NULL);
            vorbis_bitrate_addblock(&vb);

            while(vorbis_bitrate_flushpacket(&vd, &op)) 
            {
                /* Add packet to bitstream */
                ogg_stream_packetin(&os,&op);
                packetsdone++;

                /* If we've gone over a page boundary, we can do actual output,
                   so do so (for however many pages are available) */

                while(!eos)
                {
                    int result = ogg_stream_pageout(&os,&og);
                    if(!result) break;


                    ret = oe_write_page(&og, output_file, device);
                    if(ret != og.header_len + og.body_len)
                    {
                        ret = 1;
                    }
                    else
                        bytes_written += ret; 

                    if(ogg_page_eos(&og))
                        eos = 1;
                }
            }
        }
    }


    ogg_stream_clear(&os);

    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_info_clear(&vi);

	fp_error		= _controlfp_s( 0, old_float_point_control,	_MCW_EM);
	ASSERT			(fp_error == 0);

	return 0;
}

#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

}; // namespace ogg_utils

} // namespace sound
} // namespace xray

