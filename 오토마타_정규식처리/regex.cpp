#include<iostream>
#include<queue>
#include<stack>
#include<vector>
#include<string>
using namespace std;
class node {
public:
	int node_number; //번호
	char match; // 정규표현식에 포함된 문자

	int counter_max = -1;     //최대 반복
	int counter_min = -1;     //최소 반복
	int act_count = 1;       //반복된 횟수
	///특정 노드에 얼마나 많이 머물 수 있는지를 반영했습니다.
	/// 예를 들어 A{1,3} 이면 node A counter_min=1, node A counter_max=3
	

	bool transited = 0;    //전이가 되었는가?
	bool state = 0;        //현재 활성화 상태인가?
	bool isTerminal = 0;   //단말노드인가 
	/// 단말 노드의 경우 해당 상태머신의 종단에 도달했다는 것을 의미합니다.
	/// 다음 두가지 조건을 만족하면 프로그래밍을 통해 구현한 '상태머신'이 
	/// 입력된 '정규식'을 받아들인다고 할 수 있겠습니다.
	/// 1. 입력식을 끝까지 검색한 경우(즉 중간에 단말노드에 도달했다고 해당식을 받아들인 것이 아닙니다.
	/// 2. 입력식을 끝까지 검색했을 때, 단말노드가 활성화되어있을 경우
	/// 

	bool kleenestar = 0;  // '*'은 정규식 예외처리에서 조금 까다롭기에 설정한 맴버 변수입니다.
	
	vector<node*> next; //노드의 다음노드를 저장하는 vector 컨테이너 입니다.

	int visited = 0;  //연속적(마치 통로처럼) 단말노드를 찾기위한 dfs 탐색 시 필요한 맴버변수입니다.
	// 하위 traverse_auto()를 참고하면서 설명드리겠습니다.
	
	node(int num_state, char _match) {
		node_number = num_state;   //몇번째 노드인지를 명시
		match = _match;  //노드의 문자를 저장
	}  
	~node() {}  //동적으로 할당된 노드를 해제할 때 사용합니다.
	
	void ini_cnt_max(int max_cnt) {
		counter_max = max_cnt;
	}  //최대 한계 결정
	void ini_cnt_min(int min_cnt) {
		counter_min = min_cnt;
	}  //최소 한계를 결정
	
	void isKleenstar() {
		kleenestar = 1;
	} //이 맴버함수가 호출되면 해당노드는 '*'을 가지고 있는 노드입니다.
	
	void isVisited() {
		visited = 1;
	} // traverse_auto() dfs 순회 중 방문한 노드를 기록하는 함수입니다.

	void addNode(node* _next) {
		next.push_back(_next);
	}  // 맴버 함수가 호출되면 다음노드를 실제로 vector 클래스에 저장됩니다.

	node& transition() {
		transited = 1;
		return *this;
	}  //전이가 된 상태는 입력식의 다음 문자를 확인할 때 검사 후보군에 포함됩니다.
	void activation() {
		state = transited;
		transited = 0;
	} //transition(검사 후보군)->activation(실제 검사할 노드)
	bool define_Terminal(node* cur) {
		int me=0,sz=0; //me:다음 노드중에 자기자신이 있는 경우 , sz:다음 노드의 총개수
		for (int i = 0; i < cur->next.size(); i++) {
			// 먼저 '3'은 상태머신에서 단말 epsilon 입니다.(제가 임의로 설정한 개념입니다.)
			//후술하겠지만 반대로 '5'는 시작 epsilon 입니다.(제가 임의로 설정한 개념입니다.)
			//epsilon값은 실제로 있는 노드는 아니고 단지 '상태머신'을 원할하게 설계하기 위한 
			//도구임에 지나지 않는 것을 명시하겠습니다.
			if (cur->next[i] == cur||cur->next[i]->match=='3') me++;
			//만약 다음노드에 자기 자신이 있거나 의미없는 단말epsilon이 있다면 me를 증가합니다.
			sz++;  // 조건에 상관없이 전체노드의 개수를 셉니다.
		}
		if (sz - me <=0) { // 실제 자신의 뒤에 연결된 다른 노드의 개수를 의미합니다.
			//만약 이 값이 0보다 작으면 그것은 자신을 제외한 다음노드가 없는 것을 의미하므로 
			//상태머신에서 단말 노드임이 확정됩니다.
			return true;
		}
		else return false;
	}

