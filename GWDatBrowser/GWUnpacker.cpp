#include "stdafx.h"
#include <stdio.h>
#include "Xentax.h"
#include "GWUnpacker.h"
#include <algorithm>
#include <map>

using namespace std;

TCHAR* typeToString(int type)
{
	switch(type)
	{
	case AMAT:
		return L"AMAT";
	case AMP:
		return L"Amp";
	case ATEXDXT1:
		return L"ATEXDXT1";
	case ATEXDXT2:
		return L"ATEXDXT2";
	case ATEXDXT3:
		return L"ATEXDXT3";
	case ATEXDXT4:
		return L"ATEXDXT4";
	case ATEXDXT5:
		return L"ATEXDXT5";
	case ATEXDXTL:
		return L"ATEXDXTL";
	case ATEXDXTN:
		return L"ATEXDXTN";
	case ATEXDXTA:
		return L"ATEXDXTA";
	case ATTXDXT1:
		return L"ATTXDXT1";
	case ATTXDXT3:
		return L"ATTXDXT3";
	case ATTXDXT5:
		return L"ATTXDXT5";
	case ATTXDXTN:
		return L"ATTXDXTN";
	case ATTXDXTA:
		return L"ATTXDXTA";
	case ATTXDXTL:
		return L"ATTXDXTL";
	case DDS:
		return L"DDS";
	case FFNA:
		return L"FFNA";
	case MFTBASE:
		return L"MFTBase";
	case NOTREAD:
		return L"";
	case SOUND:
		return L"Sound";
	case TEXT:
		return L"Text";
	case UNKNOWN:
		return L"Unknown";
	default:
		return L"Unknown";
	}
}

void GWDat::seek(__int64 offset, int origin)
{
	LARGE_INTEGER i;
	i.QuadPart=offset;
	SetFilePointerEx(fileHandle,i,NULL,FILE_BEGIN);
}

void GWDat::read(void *buffer, int size, int count)
{
	DWORD bread;
	int errcde = ReadFile(fileHandle,buffer,size*count,&bread,NULL);
	if (!errcde)
	{
		TCHAR text[2048];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,text,1024,NULL);
		printf("%s\n",text);
	}
}

unsigned char* GWDat::readFile(unsigned int n, bool translate /* = true*/)
{
	MFTEntry& m = MFT[n];

	criticalSection.Lock();
	if (m.type == NOTREAD)
		filesRead += 1;
	criticalSection.Unlock();

	//Don't read files that were already read if we just need the type
	if (m.type != NOTREAD && !translate)
	{
		return NULL;
	}
	
	if (!m.b) 
	{
		criticalSection.Lock();
		m.type = MFTBASE;
		m.uncompressedSize = 0;
		mftBaseFiles += 1;
		criticalSection.Unlock();
		return NULL;
	}
	
	unsigned char *Input = new unsigned char[m.Size];
	unsigned char *Output = NULL;
	int OutSize = 0;
	
	criticalSection.Lock();

	seek(m.Offset, 0);
	read(Input, m.Size, 1);
	
	criticalSection.Unlock();

	if (m.a)
		UnpackGWDat(Input, m.Size, Output, OutSize);        
	else
	{
		Output = new unsigned char[m.Size];
		memcpy(Output, Input, m.Size);
		OutSize = m.Size;
	}
	delete[] Input;
	
	if (Output)
	{
		if (m.type == NOTREAD)
		{
			criticalSection.Lock();
			int type = 0;
			unsigned int i=((unsigned int*)Output)[0];
			unsigned int k=((unsigned int*)Output)[1];
			int i2=i&0xffff;
			int i3=i&0xffffff;

			switch (i)
			{
			case 'XTTA':
				textureFiles += 1;
				switch(k)
				{
				case '1TXD':
					type = ATTXDXT1;
					break;
				case '3TXD':
					type = ATTXDXT3;
					break;
				case '5TXD':
					type = ATTXDXT5;
					break;
				case 'NTXD':
					type = ATTXDXTN;
					break;
				case 'ATXD':
					type = ATTXDXTA;
					break;
				case 'LTXD':
					type = ATTXDXTL;
					break;
				}
				break;
			case 'XETA':
				textureFiles += 1;
				switch(k)
				{
				case '1TXD':
					type = ATEXDXT1;
					break;
				case '2TXD':
					type = ATEXDXT2;
					break;
				case '3TXD':
					type = ATEXDXT3;
					break;
				case '4TXD':
					type = ATEXDXT4;
					break;
				case '5TXD':
					type = ATEXDXT5;
					break;
				case 'NTXD':
					type = ATEXDXTN;
					break;
				case 'ATXD':
					type = ATEXDXTA;
					break;
				case 'LTXD':
					type = ATEXDXTL;
					break;
				}
				break;
			case '===;':
			case '***;':
				type = TEXT;
				textFiles += 1;
				break;
			case 'anff':
				type = FFNA;
				ffnaFiles += 1;
				break;
			case ' SDD':
				type = DDS;
				textureFiles += 1;
				break;
			case 'TAMA':
				type = AMAT;
				amatFiles += 1;
				break;
			default:
				type = UNKNOWN;
			}
			switch (i2)
			{
			case 0xFAFF:
			case 0xFBFF:
				type = SOUND;
				break;
			default:
				break;
			}

			switch (i3)
			{
			case 'PMA':
				type = AMP;
				break;
			case 0x334449:
				type = SOUND;
				break;
			default:
				break;
			}

			if (type == AMP || type == SOUND)
				soundFiles += 1;
			else if (type == UNKNOWN)
				unknownFiles += 1;

			m.type = type;
			m.uncompressedSize = OutSize;
			criticalSection.Unlock();
		}	
	}
	return Output;
}

