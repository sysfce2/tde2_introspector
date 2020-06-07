#pragma once


#include <cstdint>
#include <tuple>
#include <string>


namespace TDEngine2
{
	enum class E_TOKEN_TYPE: uint16_t
	{
		TT_EOF,

		// keywords
		TT_NAMESPACE,
		TT_IDENTIFIER,
		TT_ENUM,
		TT_CLASS,

		// symbols
		TT_COLON,
		TT_OPEN_BRACE,
		TT_CLOSE_BRACE,
		TT_SEMICOLON,
		TT_ASSIGN_OP,
		TT_COMMA,

		TT_NUMBER,
		TT_UNKNOWN,
	};


	struct TToken
	{
		using TCursorPos = std::tuple<uint32_t, uint32_t>;

		virtual ~TToken() = default;

		TToken(E_TOKEN_TYPE type, const TCursorPos& pos = { 0, 0 });

		E_TOKEN_TYPE mType = E_TOKEN_TYPE::TT_EOF;

		TCursorPos   mPos = { 0, 0 };
	};

	
	struct TIdentifierToken : TToken
	{
		explicit TIdentifierToken(const std::string& id);

		std::string mId;
	};

	struct TNumberToken : TToken
	{
		explicit TNumberToken(const std::string& value);

		std::string mValue;
	};
}