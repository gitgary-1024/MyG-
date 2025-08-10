#ifndef IR_BASE_H
#define IR_BASE_H

#include<string>
#include<string.h>
#include<vector>
#include<string>
#include<unordered_map>
using namespace std;

enum IROp {
	ADD, // +
	SUB, // -
	MUL, // *
	DIV, // /
	ASSIGN, // 赋值
	IF_GT, // if条件
	Goto, // 转跳（用于FuncCall以及For和If）
	INC // 自增 i++
};

string IROpToString[] = {
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"ASSIGN",
	"IF_GT",
	"Goto",
	"INC"
};

struct IRInstr {
	IROp op; // 操作符
	unordered_map<string, string> label; //label[标签名称]=标签的值
};

template<typename _Tp>
struct MyStack { //手写栈
	vector<_Tp> stackContent;
	
	void push(_Tp x) {
		stackContent.push_back(x); // 压栈
	}
	
	char top() {
		return stackContent.back(); // 出栈
	}
	
	void pop() {
		stackContent.pop_back(); // 弹出
	}
	
	int empty() {
		return stackContent.empty(); // 判断是否为空
	}
};

#endif
