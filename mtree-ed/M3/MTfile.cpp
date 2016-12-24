/*********************************************************************
*                                                                    *
* Copyright (c) 1997,1998, 1999                                      *
* Multimedia DB Group and DEIS - CSITE-CNR,                          *
* University of Bologna, Bologna, ITALY.                             *
*                                                                    *
* All Rights Reserved.                                               *
*                                                                    *
* Permission to use, copy, and distribute this software and its      *
* documentation for NON-COMMERCIAL purposes and without fee is       *
* hereby granted provided  that this copyright notice appears in     *
* all copies.                                                        *
*                                                                    *
* THE AUTHORS MAKE NO REPRESENTATIONS OR WARRANTIES ABOUT THE        *
* SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING  *
* BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,      *
* FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. THE AUTHOR  *
* SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A      *
* RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS    *
* DERIVATIVES.                                                       *
*                                                                    *
*********************************************************************/

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#ifdef UNIX
#include <unistd.h>
#else
#include <io.h>
#endif

#ifdef UNIX
#define O_BINARY 0
#endif

#include "MTfile.h"

extern int IOread, IOwrite;

// The first page in the file has these "magic words"
// and the head of the deleted page list.
static char magic[]="GiST data file";

void
MTfile::Create(const char *filename)
{
	if(IsOpen()) return;
	fileHandle=open(filename, O_RDWR|O_BINARY);
	if(fileHandle>=0) {
		close(fileHandle);
		return;
	}
	fileHandle=open(filename, O_BINARY|O_RDWR|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
	if(fileHandle<0) return;
	SetOpen(1);

	/* Reserve page 0 */
	char *page=new char[PageSize()];

	memset(page, 0, PageSize());
	memcpy(page, magic, sizeof magic);
	write(fileHandle, page, PageSize());
	delete[] page;
}

void
MTfile::Open(const char *filename)
{
	char *page;

	if(IsOpen()) return;
	fileHandle=open(filename, O_RDWR|O_BINARY);
	if(fileHandle<0) return;
	// Verify that the magic words are there
	page=new char[PageSize()];
	read(fileHandle, page, PageSize());
	if(memcmp(page, magic, sizeof(magic))) {
		close(fileHandle);
		delete page;
		return;
	}
	delete page;
	SetOpen(1);
}

void
MTfile::Close()
{
	if(!IsOpen()) return;
	close(fileHandle);
	SetOpen(0);
}

void
MTfile::Read(GiSTpage page, char *buf)
{
	if(IsOpen()) {
		lseek(fileHandle, page*PageSize(), SEEK_SET);
		read(fileHandle, buf, PageSize());
		IOread++;
	}
}

void
MTfile::Write(GiSTpage page, const char *buf)
{
	if(IsOpen()) {
		lseek(fileHandle, page*PageSize(), SEEK_SET);
		write(fileHandle, buf, PageSize());
		IOwrite++;
	}
}

GiSTpage
MTfile::Allocate()
{
	GiSTpage page;
	char *buf;

	if(!IsOpen()) return (0);
	// See if there's a deleted page
	buf=new char[PageSize()];
	Read(0, buf);
	memcpy(&page, buf+sizeof(magic), sizeof(GiSTpage));
	if(page) {
		// Reclaim this page
		Read(page, buf);
		Write(0, buf);
	}
	else {
		page=lseek(fileHandle, 0, SEEK_END)/PageSize();
		memset(buf, 0, PageSize());
		write(fileHandle, buf, PageSize());
	}
	delete[] buf;
	return page;
}

void
MTfile::Deallocate(GiSTpage page)
{
	char *buf;
	GiSTpage temp;

	if(!IsOpen()) return;
	// Get the old head of the list
	buf=new char[PageSize()];
	Read(0, buf);
	memcpy(&temp, buf+sizeof(magic), sizeof(GiSTpage));
	// Write the new head of the list
	memcpy(buf+sizeof(magic), &page, sizeof(GiSTpage));
	Write(0, buf);
	// In our new head, put link to old head
	memcpy(buf+sizeof(magic), &temp, sizeof(GiSTpage));
	Write(page, buf);
	delete buf;
}
