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

#include <cstring>
#include <unistd.h>
#include <getopt.h>
#include <cerrno>
#include "pivot.hh"
#include "githash.h"

using namespace std;

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


class _StringType:public DataType
	{
	public:
		virtual void parse(Scalar& v,const char* s,size_t len)
			{
			v.s=new char[len+1];
			strncpy(v.s,s,len);
			}
		virtual bool compare(const Scalar& a,const Scalar& b)
			{
			return strcmp(a.s,b.s)<0;
			}
		virtual void dispose(Scalar& v)
			{
			delete[] v.s;
			}
	};
static DataType* TYPE_STRING = new  _StringType;


class _DoubleType:public DataType
	{
	public:
		virtual void parse(Scalar& v,const char* s,size_t len)
			{
			char* p2;
			v.f=strtod(s,&p2);
			while(isspace(*p2)) ++p2;
			if(p2< s+len) cerr << "boum";
			}
		virtual bool compare(const Scalar& a,const Scalar& b)
			{
			return a.f < b.f ;
			}
		virtual void dispose(Scalar& v)
			{
			}
	};
static DataType* TYPE_DOUBLE = new  _DoubleType;



class _LongType:public DataType
	{
	public:
		virtual void parse(Scalar& v,const char* s,size_t len)
			{
			char* p2;
			v.d=strtol(s,10,&p2);
			while(isspace(*p2)) ++p2;
			if(p2< s+len) cerr << "boum";
			}
		virtual bool compare(const Scalar& a,const Scalar& b)
			{
			return a.d < b.d ;
			}
		virtual void dispose(Scalar& v)
			{
			}
	};
static DataType* TYPE_LONG = new  _LongType;

Archetype::Archetype():values(0)
	{
	}

Archetype::~Archetype()
	{
	if(values!=0) delete[] values;
	}	


void ColumnKeyList::parse(const char* arg)
	{
	char* p=(char*)arg;
	while(*p!=0)
		{
		ColumnKey key;
		p=key.parse(p);
		
		this->keys.push_back(key);	
		
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
		
char* ColumnKey::parse(const char* arg)
	{
	char* p2;
	char* p=(char*)arg;
	this->order=1;
	this->data_type = TYPE_STRING;
	if(*p=='+') ++p;
	else if(*p=='-')  {this->order=-1;++p;}
	
	this->column_index = strtoul(p,&p2,10);
	if( this->column_index < 1)
		{
		fprintf(stderr,"BAD COLUMN INDEX in %s.\n",arg);
		exit(EXIT_FAILURE);
		}
	this->column_index--;
	return (char*)p2;
	}

void Pivot::readData(std::istream& in)
	{
	size_t nLine=0;
	std::string line;
	std::vector<size_t> tokens;
	while(getline(in,line,'\n'))
		{
		size_t i;
		tokens.clear();
		for(i=0;i< line.size();++i)
			{
			if(line[i]!='\t') continue;
			tokens.push_back(i); 
			}
		tokens.push_back(line.size()); 
		
		
		for(int side=0;side<2;++side)
			{
			ColumnKeyList& columns=(side==0?this->leftcols:this->topcols);
			Archetype arch;
			arch.values = new Scalar[columns.keys.size()];
			for(size_t i=0;i< columns.keys.size();++i)
				{
				if( tokens.size()<= columns.keys[i].column_index)
					{
					cerr << "BOUM" << endl;
					exit(-1);
					}
				columns.keys[i].data_type->parse(arch.values[i],&line[tokens[ columns.keys[i].column_index]],10);
				}
			arch.rows.push_back(nLine);
			}
		}

	}

void Pivot::usage()
	{
	fprintf(stderr,"\n\nPivot\nAuthor: Pierre Lindenbaum PhD\nGit-Hash: "
		GIT_HASH
		"\nWWW: https://github.com/lindenb/ibddb\nCompilation: %s at %s\n\n",
		__DATE__,__TIME__);
	fprintf(stderr,"Usage:\n\tPivot (options) (stdin|file)\n\n");
	fputs("\n\n",stderr);
	}


int Pivot::instanceMain(int argc,char** argv)
	{
	for(;;)
		{
		struct option long_options[] =
		     {
			{"left",   required_argument, 0, 'L'},
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
			case 'L': leftcols.parse(optarg); break;
			case 'T':  topcols.parse(optarg); break;
			case 0: break;
			case '?': break;
			default: usage(); exit(EXIT_FAILURE); break;
			}
		}
	if(optind==argc)
		{
		readData(cin);
		}
	else if(optind+1==argc)
		{
		ifstream in(argv[optind],ios::in);
		if(!in.is_open())
			{
			fprintf(stderr,"Cannot open \"%s\" %s.\n",
				argv[optind],
				strerror(errno)
				);
			exit(EXIT_FAILURE);
			}
		readData(in);
		in.close();
		}
	else
		{
		fputs("Illegal number of arguments.\n",stderr);
		return EXIT_FAILURE;
		}
	

	
	return EXIT_SUCCESS;
	}

int main(int argc,char** argv)
	{
	Pivot instance;
	return instance.instanceMain(argc,argv);
	}

