#ifndef _FILEINFO_20200306_H_
#define _FILEINFO_20200306_H_
#include <string>
#include <iostream>
using namespace std;
namespace cbl{

class CFileInfo
{
public:
	CFileInfo();
	explicit CFileInfo(const string &filePath);
	virtual ~CFileInfo();
	bool isExists() const;
private:
	string m_filePath;
};
	/*
	bool isReadable() const;
	bool isWritable() const;
	bool isExecutable() const;
	bool isHidden() const;
	bool isRelative() const;
	inline bool isAbsolute() const { return !isRelative(); }
	bool makeAbsolute();
	bool isFile() const;
	bool isDir() const;
	bool isSymLink() const;
	bool isRoot() const;
	bool isBundle() const;
	*/




}

#endif