#pragma once

#include <GfxMath.h>

enum encoding_type_t
{
	MONOCHROME,
	MONOCHROME_RLE,
	RUN_LENGHT_ENCODING,
	LOSSY,
	REDUCED_COLOR,
	REDUCED_COLOR_RLE,
	RAW,
};

#define METADATA_BYTES 0x9 // Should be 9 bytes
struct metadata_t
{
	unsigned char type;			        // 1 byte
	unsigned short width;		        // 2 bytes
	unsigned short height;		        // 2 bytes
	unsigned int totalBytes;	        // 4 bytes
	unsigned char colorDepth;			// Not included in file, only for encoding
    unsigned short monochromeCutoff;    // Not included in file, only for encoding
};

class Encoder
{
public:
	void Encode(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void Decode(metadata_t* metadata, unsigned char* stream, unsigned short* frameBuffer);

private:
	void EncodeMonochrome(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void EncodeMonochromeRLE(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void EncodeRunLengthEncoding(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void EncodeLossy(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void EncodeReducedColor(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void EncodeReducedColorRLE(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void EncodeRaw(metadata_t* metadata, unsigned short* frameBuffer, unsigned char* outputBuffer);
	void AddMetadata(metadata_t* metadata, unsigned char* outputBuffer);

	void DecodeMonochrome(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void DecodeMonochromeRLE(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void DecodeRunLengthEncoding(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void DecodeLossy(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void DecodeReducedColor(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void DecodeReducedColorRLE(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void DecodeRaw(metadata_t* metadata, unsigned char* stream, unsigned short* outputBuffer);
	void StripMetadata(metadata_t* metadata, unsigned char* stream);
};