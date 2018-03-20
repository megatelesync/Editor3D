#include "Log.h"
#include "h\CoreUtil\CoreUtil_Assert.h"
#include <boost/format.hpp>

namespace Dv
{
	namespace Util
	{
		Log::Log(const LogSettings& InSettings) :
			_fileStream(),
			_settings(InSettings)
		{
			_fileStream.open(InSettings.logFilename, std::ios::out);
			if (false == _fileStream.is_open())
			{
				_ErrorMessage_CannotCreateLog();
				exit(-1);
			}
			else
			{
				// Push normal priority
				_verbosityStack.push(VerbosityStackElem(EVerbosity::Normal, EManipulatorScope::ToRecordEnd, 0));
				LogRecord rootRecord = LogRecord(EMessageType::SimpleText, "Log");
				BeginMessage(&rootRecord);
				_PrintLogInitializationMessage();
			}
		}
		void Log::_ErrorMessage_CannotCreateLog()
		{
			std::string text;
			text.append("Cannot create log file: \n");
			text.append("filename: ");
			text.append(_settings.logFilename);

			_ErrorMessage(text);
		}

		void Log::_ErrorMessage(const std::string& InText)
		{
			std::string fullText;
			fullText.append("Util::Log error:");

			fullText.append("logName: ");
			fullText.append(_settings.logName);

			Utils::ShowPageMessageBox(_settings.output, fullText.c_str(), "Util::Log error");
		}

		Log::~Log()
		{
			_PrintLogAfterDestructionMessage();
			// ENd the initial message, other message shold not be ended
			EndMessage();
		}
		void Log::_PrintLogInitializationMessage()
		{
			// Log initialization message should always be printed, so setup priority to LogSystem
			LogMessageRecordScoper scoper(this, EMessageType::Status, "Log initialized", Util::EVerbosity::LogSystem);
			Log_Settings(this, &_settings);
		}
		void Log::_PrintLogAfterDestructionMessage()
		{
			LogMessageRecordScoper scoper(this, EMessageType::Status, "Destroying log", Util::EVerbosity::LogSystem);
		}
		void Log::BeginMessage(const LogRecord *pInRecord)
		{
			// WARNING: NEVER ASSERT HERE THAT NESTINGS LEVELS CORRECT
			_messageStack.push(*pInRecord);
			if (GetNestingMessageLevel() > 0)
			{
				// Writing separator
				_PrintLogRecordSeparator_IfShouldDoItNow();
				// Writing log record header string, if we need
				AppendNewLine();
				*this << "[[[";
				_PrintCurrentDateTime();
				// part of log record message after the date-time part
				std::string str_afterDateTime;
				str_afterDateTime.append(" ");
				// level of message nesting
				str_afterDateTime.append("; lvl=");
				str_afterDateTime.append(std::to_string(GetNestingMessageLevel()));
				// type of message
				str_afterDateTime.append("; logMsgType=");
				str_afterDateTime.append(ToString_AbbreviatedMessageType(pInRecord->messageType));

				// end of message
				str_afterDateTime.append(":");

				// Output to this stream; it will be automatically echoed if necessary.
				*this << str_afterDateTime;

				// Message title is to be printed from the next line
				AppendNewLine();
				*this << "Caption: " << pInRecord->title;
				*this << "]]]";

				AppendNewLine();
			}
			_currentIdentationLevel++;
		}
		void Log::EndMessage()
		{
			assert(GetNestingMessageLevel() >= 0);

			--_currentIdentationLevel;
			// Output that record ended
			AppendNewLine();
			if (GetNestingMessageLevel() == 0)
			{
				*this << "[[[END OF LOG]]]]";
			}
			else
			{
				*this << (boost::format("[[[//LOG RECORD END: lvl = %1%]]]]") % GetNestingMessageLevel()).str();
			}
			AppendNewLine();

			// Flush at the end of message
			Flush();
			// Echo should done BEFORE popping the log record
			_EchoBuffered_IfShouldDotItNow();

			// Pop the verbosity elems from the stack
			_Pop_VerbosityStackElems(EManipulatorScope::ToRecordEnd, GetNestingMessageLevel());

			_messageStack.pop();
		}