	pair<int,int> traverse_auto(node* cur,vector<node*>&st_machine) {
		///이 함수는 상태머신을 생성하고 실제로 어떤 노드가 단말노드인지를 확인하기 위해
		// 순회를 통해 해당 여부를 각각의 노드에 저장하는 기능을 수행합니다.
		int iam_star = 0, kleen_starTerm=0;
		if (cur->kleenestar == 1) { iam_star = 1; }
		//중요!: iam_star=1 즉, 현재 노드가 '*'을 가지고 있는 상태입니다
		//'*'는 0번 이상 반복임을 의미하기 때문에 다르게 말하면 이 노드를 무시하고 
		//다음노드로 전이가 일어날 수 있음을 의미합니다.
		//즉 '*' 노드는 어떻게 보면 '무조건 전이'가 일어난다고 할 수 있겠습니다.
		
		cur->isVisited();//현재노드를 방문했음을 기록
		st_machine[cur->node_number] = cur; //state_machine의 노드의 정보를 기록합니다.
		if (define_Terminal(cur)) {
			cur->isTerminal = 1; //단말노드입니다.
			if (cur->kleenestar == true) kleen_starTerm = 1;
			//중요: 만약 노드가 '*'이면서 단말 노드이면?? 이론상 다음과 같은 상황이 가능해집니다.
			// 일반 노드 0 -> * 노드1 -> * 노드2 -> * 노드3 (-> 단말 epsilon은 마지막에 있을 수도 없을 수도 있습니다.)
			//이떄 단말 노드는 몇개가 될까요? 제가 이해한 바로는 '*' 노드1,2,3 모두 단말 노드가 될 수있습니다.
			//일반적으로 '*'은 무조건 전이가 일어나니깐요.... 
			//또한  잘 생각해보면 결국 '일반 노드 0' 또한 단말노드에 편입될 수있음을 확인할 수 있습니다.
		}
		for (int i = 0; i < cur->next.size(); i++) {
			if (cur->next[i]->visited != 1) { //dfs 순회 원칙에 따라 중복방문을 허락하지 않습니다.
				pair<int, int> k_k = traverse_auto(cur->next[i], st_machine);
				if (k_k.first == 1 && k_k.second == 1) kleen_starTerm = 1;
				//만약 현재 노드에 다음노드가 단말노드이면서 '*' 그렇다면 현재노드는 적어도 
				//단말 노드인 것임을 보장받을 수 있습니다. 
			}
		}
		if (kleen_starTerm == 1) cur->isTerminal = 1;  //단말 노드임을 기록!!
		return { iam_star,kleen_starTerm }; //다만 현재 노드가 '*'이 아니라면 그 이전노드는 
		// 단말 노드가 될 가능성이 지워집니다. ^^ 
		//그래서 if(k_k.first == 1 && k_k.second == 1 ) 인 것입니다. &&을 주목해주세요.
	}

	void traverse_transit(node* present) {
		// 이 함수는 조금 독특합니다. 일단 이 함수가 만들어진 사고의 과정은 다음과 같습니다.
		// 일단 '*' 노드라는 개념을 통해 노드는 연속적인 통로를 형성할 수있음을 알 수있다.
		//그렇지만 protype코드에서 전이는 오직 1단계밖에 일어나지 않는다. 
		// 만약 이럴경우 transition->activation 과정에서 활성화되어야 할 연속적인 통로가 활성화 되지 않는다.
		// 그렇다면 이것을 해결하기 위한 함수를 작성해보자!!!
		present->state = 1;
		for (int i = 0; i < present->next.size(); i++) {
			node *nextKleenstar=present->next[i];
			if (nextKleenstar->kleenestar== 1) { //만약 다음노드가 '*'이면 
				if (nextKleenstar->state != 1)
					traverse_transit(nextKleenstar); //해당 노드를 활성화 해준다.
				    //dfs를 통해 조건에 부합하는 모든 노드를 활성화 해줍니다.
			}
			nextKleenstar->state = 1; 
			// 중요! 어쨌든 방문중인 현재 노드 또한 전이를 해주어야 합니다.
			//'*'이 아니더라도 그 이전 노드가 '*'인 이상 (이래서 dfs를 활용하는 것입니다!!!)
			// 반드시 전이가 되기 때문입니다.
		}
	}

