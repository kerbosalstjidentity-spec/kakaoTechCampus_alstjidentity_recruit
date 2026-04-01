#include<iostream>
#include<vector>
#include<string>
#include<stdlib.h>
#include<time.h>
#include<regex>
using namespace std;
class node {
public:
	int node_number;
	char match;

	int counter_max = -1;     
	int counter_min = -1;     
	int act_count = 1;       

	bool transited = 0;    
	bool state = 0;        
	bool isTerminal = 0;   

	bool kleenestar = 0;  
	 
	vector<node*> next;

	int visited = 0;

	node(int num_state, char _match) {
		node_number = num_state;
		match = _match;
	}
	~node() {}
	void ini_cnt_max(int max_cnt) {
		counter_max = max_cnt;
	}
	void ini_cnt_min(int min_cnt) {
		counter_min = min_cnt;
	}
	void isKleenstar() {
		kleenestar = 1;
	}
	void isVisited() {
		visited = 1;
	}

	void addNode(node* _next) {
		next.push_back(_next);
	}

	node& transition() {
		transited = 1;
		return *this;
	}
	void activation() {
		state = transited;
		transited = 0;
	}
	bool define_Terminal(node* cur) {
		int me = 0, sz = 0;
		for (int i = 0; i < cur->next.size(); i++) {
			if (cur->next[i] == cur || cur->next[i]->match == '3') me++;
			sz++;
		}
		if (sz - me <= 0) {
			return true;
		}
		else return false;
	}

	pair<int, int> traverse_auto(node* cur, vector<node*>& st_machine) {
		int iam_star = 0, kleen_starTerm = 0;
		if (cur->kleenestar == 1) { iam_star = 1; }
		cur->isVisited();
		st_machine[cur->node_number] = cur;
		if (define_Terminal(cur)) {
			cur->isTerminal = 1;
			if (cur->kleenestar == true) kleen_starTerm = 1;
		}
		for (int i = 0; i < cur->next.size(); i++) {
			if (cur->next[i]->visited != 1) {
				pair<int, int> k_k = traverse_auto(cur->next[i], st_machine);
				if (k_k.first == 1 && k_k.second == 1) kleen_starTerm = 1;
			}
		}
		if (kleen_starTerm == 1) cur->isTerminal = 1;
		return { iam_star,kleen_starTerm };
	}

	void traverse_transit(node* present) {
		present->state = 1;
		for (int i = 0; i < present->next.size(); i++) {
			node* nextKleenstar = present->next[i];
			if (nextKleenstar->kleenestar == 1) {
				if (nextKleenstar->state != 1)
					traverse_transit(nextKleenstar);
			}
			nextKleenstar->state = 1;
		}
	}

	int input(const char& ch, string& input, int idx, vector<node*>machine) {
		int success = 0;
		node* current = this; 
		if (state && (match != ch)) {
			if (kleenestar == 1) {
				node* isNextKleenstar = this;
				traverse_transit(isNextKleenstar);
			}
		}

		if (state && (match == ch || match == '.' || match == '5')) {
			//cout << "  >>>> accepted by " << node_number << endl;
			if (isTerminal && idx == input.length() - 1) {
				if (this->counter_max == -1 || this->act_count <= this->counter_max) {
					if (this->counter_min == -1 || this->act_count >= this->counter_min)
						//cout << "this statement is accepted as regular expression" << endl;
						success = 1;
				}	
			}
			for (int i = 0; i < next.size(); i++) {
				if (next[i] == this) {
					if (next[i]->counter_min != -1 && next[i]->act_count < next[i]->counter_min) {
						next[i]->act_count++;
						next[i]->transition();
						break;
					}
					else if (next[i]->counter_max != -1 && next[i]->act_count >= next[i]->counter_max) {
						continue;
					}
					else next[i]->act_count++;
				}
				next[i]->transition();
			}
		}
		state = 0; 
		return success;
	}
};
int initial_trie(int idx, string& regex, node* cur_node) {
	if (idx == regex.size() - 1) return idx + 1;
	else {
		if (regex[idx + 1] == '*') {
			cur_node->ini_cnt_min(0);
			cur_node->isKleenstar();
			cur_node->next.push_back(cur_node);
			idx += 2;
		}
		else if (regex[idx + 1] == '+') {
			cur_node->ini_cnt_min(1);
			cur_node->next.push_back(cur_node);
			idx += 2;
		}
		else if (regex[idx + 1] == '{') {
			cur_node->ini_cnt_min(regex[idx + 2] - '0');
			cur_node->ini_cnt_max(regex[idx + 4] - '0');
			cur_node->next.push_back(cur_node);
			idx += 6;  
		}
		else idx += 1;
	}
	return idx;
}

int regex_test(string test, node* cur, vector<node*>& state_machine) {
	int regex_success = 0;
	size_t sz = test.size();
	//cout << "test for '" << test << "'" << endl;

	for (int i = 0; i <= test.length(); i++) {
		if (i == 0) {
			cur->transition().activation();
			regex_success=cur->input('5', test, -1, state_machine);
		}
		else {
			//cout << " >> input: " << test[i - 1] << endl;
			for (int j = 1; j < state_machine.size(); j++) {
				regex_success=state_machine[j]->input(test[i - 1], test, i - 1, state_machine);
			}
		}
		for (int j = 1; j < state_machine.size(); j++)
			state_machine[j]->activation();
	}
	for (int k = 0; k < state_machine.size(); k++) {
		state_machine[k]->act_count = 1;
		state_machine[k]->state = 0;
		state_machine[k]->transited = 0;
	}
	//cout << endl;
	return regex_success;
}

