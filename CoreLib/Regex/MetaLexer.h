#ifndef GX_META_LEXER_H
#define GX_META_LEXER_H

#include "Regex.h"

namespace CoreLib
{
	namespace Text
	{
		class LexToken
		{
		public:
			String Str;
			int TypeID;
			int Position;
		};

		typedef LinkedList<LexToken> LexStream;
	
		class LexerError
		{
		public:
			String Text;
			int Position;
		};

		struct LexProfileToken
		{
			String str;
			enum LexProfileTokenType
			{
				Identifier,
				Equal,
				Regex
			} type;
		};

		typedef LinkedList<LexProfileToken> LexProfileTokenStream;
		typedef LinkedNode<LexProfileToken> LexProfileTokenNode;

		class MetaLexer : public Object
		{
		private:
			RefPtr<DFA_Table> dfa;
			List<String> Regex;
			List<String> TokenNames;
			List<bool> Ignore;
			String ReadProfileToken(LexProfileTokenNode*n, LexProfileToken::LexProfileTokenType type);
			bool ParseLexProfile(String lex);
			void ConstructDFA();
		public:
			int TokensParsed;
			List<LexerError> Errors;
			MetaLexer(String LexProfile);
			MetaLexer();
			DFA_Table * GetDFA();
			String GetTokenName(int id);
			int GetRuleCount();
			void SetLexProfile(String lex);
			bool Parse(String str, LexStream & stream);
		};
	}
}

#endif