  void input(const char& ch,string& input,int idx,vector<node*>machine) {
	  //이 부분은 실제로 입력식의 현재 문자와 상태머신의 현재 노드를 비교하는 함수입니다.
	  if (state && (match != ch)) {
		  if (kleenestar == 1) {  
			  node* isNextKleenstar = this;
			  traverse_transit(isNextKleenstar);
			  //만약 입력식의 문자와 현재 문자가 맞지 않더라도 
			  //'*'노드 이면 무조건 전이가 일어날 수 있다고 언급했습니다.
		  }
	  }
		if (state && (match == ch||match=='.'||match=='5')) {
			//조건이 특이합니다. 일단 활성화되어있는 노드만 검사해야하는 것은 맞는데...
			//1. 문자가 동일하다(match == ch)
			//2. match=='.'와 match=='5'는 ?
			//먼저 '.' 어떤 문자가 와도 상관이 없으므로 설령 match != ch 여도 처리가 되어야 합니다.
			//'5'는 '시작 epsilon' 입니다. 실제노드가 아니지만 설정상 무조건 전이가 일어나므로 추가 해주었습니다.
			
			cout << "  >>>> accepted by " << node_number << endl;
			//* 정규상태머신이 해당 입력식을 받아들일 수 있는지 확인하는 로직입니다.*/
			if (isTerminal && idx == input.length()-1) {
				// terminal이고, input(검증하고자 하는 입력시)이 끝에 도달 했는가?
				if (this->counter_max == -1 || this->act_count <= this->counter_max) {
				    if(this->counter_min==-1 || this->act_count >=this->counter_min)
					cout << "this statement is accepted as regular expression" << endl;
				}	
				//중요!: 사실 이부분은 {a,b}, '*', '+'때문에 추가 되었습니다. 
				//먼저 -1의 경우 '조건이 없다' 입니다. (제한된 max,min값이 없다!!)
			    //결국 해당 노드를 방문하더라도 방문해야 되는 횟수의 하한선과 상한선 안에서 
				//방문횟수가 일어나야  '상태머신'이 해당 입력식을 받아들일 수 있다는 것입니다.
			}
            for (int i = 0; i < next.size(); i++) {
				//전이가 일어나는 조건에도 추가적인 코드가 필요하다는 것을 느꼈습니다.
				//바로 상한선과 하한선의 제한때문인데요...자기자신을 다시 전이해야 되는 경우를 살펴보겠습니다.
				if (next[i] == this) {
					if (next[i]->counter_min != -1 && next[i]->act_count < next[i]->counter_min) {
						next[i]->act_count++;
						next[i]->transition();
						break; //하한선의 제한이 있는 상태에서 아직 하한선 미만으로 자기자신을 방문했으면
						//다음노드는 쳐다보지말고 자기 자신으로만의 전이를 해야합니다.!!!
					}
					else if (next[i]->counter_max != -1 && next[i]->act_count >= next[i]->counter_max) {
						continue; 
						//반대로 상한선을 초과하여 자기자신에 대한 방문을 시도하게 되면 
					    //자기 자신으로의 전이를 막게됩니다.
					}  
					
					else next[i]->act_count++;
					// 위 두가지의 경우가 아니라면 자기 자신의 방문횟수만 증가시켜 줍니다.
				}
				next[i]->transition();
				//만약 자기자신을 방문하는 것이 아니면 전이만 일어나면 되겠군요 ^^
			   } 
	       }
	     state = 0; // deactivate this node after matching '현재 상태는 비활성화 해주기!!'
		 //(하지만 자기자신으로 전이가 일어났다면 다시 활성화 될 수 있습니다.(코드 논리상 중요하지는 않아요!!)
	}
};
int initial_trie(int idx, string& regex, node* cur_node) {
	//상태머신을 초기설계하는 코드입니다.
	
	//idx는 항상 alpahbet을 가리키거나 '|'을 가리키도록 하자
	// 단위노드로 분리하여 설계하는 과정을 담은 코드입니다.
	if (idx == regex.size() - 1) return idx+1;
	else {
		if (regex[idx + 1] == '*') {
			cur_node->ini_cnt_min(0); //최소 반복횟수는 0번
			cur_node->isKleenstar();  //'*'
			cur_node->next.push_back(cur_node); //자기자신이 반복가능하므로 
			idx += 2;//연산자 그룹이 2번연속 올리가 없음
			//ex.) a*
		}
		else if (regex[idx + 1] == '+') {
			cur_node->ini_cnt_min(1);  //최소 반복횟수는 1번
			cur_node->next.push_back(cur_node); //자기 자신이 반복가능하므로
			idx += 2;//연산자 그룹이 2번연속 올릴이 없음
			//ex.) a+
		}
		else if (regex[idx + 1] == '{') {
			cur_node->ini_cnt_min(regex[idx + 2] - '0');
			cur_node->ini_cnt_max(regex[idx + 4] - '0');
			cur_node->next.push_back(cur_node);
			idx += 6;  //연산자 그룹이 2번연속 올릴이 없음
			//ex.) {1,3} 
		}
		else idx += 1;  
	}
	//여기서 알파벳으로 끝났거나 '|'로 끝남!!
	return idx;
}

