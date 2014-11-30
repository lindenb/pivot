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
#ifndef PIVOT_HH
#define PIVOT_HH
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <ftw.h>
#include "leveldb/env.h"
#include "leveldb/db.h"
#include "leveldb/comparator.h"

#define THROW( a) do {\
	std::ostringstream _os; \
	_os << "[" << __FILE__ << ":" << __LINE__ << "]" << a << std::endl;\
	throw std::runtime_error(_os.str());\
	} while(0)
#define DEBUG(a)  do {std::cerr  << "[" << __FILE__ << ":" << __LINE__ << "]" << a << std::endl;} while(0)

enum OpCode {
	OP_CODE_ROW_INDEX='I',
	OP_CODE_LEFT='L',
	OP_CODE_TOP='T'
	};

class DataType
	{
	public:
		virtual std::size_t sizeOf(const void* ptr)=0;
		virtual void* parse(const char* s)=0;
		virtual int compare(const void* a,const void* b)=0;
		virtual void dispose(void* ptr)=0;
		virtual void* clone(const void* ptr)=0;
		virtual char* write(const void* ptr,char* out)=0;
		virtual void* read(const char* ptr, char** end_ptr)=0;
	};
	

class Scalar
	{
	private:
		DataType* datatype;
		void* ptr;
	public:
		Scalar(DataType* datatype,void* ptr);
		Scalar(const Scalar& cp);
		~Scalar();
		std::size_t sizeOf() const;
		Scalar& operator=(const Scalar& cp);
		int compare(const Scalar& cp) const; 
		char* write(char* out) const;
	};

class Archetype
	{
	public:
		std::vector<Scalar> scalars;
		Archetype();
		Archetype(const Archetype& cp);
		~Archetype();
		Archetype& operator=(const Archetype& cp);
		int compare(const Archetype& cp) const;
		std::size_t sizeOf() const;
		char* write(char* out);
	};

typedef struct ColumnKey
	{
	int order;
	size_t column_index;
	DataType* datatype;
	char* parse(const char* arg);
	Scalar scalar(const char* ptr,char** endptr);
	}*ColumnKeyPtr;

typedef class ColumnKeyList
	{
	public:
		std::vector<ColumnKey> keys;
		
		void parse(const char* arg);
		size_t size() const { return keys.size();}
		ColumnKey& at(size_t i) { return keys.at(i);}
		
	}*ColumnKeyListPtr;




class Pivot;
class PivotComparator : public leveldb::Comparator {
   private:
   	Pivot* owner;
   	bool compare_for_leveldb;
   public:
	    PivotComparator(Pivot* owner,bool compare_for_leveldb);
	    virtual ~PivotComparator();
	    virtual int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const;
	    virtual const char* Name() const;
	    virtual  void FindShortestSeparator(std::string*, const leveldb::Slice&) const;
	    virtual void FindShortSuccessor(std::string*) const;
  };

typedef class Pivot
	{
	public:
		leveldb::DB* db;
		std::string tmpDir;
  		ColumnKeyList leftcols;
		ColumnKeyList topcols;
		Scalar* observed;
		Pivot();
		~Pivot();
		void readData(std::istream& in);
		void usage();
		int instanceMain(int argc,char** argv);
	private:
		PivotComparator* comparator;
		static int _rm( const char *path, const struct stat *s, int flag, struct FTW *f );
		static void deleteDir(const char* dir);
		
	}*PivotPtr;

#endif

