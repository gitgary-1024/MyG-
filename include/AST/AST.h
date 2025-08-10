#include "../Token.h"
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
		
		// 在AST类中添加参数解析函数
		vector<pair<string, string >> parseParameters() {
			vector<pair<string, string >> params;
			
			// 解析第一个参数
			if (peek().getToken().second == "int") {
				string type = consume().getToken().second;
				string name = consume().getToken().second;
				params.emplace_back(type, name);
				
				// 解析后续参数（逗号分隔）
				while (match(",")) {
					consume(); // 消耗逗号
					
					if (peek().getToken().second != "int") {
						throw runtime_error("Expected int type for parameter");
					}
					
					type = consume().getToken().second;
					name = consume().getToken().second;
					params.emplace_back(type, name);
				}
			}
			return params;
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
		
		// 解析逻辑或（||），优先级最低
		ASTBaseNode* parseLogicalOr() {
			ASTBaseNode* node = parseLogicalAnd(); // 先解析逻辑与
			
			while (match("||")) {
				string op = consume().getToken().second;
				Expression* right = dynamic_cast<Expression*>(parseLogicalAnd());
				node = new Expression(Expression::BINARY_OPERATOR, op,
				                      dynamic_cast<Expression*>(node), right);
			}
			
			return node;
		}
		
		// 解析逻辑与（&&），优先级高于||，低于比较运算符
		ASTBaseNode* parseLogicalAnd() {
			ASTBaseNode* node = parseComparison(); // 改为先解析比较表达式
			
			while (match("&&")) {
				string op = consume().getToken().second;
				Expression* right = dynamic_cast<Expression*>(parseComparison()); // 右操作数也是比较表达式
				node = new Expression(Expression::BINARY_OPERATOR, op,
				                      dynamic_cast<Expression*>(node), right);
			}
			
			return node;
		}
		
		// 解析比较运算符（==, !=, >, <, >=, <=），优先级低于算术表达式，高于逻辑与
		ASTBaseNode* parseComparison() {
			ASTBaseNode* node = parseExpression(); // 先解析算术表达式（+ -）
			
			while (match("==") || match("!=") ||
			       match(">") || match("<") ||
			       match(">=") || match("<=")) {
				string op = consume().getToken().second;
				Expression* right = dynamic_cast<Expression*>(parseExpression()); // 右操作数也是算术表达式
				node = new Expression(Expression::BINARY_OPERATOR, op,
				                      dynamic_cast<Expression*>(node), right);
			}
			
			return node;
		}
		
		// 解析表达式（处理 + - 运算，优先级较低）
		ASTBaseNode* parseExpression() {
			ASTBaseNode* node = parseTerm(); // 先解析高优先级的项
			
			// 循环处理连续的 + 或 -
			while (match("+") || match("-")) {
				string op = consume().getToken().second;
				Expression* right = dynamic_cast<Expression*>(parseTerm());
				node = new Expression(Expression::BINARY_OPERATOR, op,
				                      dynamic_cast<Expression*>(node), right);
			}
			
			return node;
		}
		
		// 解析项（处理 * / 运算，优先级中等）
		ASTBaseNode* parseTerm() {
			ASTBaseNode* node = parseFactor(); // 先解析因子
			
			// 循环处理连续的 * 或 /
			while (match("*") || match("/")) {
				string op = consume().getToken().second;
				Expression* right = dynamic_cast<Expression*>(parseFactor());
				node = new Expression(Expression::BINARY_OPERATOR, op,
				                      dynamic_cast<Expression*>(node), right);
			}
			
			return node;
		}
		
		// 解析因子（处理原子表达式：字面量、标识符、函数调用、括号表达式）
		ASTBaseNode* parseFactor() {
			Token token = peek();
			auto [type, content] = token.getToken();
			
			// 处理单目逻辑非!，等级最高
			if (content == "!") {
				consume(); // 消耗!
				Expression* operand = dynamic_cast<Expression*>(parseFactor()); // 解析操作数
				return new Expression(Expression::BINARY_OPERATOR, "!", nullptr, operand);
			}
			
			// 处理括号表达式
			if (content == "(") {
				consume(); // 消耗 '('
				ASTBaseNode* expr = parseLogicalOr(); // 递归解析括号内的表达式（改为最后处理||）
				expect(")"); // 消耗 ')'
				return expr;
			}
			
			// 处理字面量
			if (type == Literals) {
				consume();
				return new Expression(Expression::LITERAL, content);
			}
			
			// 处理标识符或函数调用
			if (type == Identifiers) {
				consume();
				
				if (match("(")) {
					return parseFunctionCall(content); // 函数调用
				}
				
				return new Expression(Expression::IDENTIFIER, content); // 普通标识符
			}
			
			throw runtime_error("Unexpected token in factor: " + content);
		}
		
		ASTBaseNode* parseVariableDeclaration() {
			Token typeToken = expect("int");
			string varType = typeToken.getToken().second;
			Token nameToken = peek();
			auto [nameType, varName] = nameToken.getToken();
			
			if (nameType != Identifiers) {
				throw runtime_error("Expected identifier for variable name");
			}
			
			consume(); // 消耗变量名
			Expression* initExpr = nullptr; // 初始化表达式指针
			
			// 处理初始化（允许函数调用作为初始化表达式）
			if (match("=")) {
				consume(); // 消耗 '='
				ASTBaseNode* exprNode = parseLogicalOr(); // 最后处理||
				initExpr = dynamic_cast<Expression*>(exprNode);
				
				// 放宽检查条件，允许任何Expression类型（包括函数调用）
				if (!initExpr) {
					throw runtime_error("Invalid initializer expression in variable declaration");
				}
			}
			
			expect(";"); // 消耗分号
			// 将 initExpr 传入构造函数
			return new VariableDeclaration(varType, varName, initExpr);
		}
		
		// 解析if语句：if (condition) thenBlock [else elseBlock]
		ASTBaseNode* parseIfStatement() {
			consume(); // 消耗"if"关键词
			expect("("); // 解析左括号
			// 解析条件表达式（支持逻辑运算）
			ASTBaseNode* condNode = parseLogicalOr();
			Expression* condition = dynamic_cast<Expression*>(condNode);
			
			if (!condition) {
				throw runtime_error("Invalid condition in if statement");
			}
			
			expect(")"); // 解析右括号
			// 解析then分支（单语句或语句块）
			ASTBaseNode* thenBlock = parseStatement();
			
			if (!thenBlock) {
				throw runtime_error("Missing statement after if condition");
			}
			
			// 解析可选的else分支
			ASTBaseNode* elseBlock = nullptr;
			
			if (match("else")) {
				consume(); // 消耗"else"关键词
				elseBlock = parseStatement(); // else后的语句或语句块
			}
			
			return new IfStatement(condition, thenBlock, elseBlock);
		}
		
		// 在AST类中添加for循环解析函数
		ASTBaseNode* parseForStatement() {
			consume(); // 消耗"for"关键字
			expect("("); // 解析左括号
			// 解析初始化语句
			ASTBaseNode* initStmt = parseStatement();
			
			if (!initStmt) {
				throw runtime_error("Missing initialization statement in for loop");
			}
			
			// expect(";"); // 消耗分号（这里不用断言，因为在parseStatement中已经断言）
			// 解析条件表达式
			ASTBaseNode* condNode = parseLogicalOr();
			Expression* condition = dynamic_cast<Expression*>(condNode);
			
			if (!condition) {
				throw runtime_error("Invalid condition in for loop");
			}
			
			expect(";"); // 消耗分号（这里需要断言，用于跳过分隔符）
			// 解析更新语句
			ASTBaseNode* updateStmt = parseStatement();
			
			if (!updateStmt) {
				throw runtime_error("Missing update statement in for loop");
			}
			
			expect(")"); // 解析右括号
			// 解析循环体
			ASTBaseNode* body = parseStatement();
			
			if (!body) {
				throw runtime_error("Missing body in for loop");
			}
			
			// expect(";"); // 消耗分号（这里不用断言，因为在parseStatement中已经断言）
			return new ForStatement(initStmt, condition, updateStmt, body);
		}
		
		// 修改parseFunctionDeclaration函数(形如：func(int amint b))
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
			vector<pair<string, string >> params = parseParameters(); // 解析参数列表
			expect(")"); // 消耗右括号
			// 解析函数体
			ASTBaseNode* body = parseStatementBlock();
			FunctionDeclaration* func = new FunctionDeclaration(returnType, funcName, params);
			func->addChild(body);
			return func;
		}
		
		// 解析函数调用
		ASTBaseNode* parseFunctionCall(const string& funcName) {
			FunctionCall* call = new FunctionCall(funcName);
			expect("("); // 消耗左括号
			
			// 解析参数列表
			if (!match(")")) {
				do {
					ASTBaseNode* paramExpr = parseLogicalOr();//最后处理||
					Expression* param = dynamic_cast<Expression*>(paramExpr);
					
					if (!param) {
						throw runtime_error("Invalid parameter in function call");
					}
					
					call->addParameter(param);
					
				} while (match(",") && (consume(), true)); // 处理多个参数
			}
			
			expect(")"); // 消耗右括号
			return call; // 现在返回的是Expression子类
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
					ASTBaseNode* expr = parseLogicalOr();//最后处理||
					returnStmt->addChild(expr);
				}
				
				expect(";"); // 消耗分号
				return returnStmt;
			}
			
			// 处理if语句（新增）
			if (content == "if") {
				return parseIfStatement();
			}
			
			// 处理变量声明（以int关键字开头）
			if (content == "int" && peek(1).getToken().second != "(") {
				return parseVariableDeclaration();
			}
			
			// 处理for语句
			if (content == "for") {
				return parseForStatement();
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
			
			// 处理后置自增表达式 i++
			if (type == Identifiers && peek(1).getToken().second == "++") {
				string varName = content;
				consume(); // 消耗变量名
				consume(); // 消耗++
				expect(";"); // 自增语句以分号结束
				// 创建自增表达式节点
				Expression* varExpr = new Expression(Expression::IDENTIFIER, varName);
				return new Expression(Expression::UNARY_OPERATOR, "++", varExpr);
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
