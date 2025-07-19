#include "./Token.h"
#include "./ASTnode.h"
#include <vector>
#include <stdexcept>
using namespace std;

class AST {
	private:
		vector<Token> tokens;
		size_t currentPos;
		
		// 辅助工具函数：检查当前Token是否匹配目标字符串
		bool match(const string& target) {
			if (isAtEnd())
				return false;
				
			auto [type, content] = peek().getToken();
			return content == target;
		}
		
		// 辅助工具函数：消费当前Token并返回
		Token consume() {
			if (!isAtEnd())
				currentPos++;
				
			return tokens[currentPos - 1];
		}
		
		// 辅助工具函数：预览当前Token
		Token peek() {
			return tokens[currentPos];
		}
		
		// 辅助工具函数：判断是否是否到达Token末尾
		bool isAtEnd() {
			return currentPos >= tokens.size();
		}
		
		// 辅助工具函数：期望特定Token，不匹配则抛出异常
		Token expect(const string& target) {
			if (match(target))
				return consume();
				
			auto [type, content] = peek().getToken();
			throw runtime_error("Unexpected token: " + content + ", expected: " + target);
		}
		
		// 解析语句块（用{}包裹的语句集合）
		ASTBaseNode* parseStatementBlock() {
			StatementBlock* block = new StatementBlock();
			expect("{"); // 消耗左花括号
			
			// 解析块内所有语句直到右花括号
			while (!match("}") && !isAtEnd()) {
				ASTBaseNode* stmt = parseStatement();
				
				if (stmt)
					block->addChild(stmt);
			}
			
			expect("}"); // 消耗右花括号
			return block;
		}
		
		// 解析表达式（目前支持标识符和字面量）
		ASTBaseNode* parseExpression() {
			Token token = peek();
			auto [type, content] = token.getToken();
			
			if (type == Literals) {
				consume(); // 消耗字面量
				return new Expression(Expression::LITERAL, content);
			}
			else if (type == Identifiers) {
				consume(); // 消耗标识符
				
				// 检查是否是函数调用
				if (match("(")) {
					return parseFunctionCall(content);
				}
				
				return new Expression(Expression::IDENTIFIER, content);
			}
			
			throw runtime_error("Unexpected token in expression: " + content);
		}
		
		// 解析变量声明（如：int a;）
		ASTBaseNode* parseVariableDeclaration() {
			// 期望类型关键字（目前只处理int）
			Token typeToken = expect("int");
			string varType = typeToken.getToken().second;
			// 期望变量名（标识符）
			Token nameToken = peek();
			auto [nameType, varName] = nameToken.getToken();
			
			if (nameType != Identifiers) {
				throw runtime_error("Expected identifier for variable name");
			}
			
			consume(); // 消耗变量名
			expect(";"); // 消耗分号
			return new VariableDeclaration(varType, varName);
		}
		
		// 解析函数声明（如：int func() { ... }）
		ASTBaseNode* parseFunctionDeclaration() {
			// 解析返回类型
			Token returnTypeToken = expect("int");
			string returnType = returnTypeToken.getToken().second;
			// 解析函数名
			Token nameToken = peek();
			auto [nameType, funcName] = nameToken.getToken();
			
			if (nameType != Identifiers) {
				throw runtime_error("Expected function name identifier");
			}
			
			consume(); // 消耗函数名
			expect("("); // 消耗左括号
			expect(")"); // 消耗右括号（暂不支持参数）
			// 解析函数体（语句块）
			ASTBaseNode* body = parseStatementBlock();
			FunctionDeclaration* func = new FunctionDeclaration(returnType, funcName);
			func->addChild(body); // 将函数体作为子节点
			return func;
		}
		
		// 解析函数调用（如：func()）
		ASTBaseNode* parseFunctionCall(const string& funcName) {
			FunctionCall* call = new FunctionCall(funcName);
			expect("("); // 消耗左括号
			// 暂不支持参数，直接解析右括号
			expect(")"); // 消耗右括号
			return call;
		}
		
		// 解析语句（支持return语句、变量声明、函数调用）
		ASTBaseNode* parseStatement() {
			if (isAtEnd())
				return nullptr;
				
			auto [type, content] = peek().getToken();
			
			// 处理return语句
			if (content == "return") {
				consume(); // 消耗return关键字
				Statement* returnStmt = new Statement(Statement::RETURN);
				
				// 解析return后的表达式
				if (!match(";")) {
					ASTBaseNode* expr = parseExpression();
					returnStmt->addChild(expr);
				}
				
				expect(";"); // 消耗分号
				return returnStmt;
			}
			
			// 处理变量声明（以int关键字开头）
			if (content == "int" && peek(1).getToken().second != "(") {
				return parseVariableDeclaration();
			}
			
			// 处理语句块
			if (content == "{") {
				return parseStatementBlock();
			}
			
			// 处理函数调用语句（以标识符开头，后跟()）
			if (type == Identifiers && peek(1).getToken().second == "(") {
				string funcName = content;
				consume(); // 消耗函数名
				ASTBaseNode* call = parseFunctionCall(funcName);
				expect(";"); // 函数调用后加分号
				return call;
			}
			
			throw runtime_error("Unexpected statement token: " + content);
		}
		
		// 辅助工具：预览下一个Token（用于判断函数调用）
		Token peek(int offset) {
			if (currentPos + offset >= tokens.size()) {
				return Token("", -1); // 返回空Token表示越界
			}
			
			return tokens[currentPos + offset];
		}
		
	public:
		AST(const vector<Token>& tokenList) : tokens(tokenList), currentPos(0) {}
		
		// 构建AST根节点
		ASTBaseNode* buildAST() {
			StatementBlock* root = new StatementBlock(); // 根节点为语句块
			
			// 解析所有顶级语句（函数声明、全局变量等）
			while (!isAtEnd()) {
				auto [type, content] = peek().getToken();
				
				// 解析函数声明（int + 标识符 + (）
				if (content == "int" && peek(1).getToken().second != ";" &&
				peek(2).getToken().second == "(") {
					ASTBaseNode* func = parseFunctionDeclaration();
					root->addChild(func);
				}
				else {
					// 解析其他语句
					ASTBaseNode* stmt = parseStatement();
					
					if (stmt)
						root->addChild(stmt);
				}
			}
			
			return root;
		}
};
