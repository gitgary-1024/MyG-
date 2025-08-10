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
				// 在FunctionDeclaration的打印分支中添加
				case ASTBaseNode::FUNC_DECL: {
						auto* func = dynamic_cast<FunctionDeclaration*>(node);
						std::cout << "FunctionDeclaration: " << func->returnType << " " << func->funcName << "(";
						
						// 打印参数列表
						for (size_t i = 0; i < func->parameters.size(); ++i) {
							std::cout << func->parameters[i].first << " " << func->parameters[i].second;
							
							if (i != func->parameters.size() - 1) {
								std::cout << ", ";
							}
						}
						
						std::cout << ")" << std::endl;
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
						
						if (expr->exprType == Expression::LITERAL) {
							std::cout << "LiteralExpression: " << expr->value << std::endl;
						}
						else if (expr->exprType == Expression::IDENTIFIER) {
							std::cout << "IdentifierExpression: " << expr->value << std::endl;
						}
						else if (expr->exprType == Expression::BINARY_OPERATOR) {
							std::cout << "BinaryOperator: " << expr->value << std::endl;
							printNode(expr->left, depth + 1);
							printNode(expr->right, depth + 1);
						}
						else if (expr->exprType == Expression::FUNC_CALL) { // 新增函数调用打印
							auto* call = dynamic_cast<FunctionCall*>(expr);
							std::cout << "FunctionCall: " << call->funcName << "(";
							
							for (size_t i = 0; i < call->parameters.size(); ++i) {
								printNode(call->parameters[i], depth + 1);
								
								if (i != call->parameters.size() - 1) {
									std::cout << ", ";
								}
							}
							
							std::cout << ")" << std::endl;
						}
						else if (expr->exprType == Expression::UNARY_OPERATOR) {
							std::cout << "UnaryOperator: " << expr->value << std::endl;
							printIndent(depth + 1);
							std::cout << "Operand:" << std::endl;
							printNode(expr->operand, depth + 2);
						}
						
						break;
					}
					
				case ASTBaseNode::VAR_DECL: {
						auto* var = dynamic_cast<VariableDeclaration*>(node);
						std::cout << "VariableDeclaration: " << var->varType << " " << var->varName;
						
						// 打印初始化表达式（如果存在）
						if (var->initExpr) {
							std::cout << " = ";
							// 递归打印表达式
							printNode(var->initExpr, depth);
						}
						else {
							std::cout << std::endl;
						}
						
						break;
					}
					
				case ASTBaseNode::FUNC_CALL: {
						auto* call = dynamic_cast<FunctionCall*>(node);
						std::cout << "FunctionCall: " << call->funcName << "(";
						
						// 打印参数列表
						for (size_t i = 0; i < call->parameters.size(); ++i) {
							printNode(call->parameters[i], depth); // 直接打印参数表达式
							
							if (i != call->parameters.size() - 1) {
								std::cout << ", ";
							}
						}
						
						std::cout << ")" << std::endl;
						break;
					}
					
				case ASTBaseNode::IF_STATEMENT: {
						auto* ifStmt = dynamic_cast<IfStatement*>(node);
						std::cout << "IfStatement" << std::endl;
						// 打印条件表达式
						printIndent(depth + 1);
						std::cout << "Condition:" << std::endl;
						printNode(ifStmt->condition, depth + 2);
						// 打印then分支
						printIndent(depth + 1);
						std::cout << "ThenBlock:" << std::endl;
						printNode(ifStmt->thenBlock, depth + 2);
						
						// 打印else分支（如果存在）
						if (ifStmt->elseBlock) {
							printIndent(depth + 1);
							std::cout << "ElseBlock:" << std::endl;
							printNode(ifStmt->elseBlock, depth + 2);
						}
						
						break;
					}
					
				// 在printNode()函数中添加for循环处理
				case ASTBaseNode::FOR_STATEMENT: {
						auto* forStmt = dynamic_cast<ForStatement*>(node);
						std::cout << "ForStatement" << std::endl;
						// 打印初始化语句
						printIndent(depth + 1);
						std::cout << "Initializer:" << std::endl;
						printNode(forStmt->initStmt, depth + 2);
						// 打印条件表达式
						printIndent(depth + 1);
						std::cout << "Condition:" << std::endl;
						printNode(forStmt->condition, depth + 2);
						// 打印更新语句
						printIndent(depth + 1);
						std::cout << "Update:" << std::endl;
						printNode(forStmt->updateStmt, depth + 2);
						// 打印循环体
						printIndent(depth + 1);
						std::cout << "Body:" << std::endl;
						printNode(forStmt->body, depth + 2);
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
