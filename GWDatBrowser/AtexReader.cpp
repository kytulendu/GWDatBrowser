
#include "stdafx.h"
#include "AtexAsm.h"
#include <stdio.h>

#pragma pack(1)

union RGBA
{
	unsigned char c[4];
	struct { unsigned char r,g,b,a; }; 
	unsigned int dw;
}; 

union DXT1Color
{
	struct
	{
		unsigned	r1 :5,
					g1 :6,
					b1 :5,
					r2 :5,
					g2 :6,
					b2 :5;
	};
	struct { unsigned short c1,c2;};
};

struct DXT5Alpha
{
	unsigned char a0,a1;
	__int64 table;
};

RGBA *ProcessDXT1(unsigned char *data,int xr, int yr)
{
	DXT1Color *coltable=new DXT1Color[xr*yr/16];
	unsigned int *blocktable=new unsigned int[xr*yr/16];

	unsigned int *d=(unsigned int*)data;

	for (int x=0; x<xr*yr/16; x++)
	{
		coltable[x]=*(DXT1Color*)&d[x*2];
		blocktable[x]=d[x*2+1];
	}

	RGBA *image=new RGBA[xr*yr];
	memset(image,0,xr*yr*4);

	int p=0;
	for (int y=0; y<yr/4; y++)
		for (int x=0; x<xr/4; x++)
		{
			RGBA ctbl[4]; memset(ctbl,255,16);
			DXT1Color c=coltable[p];
			ctbl[0].r=c.r1<<3;
			ctbl[0].g=c.g1<<2;
			ctbl[0].b=c.b1<<3;
			ctbl[1].r=c.r2<<3;
			ctbl[1].g=c.g2<<2;
			ctbl[1].b=c.b2<<3;

			if (c.c1>c.c2)
			{
				ctbl[2].r=(int)((ctbl[0].r*2+ctbl[1].r)/3.);
				ctbl[2].g=(int)((ctbl[0].g*2+ctbl[1].g)/3.);
				ctbl[2].b=(int)((ctbl[0].b*2+ctbl[1].b)/3.);
				ctbl[3].r=(int)((ctbl[0].r+ctbl[1].r*2)/3.);
				ctbl[3].g=(int)((ctbl[0].g+ctbl[1].g*2)/3.);
				ctbl[3].b=(int)((ctbl[0].b+ctbl[1].b*2)/3.);
			}
			else
			{
				ctbl[2].r=(int)((ctbl[0].r+ctbl[1].r)/2.);
				ctbl[2].g=(int)((ctbl[0].g+ctbl[1].g)/2.);
				ctbl[2].b=(int)((ctbl[0].b+ctbl[1].b)/2.);
				ctbl[3].r=0;
				ctbl[3].g=0;
				ctbl[3].b=0;
				ctbl[3].a=0;
			}

			unsigned int t=blocktable[p];

			for (int b=0; b<4; b++)
				for (int a=0; a<4; a++)
				{
					image[x*4+a+(y*4+b)*xr]=ctbl[t&3];
					t=t>>2;
				}

			p++;
		}    

	delete[] coltable;
	delete[] blocktable;
	return image;
}

RGBA *ProcessDXT3(unsigned char *data,int xr, int yr)
{
	DXT1Color *coltable=new DXT1Color[xr*yr/16];
	__int64 *alphatable=new __int64[xr*yr/16];
	unsigned int *blocktable=new unsigned int[xr*yr/16];

	unsigned int *d=(unsigned int*)data;

	for (int x=0; x<xr*yr/16; x++)
	{
		alphatable[x]=((__int64*)d)[x*2];
		coltable[x]=*(DXT1Color*)&d[x*4+2];
		blocktable[x]=d[x*4+3];
	}


	RGBA *image=new RGBA[xr*yr];
	memset(image,0,xr*yr*4);

	int p=0;
	for (int y=0; y<yr/4; y++)
		for (int x=0; x<xr/4; x++)
		{
			RGBA ctbl[4]; memset(ctbl,255,16);
			DXT1Color c=coltable[p];
			ctbl[0].r=c.r1<<3;
			ctbl[0].g=c.g1<<2;
			ctbl[0].b=c.b1<<3;
			ctbl[1].r=c.r2<<3;
			ctbl[1].g=c.g2<<2;
			ctbl[1].b=c.b2<<3;

			ctbl[2].r=(int)((ctbl[0].r*2+ctbl[1].r)/3.);
			ctbl[2].g=(int)((ctbl[0].g*2+ctbl[1].g)/3.);
			ctbl[2].b=(int)((ctbl[0].b*2+ctbl[1].b)/3.);
			ctbl[3].r=(int)((ctbl[0].r+ctbl[1].r*2)/3.);
			ctbl[3].g=(int)((ctbl[0].g+ctbl[1].g*2)/3.);
			ctbl[3].b=(int)((ctbl[0].b+ctbl[1].b*2)/3.);

			unsigned int t=blocktable[p];
			__int64 k=alphatable[p];

			for (int b=0; b<4; b++)
				for (int a=0; a<4; a++)
				{
					image[x*4+a+(y*4+b)*xr]=ctbl[t&3];
					t=t>>2;
					image[x*4+a+(y*4+b)*xr].a=(unsigned char)((k&15)<<4);
					k=k>>4;
				}

				p++;
		}    

	delete[] coltable;
	delete[] blocktable;
	delete[] alphatable;
	return image;
}