///should be expired at the end //
//정규표현식을 무작위로 생성하는 코드입니다.
void generate_regex_s(string& regex_s) {
	srand((unsigned)time(NULL));
	string regex_str;
	char regex_char[6] = {'1','.','*','+','{','|'};
	int regex_length = rand() % 11 + 1;
	for (int i = 0; i < regex_length;i++) {
		int rand_index;
		if (i == 0) rand_index = rand() % 2;
		else if (i >= 1) {
			int is_operator = 1;
			regex_str[i - 1] == '*' ? is_operator = 1 :
				regex_str[i - 1] == '+' ? is_operator = 1 :
				regex_str[i - 1] == '}' ? is_operator = 1 :
				regex_str[i - 1] == '|' ? is_operator = 1 : is_operator = 0;
			if (is_operator == 1) rand_index = rand() % 2;
			else rand_index = rand() % 6;
		}
		///#################################33
		if (regex_char[rand_index] == '1') {
			regex_str += char(97 + rand() % 26);
		}
		else if (regex_char[rand_index] == '{') {
			int low, high;
			low = rand() % 6 + 1; high = rand() % 6 + 1;
			if (low > high) { int temp = low; low = high; high = temp; } //swap;
			string str_low=to_string(low), str_high=to_string(high);
			regex_str += "{"+str_low+","+str_high+"}";
			i += 4;
		}
		else regex_str += regex_char[rand_index];
	}
	regex_s = regex_str;
}
///should be expired at the end //
//입력식을 무작위로 생성하는 코드입니다.!!!
string generate_test() {
	int regex_length = rand() % 15 + 1;
	string regex_test;
	for (int i = 0; i < regex_length; i++) {
		regex_test += char(97 + rand() % 26);
	}
	return regex_test;
}
int main() {
	for (int i = 0; i < 1; i++) {
		string regex_s;
		//cout << "정규표현식을 생성해주세요!!: ";
		//cout << "(. , * , + , {m,n}만 가능!!!)" << endl;
		//cin >> regex_s;
		generate_regex_s(regex_s);

		node* st_epsiolon = new node(0, '5');
		node* ptr_root;
		node* ptr_prev, * has_orEpsilon = NULL;
		ptr_root = ptr_prev = st_epsiolon;

		//cout << "상태머신 생성 중....." << endl;
		int cnt_node = 1, or_state = -1;
		for (int i = 0; i < regex_s.size();) {
			node* new_node;
			if (or_state == i) {
				or_state = -1;
				new_node = ptr_prev;
				new_node->match = regex_s[i];
			}
			else {
				new_node = new node(cnt_node, regex_s[i]);
				cnt_node++;
				if (has_orEpsilon != NULL) {
					new_node->next.push_back(has_orEpsilon);
				}
				ptr_prev->addNode(new_node);
			}
			i = initial_trie(i, regex_s, new_node);
			if (i < regex_s.size()) {
				if (regex_s[i] == '|') {
					if (has_orEpsilon == NULL) {
						has_orEpsilon = new node(cnt_node, '3');
						cnt_node += 1;
					}
					new_node->addNode(has_orEpsilon);
					i += 1;
				}
				else {
					if (has_orEpsilon != NULL) {
						ptr_prev = has_orEpsilon;
						has_orEpsilon = NULL;
						or_state = i;
					}
					else ptr_prev = new_node;
				}
			}
		}

		vector<node*> state_machine(cnt_node, NULL);
		ptr_root->traverse_auto(st_epsiolon, state_machine);

		/*for (int i = 0; i < state_machine.size() - 1; i++) {
			cout << "s[" << state_machine[i]->node_number << "]:" << state_machine[i]->match << endl;
			for (int j = 0; j < state_machine[i]->next.size(); j++) {
				cout << "s[" << state_machine[i]->node_number << "]-------->";
				cout << "s[" << state_machine[i]->next[j]->node_number << "]: " << state_machine[i]->next[j]->match << endl;
			}
		}*/
		
		cout << "generate regular expression: " << regex_s<<endl;
		int count_false = 0, count_true = 0;
		for (int tc = 0; tc < 100000; tc++) {
			string test_str= generate_test();
			int stme_test=regex_test(test_str, ptr_root, state_machine);
			if (stme_test == 0&& count_false<=50) {
				cout << "wrong 케이스:" << test_str << "결과:" << stme_test << endl;
				count_false++;
			}
			else if(stme_test == 1&& count_true <= 50 && count_false>50){
				cout << "right 케이스:" << test_str << "결과:" << stme_test << endl;
				count_true++;
			}  //100000 번동안 특정 정규표현식에 대해 wrong testcase 와 right testcase를 출력해주는 코드입니다.
		}
		for (int i = 1; i < state_machine.size(); i++) {
			delete state_machine[i];
		}
	}
	return 0;
}