		void Log::PushMessageVerbosityLevel(EVerbosity InVerbosity, EManipulatorScope InManipulatorScope)
		{
			auto messageNestingLevel = GetNestingMessageLevel();
			_verbosityStack.push(VerbosityStackElem(InVerbosity, InManipulatorScope, messageNestingLevel));
		}

		void Log::_Pop_VerbosityStackElems(EManipulatorScope InScope, size_t InNestingLevel)
		{
			if (true == _verbosityStack.empty())
			{
				return;
			}

			const auto& top = _verbosityStack.top();
			if (top.scope != InScope)
			{
				return;
			}
			if (top.scope == Util::EManipulatorScope::ToRecordEnd && top.logRecordNestingLevel != InNestingLevel)
			{
				return;
			}
			_verbosityStack.pop();
		}

		bool Log::_ShouldMessageBeLogged() const
		{
			assert(_verbosityStack.size() > 0);

			auto msgType                                = GetMessageType();
			auto verbosityStackTop                      = _verbosityStack.top();
			auto msgVerbosity                           = verbosityStackTop.verbosity;

			auto result = _settings.ShouldLogMessage(msgType, msgVerbosity);
			return result;
		}

		void Log::_EchoBuffered_IfShouldDotItNow()
		{
			if (_ShouldEchoNow())
			{
				_EchoBuffered();
				// Clear the message buffer
				_messageBufferStrm.str("");
			}
		}
		bool Log::_ShouldEchoNow() const
		{
			bool bShouldDueToNestingLevel = GetNestingMessageLevel() <= _settings.recordNestingLevelToEcho;
			bool bShouldAlwaysBeEchoed = _ShouldMessageLevel_AlwaysBeEchoed(GetMessageType());
			bool bShouldBeEchoedNow = bShouldDueToNestingLevel && bShouldAlwaysBeEchoed;
			return bShouldBeEchoedNow;
		}
		void Log::_EchoBuffered()
		{
			Utils::ShowPageMessageBox(_settings.output, _messageBufferStrm.str().c_str(), "Last log record");
		}
		bool Log::_ShouldMessageLevel_AlwaysBeEchoed(EMessageType InType) const
		{
			auto messageTypeIndex = static_cast<size_t>(InType);
			auto bMessageTypeAlwaysEchoed = _settings.bAlwaysEcho[messageTypeIndex];
			return bMessageTypeAlwaysEchoed;
		}
		bool Log::_ShouldMessageLevel_BeEchoed(EMessageType InType) const
		{
			auto messageTypeIndex                        = static_cast<size_t>(InType);

			auto bMessageTypeEchoed                      = _settings.bEchoLevel[messageTypeIndex];
			auto bMessageTypeAlwaysEchoed                = _settings.bAlwaysEcho[messageTypeIndex];

			auto bShouldBeEchoed                             = bMessageTypeEchoed || bMessageTypeAlwaysEchoed;
			return bShouldBeEchoed;
		}
		bool Log::_ShouldOutputBeEchoed() const
		{
			auto messageType                           = GetMessageType();
			auto bMessageTypeEchoed                    = _ShouldMessageLevel_BeEchoed(messageType);
			return bMessageTypeEchoed;
		}
		void Log::_PrintLogRecordSeparator_IfShouldDoItNow()
		{
			if (GetNestingMessageLevel() <= _settings.recordNestingLevelToEcho)
			{
				_AppendNewLine_NoBeginLine();
				const size_t separatorSize                  = 80;
				const char   separatorChar                  = '-';

				for (auto i = 0; i < separatorSize; i++)
				{
					*this << separatorChar;
				}
				_AppendNewLine_NoBeginLine();
			}
		}
		void Log::_PrintCurrentDateTime()
		{
			auto currentDateTime = std::chrono::system_clock::now();
			*this << currentDateTime;
		}
		const LogRecord *Log::GetTopMessage() const
		{
			auto pTop = &(_messageStack.top());
			return pTop;
		}
		EMessageType Log::GetMessageType() const
		{
			return GetTopMessage()->messageType;
		}