void regex_test(string test,node* cur,vector<node*>&state_machine) {
	size_t sz = test.size();
	cout << "test for '" << test << "'" << endl;
	
	for (int i = 0; i <= test.length(); i++) {
		if (i == 0) {
			cur->transition().activation(); //시작 epsilon activation
			//시작 epsilon은 항상 활성화 되어있어야 하는게 맞는데
			// 코드의 논리상 하지 않았습니다.(최초 한번만 활성화 되게끔..)
			cur->input('5', test, -1, state_machine); 
			//상태머신의  0번째 노드 -> -1번째 문자
			//상태머신의 1번쨰 노드 ->  0번쨰 문자.....
		}
		else {
			cout << " >> input: " << test[i-1] << endl;
			for (int j = 1; j < state_machine.size(); j++) {
				state_machine[j]->input(test[i-1], test, i-1, state_machine);
			}
		}
		for (int j = 1; j < state_machine.size(); j++)
			state_machine[j]->activation();
	}
	for (int k = 0; k < state_machine.size(); k++) {
		state_machine[k]->act_count=1;
		state_machine[k]->state = 0;
		state_machine[k]->transited = 0;
	}//TestCase가 여러개이므로 다음과 같은 상태머신의 초기화 조건을 추가 했습니다.

	cout << endl;
}

