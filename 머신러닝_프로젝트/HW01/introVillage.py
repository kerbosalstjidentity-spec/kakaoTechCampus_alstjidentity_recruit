village = []   #전대 마을 grid(1:집이 있음, 0: 집이 없음)
visited = [[0]*10 for _ in range(10)]
#해당 집(칸)을 방문했는지 기록하는 리스트이다.
#dfs 탐색에서 해당 값이 0인 경우만 해당 칸을 방문할 수 있는 조건을 충족한다.
move_x = [-1,-1,-1, 1,1,1, 0,0]
move_y = [-1, 1, 0,-1,1,0,-1,1]
"""
깊이우선 탐색(dfs)을 실시했을 경우,
신입생이 한 집을 방문한 이후 다른 집을 방문하기 위해
후보가 될 수 있는 칸을 모색하는 경우의 수 (상 하 좌 우,대각선(1시, 5시 ,7시, 11시)
===> 총 8가지 방법이 가능하다.
"""
def dfs(st_x, st_y, r, c):  #깊이 우선탐색
    visited[st_x][st_y] = 1
    # memo: dfs는 스택에 함수프레임이 쌓인 후에 해당 노드(여기서는 집(칸))를 방문처리해 준다.

    connect = 0  #연결요소 개수 저장
    """
    신입생이 쭉 연결된(깊이 우선탐색에서 방문가능한 집) 경로를 따라 갔을 때 ,
    총 방문하는 집의 개수
    """
    for k in range(8):
        nxt_x = st_x + move_x[k]
        nxt_y = st_y + move_y[k]
        if nxt_x < 0 or nxt_x >= r or nxt_y < 0 or nxt_y >= c:
            continue  #전대 마을(grid) 밖의 곳을 신입생이 탐색하는 경우(out of bounds)

        if village[nxt_x][nxt_y] == 1 and visited[nxt_x][nxt_y] != 1:

            """
            새로운 칸을 탐색할 수 있는 조건
            1. 해당 칸에 집이 있으면서(값이 1)
            2. 아직 현재나 이전 깊이 우선 탐색을 통해 방문되지 않은 칸의 경우
            """

            connect += dfs(nxt_x, nxt_y,r,c)
            #깊이 우선탐색(dfs)의 결과 해당 연결요소가 집인지, 아니면 창고인지 확인하기 위한 절차

    return connect + 1
      #자신이 현재 위치한 집또한 연결요소의 개수에 포함시켜야 함!!!

n_row, n_col = map(int,input('마을의 행과 열 개수를 공백 기준으로 분리하여 입력: ').split())
for i in range(n_row):
    col=list(map(int,input(f'{i} 행 입력: ').split()))
    village.append(col)  #각 행에 칸의 상태 입력(1=집이 있음, 0= 집이 없음)

garage=0
house=0
for i in range(n_row):
    for j in range(n_col):
        #모든 전대마을 집(grid의 칸)에 대해 깊이우선 탐색(dfs) 시행 판정 및 실행을 함.

        if village[i][j] == 1 and visited[i][j] != 1:
            #해당 칸이 집이고, 아직 방문하지 않았다면 , 연결 요소(다른 집)의 개수를 파악해야함.
            is_garage= dfs(i,j,n_row,n_col)
            if is_garage == 1:
                garage += 1  #창고의 개수 증가(increase variable 'garage')
            else:
                house += 1 #집의 개수 증가(increase variable 'house')

print(f'전대 마을에는 {house} 개의 집, {garage} 개의 창고가 있습니다. ')  #최종 결과 출력
