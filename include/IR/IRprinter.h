#include<vector>
#include<iostream>
#include"./IRbase.h"
#include"./IR.h"

class IRprinter {
		vector<IRInstr> IR;
	public:
		IRprinter() {}
		
		IRprinter(const vector<IRInstr>& IR) {
			this->IR = IR;
		}
		
		void print() {
			for (IRInstr i : IR) {
				cout << IROpToString[i.op] << ": ";
				
				for (pair<string, string> j : i.label) {
					cout << "    " << j.first << " : " << j.second;
				}
				
				cout << endl;
			}
		}
		
		~IRprinter() {
		}
		
};
