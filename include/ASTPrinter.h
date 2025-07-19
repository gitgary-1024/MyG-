#include "./ASTnode.h"
#include <iostream>
#include <string>

class ASTPrinter {
	private:
		// 打印缩进
		void printIndent(int depth) {
			for (int i = 0; i < depth; ++i) {
				std::cout << "  ";
			}
		}
		
		// 递归打印节点
		void printNode(ASTBaseNode* node, int depth) {
			if (!node)
				return;
				
			printIndent(depth);
			
			switch (node->getNodeType()) {
				case ASTBaseNode::FUNC_DECL: {
						auto* func = dynamic_cast<FunctionDeclaration*>(node);
						std::cout << "FunctionDeclaration: " << func->returnType << " " << func->funcName << "()" << std::endl;
						break;
					}
					
				case ASTBaseNode::STMT_BLOCK: {
						std::cout << "StatementBlock" << std::endl;
						break;
					}
					
				case ASTBaseNode::STATEMENT: {
						auto* stmt = dynamic_cast<Statement*>(node);
						
						if (stmt->getStmtType() == Statement::RETURN) {
							std::cout << "ReturnStatement" << std::endl;
						}
						else {
							std::cout << "EmptyStatement" << std::endl;
						}
						
						break;
					}
					
				case ASTBaseNode::EXPRESSION: {
						auto* expr = dynamic_cast<Expression*>(node);
						
						if (expr->getExprType() == Expression::LITERAL) {
							std::cout << "LiteralExpression: " << expr->getValue() << std::endl;
						}
						else {
							std::cout << "IdentifierExpression: " << expr->getValue() << std::endl;
						}
						
						break;
					}
					
				case ASTBaseNode::VAR_DECL: {
						auto* var = dynamic_cast<VariableDeclaration*>(node);
						std::cout << "VariableDeclaration: " << var->varType << " " << var->varName << std::endl;
						break;
					}
					
				case ASTBaseNode::FUNC_CALL: {
						auto* call = dynamic_cast<FunctionCall*>(node);
						std::cout << "FunctionCall: " << call->funcName << "()" << std::endl;
						break;
					}
					
				default:
					std::cout << "UnknownNode" << std::endl;
			}
			
			// 递归打印子节点
			auto children = node->getAllChildren();
			
			for (auto child : children) {
				printNode(child, depth + 1);
			}
		}
		
	public:
		void printAST(ASTBaseNode* root) {
			std::cout << "===== AST Structure =====" << std::endl;
			printNode(root, 0);
			std::cout << "=========================" << std::endl;
		}
};