bool compareH(MFTExpansion& a, MFTExpansion b)
{
	return a.FileOffset < b.FileOffset;
}

unsigned int GWDat::readDat(const TCHAR* file)
{
	fileHandle = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		TCHAR text[2048];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,text,2048,NULL);
		
		CString s;
		s.Format(L"Error while opening \"%s\": %s", file, text);
		MessageBox(NULL,s, L"Error", MB_ICONERROR | MB_OK);
		return 0;
	}

	read(&GWHead,sizeof(GWHead),1);
	
	
	if (!(GWHead.ID[0]==0x33 && GWHead.ID[1]==0x41 && GWHead.ID[2]==0x4e && GWHead.ID[3]==0x1a))
	{
		CString s;
		s.Format(L"The input file \"%s\"is not a Guild Wars datafile!", file);
		MessageBox(NULL,s, L"Error", MB_ICONERROR | MB_OK);
		CloseHandle(fileHandle);
		return 0;
	}

	//read reserved MFT entries
	seek(GWHead.MFTOffset,SEEK_SET);
	read(&MFTH,sizeof(MFTH),1);
	for (int x = 0; x < 15; ++x)
	{
		MFTEntry ME;
		read(&ME,0x18,1);
		ME.type = NOTREAD;
		ME.uncompressedSize = -1;
		ME.Hash = 0;
		MFT.push_back(ME);
	}

	//read Hashlist
	seek(MFT[1].Offset,SEEK_SET);
	int mftxsize=MFT[1].Size/sizeof(MFTExpansion);
	for (unsigned int x=0; x<MFT[1].Size/sizeof(MFTExpansion); x++)
	{
		MFTExpansion _MFTX;
		read(&_MFTX,sizeof(MFTExpansion),1);
		MFTX.push_back(_MFTX);
	}

	std::sort(MFTX.begin(), MFTX.end(), compareH);

	//read MFT entries
	unsigned int hashcounter = 0;
	while (MFTX[hashcounter].FileOffset < 16)
		++hashcounter;

	seek(GWHead.MFTOffset + 24 * 16,SEEK_SET);
	for (int x = 16; x < MFTH.EntryCount - 1; ++x)
	{
		MFTEntry ME;
		read(&ME,0x18,1);
		ME.type = NOTREAD;
		ME.uncompressedSize = -1;

		if (hashcounter < MFTX.size() && x == MFTX[hashcounter].FileOffset)
		{
			ME.Hash = MFTX[hashcounter].FileNumber;
			MFT.push_back(ME);

			while (hashcounter + 1 < MFTX.size() && MFTX[hashcounter].FileOffset == MFTX[hashcounter + 1].FileOffset)
			{
				++hashcounter;
				ME.Hash = MFTX[hashcounter].FileNumber;
				MFT.push_back(ME);
			}

			++hashcounter;
		}
		else
		{
			ME.Hash = 0;
			MFT.push_back(ME);
		}		
	}
	
	filesRead = 0;
	textureFiles = 0;
	soundFiles = 0;
	ffnaFiles = 0;
	unknownFiles = 0;
	textFiles = 0;
	mftBaseFiles = 0;
	amatFiles = 0;

	return (unsigned int)MFT.size();
}

