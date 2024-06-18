/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 2014 - 2022 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* - Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************

File    : newlib_retarget.c
Purpose : Implement newlib functions that enable printf

-------------------------- END-OF-HEADER -----------------------------
*/
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <SEGGER_SEMIHOST.h>

void *_sbrk(ptrdiff_t incr)
{
  static uint8_t *__sbrk_heap;
  extern uint8_t __heap_start__[];
  extern uint8_t __heap_end__[];  
  if (!__sbrk_heap)
    __sbrk_heap = __heap_start__;
  if (__sbrk_heap + incr > __heap_end__)
    {
      errno = ENOMEM;
      return (void *)-1;
    }
  uint8_t *prev_heap = __sbrk_heap;
  __sbrk_heap += incr;
  return (void *)prev_heap;
}

#define	_FAPPEND 0x0008	/* append (writes guaranteed at the end) */
#define	_FCREAT	0x0200	/* open with file create */
#define	_FTRUNC 0x0400	/* open with truncation */
#define	O_RDONLY 0
#define	O_WRONLY 1
#define	O_RDWR 2
#define	O_APPEND _FAPPEND
#define	O_CREAT _FCREAT
#define	O_TRUNC	_FTRUNC
#define _FBINARY 0x10000
#define _FTEXT 0x20000
#define O_BINARY _FBINARY
#define O_TEXT _FTEXT

static int current_pos[20];

int _open(char *path, int flags, ...)
{	
  int aflags = 0;
  if (flags & O_BINARY)
    aflags |= 1;
  if (flags & O_RDWR)
    aflags |= 2;
  if (flags & O_CREAT)
    aflags |= 4;
  if (flags & O_TRUNC)
    aflags |= 4;
  if (flags & O_APPEND)
    {
      aflags &= ~4;
      aflags |= 8;
    }
  int file = SEGGER_SEMIHOST_Open(path, aflags, strlen(path));
  current_pos[file] = 0;
  return file;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  st->st_size = 1024;
  return 0;
}

int _isatty(int file)
{
  return file <= 2;
}

int _write(int file, char *ptr, int len)
{
  int res = SEGGER_SEMIHOST_Write(file, ptr, len);
  if (!res)
    {
      current_pos[file] += len;
      return len;
    }
  else
    return -1;
}

int _read(int file, char *ptr, int len)
{
  int res = SEGGER_SEMIHOST_Read(file, ptr, len);
  if (!res)
    {
      current_pos[file] += len;
      return len;
    }
  else
    return -1;
}

#define	SEEK_SET 0
#define	SEEK_CUR 1
#define	SEEK_END 2

int _lseek(int file, int ptr, int dir)
{
  int pos = current_pos[file];
  if (dir == SEEK_SET)
    pos = ptr ;
  else if (dir == SEEK_CUR)
    pos += ptr;
  else if (dir == SEEK_END)
    {
      int flen = SEGGER_SEMIHOST_FLen(file);
      pos = flen+ptr;     
    }
  return SEGGER_SEMIHOST_Seek(file, pos);
}

int _close(int file)
{
  return SEGGER_SEMIHOST_Close(file);
}

int _getpid(void)
{
  return 1;
}

int _kill(int pid, int sig)
{
 errno = EINVAL;
 return -1;
}

void _exit (int status)
{
  SEGGER_SEMIHOST_Exit(status);
  while (1);
}

int
_unlink (const char *path)
{
  return SEGGER_SEMIHOST_Remove(path, strlen(path));
}

int
_link (char *oldname, char *newname)
{
  return SEGGER_SEMIHOST_Rename(oldname, strlen(oldname), newname, strlen(newname));
}
