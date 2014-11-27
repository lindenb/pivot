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
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include "leveldb/env.h"
#include "leveldb/db.h"

#define WHERENL fprintf(stderr,"[%s:%d] ",__FILE__,__LINE__)
#define DIE_FAILURE(FormatLiteral,...) do { WHERENL; fprintf (stderr,"Git-Hash:"GIT_HASH". Exiting: " FormatLiteral "\n", ##__VA_ARGS__); exit(EXIT_FAILURE);} while(0)
#define DEBUG(FormatLiteral, ...)  do { fputs("[DEBUG]",stderr); WHERENL; fprintf (stderr,"" FormatLiteral "\n", ##__VA_ARGS__);} while(0)

typedef union scalar
	{
	char* s;
	double f;
	long d;
	} Scalar,*ScalarPtr;

class DataType
	{
	public:
		virtual void parse(Scalar& v,const char* s,size_t len)=0;
		virtual bool compare(const Scalar& a,const Scalar& b)=0;
		virtual void dispose(Scalar& v)=0;
		virtual void write(std::ostream& out,const Scalar& a);
		virtual void read(std::istream& in,Scalar& a);
	};
	



typedef struct ColumnKey
	{
	int order;
	size_t column_index;
	DataType* data_type;
	char* parse(const char* arg);
	}*ColumnKeyPtr;

typedef class ColumnKeyList
	{
	public:
		std::vector<ColumnKey> keys;
		
		void parse(const char* arg);
	}*ColumnKeyListPtr;

class Archetype
	{
	public:
		ScalarPtr values;
		std::vector<size_t> rows;
		Archetype();
		~Archetype();
	};

typedef class archetype_list_t
	{
	Archetype* archetypes;
	size_t size;
	} ArchetypeList,*ArchetypeListPtr;

typedef class Pivot
	{
	public:
		leveldb::DB* db;
		std::string tmpDir;
  		ColumnKeyList leftcols;
		ColumnKeyList topcols;
		ScalarPtr observed;
		ArchetypeList left;
		ArchetypeList top;
		Pivot();
		~Pivot();
		void readData(std::istream& in);
		void usage();
		int instanceMain(int argc,char** argv);
	}*PivotPtr;

#endif

