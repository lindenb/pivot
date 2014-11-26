/*
The MIT License (MIT)

Copyright (c) 2014 Pierre Lindenbaum

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef PIVOT_H
#define PIVOT_H
#include <stdio.h>
#include <stdlib.h>

/** stdlib */
void* _safeMalloc(const char*,int,size_t);
void* _safeCalloc(const char*,int,size_t,size_t);
void* _safeRealloc(const char*,int,void*,size_t);
#define safeMalloc(n) _safeMalloc(__FILE__,__LINE__,n)
#define safeCalloc(n,m) _safeCalloc(__FILE__,__LINE__,n,m)
#define safeRealloc(p,n) _safeRealloc(__FILE__,__LINE__,p,n)
#define WHERENL fprintf(stderr,"[%s:%d] ",__FILE__,__LINE__)
#define DIE_FAILURE(FormatLiteral,...) do { WHERENL; fprintf (stderr,"Git-Hash:"GIT_HASH". Exiting: " FormatLiteral "\n", ##__VA_ARGS__); exit(EXIT_FAILURE);} while(0)
#define DEBUG(FormatLiteral, ...)  do { fputs("[DEBUG]",stderr); WHERENL; fprintf (stderr,"" FormatLiteral "\n", ##__VA_ARGS__);} while(0)

typedef union scalar
	{
	char* s;
	double f;
	} Scalar,*ScalarPtr;

typedef struct column_key_t
	{
	int order;
	size_t column_index;
	}ColumnKey,*ColumnKeyPtr;

typedef struct column_key_list_t
	{
	ColumnKey* keys;
	size_t size;
	size_t max_column_index;
	}ColumnKeyList,*ColumnKeyListPtr;

typedef struct archetype_t
	{
	ScalarPtr values;
	size_t* rows;
	size_t row_count;
	} Archetype,*ArchetypePtr;

typedef struct archetype_list_t
	{
	ArchetypePtr archetypes;
	size_t size;
	} ArchetypeList,*ArchetypeListPtr;

typedef struct pivot_t
	{
	ColumnKeyList leftcols;
	ColumnKeyList topcols;
	ScalarPtr observed;
	ArchetypeList left;
	ArchetypeList top;
	}Pivot,*PivotPtr;

#endif

