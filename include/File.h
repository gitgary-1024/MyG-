#include<fstream>
#include<string>
#include<vector>
#include"./Token.h"
#include"./AST/AST.h"
#include"./AST/ASTPrinter.h"
#include"./IR/IR.h"
using namespace std;

const string symbolList = "+-*/=(){}[];,&|!><";

bool issymbol(char c) {
	return symbolList.find(c) != symbolList.npos;
}

bool islitter(char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

class File {
		fstream codeFile;
		fstream tempFile;
		vector<Token> TokenList;
		ASTBaseNode* ASTroot;
		vector<IRInstr> IR;
		
		void format() {
			string line = "";
			
			while (getline(codeFile, line)) {
				for (size_t i = 0; i < line.size(); i++) {
					tempFile << line[i];
					bool isMultiCharOp = false;
					
					// 检查是否为多字符逻辑运算符（如&&、||）
					if (i + 1 < line.size()) {
						string twoChars = string(1, line[i]) + string(1, line[i + 1]);
						
						// 包含逻辑运算符和比较运算符
						if (twoChars == "&&" || twoChars == "||" ||
						twoChars == "==" || twoChars == "!=" ||
						twoChars == ">=" || twoChars == "<=" ||
						twoChars == "++") {
							isMultiCharOp = true;
						}
					}
					
					// 仅在不是多字符运算符的情况下，才添加空格分隔
					if (issymbol(line[i])) {
						if ((i + 1 < line.size()) && isdigit(line[i + 1]) && !isMultiCharOp) {
							tempFile << " ";
						}
						
						if ((i + 1 < line.size()) && islitter(line[i + 1]) && !isMultiCharOp) {
							tempFile << " ";
						}
						
						if ((i + 1 < line.size()) && issymbol(line[i + 1]) && !isMultiCharOp) {
							tempFile << " ";
						}
					}
					
					if (isdigit(line[i])) { // 数字
						if ((i + 1 < line.size()) && issymbol(line[i + 1])) { // 符号
							tempFile << " ";
						}
					}
					
					if (islitter(line[i])) { // 字母
						if ((i + 1 < line.size()) && issymbol(line[i + 1])) { // 符号
							tempFile << " ";
						}
					}
				}
				
				tempFile << "\n";
			}
		}
		
		void getAllToken() {
			tempFile.seekg(0, std::ios::beg);
			string content = "";
			int c = 0;
			
			while (tempFile >> content) {
				TokenList.push_back({content, c});
				c += content.size() + 1;
			}
		}
		
		void compileAST() {
			AST ast(TokenList);
			ASTroot = ast.buildAST();
		}
		
		void compileIR() {
			IR = getIRFromAST(ASTroot);
		}
		
	public:
		File() {}
		
		File(const string& fileName) {
			codeFile.open(fileName);
			tempFile.open("temp.txt", ios::out | ios::in | ios::trunc);
			compile();
		}
		
		~File() {
			codeFile.close();
			tempFile.close();
		}
		
		void compile() {
			format(); // 格式化
			getAllToken(); // 转为token形式
			compileAST(); // 转为AST
			compileIR();
			#ifdef _DEBUG
			printAST();
			#endif
		}
		
		#ifdef _DEBUG
		void printAST() {
			if (ASTroot) {
				ASTPrinter printer;
				printer.printAST(ASTroot); // 调用ASTPrinter打印AST结构
			}
			else {
				cout << "No AST generated." << endl;
			}
		}
		
		#endif
		
		vector<Token> returnAllToken() {
			return TokenList;
		}
};
