#ifndef _Util_log_h_
#define _Util_log_h_

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <stack>
#include <cassert>
#include "boost/optional.hpp"
#include "../Util/h/Util_Output.h"

namespace Dv
{
	namespace Util
	{
		/*************************************************************************************************************************
		* Fwd declarations
		*************************************************************************************************************************/
		class Log;

		/*************************************************************************************************************************
		* Types
		*************************************************************************************************************************/
		enum class EMessageType
		{
			SimpleText = 0,
			FatalError, // critical error (for example, "Cannot initialize rendering subsystem")
			Error, // non-critical error (for example, "texture has not been loaded")
			Status, // For debug messages (for example, "output from destructor")
			Info, // For debugging information  (for example, "log state")
			NUM_TYPES
		};
		std::string ToString_AbbreviatedMessageType(EMessageType InMessageType);

		enum class EVerbosity
		{
			LogSystem, // All message from the log itself to be outputted using this priority
			ID, // Output minimal info that identifies the given object (that includes only ID and name)
			Normal, // Output normal public info
			Informative, // Output extended public info
			Debug, // Output debugging info
			Full, // output full info
			NUM
		};
		std::string ToString_Verbosity(EVerbosity InVerbosity);

		/*************************************************************************************************************************
		* Log settings
		*
		* Default ctor calls SetDefaultEchoLevel() automatically.
		*************************************************************************************************************************/
		std::string GetLog_DefaultFilename(const std::string& InName);

		struct LogSettings
		{
			/*************************************************************************************************************************
			* Fields
			*************************************************************************************************************************/
			std::string                                   logName                            = std::string("DEFAULT"); // name of the log object
			std::string                                   logFilename                        = std::string("C:\\!DV\\!LOGS\\")+logName+std::string(".txt");
			// If this value is set, then for all message type the actual verbosity to be used is this verbosity (irrespective of overrideVerbosityForType values)
			boost::optional<EVerbosity>                   overrideVerbosity                  = boost::optional<EVerbosity>();
			// If this value is set, then for messages of the given type, verbosity to use is this verbosity (irrespective of verbosity_ForMessageType values)
			boost::optional<EVerbosity>                   overrideVerbosityForMessageType    [static_cast<size_t>(EMessageType::NUM_TYPES)];

			// Verbosity levels for each of the message types
			EVerbosity                                    verbosity_ForMessageType[static_cast<size_t>(EMessageType::NUM_TYPES)];
			// should we echo messages of corresponding type to screen?
			bool                                          bEchoLevel[static_cast<size_t>(EMessageType::NUM_TYPES)];
			// If corresponding element is set, messages of corresponding types are echoed ever if nesting level is not appropriate
			bool                                          bAlwaysEcho[static_cast<size_t>(EMessageType::NUM_TYPES)];
			// If true, filter out completely messages of the given message type (do NOT even write into the log file)
			bool                                          bFilterOut_MessageLevel[static_cast<size_t>(EMessageType::NUM_TYPES)];
			Dv::Utils::OutputParams                       output;
			// Level of nested levels on which the messages should be echoed
			// (WARNING!!!  Setting up value of ZERO will disable eching completely)
			size_t                                        recordNestingLevelToEcho = 1;
			// Identation used for each level of the log message (identation is correctly applied for newlines)
			size_t                                        ident_numSpaces_perLevel = 1;

			/*************************************************************************************************************************
			* Working with name and filename
			*************************************************************************************************************************/
			// Sets name of log to the given value and filename to default value dependent of name
			void                                         SetName_AndDefaultFilename                        (const std::string& InName);

			/*************************************************************************************************************************
			* Checking whether logging is enabled for the given messages
			*************************************************************************************************************************/
			bool                                          ShouldLogMessage                                 (EMessageType InMsgType, EVerbosity InMsgVerbosity) const;

			/*************************************************************************************************************************
			* Working with verbosity
			*************************************************************************************************************************/
			// This is actual verbosity to be used for the given message type. Period.
			EVerbosity                                    GetActualVerbosity_ForMessageType                (EMessageType InMessageType) const;

			/*************************************************************************************************************************
			* Working with echo level
			*************************************************************************************************************************/
			bool                                          GetEchoLevel                       (EMessageType msgType)                                                           { return bEchoLevel[(int)msgType]; }
			bool                                          ShouldMessageType_AlwaysBeEchoed   (EMessageType msgType) const                                                     { return bAlwaysEcho[(int)msgType];  }
			bool                                          ShouldMessageType_BeFilteredOut    (EMessageType msgType) const                                                     { return bFilterOut_MessageLevel[(int)msgType]; }

			void                                          SetEchoLevel                       (EMessageType msgType, EVerbosity InVerbosity, bool bEnabled, bool bInAlwaysEcho = false);
			void                                          SetDefaultEchoLevel                ();
			void                                          SetFilterOut_ForMessageType        (EMessageType msgType, bool bEnabled);

