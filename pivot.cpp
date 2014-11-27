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
			out.write(&n,sizeof(size_t));
			out.write(a.s,n);
			}
		virtual void read(std::istream& in,Scalar& a)
			{
			size_t len;
			in.read(&len,sizeof(size_t));
			v.s=new char[len+1];
			v.s[len]=0;
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
			out.write(&a.f,sizeof(double));
			}
		virtual void read(std::istream& in,Scalar& a)
			{
			in.read(&a.f,sizeof(double));
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
			out.write(&a.d,sizeof(long));
			}
		virtual void read(std::istream& in,Scalar& a)
			{
			in.read(&a.d,sizeof(long));
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

	
void Pivot::readData(std::istream& in)
	{
	OperatorRowIndex rowIndex;
	rowIndex.opcode= OP_CODE_ROW_INDEX;
	rowIndex.nLine=0;
	std::string line;
	std::vector<size_t> tokens;
	
	leveldb::Options options;
	options.create_if_missing=true;
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
		rowIndex.nLine++;
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
		datastr.write((const char*)&tmp_size,sizeof(size_t));
		for(i=0;i< tokens.size();++i)
			{
			tmp_size=tokens[i];
			datastr.write((const char*)&tmp_size,sizeof(size_t));
			}
		tmp_size =line.size();
		datastr.write((const char*)&tmp_size,sizeof(size_t));
		datastr.write((const char*)line.c_str(),sizeof(char)*(line.size()+1));
		
		
		leveldb::Slice key((const char*)&rowIndex,sizeof(OperatorRowIndex));
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

Pivot::Pivot():db(0)
	{
	}

Pivot::~Pivot()
	{
	if(this->db!=0)
		{
		delete this->db;
		leveldb::Env* env= leveldb::Env::Default();
		if(!this->tmpDir.empty())
			{
			clog << "removing "<< tmpDir << endl;
			
			leveldb::Status status = env->DeleteDir(this->tmpDir);
			if(!status.ok())
				{
				cerr << "cannot remove " << this->tmpDir << endl;
				}
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