class compareNAsc
{
public:
	compareNAsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return a > b;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareNDsc
{
public:
	compareNDsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return b > a;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareIDAsc
{
public:
	compareIDAsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return MFT[a].ID > MFT[b].ID;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareIDDsc
{
public:
	compareIDDsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return MFT[b].ID > MFT[a].ID;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareOffsetAsc
{
public:
	compareOffsetAsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return MFT[a].Offset > MFT[b].Offset;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareOffsetDsc
{
public:
	compareOffsetDsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return MFT[b].Offset > MFT[a].Offset;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareSizeAsc
{
public:
	compareSizeAsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return MFT[a].Size > MFT[b].Size;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareSizeDsc
{
public:
	compareSizeDsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return MFT[b].Size > MFT[a].Size;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareUncompressedSizeAsc
{
public:
	compareUncompressedSizeAsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return MFT[a].uncompressedSize > MFT[b].uncompressedSize;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareUncompressedSizeDsc
{
public:
	compareUncompressedSizeDsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return MFT[b].uncompressedSize > MFT[a].uncompressedSize;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareFlagsAsc
{
public:
	compareFlagsAsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		if ( MFT[a].a != MFT[b].a)
			return  MFT[a].a > MFT[b].a;
		else if (MFT[a].b != MFT[b].b)
			return  MFT[a].b > MFT[b].b;
		else
			return  MFT[a].c > MFT[b].c;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareFlagsDsc
{
public:
	compareFlagsDsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		if (MFT[b].a != MFT[a].a)
			return MFT[b].a > MFT[a].a;
		else if (MFT[b].b != MFT[a].b)
			return MFT[b].b > MFT[a].b;
		else
			return MFT[b].c > MFT[a].c;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareTypeAsc
{
public:
	compareTypeAsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return MFT[a].type > MFT[b].type;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareTypeDsc
{
public:
	compareTypeDsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return MFT[b].type > MFT[a].type;
	}
protected:
	const vector<MFTEntry>& MFT;
};


class compareHashAsc
{
public:
	compareHashAsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return (unsigned int)MFT[a].Hash > (unsigned int)MFT[b].Hash;
	}
protected:
	const vector<MFTEntry>& MFT;
};

class compareHashDsc
{
public:
	compareHashDsc(const vector<MFTEntry>& MFT) : MFT(MFT)
	{

	}

	bool operator()(int a, int b)
	{
		return (unsigned int)MFT[b].Hash > (unsigned int)MFT[a].Hash;
	}
protected:
	const vector<MFTEntry>& MFT;
};

void GWDat::sort(unsigned int* index, int column, bool ascending)
{
	switch (column)
	{
	case 0:
		if (ascending)
			std::sort(index, index + MFT.size(), compareNAsc(MFT));
		else
			std::sort(index, index + MFT.size(), compareNDsc(MFT));
		break;
	case 1:
		if (ascending)
			std::sort(index, index + MFT.size(), compareIDAsc(MFT));
		else
			std::sort(index, index + MFT.size(), compareIDDsc(MFT));
		break;
	case 2:
	case 3:
		if (ascending)
			std::sort(index, index + MFT.size(), compareOffsetAsc(MFT));
		else
			std::sort(index, index + MFT.size(), compareOffsetDsc(MFT));
		break;
	case 4:
		if (ascending)
			std::sort(index, index + MFT.size(), compareSizeAsc(MFT));
		else
			std::sort(index, index + MFT.size(), compareSizeDsc(MFT));
		break;
	case 5:
		if (ascending)
			std::sort(index, index + MFT.size(), compareUncompressedSizeAsc(MFT));
		else
			std::sort(index, index + MFT.size(), compareUncompressedSizeDsc(MFT));
		break;
	case 6:
		if (ascending)
			std::sort(index, index + MFT.size(), compareFlagsAsc(MFT));
		else
			std::sort(index, index + MFT.size(), compareFlagsDsc(MFT));
		break;
	case 7:
		if (ascending)
			std::sort(index, index + MFT.size(), compareTypeAsc(MFT));
		else
			std::sort(index, index + MFT.size(), compareTypeDsc(MFT));
		break;
	case 8:
		if (ascending)
			std::sort(index, index + MFT.size(), compareHashAsc(MFT));
		else
			std::sort(index, index + MFT.size(), compareHashDsc(MFT));
		break;
	}
}