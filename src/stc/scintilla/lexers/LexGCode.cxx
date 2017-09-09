// Scintilla source code edit control
/** @file LexGCode.cxx
 ** Lexer for GCode Simple Numerical Control Language
 ** Written by SH
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

static inline bool IsAWordChar(const int ch) {
	return (ch < 0x80) && (IsADigit(ch) || ch == '.' || ch == '-' || ch == '+' || ch == 'e' || ch == 'E');
}

static inline bool IsAWordStart(const int ch) {
	return (ch < 0x80) && 
		(ch == 'G' ||
		 ch == 'M' || 
		 ch == 'T' ||
		 ch == 'S' ||
		 ch == 'N' );
}
static inline bool IsAWordEnd(const int ch) {
	return (ch < 0x80) && 
		(ch == ' ' ||
		 ch == ';' || 
		 ch == '(' );
}
static inline bool IsAParamStart(const int ch) {
	return (ch < 0x80) && 
		(ch == 'X' ||
		 ch == 'Y' ||
		 ch == 'Z' ||
		 ch == 'A' ||
		 ch == 'B' ||
		 ch == 'C' ||
		 ch == 'D' ||
		 ch == 'H' ||
		 ch == 'S' ||
		 ch == 'F' ||
		 ch == 'P' ||
		 ch == 'I' ||
		 ch == 'J' ||
		 ch == 'K' );
}

static inline bool IsAsmOperator(char ch) {
	if (isalnum(ch))
		return false;
	// '.' left out as it is used to make up numbers
	if (ch == '*' || ch == '/' || ch == '-' || ch == '+')
		return true;
	return false;
}

//#pragma warning(disable: 4100)

static void ColouriseGCodeDoc(unsigned int startPos, int length, int initStyle, WordList *keywordlists[], Accessor &styler)
{
	StyleContext sc(startPos, length, initStyle, styler);

	for ( ; sc.More(); sc.Forward() ) {

		// Determine if the current state should terminate.
		if ( sc.state == SCE_GCODE_OPERATOR ) {
			if ( !IsAsmOperator(static_cast<char>(sc.ch))) {
			    sc.SetState(SCE_GCODE_DEFAULT);
			}
		} else if ( sc.state == SCE_GCODE_NUMBER ) {
			if ( !IsAWordChar(sc.ch)) {
				sc.SetState(SCE_GCODE_DEFAULT);
			}
		} else if ( sc.state == SCE_GCODE_IDENTIFIER ) {
			if ( IsAWordEnd(sc.ch) || sc.atLineEnd ) {
				sc.SetState(SCE_GCODE_DEFAULT);
			}
		} else if ( sc.state == SCE_GCODE_PARAM ) {
			if ( IsAWordChar(sc.ch) ) {
				sc.SetState(SCE_GCODE_DEFAULT);
			}
		} else if ( sc.state == SCE_GCODE_COMMENT ) {
			if (sc.ch == ')') {
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			}
		} else if ( sc.state == SCE_GCODE_COMMENT_LINE ) {
			if ( sc.atLineEnd ) {
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			}
		} else if ( sc.state == SCE_GCODE_DIRECTIVE ) {
			if ( sc.atLineEnd ) {
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			}
		} else if ( sc.state == SCE_GCODE_BUF ) {
			if (sc.ch == ')') {
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			}
		} else if ( sc.state == SCE_GCODE_CMD ) {
			if (sc.ch == ')') {
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			}
		} else if ( sc.state == SCE_GCODE_MSG ) {
			if (sc.ch == ')') {
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			}
		}

		// Determine if a new state should be entered.
		if ( sc.state == SCE_GCODE_DEFAULT ) {
			
			if ( sc.MatchIgnoreCase("(buf,") ) {
				sc.SetState(SCE_GCODE_BUF);
				
			} else if ( sc.MatchIgnoreCase("(cmd,") ) {
				sc.SetState(SCE_GCODE_CMD);
				
			} else if ( sc.MatchIgnoreCase("(msg,") ) {
				sc.SetState(SCE_GCODE_MSG);
				
			} else if ( sc.ch == '(' ) {
				sc.SetState(SCE_GCODE_COMMENT);
				
			} else if ( sc.ch == ';' ) {
				sc.SetState(SCE_GCODE_COMMENT_LINE);
				
			} else if ( sc.ch == '%' ) {
				sc.SetState(SCE_GCODE_DIRECTIVE);
				
			} else if ( isdigit(sc.ch) || ( sc.ch == '.' && isdigit(sc.chNext)) ) {
				sc.SetState(SCE_GCODE_NUMBER);
				
			} else if ( IsAWordStart(sc.ch) ) {
				sc.SetState(SCE_GCODE_IDENTIFIER);
				
			} else if ( IsAParamStart(sc.ch) ) {
				sc.SetState(SCE_GCODE_PARAM);
				
			} else if ( sc.ch == '\"' ) {
				sc.SetState(SCE_GCODE_STRING);
				
			} else if ( IsAsmOperator(static_cast<char>(sc.ch)) ) {
				sc.SetState(SCE_GCODE_OPERATOR);
			}
		}
	}
	sc.Complete();
}
 
static const char * const gcodeWordListDesc[] = {"Command","Parameter", "Constant", 0};

LexerModule lmGCode(SCLEX_GCODE, ColouriseGCodeDoc, "gcode", 0, gcodeWordListDesc);