			/*************************************************************************************************************************
			* Working with filters
			*************************************************************************************************************************/
			bool                                          IsMessageTypeFilteredOut           (EMessageType msgTyp) const;

			/*************************************************************************************************************************
			* Construction
			*************************************************************************************************************************/
			inline                                        LogSettings                        ();
			inline                                        LogSettings                        (const char *logFilenameIn);
		};
		void                                              Log_Settings                       (Log *pLog, const LogSettings *pInSettings);

		/*************************************************************************************************************************
		* Record of the log
		*************************************************************************************************************************/
		struct LogRecord
		{
			EMessageType                                  messageType;
			std::string                                   title;

			LogRecord(EMessageType InMessageType, const std::string& InTitle) :
				messageType(InMessageType), title(InTitle) {}
		};


		/*************************************************************************************************************************
		* Manipulators
		*************************************************************************************************************************/
		using LogManip = Log& (Log&);

		// Verbosity setters on the level of line
		Log& Verb_ID(Log&);
		Log& Verb_Normal(Log&);
		Log& Verb_Info(Log&);
		Log& Verb_Debug(Log&);
		Log& Verb_Full(Log&);

		/*************************************************************************************************************************
		* Scope of the manipulator
		*************************************************************************************************************************/
		enum EManipulatorScope
		{
			Inserter, // This manipulator is actual within the given inserter only
			ToLineEnd, // This manipulator is actual to the end of the given line
			ToRecordEnd // This manipulator is actual to the end of the given log record
		};

		/*************************************************************************************************************************
		* Element of the verbosity stack
		*************************************************************************************************************************/
		struct VerbosityStackElem
		{
			EVerbosity                                     verbosity;
			EManipulatorScope                              scope;
			size_t                                         logRecordNestingLevel;

			VerbosityStackElem(EVerbosity InVerbosity, EManipulatorScope InScope) :
				VerbosityStackElem(InVerbosity, InScope, 0) {}
			VerbosityStackElem(EVerbosity InVerbosity, EManipulatorScope InScope, size_t InLogRecordNestingLevel) :
				verbosity(InVerbosity),
				scope(InScope),
				logRecordNestingLevel(InLogRecordNestingLevel) {}
		};

		/*************************************************************************************************************************
		* Log:
		*
		* BeginMessage()/EndMessage() can be nested.
		*
		* Logs message to file and echoes messages to user (if corresponding settings are set).
		* If message echoing is enabled, message are echoed only when EndMessage() is called.
		*
		* TODO:
		* 1. Message scoper
		* 2. Output:
		* 2.1. Outputting 64-bit values
		* 2.2 Output durations
		* 2. Log record formatting:
		* 2.1. nested level identation
		* 2.2. Separation between log records
		*************************************************************************************************************************/
		class Log
		{
		public:
			/*************************************************************************************************************************
			* Construction
			*************************************************************************************************************************/
			Log                                                                                 (const LogSettings& settingsIn = LogSettings());
			~Log                                                                                ();

			/*************************************************************************************************************************
			* Output functions
			*************************************************************************************************************************/
			// Output functions: output as text type of GetMessageType()
			friend Log&                            operator<<                                   (Log& oStrm, const std::string_view& s);
			friend Log&                            operator<<                                   (Log& oStrm, const std::string& s);
			friend Log&                            operator<<                                   (Log& oStrm, const char *str);
			friend Log&                            operator<<                                   (Log& oStrm, void *str);
			friend Log&                            operator<<                                   (Log& oStrm, unsigned int val);
			friend Log&                            operator<<                                   (Log& oStrm, int val);
			friend Log&                            operator<<                                   (Log& oStrm, char val);
			friend Log&                            operator<<                                   (Log& oStrm, bool bVal);
			friend Log&                            operator<<                                   (Log& oStrm, double val);
			friend Log&                            operator<<                                   (Log& oStrm, float val);
			friend Log&                            operator<<                                   (Log& oStrm, const std::chrono::system_clock::time_point& InTimepoint);
			friend Log&                            operator<<                                   (Log& oStrm, const std::time_t& InTime);
			friend Log&                            operator<<                                   (Log& oStrm, LogManip InManip);

			void                                   AppendNewLine                                ();
			void                                   Flush                                        ()                                      { _fileStream.flush(); }

			/*************************************************************************************************************************
			* Message
			*************************************************************************************************************************/
			void                                   BeginMessage                                 (const LogRecord *pInRecord);
			void                                   EndMessage                                   ();
			const LogRecord                       *GetTopMessage                                () const;
			EMessageType                           GetMessageType                               () const;
			// Get nesting level of the message
			size_t                                 GetNestingMessageLevel                       () const                                 { return _messageStack.empty() ? (-1) : (_messageStack.size() - 1); }

			/*************************************************************************************************************************
			* Verbosity levels
			*************************************************************************************************************************/
			// Push current message verbosity level;
			// Popping will be performed automatically at the moment determined by the manipulator scope argument.
			void                                   PushMessageVerbosityLevel                    (EVerbosity InVerbosity, EManipulatorScope InManipulatorScope);

