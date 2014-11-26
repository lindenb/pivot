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

#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include "pivot.h"
#include "githash.h"

#ifdef XXXXX
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
#endif

void* _safeMalloc(const char* fname,int line, size_t size)
	{
	void * ptr=malloc(size);
	if(ptr==NULL)
		{
		DIE_FAILURE("Out of memory \"%s\":%d  N=%ld.\n",
			fname,line,size
			);
		}
	return ptr;
	}

void* _safeCalloc(const char* fname,int line, size_t n,size_t size)
	{
	void * ptr=calloc(n,size);
	if(ptr==NULL)
		{
		DIE_FAILURE("Out of memory \"%s\":%d N=%ld*%ld.\n",
			fname,line,n,size
			);
		}
	return ptr;
	}

void* _safeRealloc(const char* fname,int line, void* ptr,size_t size)
	{
	ptr=realloc(ptr,size);
	if(ptr==NULL)
		{
		DIE_FAILURE("Out of memory \"%s\":%d N=%ld\n",
			fname,line,size
			);
		}
	return ptr;
	}

static char* readLine(FILE* in)
	{
	size_t len=0UL;
	char* s=NULL;
	int c;
	if(feof(in)) return NULL;
	s=safeMalloc(1);
	while((c=fgetc(in))!=-1 && c!='\n')
		{
		s=safeRealloc(s,sizeof(char*)*(len+2));
		s[len++]=c;
		}
	if(len==0 && feof(in)) { free(s); return NULL;}
	s[len]=0;
	return s;
	}
	

static void parseKeys(ColumnKeyListPtr keyList, const char* arg)
	{
	char* p=(char*)arg;
	while(*p!=0)
		{
		char* p2;
		ColumnKeyPtr key=NULL;
		keyList->keys=(ColumnKeyPtr)safeRealloc(keyList->keys,sizeof(ColumnKey)*(keyList->count+1));
		key=&keyList->keys[keyList->count ];
		keyList->count++;
		
		key->order=1;
		if(*p=='+') ++p;
		else if(*p=='-')  {key->order=-1;++p;}
		
		key->column_index = strtoul(p,&p2,10);
		if( key->column_index < 1)
			{
			fprintf(stderr,"BAD COLUMN INDEX in %s.\n",arg);
			exit(EXIT_FAILURE);
			}
		key->column_index--;
		
		p=p2;
		
		if( key->column_index > keyList->max_column_index)
			{
			keyList->max_column_index =  key->column_index;
			}
			
		if(*p==0) break;
		if(*p==',')
			{
			++p;
			continue;
			}
		fprintf(stderr,"BAD input in %s.\n",arg);
		exit(EXIT_FAILURE);
		}
	}

static size_t lower_bound(ArchetypeListPtr archetypes,ArchetypePtr archetype,const ColumnKeyListPtr columnKeyList)
	{
	return 0UL;
	}

static void usage(int argc,char** argv)
	{
	fprintf(stderr,"\n\n%s\nAuthor: Pierre Lindenbaum PhD\nGit-Hash: "
		GIT_HASH
		"\nWWW: https://github.com/lindenb/ibddb\nCompilation: %s at %s\n\n",
		argv[0],__DATE__,__TIME__);
	fprintf(stderr,"Usage:\n\t%s (options) (stdin|file)\n\n",argv[0]);


	fputs("\n\n",stderr);
	}


int main(int argc,char** argv)
	{
	PivotPtr ptr = (PivotPtr)safeCalloc(1,sizeof(Pivot));
	FILE* in;
	for(;;)
		{
		struct option long_options[] =
		     {
			{"left",    required_argument, 0, 'L'},
 			{"top",    required_argument, 0, 'T'},
		       {0, 0, 0, 0}
		     };
		 /* getopt_long stores the option index here. */
		int option_index = 0;
	     	int c = getopt_long (argc, argv, "L:T:",
		                    long_options, &option_index);
		if(c==-1) break;
		switch(c)
			{
			case 'L': parseKeys(&ptr->leftcols,optarg); break;
			case 'T': parseKeys(&ptr->topcols,optarg); break;
			case 0: break;
			case '?': break;
			default: usage(argc,argv); exit(EXIT_FAILURE); break;
			}
		}
	if(optind==argc)
		{
		in=stdin;
		}
	else if(optind+1==argc)
		{
		in=fopen(argv[optind],"r");
		if(in==NULL)
			{
			fprintf(stderr,"Cannot open \"%s\" %s.\n",
				argv[optind],
				strerror(errno)
				);
			exit(EXIT_FAILURE);
			}
		}
	else
		{
		fputs("Illegal number of arguments.\n",stderr);
		return EXIT_FAILURE;
		}
	size_t nLine=0;

	while(!feof(in))
		{
		int side;
		size_t i;
		char* line=readLine(in);
		char** tokens=(char**)safeMalloc(sizeof(char*));
		size_t token_count=1UL;
		tokens[0]=line;
		for(i=0;line[i]!=0;++i)
			{
			if(line[i]!='\t') continue;
			
			line[i]=0;
			tokens=(char**)safeRealloc(tokens,sizeof(char*)*(token_count+1));
			tokens[token_count++]=&line[i+1];
			}
		
		
		for(side=0;side<2;++side)
			{
			ColumnKeyListPtr columnKeyList=(side==0?&ptr->leftcols:&ptr->topcols);
			ArchetypeListPtr archetypes=(side==0?&ptr->left:&ptr->top);
			//archetype
			Archetype archetype;
			memset((void*)archetype,0,sizeof(Archetype));
			//prepare values
			archetype.values = (ScalarPtr)safeCAlloc(columnKeyList->size,sizeof(Scalar)); 
			
			for(i=0;i< columnKeyList->size;++i)
				{
				ColumnKeyPtr key=(ColumnKeyPtr)&columnKeyList->keys[i];
				size_t column_index= key->column_index;
				if(column_index > = token_count) 
					{
					fprintf(stdert,"No enough column ");
					exit(EXIT_FAILURE);
					}
				if(IS_NUMERIC(key))
					{
					archetype.values[i].f= strtod(tokens[column_index]);
					}
				else
					{
					archetype.values[i].s= strdup(tokens[column_index]);
					}
				}
			size_t insert_index=lower_bound(archetypes,archetype,columnKeyList);
			if(insert_index == tokens->size ||
				ArchetypeCompare(&archetypes->archetypes[insert_index],archetype.value,columnKeyList)!=0)
				{
				//realloc
				//tokens[insert_index]=archetype;
				}
			
			//rows+=currLine;
			}
		
		tokens(tokens);
		free(line);
		}
	free(ptr);
	return EXIT_SUCCESS;
	}