RGBA *ProcessDXT5(unsigned char *data,int xr, int yr)
{
	DXT1Color *coltable=new DXT1Color[xr*yr/16];
	DXT5Alpha *alphatable=new DXT5Alpha[xr*yr/16];
	unsigned int *blocktable=new unsigned int[xr*yr/16];

	unsigned int *d=(unsigned int*)data;

	for (int x=0; x<xr*yr/16; x++)
	{
		alphatable[x]=*(DXT5Alpha*)&(((__int64*)d)[x*2]);
		coltable[x]=*(DXT1Color*)&d[x*4+2];
		blocktable[x]=d[x*4+3];
	}


	RGBA *image=new RGBA[xr*yr];
	memset(image,0,xr*yr*4);

	int p=0;
	for (int y=0; y<yr/4; y++)
		for (int x=0; x<xr/4; x++)
		{
			RGBA ctbl[4]; memset(ctbl,255,16);
			DXT1Color c=coltable[p];
			ctbl[0].r=c.r1<<3;
			ctbl[0].g=c.g1<<2;
			ctbl[0].b=c.b1<<3;
			ctbl[1].r=c.r2<<3;
			ctbl[1].g=c.g2<<2;
			ctbl[1].b=c.b2<<3;

			ctbl[2].r=(int)((ctbl[0].r*2+ctbl[1].r)/3.);
			ctbl[2].g=(int)((ctbl[0].g*2+ctbl[1].g)/3.);
			ctbl[2].b=(int)((ctbl[0].b*2+ctbl[1].b)/3.);
			ctbl[3].r=(int)((ctbl[0].r+ctbl[1].r*2)/3.);
			ctbl[3].g=(int)((ctbl[0].g+ctbl[1].g*2)/3.);
			ctbl[3].b=(int)((ctbl[0].b+ctbl[1].b*2)/3.);

			unsigned char atbl[8];
			DXT5Alpha l=alphatable[p];

			atbl[0]=l.a0;
			atbl[1]=l.a1;

			if (l.a0>l.a1)
			{
				for (int z=0; z<6; z++)
					atbl[z+2]=((6-z)*l.a0+(z+1)*l.a1)/7;
			}
			else
			{
				for (int z=0; z<4; z++)
					atbl[z+2]=((4-z)*l.a0+(z+1)*l.a1)/5;
				atbl[6]=0;
				atbl[7]=255;
			}

			unsigned int t=blocktable[p];
			__int64 k=alphatable[p].table;

			for (int b=0; b<4; b++)
				for (int a=0; a<4; a++)
				{
					image[x*4+a+(y*4+b)*xr]=ctbl[t&3];
					t=t>>2;
					image[x*4+a+(y*4+b)*xr].a=atbl[k&7];
					k=k>>3;
				}

				p++;
		}    

	delete[] coltable;
	delete[] blocktable;
	delete[] alphatable;
	return image;
}

char fname[2048];

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	unsigned int  num = 0;    // number of image encoders
	unsigned int  size = 0;   // size of the image encoder array in bytes

	GetImageEncodersSize(&num, &size);
	if(size == 0)return -1;

	ImageCodecInfo* imageCodecInfo = new ImageCodecInfo[size];
	GetImageEncoders(num, size, imageCodecInfo);

	for(unsigned int i = 0; i < num; ++i)
	{
		if( wcscmp(imageCodecInfo[i].MimeType, format) == 0 )
		{
			*pClsid = imageCodecInfo[i].Clsid;
			delete[] imageCodecInfo;
			return i;
		}    
	}
	delete[] imageCodecInfo;
	return -1;
}

Picture ProcessImageFile(unsigned char *img, int size)
{
	int id1,id2;
	
	id1=((unsigned int*)img)[0];
	id2=((unsigned int*)img)[1];

	if (id1!='XTTA' && id1!='XETA')
	{
		printf("File NOT an ATEX/ATTX file!\n");
		return Picture();
	}

	if ((id2&0xffffff)!='TXD')
	{
		printf("File NOT DXT compressed!\n");
		return Picture();
	}

	int cmptype=id2>>24;

	SImageDescriptor r;
	r.xres=*(unsigned short*)(img+8);
	r.yres=*(unsigned short*)(img+10);
	r.Data=img;
	r.imageformat=0xf;
	r.a=size;
	r.b=6;
	r.c=0;

	RGBA *output=new RGBA[r.xres*r.yres];
	r.image=(unsigned char*)output;

	RGBA *image = NULL;

	switch (cmptype)
	{
	case '1':
		AtexDecompress((unsigned int*)img,size,0xf,r,(unsigned int*)output);
		image=ProcessDXT1((unsigned char*)output,r.xres,r.yres);
		break;
	case '2':
	case '3':
	case 'N':
		AtexDecompress((unsigned int*)img,size,0x11,r,(unsigned int*)output);
		image=ProcessDXT3((unsigned char*)output,r.xres,r.yres);
		break;
	case '4':
	case '5':
		AtexDecompress((unsigned int*)img,size,0x13,r,(unsigned int*)output);
		image=ProcessDXT5((unsigned char*)output,r.xres,r.yres);
		break;
	case 'L':
		AtexDecompress((unsigned int*)img,size,0x12,r,(unsigned int*)output);
		image=ProcessDXT5((unsigned char*)output,r.xres,r.yres);
		for (int x=0; x<r.xres*r.yres; x++)
		{
			image[x].r=(image[x].r*image[x].a)/255;
			image[x].g=(image[x].g*image[x].a)/255;
			image[x].b=(image[x].b*image[x].a)/255;
		}
		break;
	default:
		printf("Unsupported compression format! (%c)\n",cmptype);
		delete[] output;
		return Picture();
		break;
	}

	Bitmap* b = new Bitmap(r.xres, r.yres, 4* r.xres, PixelFormat32bppARGB, (BYTE*) image);
	delete[] output;

	return Picture (b, (BYTE*) image);
}

