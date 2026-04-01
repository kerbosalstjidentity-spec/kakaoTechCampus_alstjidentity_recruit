import random
import time

SIZE = 3   #tic-tac-toe는 3x3에서 일어남
Ai_turn = ['X']  #computer는 후공이라는 기저 가정

def getValidAction(board):
    return [i for i in range(SIZE * SIZE) if board[i] == '*']   #아직 빈 칸인 곳의 위치를 리스트에 저장

def move(board, action, turn):  # 특정 player가  칸을 선택하는 작업
    board[action] = turn[0]
    turn[0] = 'X' if turn[0] == 'O' else 'O'   #다음 player로 턴을 넘긴다.
def undo(board, action, turn):  # 칸을 다시 복구 하는 작업(재귀함수의 특성떄문)
    board[action] = '*'
    turn[0] = 'X' if turn[0] == 'O' else 'O' #다음 player로 턴을 넘긴다.

def score(board):  #tic_tac_toe에서 player가 얻을 수 있는 점수를 계산하는 함수
    comb_to_win = [
        [0, 1, 2], [3, 4, 5], [6, 7, 8],  # 행 조합
        [0, 3, 6], [1, 4, 7], [2, 5, 8],  # 열 조합
        [0, 4, 8], [2, 4, 6]  # 대각선 조합
    ]
    for winning in comb_to_win:
        if (board[winning[0]] != '*' and
                board[winning[0]] == board[winning[1]] and
                board[winning[1]] == board[winning[2]]): #이길 수 있는 조합이면 점수를 얻게 된다.

            if(board[winning[0]]=='O'):   #사람이 선공인지, 컴퓨터가 선공인지는 중요하지 않다.(우린 O,X로 판단!!!)
                return 10  #O는 항상 선공이므로 maximize player
            else:
                return -10  #X는 항상 후공이므로 maximize player

    if '*' not in board:
        return 0   #두 player 모두 더 이상 놓을 자리(칸)이 없다는 것(즉, 무승부)

    return -1  # 아직 승패무가 확정되지 않은 상태

def print_board(board):
    # 보드의 상태를 출력.
    for i in range(SIZE):
        for j in range(SIZE):
            # 각 칸의 값(O,X)을 출력.
            print(board[i * SIZE + j] , end="")
        print()


def Computer(board, turn, alpha, beta):  #Game_tree에서 루트노드의 과정이다.
    random.seed(time.time())
    bestAction = -1  # computer는 minimax(alpha-beta pruning 기법)알고리즘의 결과값을 활용하여 최적의 위치를 선택한다.

    validAction = getValidAction(board)  #선택 가능한 모든 칸을 저장한다.
    usedAction = [0] * len(validAction)

    trial = 1
    while trial <= len(validAction):
        random_r = random.randint(0, len(validAction) - 1)

        while usedAction[random_r] == 1:
            random_r = random.randint(0, len(validAction) - 1)
        action = validAction[random_r]
        usedAction[random_r] = 1  #---> minimax 알고리즘 탐색에서 이미 선택한 경우는 제외해야 하므로

        """
        random을 사용하는 이유는 매 시행마다 computer가 이길 수 있는 경우의 수가 여러 개 일 수 있으므로
        난수 생성을 통해 여러 경우 중 한개를 공정하게 택할 수 있게 끔 설계함.
        """

        move(board, action, turn)   #----> 가능한 칸 중 computer는 임의로 (중복없이) 하나의 칸을 선택한다.
        # 로그: print("BestAction: ", board)

        if score(board) != -1:
            bestAction = action
            return bestAction
            #---> minimax()를 호출하기 전, 바로 최상의 선택(MAX: 최대 이익, min: 최소 이득)인 경우 호출할 이유가 없다.(종료조건)

        player_score = minimax(board, alpha, beta, turn)  #minimax() 호출

        if Ai_turn[0] == 'O':   #Maximize player
            if player_score > alpha:
                alpha = player_score
                bestAction = action
        else:
            if player_score < beta:  #Minimize player
                beta = player_score
                bestAction = action
                """
                Alpha_beta pruning: alpha값은 Maximize player만, beta값은 Minimize player만 갱신할 수 있다.
                alpha의 초기값:-∞, beta의 초깃값:∞  (여기선 편의상 tic_tac_toe이므로 -1000,1000으로 가정)
                alpha>=beta
                :-∞ alpha가 beta를 넘어가는 구간으로 alpha>=beta 인 경우 game_tree에서 자식노드의 결과가 어떻든
                Maximize player(여기선 'O')는 무조건 alpha를 , Minimize player(여기선 'X') 무조건 beta를 선택하게 된다.
                """

        undo(board, action, turn)  #다음 minimax()탐색을 위해 이전 상태로 복구시킨다.
        trial += 1  #validAction의 가능한 모든 경우를 탐색

    return bestAction
