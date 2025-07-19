#ifndef TOKEN_H
#define TOKEN_H

#include<string>
#include<unordered_map>
using namespace std;

enum TokenType {
	Keywords, // 关键词
	Identifiers, // 变量/函数
	Literals, // 常数
	Operators, // 运算符
	Punctuators // 分隔符
};

unordered_map<string, TokenType> StringToTokenType = {
	{"int", Keywords}, {"return", Keywords},
	{"+", Operators}, {"-", Operators},
	{"*", Operators}, {"/", Operators},
	{"(", Punctuators}, {")", Punctuators},
	{"[", Punctuators}, {"]", Punctuators},
	{"{", Punctuators}, {"}", Punctuators},
	{";", Punctuators}, {",", Punctuators}
};

bool isStringDigit(const string& str) {
	for (const char& c : str) {
		if (!isdigit(c)) {
			return 0;
		}
	}
	
	return 1;
}

TokenType getTokenType(const string& str) {
	if (StringToTokenType.find(str) != StringToTokenType.end()) {
		return StringToTokenType[str];
	}
	else if (isStringDigit(str)) {
		return Literals;
	}
	
	return Identifiers;
}

class Token {
		TokenType type;
		string content;
		int c;
	public:
		Token() {}
		
		Token(const string& content, int c) {
			this->content = content;
			type = getTokenType(content);
			this->c = c;
		}
		
		pair<TokenType, string> getToken() {
			return {type, content};
		}
		
		~Token() {
		}
};

#endif