		void Log::UpdateSettings(const LogSettings& newValue)
		{
			_settings = newValue;
		}

		Log& operator<<(Log& oStrm, const std::string_view& s)
		{
			return operator<<(oStrm, std::string(s));
		}

		Log& operator<<(Log& oStrm, const std::string& s)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			if (s.empty() || s[s.size()-1] != '\n')
			{
				oStrm._bLastOutputCharacterIsNewLine = false;
			}
			else
			{
				oStrm._bLastOutputCharacterIsNewLine = true;
			}
			oStrm << s.c_str();
			if (oStrm._ShouldMessageBeLogged())
			{
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << s.c_str();
				}
				oStrm._Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
			return oStrm;
		}

		Log& operator<<(Log& oStrm, const char *str)
		{
			if(oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			if(str == nullptr || str[strlen(str)-1] != '\n')
			{
				oStrm._bLastOutputCharacterIsNewLine = false;
			}
			else
			{
				oStrm._bLastOutputCharacterIsNewLine = true;
			}
			oStrm._fileStream << str;
			if(oStrm._ShouldMessageBeLogged())
			{
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << str;
				}
				oStrm._Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
			return oStrm;
		}

		Log& operator<<(Log& oStrm, void *str)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			oStrm._bLastOutputCharacterIsNewLine = false;
			if (oStrm._ShouldMessageBeLogged())
			{
				oStrm._fileStream << str;
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << str;
				}
				oStrm._Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
			return oStrm;
		}

