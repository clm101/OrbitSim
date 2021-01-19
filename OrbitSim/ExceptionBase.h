#ifndef EXCEPTION_BASE_H
#define EXCEPTION_BASE_H

#include "DebugDefines/MemoryDebug.h"
#include <exception>
#include <string>
#include <sstream>

class ExceptionBase : public std::exception {
private:
	int nLine;
	std::string sFile;
protected:
	mutable std::string strWhatBuffer;
public:
	ExceptionBase(int, const char*) noexcept;
	const int getLine() const noexcept;
	const std::string& getFile() const noexcept;
	virtual const char* getType() const noexcept;
	const char* what() const noexcept override;
};

#endif