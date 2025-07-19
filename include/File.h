#include<fstream>
#include<string>
#include<vector>
#include"./Token.h"
#include"./AST.h"
#include"./ASTPrinter.h"
using namespace std;

const string symbolList = "+-*/=(){}[];,";

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
		
		void format() {
			string line = "";
			
			while (getline(codeFile, line)) {
				for (size_t i = 0; i < line.size(); i++) {
					tempFile << line[i];
					
					if (isdigit(line[i])) { // 数字
						if ((i + 1 < line.size()) && issymbol(line[i + 1])) { // 符号
							tempFile << " ";
						}
					}
					
					if (issymbol(line[i])) { // 数字
						if ((i + 1 < line.size()) && isdigit(line[i + 1])) { // 符号
							tempFile << " ";
						}
					}
					
					if (issymbol(line[i])) { // 符号
						if ((i + 1 < line.size()) && islitter(line[i + 1])) { // 字母
							tempFile << " ";
						}
					}
					
					if (issymbol(line[i])) { // 符号
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
		
	public:
		File() {}
		
		File(const string& fileName) {
			codeFile.open(fileName);
			tempFile.open("temp.txt");
			compile();
		}
		
		~File() {
			codeFile.close();
			tempFile.close();
			delete ASTroot;
		}
		
		void compile() {
			format(); // 格式化
			getAllToken(); // 转为token形式
			compileAST(); // 转为AST
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