		Log& operator<<(Log& oStrm, unsigned int val)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			oStrm._bLastOutputCharacterIsNewLine = false;
			if (oStrm._ShouldMessageBeLogged())
			{
				oStrm._fileStream << val;
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << val;
				}
				oStrm._Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
			return oStrm;
		}

		Log& operator<<(Log& oStrm, int val)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			oStrm._bLastOutputCharacterIsNewLine = false;
			if (oStrm._ShouldMessageBeLogged())
			{
				oStrm._fileStream << val;
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << val;
				}
				oStrm._Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
			return oStrm;
		}

		Log& operator<<(Log& oStrm, char val)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			oStrm._bLastOutputCharacterIsNewLine = false;
			if (oStrm._ShouldMessageBeLogged())
			{
				oStrm._fileStream << val;
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << val;
				}
				oStrm._Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
			return oStrm;
		}

		Log& operator<<(Log& oStrm, bool bVal)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			oStrm._bLastOutputCharacterIsNewLine = false;
			if (oStrm._ShouldMessageBeLogged())
			{
				oStrm._fileStream << bVal;
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << bVal;
				}
				oStrm._Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
			return oStrm;
		}

		Log& operator<<(Log& oStrm, double val)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			oStrm._bLastOutputCharacterIsNewLine = false;
			if (oStrm._ShouldMessageBeLogged())
			{
				oStrm._fileStream << val;
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << val;
				}
				oStrm._Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
			return oStrm;
		}

		Log& operator<<(Log& oStrm, float val)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			oStrm._bLastOutputCharacterIsNewLine = false;
			if (oStrm._ShouldMessageBeLogged())
			{
				oStrm._fileStream << val;
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << val;
				}
				oStrm._Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
			return oStrm;
		}

		Log& operator<<(Log& oStrm, const std::time_t& InTime)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			oStrm._bLastOutputCharacterIsNewLine = false;
			if (oStrm._ShouldMessageBeLogged())
			{
				auto puttedTime = std::put_time(std::localtime(&InTime), "%F %T");
				oStrm._fileStream << puttedTime;
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << puttedTime;
				}
				oStrm._Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
			return oStrm;
		}

		void Log::SetW(size_t InLength)
		{
			Util::NotYetImplVoid_Silent("Log::SetW: quick'n'dirty solution, not designed or tested properly");
			if (_bLastOutputCharacterIsNewLine)
			{
				_BeginLine();
			}
			_bLastOutputCharacterIsNewLine = false;
			if (_ShouldMessageBeLogged())
			{
				_fileStream << std::setw(InLength);
				if (_ShouldOutputBeEchoed())
				{
					_messageBufferStrm << std::setw(InLength);
				}
				_Pop_VerbosityStackElems(EManipulatorScope::Inserter, 0);
			}
		}

		Log& operator<<(Log& oStrm, LogManip InManip)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			oStrm._bLastOutputCharacterIsNewLine = false;
			InManip(oStrm);
			return oStrm;
		}

		Log& operator<<(Log& oStrm, const std::chrono::system_clock::time_point& InTimePoint)
		{
			if (oStrm._bLastOutputCharacterIsNewLine)
			{
				oStrm._BeginLine();
			}
			oStrm._bLastOutputCharacterIsNewLine = false;
			if (oStrm._ShouldMessageBeLogged())
			{
				std::time_t tm = std::chrono::system_clock::to_time_t(InTimePoint);
				oStrm << tm;
				if (oStrm._ShouldOutputBeEchoed())
				{
					oStrm._messageBufferStrm << tm;
				}
				Util::Note("Here we may NOT pop verbosity stack elems, as it's to be done inside the time_t inserter");
			}
			return oStrm;
		}

		void Log::AppendNewLine()
		{
			if (_ShouldMessageBeLogged())
			{
				if(false == _bLastOutputCharacterIsNewLine)
				{
					_AppendNewLine_NoBeginLine();
					// Begin the new line:
					_BeginLine();
					_bLastOutputCharacterIsNewLine = true;
				}
				_Pop_VerbosityStackElems(EManipulatorScope::ToLineEnd, 0);
			}
		}

		void Log::_AppendNewLine_NoBeginLine()
		{
			if(false == _bLastOutputCharacterIsNewLine)
			{
				_fileStream << std::endl;
				if (_ShouldOutputBeEchoed())
				{
					_messageBufferStrm << std::endl;
				}
			}
			_bLastOutputCharacterIsNewLine = true;
		}

		void Log::_BeginLine()
		{
			_bLastOutputCharacterIsNewLine = false;
			//auto numIdentations = GetNestingMessageLevel() - 1;
			//_BeginLine_NumIdentations(numIdentations);
			_BeginLine_NumIdentations(_currentIdentationLevel);
		}

		void Log::_BeginLine_NumIdentations(size_t InIdentationLevel)
		{
			auto numIdentations = InIdentationLevel;
			assert(numIdentations >= 0);
			for (auto i = 0; i < numIdentations; i++)
			{
				// Here we can safely use the output operators (except the AppendNewLine, of course), no recursion:
				_WriteIdentation();
			}
		}

		void Log::_WriteIdentation()
		{
			*this << "    ";
			_bLastOutputCharacterIsNewLine = false;
		}
		
		void Log::_BeginLine_OnLevelNestier()
		{
			_BeginLine_NumIdentations(GetNestingMessageLevel());
		}

		/***************************************************************************************************************
		* Manipulator implementation
		***************************************************************************************************************/
		Log& Verb_ID(Log& InLog)
		{
			InLog.PushMessageVerbosityLevel(EVerbosity::ID, EManipulatorScope::ToLineEnd);
			return InLog;
		}

		Log& Verb_Normal(Log& InLog)
		{
			InLog.PushMessageVerbosityLevel(EVerbosity::Normal, EManipulatorScope::ToLineEnd);
			return InLog;
		}

		Log& Verb_Info(Log& InLog)
		{
			InLog.PushMessageVerbosityLevel(EVerbosity::Informative, EManipulatorScope::ToLineEnd);
			return InLog;
		}

		Log& Verb_Debug(Log& InLog)
		{
			InLog.PushMessageVerbosityLevel(EVerbosity::Debug, EManipulatorScope::ToLineEnd);
			return InLog;
		}

		Log& Verb_Full(Log& InLog)
		{
			InLog.PushMessageVerbosityLevel(EVerbosity::Full, EManipulatorScope::ToLineEnd);
			return InLog;
		}

		/***************************************************************************************************************
		* ToString implementation
		***************************************************************************************************************/
		std::string ToString_AbbreviatedMessageType(EMessageType InMessageType)
		{
			switch (InMessageType)
			{
			case EMessageType::SimpleText:
				return "Txt";
			case EMessageType::Error:
				return "ERROR";
			case EMessageType::FatalError:
				return "FATAL_ERROR(!)";
			case EMessageType::Status:
				return "Status";
			case EMessageType::Info:
				return "Info";
			default:
				return "Unknown";
			}
		}

		std::string ToString_Verbosity(EVerbosity InVerbosity)
		{
			switch (InVerbosity)
			{
			case EVerbosity::LogSystem:
				return "LogSystem";

			case EVerbosity::ID:
				return "ID";

			case EVerbosity::Normal:
				return "Normal";

			case EVerbosity::Informative:
				return "Informative";

			case EVerbosity::Debug:
				return "Debug";

			case EVerbosity::Full:
				return "Full";

			default:
				return "Unknown";
			}
		}

		/*********************************************************************************************
		* LogSettings utils implementation
		**********************************************************************************************/
		bool LogSettings::ShouldLogMessage(EMessageType InMsgType, EVerbosity InMsgVerbosity) const
		{
			auto index_MsgVerbosity = InMsgVerbosity;
			auto index_actualMaximalVerbosity_ForMessageType = GetActualVerbosity_ForMessageType(InMsgType);

			if (IsMessageTypeFilteredOut(InMsgType))
			{
				return false;
			}

			if (index_MsgVerbosity > index_actualMaximalVerbosity_ForMessageType)
			{
				return false;
			}
			return true;
		}

		EVerbosity LogSettings::GetActualVerbosity_ForMessageType(EMessageType InMessageType) const
		{
			if (overrideVerbosity.is_initialized())
			{
				return overrideVerbosity.get();
			}

			auto messageTypeIndex = static_cast<size_t>(InMessageType);
			auto messageType_OverrideVerbosity = overrideVerbosityForMessageType[messageTypeIndex];
			if (messageType_OverrideVerbosity.is_initialized())
			{
				return messageType_OverrideVerbosity.get();
			}

			auto realVerbosity = verbosity_ForMessageType[messageTypeIndex];
			return realVerbosity;
		}

		void LogSettings::SetEchoLevel(EMessageType msgType, EVerbosity InVerbosity, bool bEnabled, bool bInAlwaysEcho)
		{
			auto msgIndex                         = static_cast<int>(msgType);

			bEchoLevel[msgIndex]                  = bEnabled;
			bAlwaysEcho[msgIndex]                 = bInAlwaysEcho;
			bFilterOut_MessageLevel[msgIndex]     = false;
			verbosity_ForMessageType[msgIndex]    = InVerbosity;
		}

		void LogSettings::SetDefaultEchoLevel()
		{
			for (auto i = 0; i < static_cast<size_t>(EMessageType::NUM_TYPES); i++)
			{
				auto messageType = static_cast<EMessageType>(i);
				SetEchoLevel(messageType, EVerbosity::Full, false, false);
			}

			SetEchoLevel(EMessageType::SimpleText, EVerbosity::Full, false);
			SetEchoLevel(EMessageType::FatalError, EVerbosity::Full, true, true);
			SetEchoLevel(EMessageType::Error, EVerbosity::Full, true, true);
			SetEchoLevel(EMessageType::Status, EVerbosity::Full, false);
			SetEchoLevel(EMessageType::Info, EVerbosity::Full, false);
		}

		void LogSettings::SetFilterOut_ForMessageType(EMessageType msgType, bool bEnabled)
		{
			auto index                                   = static_cast<int>(msgType);
			bFilterOut_MessageLevel[index]               = bEnabled;
		}

		bool LogSettings::IsMessageTypeFilteredOut(EMessageType msgType) const
		{
			auto msgIndex = static_cast<size_t>(msgType);
			assert(msgIndex < static_cast<size_t>(EMessageType::NUM_TYPES));
			auto bFilteredOut = bFilterOut_MessageLevel[msgIndex];
			return bFilteredOut;
		}

		void Log_Settings(Log *pLog, const LogSettings *pInSettings)
		{
			LogMessageRecordScoper scoper(pLog, EMessageType::Info, "LogSettings state");

			*pLog << "logName: " << pInSettings->logName;
			
			pLog->AppendNewLine();
			*pLog << "logFilename: " << pInSettings->logFilename;
			
			pLog->AppendNewLine();
			*pLog << "recordNestingLevelToEcho= " << pInSettings->recordNestingLevelToEcho;
			*pLog << "; ident_numSpaces_perLevel= " << pInSettings->ident_numSpaces_perLevel;

			pLog->AppendNewLine();
			*pLog << "maxLinesPerPage= " << pInSettings->output.maxLinesPerPage;
			*pLog << "; maxCharsPerLine= " << pInSettings->output.maxCharsPerLine;
			
			pLog->AppendNewLine();
			*pLog << "MessageTypes: ";

			auto optional_verbosityOverridenForAll = pInSettings->overrideVerbosity;
			std::string str_optional_verbosityOverridenForAll = optional_verbosityOverridenForAll.is_initialized() ? ToString_Verbosity(optional_verbosityOverridenForAll.get()) : "{NULL}";

			for (auto i = 0; i < static_cast<size_t>(EMessageType::NUM_TYPES); i++)
			{
				// Extra scope is necessary for log record scoper
				{
					auto messageType = static_cast<EMessageType>(i);

					Util::LogMessageRecordScoper scoper(pLog, Util::EMessageType::Info, std::string("MsgType: ") + ToString_AbbreviatedMessageType(messageType));

					bool bEchoLevel = pInSettings->bEchoLevel[i];
					bool bAlwaysEcho = pInSettings->bAlwaysEcho[i];
					bool bFilterOut = pInSettings->bFilterOut_MessageLevel[i];

					auto verbosity_ForMessageType = pInSettings->verbosity_ForMessageType[i];
					auto actualVerbosity_ForMessageType = pInSettings->GetActualVerbosity_ForMessageType(messageType);
					auto optional_verbosityOverridenForType = pInSettings->overrideVerbosityForMessageType[i];

					std::string str_optional_verbosityOverridenForType = optional_verbosityOverridenForType.is_initialized() ? ToString_Verbosity(optional_verbosityOverridenForType.get()) : "{NULL}";


					std::string verbosity_str = std::string(" ( ") +
						std::string("actualVerbosity = ") + ToString_Verbosity(actualVerbosity_ForMessageType) +
						std::string("; OVERRIDE_FOR_TYPE= ") + str_optional_verbosityOverridenForType +
						std::string("; OVERRIDE: ") + str_optional_verbosityOverridenForAll +
						std::string(")");

					*pLog << "bEchoLevel= " << bEchoLevel;
					*pLog << "; bAlwaysEcho= " << bAlwaysEcho;
					*pLog << "; bFilterOut= " << bFilterOut;
					*pLog << "; verbosity= " << verbosity_str;
				}
			}
		}
	} // Util
} // Dv