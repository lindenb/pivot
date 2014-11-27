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
		virtual void write(std::ostream& out,const Scalar& a)
			{
			size_t n=strlen(a.s);
			out.write((const char*)&n,sizeof(size_t));
			out.write((const char*)a.s,n);
			}
		virtual void read(std::istream& in,Scalar& a)
			{
			size_t len;
			in.read((char*)&len,sizeof(size_t));
			a.s=new char[len+1];
			in.read(a.s,sizeof(char)*len);
			a.s[len]=0;
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
		virtual void write(std::ostream& out,const Scalar& a)
			{
			out.write((const char*)&a.f,sizeof(double));
			}
		virtual void read(std::istream& in,Scalar& a)
			{
			in.read((char*)&a.f,sizeof(double));
			}
	};
static DataType* TYPE_DOUBLE = new  _DoubleType;



class _LongType:public DataType
	{
	public:
		virtual void parse(Scalar& v,const char* s,size_t len)
			{
			char* p2;
			v.d=strtol(s,&p2,10);
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
		virtual void write(std::ostream& out,const Scalar& a)
			{
			out.write((const char*)&a.d,sizeof(long));
			}
		virtual void read(std::istream& in,Scalar& a)
			{
			in.read((char*)&a.d,sizeof(long));
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
#define OP_CODE_ROW_INDEX 'I'
struct OperatorRowIndex
	{
	char opcode;
	size_t nLine;
	};

PivotComparator::PivotComparator(Pivot* owner):owner(owner)
	{
	}

PivotComparator::~PivotComparator()
	{
	}

int PivotComparator::Compare(const leveldb::Slice& a, const leveldb::Slice& b) const
	{
	istringstream ina;
	istringstream inb;
	ina.str(a.ToString());
	inb.str(b.ToString());
	char opcodea=0;
	char opcodeb=0;
	ina.read(&opcodea,sizeof(char));
	inb.read(&opcodeb,sizeof(char));
	if(opcodea!=opcodeb)
		{
		throw runtime_error("boum");
		}
	switch(opcodea)
		{
		case OP_CODE_ROW_INDEX:
			{
			size_t linea=0UL;
			size_t lineb=0UL;
			ina.read((char*)&linea,sizeof(size_t));
			inb.read((char*)&lineb,sizeof(size_t));
			if(linea<lineb) return -1;
			if(linea>lineb) return 1;
			return 0;
			}
		default: 
			{
			cerr << "BAD OPCODE" << (char)opcodea << endl;
			throw runtime_error("boum");
			}
		}
	return 0;
	}

const char* PivotComparator::Name() const
	{
	return "PivotComparator";
	}

void PivotComparator::FindShortestSeparator(std::string*, const leveldb::Slice&) const{}
void PivotComparator::FindShortSuccessor(std::string*) const {}

	
void Pivot::readData(std::istream& in)
	{
	OperatorRowIndex rowIndex;
	char opcode= OP_CODE_ROW_INDEX;
	size_t nLine=0UL;
	std::string line;
	std::vector<size_t> tokens;
	this->comparator = new PivotComparator(this);
	leveldb::Options options;
	options.create_if_missing=true;
	options.comparator = comparator;
	char* dir=new char[100];
	strcpy(dir,"pivot.leveldb.XXXXXX");
	if(mkdtemp(dir)==NULL)
		{
		ostringstream err;
		err << "mkdtemp failed: " << strerror(errno) << endl;
		throw std::runtime_error(err.str());
		}
		
	this->tmpDir.assign(dir);
	leveldb::Status s = leveldb::DB::Open(options,dir,&db);
	delete [] dir;
	
	while(getline(in,line,'\n'))
		{
		size_t i;
		nLine++;
		tokens.clear();


		tokens.push_back(0UL);
		for(i=0;i< line.size();++i)
			{
			if(line[i]!='\t') continue;
			line[i]='0';//set to EOS
			tokens.push_back(i); 
			}
		
		size_t tmp_size =tokens.size();
		ostringstream datastr;
		datastr.write("X",sizeof(char));
		datastr.write((const char*)&tmp_size,sizeof(size_t));
		for(i=0;i< tokens.size();++i)
			{
			tmp_size=tokens[i];
			datastr.write((const char*)&tmp_size,sizeof(size_t));
			}
		tmp_size =line.size();
		datastr.write((const char*)&tmp_size,sizeof(size_t));
		datastr.write((const char*)line.c_str(),sizeof(char)*(line.size()+1));
		
		
		ostringstream keystr;
		char ss[5];
		ss[0]=OP_CODE_ROW_INDEX;
		memcpy(&ss[1],(const void*)&nLine,sizeof(size_t));
		//keystr.write((const char*)&opcode,sizeof(char));
		//keystr.write((const char*)&nLine,sizeof(size_t));
		
		leveldb::Slice key(ss,5);
		leveldb::Slice data(datastr.str());
		
		leveldb::Status status = db->Put(leveldb::WriteOptions(),key,data);
		if(!status.ok())
			{
			ostringstream err;
			err << "cannot insert " << line << endl;
			throw std::runtime_error(err.str());
			}
		/*
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
			}*/
		}
	clog << "Inserted "<< rowIndex.nLine << " lines." << endl;
	}

Pivot::Pivot():db(0),comparator(0)
	{
	}

Pivot::~Pivot()
	{
	if(comparator!=0) delete comparator;
	if(this->db!=0)
		{
		delete this->db;
		leveldb::Env* env= leveldb::Env::Default();
		if(!this->tmpDir.empty())
			{
			Pivot::deleteDir(this->tmpDir.c_str());
			}
		}
	}

/* http://stackoverflow.com/questions/1149764/ */
int Pivot::_rm( const char *path, const struct stat *s, int flag, struct FTW *f )
	{
	int status;
	int (*rm_func)( const char * );
	switch( flag )
		{
		case FTW_DP: rm_func = rmdir; break;
		default:     rm_func = unlink; break;
		}
	if( status = rm_func( path ), status != 0 )
		perror( path );
	else
		clog << "delete " << path << endl;
	return status;
	}

void Pivot::deleteDir(const char* dir)
	{
	/* ftw() walks through the directory tree that is located under the directory dirpath,
	* and calls fn() once for each entry in the tree */
	::nftw(dir, Pivot::_rm, 2, FTW_DEPTH );
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
			case 'L':
				if(leftcols.size()!=0)
					{
					cerr << "Option -L defined twice" << endl;
					return -1;
					}
				leftcols.parse(optarg);
				break;
			case 'T':  topcols.parse(optarg); break;
			case 0: break;
			case '?': break;
			default: usage(); exit(EXIT_FAILURE); break;
			}
		}
	
	/*if(leftcols.size()==0)
		{
		cerr << "Option -L undefined." << endl;
		return EXIT_FAILURE;
		}*/
	
	if(optind==argc)
		{
		clog << "Reading from stdin" << endl;
		readData(cin);
		}
	else if(optind+1==argc)
		{
		clog << "Reading from "<< argv[optind] << endl;
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
	try
		{
		return instance.instanceMain(argc,argv);
		}
	catch(exception err)
		{
		cerr << err.what() << endl;
		return EXIT_FAILURE;
		}
	}