void ExportImage(HWND wnd, Image* img, const CString& defaultName)
{
	TCHAR filename[1024];

	if (defaultName.IsEmpty())
		_tcscpy(filename, L"exported_file");
	else
		_tcsncpy(filename, defaultName, 1024);

	OPENFILENAME of;
	ZeroMemory(&of, sizeof(of));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = wnd;
	of.lpstrFile = filename;
	of.nMaxFile = 1024;
	of.lpstrFilter =L"PNG (.png)\0*.png\0Bitmap (.bmp)\0*.bmp\0Jpeg (.jpg)\0*.jpg\0Tiff (.tiff)\0*.tiff\0";
	if (!GetSaveFileName(&of))
		return;

	CString file(of.lpstrFile);

	CLSID pngClsid;
	switch(of.nFilterIndex)
	{
	case 1:
		GetEncoderClsid(L"image/png", &pngClsid);
		if (file.Left(file.GetLength() - file.ReverseFind(L'.')) != L".png")
			file += L".png";
		break;
	case 2:
		GetEncoderClsid(L"image/bmp", &pngClsid);
		if (file.Left(file.GetLength() - file.ReverseFind(L'.')) != L".bmp")
			file += L".bmp";
		break;
	case 3:
		GetEncoderClsid(L"image/jpeg", &pngClsid);
		if (file.Left(file.GetLength() - file.ReverseFind(L'.')) != L".jpg")
			file += L".jpg";
		break;
	case 4:
		GetEncoderClsid(L"image/tiff", &pngClsid);
		if (file.Left(file.GetLength() - file.ReverseFind(L'.')) != L".tiff")
			file += L".tiff";
		break;
	default:
		GetEncoderClsid(L"image/png", &pngClsid);
		if (file.Left(file.GetLength() - file.ReverseFind(L'.')) != L".png")
			file += L".png";
		break;
	}

	img->Save(file, &pngClsid);
}

void ExportRaw( HWND wnd, unsigned char* data, int size, const CString& defaultName, const MFTEntry& file)
{
	TCHAR filename[1024];

	if (defaultName.IsEmpty())
		_tcscpy(filename, L"exported_file");
	else
		_tcsncpy(filename, defaultName, 1024);

	OPENFILENAME of;
	ZeroMemory(&of, sizeof(of));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = wnd;
	of.lpstrFile = filename;
	of.nMaxFile = 1024;
	of.Flags = OFN_OVERWRITEPROMPT;
	
	if (file.type == SOUND || file.type == AMP)
		of.lpstrFilter = L"mp3 (.mp3)\0*.mp3\0\0";
	else if (file.type == TEXT)
		of.lpstrFilter = L"Text (.txt)\0*.txt\0\0";
	else if (file.type == DDS)
		of.lpstrFilter = L"DDS (.dds)\0*.dds\0\0";
	else
		of.lpstrFilter = L"raw (.raw)\0*.raw\0\0";
	
	if (!GetSaveFileName(&of))
		return;

	CString outfile(of.lpstrFile);

	if (file.type == SOUND || file.type == AMP)
	{
		if (outfile.Left(outfile.GetLength() - outfile.ReverseFind(L'.')) != L".mp3")
			outfile += L".mp3";
	}
	else if (file.type == TEXT)
	{	
		if (outfile.Left(outfile.GetLength() - outfile.ReverseFind(L'.')) != L".txt")
			outfile += L".txt";
	}
	else if (file.type == DDS)
	{	
		if (outfile.Left(outfile.GetLength() - outfile.ReverseFind(L'.')) != L".dds")
			outfile += L".dds";
	}
	else
		if (outfile.Left(outfile.GetLength() - outfile.ReverseFind(L'.')) != L".raw")
			outfile += L".raw";

	HANDLE f = CreateFile(outfile,GENERIC_WRITE,NULL,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD written = 0;
	WriteFile(f, data, size, &written, NULL);
	CloseHandle(f);
}