def minimax(board, alpha, beta, turn):
    validAction = getValidAction(board) #Game_tree에서 루트노드 이후의 과정이다.

    if len(validAction) == 0:
        print(score(board))
        return score(board)    #만약 Game_tree의 terminal_node라면 승패무를 결정하고 결과값을 반환해야 한다.

    if turn[0] == 'O':    #game_tree 상 Maximize player
        player_score=-1000
        usedAction = [0] * len(validAction)
        trial = 1
        while trial <= len(validAction):
         random_r = random.randint(0, len(validAction) - 1)

         while usedAction[random_r] == 1:
            random_r = random.randint(0, len(validAction) - 1)

         mov = validAction[random_r]
         usedAction[random_r] = 1
         #가능한 행동 중 무작위 선택

         move(board, mov, turn)  #----> 가능한 칸 중 player는 임의로 (중복없이) 하나의 칸을 선택한다.
         heuristic = score(board)
         if heuristic != -1:
            #print("휴리스틱", turn[0], ":", heuristic)
             undo(board, mov, turn)
             return heuristic
         """
         휴리스틱 값이 -1이 아닌경우: 3X3의 board에서 turn은 최대 9번이지만,
         9번안에 승패가 결정된 경우이므로, 해당 결과 값을 반환해야 한다.
         """

         player_score = max(minimax(board, alpha, beta, turn), player_score)
         alpha = max(player_score, alpha)
         #alpha 값만 갱신!!!!!

         undo(board, mov, turn) #다음 하위 minimax()탐색을 위해 이전 상태로 복구시킨다.

         if alpha >= beta:
             break
         trial += 1 #validAction의 가능한 모든 경우를 탐색
        return player_score
    else:
        player_score = 1000  #game_tree 상 Minimize player
        usedAction = [0] * len(validAction)
        trial = 1
        while trial <= len(validAction):
            random_r = random.randint(0, len(validAction) - 1)

            while usedAction[random_r] == 1:
                random_r = random.randint(0, len(validAction) - 1)
            mov = validAction[random_r]
            usedAction[random_r] = 1
            # 가능한 행동 중 무작위 선택

            move(board, mov, turn)  #----> 가능한 칸 중 player는 임의로 (중복없이) 하나의 칸을 선택한다.
            heuristic = score(board)

            if heuristic != -1:
                #print("휴리스틱", turn[0], ":", heuristic)
                undo(board, mov, turn)
                return heuristic

            """
            휴리스틱 값이 -1이 아닌경우: 3X3의 board에서 turn은 최대 9번이지만,
            9번안에 승패가 결정된 경우이므로,해당 결과 값을 반환해야 한다.
            """

            player_score = min(minimax(board, alpha, beta, turn), player_score)
            beta = min(player_score, beta)
            # beta 값만 갱신!!!!!

            undo(board, mov, turn) #다음 하위 minimax()탐색을 위해 이전 상태로 복구시킨다.
            if alpha >= beta:
                break
            trial += 1
            #validAction의 가능한 모든 경우를 탐색
            # print("가지치기", turn[0], ":", player_score)
        return player_score
def main():
    board = list("*********")
    turn = ['O']  #항상 처음은 'O'가 시작!!!1
    print("게임시작")
    print_board(board)

    actionString = input("먼저 하시겠습니까? (1. 아니요, 2. 네)\n")
    if actionString == "1":
        print("==========Computer=============")
        Ai_turn[0] = 'O'
        AI_Action = Computer(board, turn,-1000,1000)
        move(board, AI_Action, turn)
        print_board(board)

    while True:
        print("==========Your turn=============")
        t_row = int(input("돌을 놓을 행을 입력하세요 (0~2): "))
        t_col =int(input("돌을 놓을 열을 입력하세요 (0~2): "))
        action_h = t_row*3+t_col

        if board[action_h] == '*':
            move(board, action_h, turn)
            print_board(board)

        print("==========Computer=============")
        action_ai = Computer(board, turn,-1000,1000)

        tic_score = score(board)
        if tic_score != -1:
            winner = 'X' if turn[0] == 'O' else 'O'
            if tic_score:
                 print_board(board)
                 print(winner + "가 승리하였습니다.")

            elif (Ai_turn[0] != 'X'):
                print_board(board)
                print("무승부 입니다.")

            else: print("무승부 입니다.")
            return

        if action_ai != -1 and board[action_ai] == '*':
           move(board, action_ai, turn)
           print_board(board)

if __name__ == "__main__":
    main()



