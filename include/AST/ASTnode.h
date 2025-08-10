#ifndef ASTnode_H
#define ASTnode_H

#include<vector>
#include<string>
#include<unordered_map>
#include"../Token.h"
using namespace std;

#ifndef NULL_ATTRIBUTE_VALUE
	#define NULL_ATTRIBUTE_VALUE "NULLATTRIBUTEVALUE"
#endif

class ASTBaseNode {
	public:
		enum NodeType {
			BASE,
			STATEMENT,
			STMT_BLOCK,
			EXPRESSION,
			VAR_DECL,
			FUNC_DECL,
			FUNC_CALL,
			IF_STATEMENT,
			FOR_STATEMENT
		};
	protected:
		vector<ASTBaseNode*> children;
		unordered_map<string, string> attribute;
		NodeType nodeType;
	public:
	
		ASTBaseNode(): nodeType(BASE) {}
		
		virtual ~ASTBaseNode() {
			for (auto child : children) {
				delete child;
			}
		}
		
		void addChild(ASTBaseNode* child) {
			children.push_back(child);
		}
		
		void addAttribute(string& attributeName, string& attributeNum) {
			attribute[attributeName] = attributeNum;
		}
		
		string getAttribute(string& attributeName) {
			if (attribute.find(attributeName) != attribute.end()) {
				return attribute[attributeName];
			}
			
			return NULL_ATTRIBUTE_VALUE;
		}
		
		vector<ASTBaseNode*> getAllChildren() {
			return children;
		}
		
		NodeType getNodeType() {
			return nodeType;
		}
};

class Statement: public ASTBaseNode {
	public:
		enum StmtType { RETURN, EMPTY };
		StmtType stmtType;
		
		Statement(StmtType type) : stmtType(type) {
			nodeType = STATEMENT;
		}
		
		StmtType getStmtType() {
			return stmtType;
		}
		
		~Statement() {}
};

class StatementBlock: public ASTBaseNode {
	public:
		StatementBlock() {
			nodeType = STMT_BLOCK;
		}
		
		~StatementBlock() {}
};

// 在ASTnode.h的Expression类中添加
class Expression: public ASTBaseNode {
	public:
		enum ExprType { LITERAL,
		                IDENTIFIER,
		                BINARY_OPERATOR,// 支持算术运算符、逻辑运算符（+、-、*、/、&&、||等）
		                FUNC_CALL,
		                UNARY_OPERATOR
		              };
		ExprType exprType;
		string value; // 用于字面量、标识符或运算符
		Expression* operand; // 新增：单目运算符的操作数（如自增的变量）
		Expression* left; // 左操作数（二元运算时）
		Expression* right; // 右操作数（二元运算时）
		
		// 构造函数：字面量/标识符
		Expression(ExprType type, const string& val)
			: exprType(type), value(val), left(nullptr), right(nullptr) {
			nodeType = EXPRESSION;
		}
		
		// 构造函数：二元运算符
		Expression(ExprType type, const string& op, Expression* l, Expression* r)
			: exprType(type), value(op), left(l), right(r) {
			nodeType = EXPRESSION;
		}
		
		// 新增：单目运算符构造函数（自增等）
		Expression(ExprType type, const string& op, Expression* opnd)
			: exprType(type), value(op), operand(opnd), left(nullptr), right(nullptr) {
			nodeType = EXPRESSION;
		}
		
		~Expression() {
			delete left;
			delete right;
			delete operand; // 释放单目操作数
		}
};

class VariableDeclaration: public ASTBaseNode {
	public:
		string varType;
		string varName;
		Expression* initExpr; // 新增：存储初始化表达式
		
		VariableDeclaration(const string& type, const string& name, Expression* init = nullptr)
			: varType(type), varName(name), initExpr(init) {
			nodeType = VAR_DECL;
		}
		
		~VariableDeclaration() {
			delete initExpr; // 释放初始化表达式
		}
};

// 在FunctionDeclaration类中添加参数存储
class FunctionDeclaration: public ASTBaseNode {
	public:
		string returnType;
		string funcName;
		vector<pair<string, string >> parameters; // 新增：存储参数类型和名称
		
		FunctionDeclaration(const string& retType, const string& name,
		                    const vector<pair<string, string >>& params)
			: returnType(retType), funcName(name), parameters(params) {
			nodeType = FUNC_DECL;
		}
		
		~FunctionDeclaration() {}
};

class FunctionCall: public Expression {
	public:
		string funcName;
		vector<Expression*> parameters; // 新增：存储函数调用参数
		
		FunctionCall(const string& name)
			: Expression(Expression::FUNC_CALL, name), funcName(name) { // 使用新的表达式类型
		}
		
		// 新增：添加参数方法
		void addParameter(Expression* param) {
			parameters.push_back(param);
		}
		
		~FunctionCall() {
			// 释放参数表达式
			for (auto param : parameters) {
				delete param;
			}
		}
};

class IfStatement : public ASTBaseNode {
	public:
		Expression* condition; // if条件表达式
		ASTBaseNode* thenBlock; // 条件为真时的语句块
		ASTBaseNode* elseBlock; // 可选的else语句块（可为nullptr）
		
		IfStatement(Expression* cond, ASTBaseNode* thenStmt, ASTBaseNode* elseStmt = nullptr)
			: condition(cond), thenBlock(thenStmt), elseBlock(elseStmt) {
			nodeType = IF_STATEMENT; // 需要在NodeType中添加枚举值
		}
		
		~IfStatement() {
			delete condition;
			delete thenBlock;
			delete elseBlock;
		}
};

// 添加ForStatement类
class ForStatement : public ASTBaseNode {
	public:
		ASTBaseNode* initStmt; // 初始化语句
		Expression* condition; // 条件表达式
		ASTBaseNode* updateStmt; // 更新语句
		ASTBaseNode* body; // 循环体
		
		ForStatement(ASTBaseNode* init, Expression* cond, ASTBaseNode* update, ASTBaseNode* b)
			: initStmt(init), condition(cond), updateStmt(update), body(b) {
			nodeType = FOR_STATEMENT;
		}
		
		~ForStatement() {
			delete initStmt;
			delete condition;
			delete updateStmt;
			delete body;
		}
};

#endif /*ASTnode_H*/
