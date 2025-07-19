#ifndef ASTnode_H
#define ASTnode_H

#include<vector>
#include<string>
#include<unordered_map>
#include"./Token.h"
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
			FUNC_CALL
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

class Expression: public ASTBaseNode {
	public:
		enum ExprType { LITERAL, IDENTIFIER };
		ExprType exprType;
		string value;
		
		Expression(ExprType type, const string& val) : exprType(type), value(val) {
			nodeType = EXPRESSION;
		}
		
		ExprType getExprType() {
			return exprType;
		}
		
		string getValue() {
			return value;
		}
		
		~Expression() {}
};

class VariableDeclaration: public ASTBaseNode {
	public:
		string varType;
		string varName;
		
		VariableDeclaration(const string& type, const string& name)
			: varType(type), varName(name) {
			nodeType = VAR_DECL;
		}
		
		~VariableDeclaration() {
			nodeType = VAR_DECL;
		}
};

class FunctionDeclaration: public ASTBaseNode {
	public:
		string returnType;
		string funcName;
		
		FunctionDeclaration(const string& retType, const string& name)
			: returnType(retType), funcName(name) {
			nodeType = FUNC_DECL;
		}
		
		~FunctionDeclaration() {
		}
};

class FunctionCall: public ASTBaseNode {
	public:
		string funcName;
		
		FunctionCall(const string& name) : funcName(name) {
			nodeType = FUNC_CALL;
		}
		
		~FunctionCall() {}
};

#endif /*ASTnode_H*/
