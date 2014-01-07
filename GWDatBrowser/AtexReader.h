#pragma once

struct Picture
{
	Picture() : bitmap(NULL), imageData(NULL) {};
	Picture(Bitmap* bitmap, BYTE* imageData) : bitmap(bitmap), imageData(imageData) {};
	Bitmap* bitmap;
	BYTE* imageData;
};

Picture ProcessImageFile(unsigned char *img, int size);
void ExportImage(HWND wnd, Image* img, const CString& defaultName);
void ExportRaw(HWND wnd, unsigned char* data, int size, const CString& defaultName, const MFTEntry& file);