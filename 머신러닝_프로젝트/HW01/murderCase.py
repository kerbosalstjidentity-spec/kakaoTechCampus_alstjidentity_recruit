from collections import deque
#bfs(너비 우선 탐색) 수행 시 자료구조(queue)를 사용하기 위해 deque moudule을 import 해줌.

bfs_result=[]
dfs_result=[]
#각각 dfs, bfs 탐색에 따른 방문 경로(방문 친구 순서)를 기록하는 각각의 리스트

neighbor = [[] for _ in range(1001)]
#용지 마을 친구 관계 graph

visited = [0 for _ in range(1001)]
#dfs 탐색 시 한 번 방문한 사람은 해당 탐색에 포함하지 않는다.
#visited[person]=0인경우 만 '깊이 우선탐색' 시 방문

def dfs(st,dfs_result): #깊이 우선탐색
    visited[st] = 1
    #Note: dfs는 스택에 함수프레임이 쌓인 후에 해당 노드(여기서는 집(칸))를 방문처리해 준다.
    dfs_result.append(st)  #경로 저장
    for nxt in neighbor[st]:
        if visited[nxt] != 1:  #아직 방문하지 않는 친구라면
            dfs(nxt,dfs_result) #방문을 수행 해준다.
            """
            절대 방문 '처리'를 해서는 안된다. 방문 수행(재귀 함수 호출만!!!) 만 해줘야 한다.
            방문 순서가 의도치 않게 꼬일 수 있다...
            """

def bfs(st,bfs_result):  #너비 우선탐색
    global visited  #이전 깊이 우선탐색을 하고 온 상황이므로 새로운 'visited' 리스트가 필요하다.
    visited = [0 for _ in range(1001)]
    path = deque([st])  #처음 방문한 노드(=victim)를 큐에 삽입
    visited[st] = 1    #Note:반대로 bfs는 큐에 삽입 시 방문처리가 이루어진다.
    while path:
        find_mate = path.popleft()
        #큐 :FIFO(First-in, First-out 구조
        # neighbor graph에서 victim을 기준으로 level 순으로 탐색을 가능하게 해주는 도구.)
        #level: 시작점(victim)에서 특정 노드(친구/사람)를 방문하기 위해, 최소한으로 거쳐야할 사람의 수
        #시작점은 level=0

        bfs_result.append(find_mate)
        for nxt_mate in neighbor[find_mate]:
            if  visited[nxt_mate] != 1:  #방문하지 않았다면
                visited[nxt_mate] = 1   #방문처리를 해주고
                path.append(nxt_mate) #Note:반대로 bfs는 큐에 삽입 시 방문처리가 이루어진다.

                """
                 bfs는 큐에 추출단계가 아닌 삽입단계 시에 방문처리를 해주는 이유:
                 ex.) 예를 들어 다음과 같은 그래프가 있다고 가정해보자....
                 1-2
                 1-3
                 2-4
                 3-4
                 (1이 시작 노드라고 해보자, 이럴 경우 level_0=1, level_1=2,3, level_2=4가 된다.)
                  편의상 작은 번호의 노드부터 먼저 탐색 된다고 가정해보면 level_0를 수행한 이 후 큐의 상태는 다음과 같다.
                  queue:2,3
                  1)2가 pop()되고 4가 삽입 된다. --> queue:3,4
                  만약 1)의 과정에서 4를 방문처리 해주지 않았다면,(즉 해당 노드를 추출(pop)할 떄 방문처리 해준다면)
                  2)3을 pop()하고 4가 '방문처리 되지 않았'으므로 4가 '다시'삽입된다.
                  3) 4를 pop() 한다.
                  4) 4를 pop() 한다.
                  이렇듯 방문처리를 큐에서 '추출'(pop)할 때 해준다면,
                  중복이 발생(두번 이상 방문)하여 원치않는 탐색 과정이나, 큐에 오버헤드가 발생할 수 있다.
                 """

def solution(victim):  # DFS와 BFS를 호출 하는 함수
    dfs(victim,dfs_result)
    print( "동선 계획 1 (DFS) : " + " ".join( map( str,dfs_result) ) )

    print()

    bfs(victim, bfs_result)
    print( "동선 계획 2 (BFS) : " + " ".join( map( str,bfs_result ) ) )


if __name__ == "__main__":
    people, mate, victim = map(int, input("마을 주민의 수, 친구 관계 수, 피해자의 집 주소: ").split())
    """
    people=마을 주민의 수
    mate= 친구(집) 관계 수
    victim= 피해자의 집 주소
    """
    print("친구관계: ")
    for _ in range(mate):
        bro1, bro2 = map(int, input().split())
        neighbor[bro1].append(bro2)
        neighbor[bro2].append(bro1)
        #친구(집) 관계는 양방향 관계이므로 두 개의 간선 정보를 graph에 저장

    for j in range(1, people+1):
        neighbor[j].sort()
        """
        탐색을 진행하는 경우 neighbor[person]에 대해 neighbor[person][friend]를 앞에서 부터 순서대로 순회하게 되므로
        낮은 번호의 친구부터 먼저 탐색하기 위해(경로에 포함하기 위해) 각 주민에 대한 '친구관계를 내림차순 정렬'함.
        """
    solution(victim)
