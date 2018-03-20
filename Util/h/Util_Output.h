#ifndef _Util_Output_h_
#define _Util_Output_h_

// This file defines varius utils for debug output
// (log files, message box output functions etc.)

#include <Windows.h>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <string>
#include <cassert>

namespace Dv
{
	namespace Utils
	{
		struct OutputParams
		{
			// maximal number of lines before flipping page
			int maxLinesPerPage;
			// maximal number of chars before flipping line
			int maxCharsPerLine;

			inline OutputParams(int maxLinesPerPageIn, int maxCharsPerLineIn) : maxLinesPerPage(maxLinesPerPageIn), maxCharsPerLine(maxCharsPerLineIn) {}
			inline OutputParams() : maxLinesPerPage(200), maxCharsPerLine(300) {}
		};

		typedef						std::vector<std::string> 						LineVector; 
		typedef						std::vector<LineVector>							PageVector;

		std::string 				GetPageText(const LineVector& pageText);
		// Split the text to one or more pages
		PageVector SplitTextToPages
		(
			const OutputParams& outParams, const char *message
		);

		// Helper function: find end of line restricted by length
		// (maxLineLength==0 means "no length restriction")
		const char *find_end_of_line(const char *str, int maxLineLength);

		// Show message box devided into pages
		void ShowPageMessageBox
		(
			const PageVector& textPages, 
			const char *title,
			UINT flags = MB_ICONEXCLAMATION
		);

		// Shows message box devided into pages
		void ShowPageMessageBox
		(
			const OutputParams& outParams, 
			const char *message, 
			const char *title,
			UINT flags = MB_ICONEXCLAMATION
		);
	} // Util
} // Dv

#endif // _Util_Output_h_
