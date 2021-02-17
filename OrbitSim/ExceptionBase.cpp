#include "ExceptionBase.h"

namespace clm {
	ExceptionBase::ExceptionBase(int nLine_in, const char* cFile_in) noexcept : nLine(nLine_in), sFile(cFile_in) {}

	const int ExceptionBase::getLine() const noexcept {
		return nLine;
	}

	const std::string& ExceptionBase::getFile() const noexcept {
		return sFile;
	}

	const char* ExceptionBase::getType() const noexcept {
		return "Base exception";
	}

	const char* ExceptionBase::what() const noexcept {
		std::ostringstream ossOut;
		ossOut << getType() << std::endl
			<< "Line: " << nLine << std::endl
			<< "File: " << sFile << std::endl;
		strWhatBuffer = ossOut.str();
		return strWhatBuffer.c_str();
	}
}