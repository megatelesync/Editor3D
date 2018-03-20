#include "h/Util_Output.h"

namespace Dv
{
	namespace Utils
	{
		// Impl
		PageVector SplitTextToPages
		(
			const OutputParams& outParams, const char *message
		)
		{
			// text of all pages
			std::vector<LineVector> text;
			// first page
			text.push_back(std::vector<std::string>());
			auto &firstPage = text.front();

			// loop for lines of output text
			// (each line of output text has length not greater then maxLineLength)
			const char *pLineStart = message; 	
			// current page
			std::vector<std::string> *pCurrPage = &firstPage;
			while(true)
			{				
				if(*pLineStart == '\0') { return text; }

				const char *pLineEnd = find_end_of_line(pLineStart, outParams.maxCharsPerLine);
				
				std::string line(pLineStart, pLineEnd);
			
				// add new page if necessary
				if(pCurrPage->size() >= outParams.maxLinesPerPage)
				{
					text.push_back(LineVector());
					pCurrPage = &(text.back());
				}

				pCurrPage->push_back(line);
				
				if(*pLineEnd == '\0') { return text; }
				pLineStart = pLineEnd;
				if(*pLineStart == '\n') { pLineStart++; }
			}
			return text;
		}

		// Shows message box devided into pages
		void ShowPageMessageBox
		(
			const OutputParams& outParams, 
			const char *message, 
			const char *title,
			UINT flags
		)
		{
			PageVector textPages = SplitTextToPages(outParams, message);
			return ShowPageMessageBox(textPages, title, flags);
		}

		void ShowPageMessageBox
		(
			const PageVector& textPages, 
			const char *title,
			UINT flags
		)
		{
			const char * const headerText = 
				  "***************************************************\n";
			const char * const footerText = 
				"\n***************************************************"
				"\n** Show next page?"
				"\n** PRESS \"YES\" for next page;"
				"\n** PRESS \"NO\" for previus page;"
				"\n** PRESS \"CANCEL\" to close";
			
			int pageIndex = 0;
			while(true)
			{
				const LineVector *pPage = &(textPages[pageIndex]);				
				
				std::string pageText;
				pageText.append("**\npage:");
				pageText.append(std::to_string(pageIndex));
				pageText.append("/(");
				pageText.append(std::to_string(textPages.size()));
				pageText.append("-1)\n");
				pageText.append(headerText);
				pageText.append(GetPageText(*pPage)); 
				pageText.append(footerText);

				int res = MessageBox(nullptr, pageText.c_str(), title, MB_YESNOCANCEL);

				switch(res)
				{
				case IDNO:
					if(pageIndex > 0) { pageIndex--; }
					break;

				case IDYES:
					if((pageIndex + 1) < textPages.size() ) { pageIndex++; }
					break;
				
				case IDCANCEL:
					return;

				default:
					assert(!"Unknown message box result");
				}
			}		
		}

		const char *find_end_of_line(const char *str, int maxLineLength)
		{
			const char *pChar = str;
			int length = 0;
			while(true)
			{
				if(*pChar == '\0') { return pChar; }
				if(*pChar == '\n') { return pChar; }
				if(maxLineLength > 0 && length == maxLineLength) { return pChar; }
				++length;
				++pChar;
			}
		}

		std::string GetPageText(const LineVector& pageText)
		{
			std::string res;
			res.reserve(pageText.size() * 40);
			std::for_each
			(
				pageText.cbegin(), pageText.cend(), 
				[&res](const std::string& line) 
				{
					res.append(line); 
					res.append("\n"); 
				}
			);
			return res;
		}		
	} // Util
} // Dv