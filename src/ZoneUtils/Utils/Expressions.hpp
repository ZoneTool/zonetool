#pragma once

#define MAX_TOKEN_CHARS 1024

namespace ZoneTool
{
	class ExpressionParser
	{
	public:
		ExpressionParser(const char* buffer)
		{
			dataPtr = buffer;
			memset(com_token, 0, sizeof com_token);
			memset(com_parseName, 0, sizeof com_parseName);
			com_lines = 0;
		}

		ExpressionParser(const ExpressionParser& other)
		{
			this->dataPtr = other.dataPtr;
			memcpy(this->com_token, other.com_token, sizeof this->com_token);
			memcpy(this->com_parseName, other.com_parseName, sizeof this->com_parseName);
			this->com_lines = other.com_lines;
		}

		std::string Parse(bool allowLineBreaks = false)
		{
			int c = 0, len;
			bool hasNewLines = false;
			char* data;

			data = (char*)dataPtr;
			len = 0;
			com_token[0] = 0;

			// make sure incoming data is valid
			if (!data)
			{
				dataPtr = nullptr;
				return com_token;
			}

			while (true)
			{
				// skip whitespace
				data = SkipWhitespace(data, &hasNewLines);
				if (!data)
				{
					dataPtr = nullptr;
					return com_token;
				}
				if (hasNewLines && !allowLineBreaks)
				{
					dataPtr = nullptr;
					return com_token;
				}

				c = *data;

				// skip double slash comments
				if (c == '/' && data[1] == '/')
				{
					data += 2;
					while (*data && *data != '\n')
					{
						data++;
					}
				}
					// skip /* */ comments
				else if (c == '/' && data[1] == '*')
				{
					data += 2;
					while (*data && (*data != '*' || data[1] != '/'))
					{
						data++;
					}
					if (*data)
					{
						data += 2;
					}
				}
				else
				{
					break;
				}
			}

			// handle quoted strings
			if (c == '\"')
			{
				data++;
				while (true)
				{
					c = *data++;
					if (c == '\"' || !c)
					{
						com_token[len] = 0;
						dataPtr = (char *)data;
						return com_token;
					}
					if (len < MAX_TOKEN_CHARS)
					{
						com_token[len] = c;
						len++;
					}
				}
			}

			// parse a regular word
			do
			{
				if (len < MAX_TOKEN_CHARS)
				{
					com_token[len] = c;
					len++;
				}
				data++;
				c = *data;
				if (c == '\n')
					com_lines++;
			}
			while (c > 32);

			if (len == MAX_TOKEN_CHARS)
			{
				//		Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
				len = 0;
			}
			com_token[len] = 0;

			dataPtr = (char *)data;
			return com_token;
		}

	private:
		const char* dataPtr;
		char com_token[MAX_TOKEN_CHARS];
		char com_parseName[MAX_TOKEN_CHARS];
		int com_lines;

		char* SkipWhitespace(char* data, bool* hasNewLines)
		{
			int c;

			while ((c = *data) <= ' ')
			{
				if (!c)
				{
					return nullptr;
				}

				if (c == '\n')
				{
					this->com_lines++;
					*hasNewLines = true;
				}

				data++;
			}

			return data;
		}
	};
}
