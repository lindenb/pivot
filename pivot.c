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
#include <stdio.h>
#include <stdlib.h>

#define DEF_VECTOR(T,name) T* name;size_t name##_size 
#define REALLOC_ONE(T,name) name=(T*)realloc(name,sizeof(T)*(name##_size + 1))
#define THROW_ERROR(msg) do { fputs(msg,stderr); exit(EXIT_FAILURE);} while(0)

typedef long integer_t;
typedef double floating_t;

enum	dataType_t 
	{
	NUMBER_T,
	STRING_T,
	NILL_T
	};

/** a column in the spreadsheet */
typedef struct column_t
	{
	/* name of the column */
	char* name;
	/* 0-based index in the spreadsheet */
	size_t index;
	/* data type of this column */
	enum dataType_t type;
	} Column,*ColumnPtr;
	
typedef struct cell_t
	{
	union {
		char* s;
		floating_t f;
		integer_t d;
		} core;
	char nil;
	}  Cell,*CellPtr;


static int CellStringCompare(const CellPtr c1, const CellPtr c2)
	{
	return strcmp(c1->core.s, c2->core.s);
	}

static int CellIStringCompare(const CellPtr c1, const CellPtr c2)
	{
	return strcasecmp(c1->core.s, c2->core.s);
	}

static int CellFloatCompare(const CellPtr c1, const CellPtr c2)
	{
	return c1->core.f - c2->core.f;
	}


static int (comparator*)(const CellPtr,const CellPtr ) ColumnComparator(const ColumnPtr col)
	{
	switch(col->type)
		{
		case FLOATING_T: return CellFloatCompare ; break;
		default: THROW_ERROR("error");
		}
	THROW_ERROR("error");
	}

int CellCompare(const ColumnPtr col,const CellPtr cell1,const CellPtr cell2)
	{
	if(cell1->nil)
		{
		return(cell2->nil?0:1);
		}
	return (cell2->nil?-1: ColumnComparator(col)(cell1,cell2));
	}

/** the spreadsheet, an array of column, some cells */
typedef struct spreadsheet_t
	{
	DEF_VECTOR(Column*,column);
	DEF_VECTOR(Cell,cell);
	
	} SpreadSheet,*SpreadSheetPtr;


/** create column */
static ColumnPtr createColumn(SpreadSheetPtr ptr,const char* name)
	{
	ColumnPtr newcol=NULL;
	ptr->column=(Column**)realloc(ptr->column,sizeof(ColumnPtr)*(ptr->column_size+1));
	if(ptr->column==NULL)  THROW_ERROR("out of memory");
	newcol=(ColumnPtr)calloc(1,sizeof(Column));
	if(newcol==NULL)  THROW_ERROR("out of memory");
	if(name==NULL)
		{
		char buff[100];
		sprintf(buff,"$%d",(1 + ptr->column_size ));
		newcol->name = strdup(buff);
		}
	else
		{
		newcol->name = strdup(name);
		}
	if(newcol->name==NULL)  THROW_ERROR("out of memory");
	newcol->index = ptr->column_size;
	newcol->type = NUMBER_T; /* default type is number */
	ptr->column[ptr->column_size]=newcol;
	ptr->column_size++;
	return ptr;
	}

static size_t SpreadSheetColumnCount(const SpreadSheetPtr ptr)
	{
	return ptr->column_size;
	}
static size_t SpreadSheetRowCount(const SpreadSheetPtr ptr)
	{
	return ptr->cell_size/SpreadSheetColumnCount(ptr);
	}


typedef struct indexset_t
	{
	DEF_VECTOR(size_t,index);
	} IndexSet,*IndexSetPtr;


typedef struct archetype_t
	{
	SpreadSheetPtr spreadsheet;
	IndexSetPtr column;
	size_t rowIndex;
	} Archetype,*ArchetypePtr;


static size_t  SpreadSheetColumnCount(cont SpreadSheetPtr ptr)
	{
	return ptr->column_size;
	}

static size_t  SpreadSheetRowCount(cont SpreadSheetPtr ptr)
	{
	return ptr->cell_size / SpreadSheetColumnCount(ptr);
	}

static CellPtr  SpreadSheetAt(SpreadSheetPtr ptr,size_t y, size_t x)
	{
	if(y>= SpreadSheetColumnCount(ptr)) THROW_ERROR("error");
	if(x>= SpreadSheetRowCount(ptr)) THROW_ERROR("error");
	return ptr->cell[y*SpreadSheetColumnCount(ptr) + x];
	}

static size_t IndexSetSize(const IndexSetPtr ptr)
	{
	return ptr->index_size;
	}

static size_t IndexSetAt(const IndexSetPtr ptr,size_t n)
	{
	if(n>= IndexSetSize(ptr)) THROW_ERROR("error");
	return ptr->index[n];
	}

static int ArchetypeCompare(
	const ArchetypePtr ptr1,
	const ArchetypePtr ptr2
	)
	{
	size_t i;
	if( ArchetypeSize(ptr1) != ArchetypeSize(ptr2) ) THROW_ERROR("error");
	for(i=0;i< ArchetypeSize(ptr1);++i)
		{
		const ColumnPtr col1= 
		int d= ArchetypeAt(ptr1,i) ArchetypeAt(ptr2,i);
		if( d!=0) return d;
		}
	return 0;
	}


int main(int argc,char** argv)
	{
	SpreadSheet t;

	return EXIT_SUCCESS;
	}