int main() {
	string regex_s;
	cout << "정규표현식을 생성해주세요!!: ";
	cout << "(. , * , + , {m,n}만 가능!!!)"<<endl;
	cin >> regex_s;  
	//정규식을 입력하면 됩니다.
		
	node* st_epsiolon= new node(0,'5'); 
	//시작 epsilon 값 설정
	
	node* ptr_root; //시작 epsilon을 가리킴
	node* ptr_prev,*has_orEpsilon=NULL;
	ptr_root = ptr_prev = st_epsiolon;
    //has_orEpsilon:현재 노드가 다음노드로 단말 epsilon을 가지고 있는가?
    //사실 단말 epsilon은 초기 설정은 '3'이지만 (가상노드로 취급, 형태만 있음)
	//만약 실제 노드가 되는 상황이라면 '3'이 다른 문자로 바뀌게 됩니다!(실제 노드로 취급)
    
	cout << "상태머신 생성 중....."<<endl;
    int cnt_node = 1,or_state = -1;
    for (int i = 0; i < regex_s.size();) {
		node* new_node;
		if (or_state == i) {  
			 or_state = -1;
			 new_node = ptr_prev; //현재노드는 새로 만든게 아닙니다.
			 //기존 단말 epsilon을 실제노드로 취급한 것 뿐입니다.

			 //*or_state는 'ε'을 다른 문자로 치환해줍니다(알파벳) 
			 //자세한 것 해당부분에서 후술하겠습니다.
			 new_node->match = regex_s[i];
		}
		else {
		new_node=new node(cnt_node,regex_s[i]);
		cnt_node++;
		//반대로 단말 epsilon에서 치환된게 아니라면
		//노드의 개수는 증가 되겠습니다!
		if (has_orEpsilon != NULL) {
			new_node->next.push_back(has_orEpsilon);
		}
		ptr_prev->addNode(new_node);//prev와 current연결
		}
		i=initial_trie(i,regex_s,new_node); 
		//현재 노드의 작업인덱스로 i는 알파벳  또는 연산자 '|'를 가리킴
		if (i < regex_s.size()) {
			if (regex_s[i] == '|') {  //중요!: 만약 '|' 나온다면 
				//특정 노드의 이전노드가 여러개가 되는 경우가 있습니다!!!!
				//이런경우 '|' 의 개수만큼 비례하게 여러 노드를 만드는게 아니라 
				//임의의 단말 epsilon노드를 만드는 것입니다.
				//ex. ab|c|d(ε) 
				//      b
				//  ↗     ↘
				//a ➝  c  ➝  ε(ε는 단말노드로 머물 수도 있고, 새로운 실존노드가 될수도 있습니다.
				//  ↘     ↗
				//      d
				if (has_orEpsilon == NULL) {
					has_orEpsilon = new node(cnt_node,'3');
					cnt_node += 1; //잠재적으로 노드의 개수가 추가된 것이다. 
				}
				new_node->addNode(has_orEpsilon);
				i += 1;  //'|'의 다음을 가리킵니다.
			}
			else {
				if (has_orEpsilon != NULL) {
					ptr_prev = has_orEpsilon;
					has_orEpsilon = NULL;
					or_state = i;  //인덱스를 기록해줍니다.
				} //상태머신 설계시 알파벳이 확정되었는데 ptr_prev가 'ε'이면 노드를 만드는 것이 아니라
				// 다음 턴에 'ε'를 new alphabet으로 치환한다.
				
				else ptr_prev = new_node; //항상 ptr_prev는 현재노드를 가리키게 만든다.
				//다음노드 기준 이전노드가 된다.
			}
		}
	}
	
	vector<node*> state_machine(cnt_node,NULL); //정규식 처리기 상태를 저장할 클래스를 지정
	ptr_root->traverse_auto(st_epsiolon,state_machine);  //traverse_auto()를 통해 필요한 작업을 수행합니다.
	
	for (int i = 0; i < state_machine.size()-1; i++) {
		cout << "s[" << state_machine[i]->node_number << "]:" << state_machine[i]->match<<endl;
		for (int j = 0; j < state_machine[i]->next.size(); j++) {
			cout << "s[" << state_machine[i]->node_number << "]-------->";
			cout << "s[" << state_machine[i]->next[j]->node_number << "]: " << state_machine[i]->next[j]->match << endl;
		}
	}//현재 정규표현식의 상태를 출력하는 코드입니다.
	
	for (int tc = 0; tc < 100; tc++) {
		string test_str;
		cin >> test_str;
		regex_test(test_str,ptr_root,state_machine);
	}//한 상태머신에 대해 여러개의 testcase를 생성하는 코드입니다.
	for (int i = 0; i < state_machine.size(); i++) {
		delete state_machine[i];
	} //동적 할당된 노드들은 자동해제가 됩니다.
	return 0;
}


