#ifndef CORELIB_TEXT_PARSER_H
#define CORELIB_TEXT_PARSER_H

#include "Basic.h"
#include "Regex/MetaLexer.h"

namespace CoreLib
{
	namespace Text
	{
		class TextFormatException : public Exception
		{
		public:
			TextFormatException(String message)
				: Exception(message)
			{}
		};

		const int TokenType_Identifier = 3;
		const int TokenType_Int = 4;
		const int TokenType_Float = 5;
		const int TokenType_StringLiteral = 6;
		const int TokenType_CharLiteral = 7;

		class Parser
		{
		private:
			LexStream stream;
			bool legal;
			LinkedNode<LexToken> * curNode; 
			
		public:
			Parser(Basic::String text);
			int ReadInt()
			{
				auto token = ReadToken();
				if (token.TypeID == TokenType_Int)
				{
					return StringToInt(token.Str);
				}
				throw TextFormatException(L"Text parsing error: int expected.");
			}
			double ReadDouble()
			{
				auto token = ReadToken();
				if (token.TypeID == TokenType_Float || token.TypeID == TokenType_Int)
				{
					return StringToDouble(token.Str);
				}
				throw TextFormatException(L"Text parsing error: floating point value expected.");
			}
			String ReadWord()
			{
				auto token = ReadToken();
				if (token.TypeID == TokenType_Identifier)
				{
					return token.Str;
				}
				throw TextFormatException(L"Text parsing error: identifier expected.");
			}
			String Read(const wchar_t * expectedStr)
			{
				auto token = ReadToken();
				if (token.Str == expectedStr)
				{
					return token.Str;
				}
				throw TextFormatException(L"Text parsing error: \'" + String(expectedStr) + L"\' expected.");
			}
			String Read(String expectedStr)
			{
				auto token = ReadToken();
				if (token.Str == expectedStr)
				{
					return token.Str;
				}
				throw TextFormatException(L"Text parsing error: \'" + expectedStr + L"\' expected.");
			}
			String ReadStringLiteral()
			{
				auto token = ReadToken();
				if (token.TypeID == TokenType_StringLiteral)
				{
					return token.Str.SubString(1, token.Str.Length()-2);
				}
				throw TextFormatException(L"Text parsing error: string literal expected.");
			}
			LexToken ReadToken()
			{
				if (curNode)
				{
					LexToken rs = curNode->Value;
					curNode = curNode->GetNext();
					return rs;
				}
				throw TextFormatException(L"Unexpected ending.");
			}
			LexToken NextToken()
			{
				if (curNode)
					return curNode->Value;
				else
				{
					LexToken rs;
					rs.TypeID = -1;
					rs.Position = -1;
					return rs;
				}
			}
			bool IsEnd()
			{
				return curNode == 0;
			}
		public:
			bool IsLegalText()
			{
				return legal;
			}
		};
	}
}

#endif