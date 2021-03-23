﻿#include "PoltergeistEngine/Image/JpegImage.hpp"
#include <jpeglib.h>
#include <iostream>
#include <csetjmp>

struct my_error_mgr
{
    jpeg_error_mgr publicErrorManager;
    jmp_buf setJumpBuffer;
};

void my_error_exit(j_common_ptr cinfo)
{
    my_error_mgr* error = (my_error_mgr*)cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(error->setJumpBuffer, 1);
}

bool JpegImage::IsValidHeader(FILE* file)
{
	uint16_t header;
	fread(&header, 1, 2, file);
	fseek(file, -2, SEEK_CUR);
	return header == 0xD8FF;
}

std::shared_ptr<JpegImage> JpegImage::LoadFromFile(FILE* file)
{
	jpeg_decompress_struct decompressInfo;
	my_error_mgr error;
	decompressInfo.err = jpeg_std_error(&error.publicErrorManager);
	error.publicErrorManager.error_exit = my_error_exit;
	if (setjmp(error.setJumpBuffer))
	{
		jpeg_destroy_decompress(&decompressInfo);
		throw std::runtime_error("jpg:jmp error");
	}

	jpeg_create_decompress(&decompressInfo);
	jpeg_stdio_src(&decompressInfo, file);
	jpeg_read_header(&decompressInfo, TRUE);
	jpeg_start_decompress(&decompressInfo);

	int rowLength = decompressInfo.output_width * decompressInfo.output_components;
	JSAMPARRAY pixelBuffer;
	pixelBuffer = (*decompressInfo.mem->alloc_sarray)
		((j_common_ptr)&decompressInfo, JPOOL_IMAGE, rowLength, 1);

	std::shared_ptr<JpegImage> result = std::make_shared<JpegImage>();
	result->m_data = new uint8_t[decompressInfo.output_width * decompressInfo.output_height * 3];
	size_t data_offest = 0;
	while (decompressInfo.output_scanline < decompressInfo.output_height)
	{
		jpeg_read_scanlines(&decompressInfo, pixelBuffer, 1);
		memcpy(result->m_data + data_offest, pixelBuffer[0], rowLength);
		data_offest += rowLength;
	}

	jpeg_finish_decompress(&decompressInfo);
	jpeg_destroy_decompress(&decompressInfo);

	if (error.publicErrorManager.num_warnings) throw std::runtime_error("Decompressing error");

	result->m_width = decompressInfo.output_width;
	result->m_height = decompressInfo.output_height;
}