			/*************************************************************************************************************************
			* SetW
			*************************************************************************************************************************/
			void                                   SetW                                         (size_t InLength);

			/*************************************************************************************************************************
			* Settings
			*************************************************************************************************************************/
			const LogSettings&                     GetSettings                                  () const                                  { return _settings; }
			void                                   UpdateSettings                               (const LogSettings& newValue);			

		private:
			void                                                    _PrintLogRecordSeparator_IfShouldDoItNow();
			void                                                    _PrintCurrentDateTime();
			void                                                    _PrintLogInitializationMessage();
			void                                                    _PrintLogAfterDestructionMessage();

			void                                                    _ErrorMessage_CannotCreateLog();
			void                                                    _ErrorMessage(const std::string& InText);

			// Pop all verbosity stack elems of the given scope
			void                                                    _Pop_VerbosityStackElems               (EManipulatorScope InScope, size_t InNestingLevel);
			bool                                                    _ShouldMessageBeLogged                 () const;

			// To be called before writing each line:
			// - Writes log record level identations
			void                                                    _BeginLine();
			void                                                    _BeginLine_NumIdentations(size_t InIdentationLevel);
			// Begin line and number of identations number of levels nestier then the current message
			void                                                    _BeginLine_OnLevelNestier();
			void                                                    _WriteIdentation();
			// Append newline without beginning the new line (for System purposes)
			void                                                   _AppendNewLine_NoBeginLine();

			bool                                                   _ShouldMessageLevel_AlwaysBeEchoed       (EMessageType InType) const;
			bool                                                   _ShouldMessageLevel_BeEchoed             (EMessageType InType) const;
			// Should we echo current output according to current LogRecord and log settings
			bool                                                    _ShouldOutputBeEchoed                   () const;

			void                                                    _EchoBuffered_IfShouldDotItNow          ();
			void                                                    _EchoBuffered                           ();
			bool                                                    _ShouldEchoNow                          () const;

			size_t                                                  _currentIdentationLevel = 0;
			bool _bLastOutputCharacterIsNewLine                     = true;
			
			// Stack for verbosity levels
			std::stack<VerbosityStackElem>                          _verbosityStack;

			// messageBuffer: this buffer is for echoing only, messages are outputted to files using normal std::ofstream buffering.
			std::ostringstream                                      _messageBufferStrm;

			std::ofstream                                           _fileStream;
			LogSettings                                             _settings;
			// Stack always must contain at lesat one element
			std::stack<LogRecord>                                   _messageStack;
		};

		Log& operator<<(Log& oStrm, const std::string_view& s);
		Log& operator<<(Log& oStrm, const std::string& s);		
		Log& operator<<(Log& oStrm, const char *str);
		Log& operator<<(Log& oStrm, void *str);
		Log& operator<<(Log& oStrm, unsigned int val);
		Log& operator<<(Log& oStrm, int val);
		Log& operator<<(Log& oStrm, char val);
		Log& operator<<(Log& oStrm, bool bVal);		
		Log& operator<<(Log& oStrm, double val);
		Log& operator<<(Log& oStrm, float val);
		Log& operator<<(Log& oStrm, const std::chrono::system_clock::time_point& InTimepoint);
		Log& operator<<(Log& oStrm, const std::time_t& InTime);

		/***********************************************************************************************************************************************
		* Log record scoper
		***********************************************************************************************************************************************/
		class LogMessageRecordScoper
		{
		public:
			LogMessageRecordScoper(Log *pInLog, EMessageType InMessageType, const std::string &InTitle, boost::optional<EVerbosity> InVerbosity = boost::optional<EVerbosity>()) :
				_verbosity(InVerbosity),
				_pLog(pInLog)
			{
				assert(pInLog != nullptr);
				Util::LogRecord record(InMessageType, InTitle);
				_pLog->BeginMessage(&record);
				if (_verbosity.is_initialized())
				{
					_pLog->PushMessageVerbosityLevel(_verbosity.get(), EManipulatorScope::ToRecordEnd);
				}
			}
			~LogMessageRecordScoper()
			{
				_pLog->EndMessage();
			}

		private:
			boost::optional<EVerbosity>                _verbosity;
			Log                                       *_pLog;
		};

		/*****************************************************************************************************************************************
		* Log settings implementation
		******************************************************************************************************************************************/
		inline LogSettings::LogSettings()
		{
			SetDefaultEchoLevel();
		}

		inline LogSettings::LogSettings(const char *logFilenameIn) :
			logFilename(logFilenameIn)
		{
			SetDefaultEchoLevel();
		}

		inline void LogSettings::SetName_AndDefaultFilename(const std::string& InName)
		{
			logName = InName;
			logFilename = GetLog_DefaultFilename(InName);
		}

		inline std::string GetLog_DefaultFilename(const std::string& InName)
		{
			return std::string("C:\\!DV\\!LOGS\\") + InName + std::string(".txt");
		}
	} // Util
} // Dv

#endif // _Util_log_h_