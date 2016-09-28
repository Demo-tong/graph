#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"map.h"
#include"welcome.h"

#define TRUE 1
#define FALSE 0
#define INFINITY 32767

#define T 10
#define VEXMAX 30
#define Max 100

typedef struct ArcNode{

	int data; 	//用来存放景点编号
	double weight;	//权值即各景点之间的距离
	struct ArcNode *nextarc;//存放下一个结点的地址
} ArcNode;//链表结点类型

typedef struct VertexNode{

	char name[Max]; 	//存放景点名称
	int num;		//存放景点编号
	char describe[Max];	//对景点的简单介绍
	ArcNode *first;		//结点链表头指针
} VerNode;//表头数组单元定义

typedef struct {

	VerNode vertex[VEXMAX];	//表头数组
	int vernum, arcnum; 	//图中顶点，边个数
} AdjList;//邻接表

typedef struct{
	int data[Max];//存入队的景点编号
	int front, rear;//队头，队尾元素
}CSqu;

//函数声明
int Locate(AdjList *g, char *name);//查找景点在数组中的位置
void Insert(ArcNode **first, int i, double w);//将有邻接关系的景点有序插入到链表中
void PrintList(AdjList *G);//以编号、景点名称、景点介绍信息的格式输出景点信息
void Print(AdjList *G);//输出邻接表
void SaveAddPath(AdjList *G);//将添加路线后的路径信息
void Delete(AdjList *G, int m, int n);//删除路径具体实现函数
void SaveDelPath(AdjList *G);//将删除后的路径保存到文件的函数
void InitQueue(CSqu *Q);//初始化队列
int IsEmpty(CSqu *Q);//判队空
int IsFull(CSqu *Q);//判队满
void EnQueue(CSqu *Q, int num);//入队函数
void DelQueue(CSqu *Q, int *num);//出对函数

//管理员登陆函数
void AdminEnter()
{
	system("clear");
	char password[T];
	char user[T]="admin";
	getchar();				//清除缓存
	printf("\n请输入用户名:\n");
	gets(user);
	printf("\n请输入密码:\n");
	system("stty -echo");
	scanf("%s",password);
	system("stty echo");
	printf("*******");
	if(!strcmp(user,password))
	{
		getchar();			//清除缓存
		printf("\n您已登陆成功，按任意键继续!\n");
		getchar();
	}
	else
	{
		getchar();			//清除缓存
		printf("\n密码或用户名错误，按任意键退出\n");
		getchar();
		exit(0);
	}
}


//游客登陆函数
void PassEnter()
{
	system("clear");
	char password[T];
	char pass[T]="passenger";
	getchar();				//清除缓存
	printf("\n请输入用户名:\n");
	gets(pass);
	printf("\n请输入密码:\n");
	system("stty -echo");
	scanf("%s",password);
	system("stty echo");
	printf("*******");
	if(!strcmp(pass,password))
	{
		getchar();			//清除缓存
		printf("\n您已登陆成功，按任意键继续!\n");
		getchar();
	}
	else
	{
		getchar();			//清除缓存
		printf("\n密码或用户名错误，按任意键退出\n");
		getchar();
		exit(0);
	}
}


//读取景点信息函数(列表形式)
void ReadList(AdjList **g)
{
	FILE *fp;
	int i;

	fp = fopen("graph.txt", "rt");//以只读的形式打开文件
	if(fp == NULL){
		printf("\t\t\t************************************\n");
		printf("\t\t\t****文件打开失败,请先创建该文件!****\n");
		printf("\t\t\t************************************\n");
		exit(0);
	}
	*g = malloc(sizeof(AdjList));
	fscanf(fp, "%d%d", &(*g)->vernum, &(*g)->arcnum);//先将景点个数和边数写到邻接表的相应单元内
	while(!feof(fp)){
			for(i=1; i<=(*g)->vernum; i++){
				fscanf(fp, "%d%s%s", &(*g)->vertex[i].num, (*g)->vertex[i].name, (*g)->vertex[i].describe);//将顶点信息读到邻接表中
				(*g)->vertex[i].first = NULL;
			}
	}

	fclose(fp);
	system("clear");
}


//将文件中的信息写到邻接表中
void RGraph(AdjList **G)
{
	char ver1[Max], ver2[Max]; //用来存放输入的景点
	int i, j, k;
	double weight;
	FILE *fp1;

	ReadList(&(*G));
	if((fp1 = fopen("path.txt", "rt")) == NULL){
		printf("\t\t\t************************************\n");
		printf("\t\t\t****文件打开失败,请先创建该文件!****\n");
		printf("\t\t\t************************************\n");

		exit(0);
	}

	while(fscanf(fp1,"%s%s%lf", ver1, ver2, &weight) != EOF){//将路径信息读到邻接表中

			//查找读取到的景点所在数组中的位置
			i = Locate(*G, ver1);
			j = Locate(*G, ver2);

			//有序将其插入
			Insert(&((*G)->vertex[j].first), i, weight);
			Insert(&((*G)->vertex[i].first), j, weight);
	}
	fclose(fp1);
}


//插入函数，以此来创建邻接表中的结点链表
void Insert(ArcNode **first, int i, double w)
{
	ArcNode *s;
	ArcNode *p = *first;

	s = malloc(sizeof(ArcNode));//申请空间，存储待插入景点的信息
	s->data = i;
	s->weight = w;
	s->nextarc = NULL;

	if(p == NULL)//无后继结点,直接插入
		*first = s;
	else if(p->data > i){//有一个后继结点，且其序号大于s->data,插入并修改头指针
		s->nextarc = p;
		*first = s;
	}
	else {
		while(p->nextarc && p->nextarc->data < i)//p的后继存在且其序号比s->data小,p后移
			p = p->nextarc;
		s->nextarc = p->nextarc;
		p->nextarc = s;
	}
}


//查找输入的景点所在位置
int Locate(AdjList *g, char *name)
{
	int i;
	for(i=1; i<=g->vernum; i++)
		if(strcmp(g->vertex[i].name, name) == 0)
			return i;
	return -1;
}

//输出函数(列表形式)
void PrintList(AdjList *G)
{
	int i;

	system("clear");
	printf("\t\t\t#############################################################################\n");
	printf("\t\t\t#########################****以下是西安邮电大学的景点****####################\n");
	printf("\t\t\t#############################################################################\n");
	printf("\t\t\t+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	for(i=1; i<=G->vernum; i++)
		printf("\t\t\t+++%d.%s--->%s\n", G->vertex[i].num, G->vertex[i].name, G->vertex[i].describe);
	printf("\t\t\t+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	printf("\t\t\t#############################################################################\n");
	printf("\t\t\t#############################################################################\n");
	getchar();
	printf("请按任意键继续...");
	getchar();
}


//输出函数打印出结点信息
void Print(AdjList *G)
{
	ArcNode *p;
	int i;

	for(i=1; i<=G->vernum; i++){
		printf("\n------------------------------------------------------------------------------------------\n");
		printf("%d \t%s \t%s\n", G->vertex[i].num, G->vertex[i].name, G->vertex[i].describe);
		printf("--------------------------------------------------------------------------------------------\n");
		printf("\n与该结点连接的景点有如下:\n");
		p = G->vertex[i].first;
		while(p){
			printf("%3d %.3lf\n", p->data, p->weight);
			p = p->nextarc;
		}
	}

	getchar();
	printf("请按任意键继续.....\n");
}


//管理员增加新景点及路线
void AddPath(AdjList *G)
{
	char path1[Max], path2[Max];//保存输入的景点编号
	int i;
	int flag[Max];//判断数组，判断某两景点之间的路径是否已输出
	ArcNode *p;
	char ch;
	int m, n;//存输入的起终点景点的位置
	double weight;//存两景点之间的距离

	for(i=1; i<=G->vernum; i++)
		flag[i] = 0;//初始化为0
	//输出当前存在的所有路径
	for(i=1; i<=G->vernum; i++){
		flag[i] = 1;//将当前所在的结点的判断标志置为1,在后来遇到有关当前结点的路径则不需要输出
		p = G->vertex[i].first;
		while(p){
			if(flag[p->data] == 0)
				printf("%d.%s<---->%d.%s\n", G->vertex[i].num, G->vertex[i].name, G->vertex[p->data].num, G->vertex[p->data].name);
			p = p->nextarc;
			}
	}

	getchar();
	printf("以上是当前存在的路径,是否继续进行增加?(Y(y) OR N(n))\n");
	scanf("%c", &ch);
	if(ch == 'Y' || ch == 'y'){
		printf("请输入您要添加的路线的起点景点名称:\n");
		scanf("%s", path1);
		printf("请输入终点景点名称:\n");
		scanf("%s", path2);
		printf("请输入两景点之间的距离:\n");
		scanf("%lf", &weight);

		m = Locate(G, path1);
		n = Locate(G, path2);

		Insert(&(G->vertex[m].first), n, weight);
		Insert(&(G->vertex[n].first), m, weight);

		G->arcnum = G->arcnum + 1;//增加路径后,路径条数加一
		SaveAddPath(G);
	}

	else
		return ;
	getchar();
	printf("已成功将添加的路线保存到文件中,请按任意键返回上一页...");
	getchar();
}


//增加路线后将路径信息保存到文件中
void SaveAddPath(AdjList *G)
{
	FILE *info, *fp;
	int i;
	int flag[Max];
	ArcNode *p;

	if((info = fopen("path.txt", "wt")) == NULL || (fp = fopen("graph.txt", "wt")) == NULL){
		printf("打开文件失败!\n");
		exit(0);
	}

	for(i=1; i<=G->vernum; i++)
		flag[i] = 0;

	fprintf(fp, "%3d %3d\n", G->vernum, G->arcnum);
	for(i=1; i<=G->vernum; i++){
		fprintf(fp, "%d %s %s\n", G->vertex[i].num, G->vertex[i].name, G->vertex[i].describe);
		flag[i] = 1;
		p = G->vertex[i].first;

		while(p){

			if(flag[p->data] == 0)
				fprintf(info, "%s %s %.3lf\n", G->vertex[i].name, G->vertex[p->data].name, p->weight);
			p = p->nextarc;
		}
	}

	fclose(info);
	fclose(fp);
	printf("SAVED SUCCESS\n");
}


//删除原有的某路径
void DelPath(AdjList *G)
{
	char path1[Max], path2[Max];//保存输入的景点编号
	int i;
	int flag[Max];//判断数组，判断某两景点之间的路径是否已输出
	ArcNode *p, *q;
	char ch;
	int m, n;//存输入的起终点景点的位置

	for(i=1; i<=G->vernum; i++)
		flag[i] = 0;//初始化为0
	//输出当前存在的所有路径
	for(i=1; i<=G->vernum; i++){
		flag[i] = 1;//将当前所在的结点的判断标志置为1,在后来遇到有关当前结点的路径则不需要输出
		p = G->vertex[i].first;
		while(p){
			if(flag[p->data] == 0)
				printf("%d.%s<---->%d.%s\n", G->vertex[i].num, G->vertex[i].name, G->vertex[p->data].num, G->vertex[p->data].name);
			p = p->nextarc;
			}
	}

	getchar();
	printf("以上是当前存在的路径,是否继续进行删除?(Y(y) OR N(n))\n");
	scanf("%c", &ch);

	if(ch == 'Y' || ch == 'y'){
		printf("请输入您要删除的路线的起点景点名称:\n");
		scanf("%s", path1);
		printf("请输入终点景点名称:\n");
		scanf("%s", path2);

		m = Locate(G, path1);
		n = Locate(G, path2);

		Delete(G, m, n);
		G->arcnum = G->arcnum - 1;
		SaveDelPath(G);
	}
	else
		return ;

	getchar();
	printf("指定路径已删除，请按任意键返回上一页...");
	getchar();
}

//删除路径
void Delete(AdjList *G, int m, int n)
{
	int i;
	ArcNode *p, *q;
	for(i=1; i<=G->vernum; i++){//在景点数组中查找需要删除的路线的景点名称,找到后将该路线删除
		if(G->vertex[i].num == m){//在起点的链表中删除终点
			p = G->vertex[i].first;
			q = p->nextarc;
			while(p){
				if(p->data == n){

					G->vertex[i].first = q;//若p为该路线的终点并且当前是链表的头指针,修改修改头指针
					break ;
				}else if( q && (q->data == n)){
					p->nextarc = q->nextarc;//若q为该路线的终点，使p的下一个指向q的下一个
					break ;
				}else {
					p = p->nextarc;
					q = p->nextarc;
				}
			}
		}else if(G->vertex[i].num == n){//在终点的链表中删除起点
			p = G->vertex[i].first;
			q = p->nextarc;
			while(p){
				if(p->data == m){
					G->vertex[i].first = q;//若p为该路线的终点并且当前是链表的头指针,修改修改头指针
					break ;
				}else if( q && (q->data == m)){
						p->nextarc = q->nextarc;//若q为该路线的终点，使p的下一个指向q的下一个
						break ;
					}else {
						p = p->nextarc;
						q = p->nextarc;
					}
				}
			}
	}
}

//将删除后的路径信息及景点信息保存到文件中
void SaveDelPath(AdjList *G)
{
	FILE *FP, *file;
	int vexnum, arcnum;//保存景点个数几路径边数
	int i;//循环变量
	ArcNode *p;
	int flag[Max];//判定数组

	if((FP = fopen("path.txt", "wt")) == NULL || (file = fopen("graph.txt", "wt")) == NULL){
		printf("文件打开失败!\n");
		exit(0);
	}

	for(i=1; i<=G->vernum; i++)
		flag[i] = 0;
	vexnum = G->vernum;
	arcnum = G->arcnum;

	fprintf(file, "%3d %3d\n", G->vernum, G->arcnum);
	for(i=1; i<=vexnum; i++){
		fprintf(file, "%d %s %s\n", G->vertex[i].num, G->vertex[i].name, G->vertex[i].describe);
		flag[i] = 1;
		p = G->vertex[i].first;

		while(p){
			if( flag[p->data] == 0 )
				fprintf(FP, "%s %s %.3lf\n", G->vertex[i].name, G->vertex[p->data].name, p->weight);
			p = p->nextarc;
		}
	}

	fclose(FP);
	fclose(file);

	printf("SAVE SUCCESS!\n");
}

//查询某景点周围可供浏览的景点
void Search(AdjList *g)
{
	ArcNode *p;
	int num;//存用户输入的景点编号
	int i;//循环变量

	PrintList(g);

	printf("\n请输入您要查询的景点编号:\n");
	scanf("%d", &num);

	printf("您所查询的景点周围有:\n");
	for(i=1; i<=g->vernum; i++){
		if(num == g->vertex[i].num)
		{
			p = g->vertex[i].first;//使p指向查找到的结点的头指针
			for( ; p; p = p->nextarc)
				printf("%s<--%.3lfm-->%s(%s)\n" , g->vertex[i].name, p->weight, g->vertex[p->data].name, g->vertex[p->data].describe);
			break;//找到结点输出与其相邻接的景点则退出循环
 		}
	}
	getchar();
	printf("请按任意键继续....");
}


//输出景点
void SpotOut(AdjList *G)
{
	int i;

	for(i = 1; i <= G->vernum; i++){
		printf("%d.%s\n", G->vertex[i].num, G->vertex[i].name);
	}
}

int visit[Max];//判断数组
//广度优先遍历具体实现
void BFS(AdjList *g1, int vi, int *count)
{
	ArcNode *p;
	++(*count);
	if((*count) < g1->vernum)
		printf("%d.%s---->", g1->vertex[vi].num, g1->vertex[vi].name);
	else
		printf("%d.%s", g1->vertex[vi].num, g1->vertex[vi].name);
	visit[vi] = 1;//vi出的景点已被访问，将其赋值为一

	p = g1->vertex[vi].first;
	while(p){
		if(!visit[p->data])
			BFS(g1, p->data, count);
		p = p->nextarc;
	}

}

//广度优先遍历图中的景点
void BaordFS(AdjList *GB, int m)
{
	int i;
	int count = 0;//使用count控制输出格式

	for(i = 1; i <= GB->vernum; i++)
		visit[i] = 0;//初始化为0,表示景点都未被访问过
	for(i = 1; i <= GB->vernum; i++)
		if(!visit[i] && (m == GB->vertex[i].num))//当前景点未被访问过,进入广度遍历
			BFS(GB, i, &count);
}


/*队列的功能实现*/
//初始化
void InitQueue(CSqu *Q)
{
	Q->rear = Q->front = -1;
	return ;
}

//判断空
int IsEmpty(CSqu *Q)
{
	if( Q->rear == Q->front )
		return TRUE;
	return FALSE;
}

//判队满
int IsFull(CSqu *Q)
{
	if ( (Q->rear + 1)%Max == Q->front )
		return TRUE;
	return FALSE;
}
//入队
void EnQueue(CSqu *Q, int num)
{
	if(IsFull(Q) == TRUE){
		printf("队已满，无法入队!\n");
		return ;
	}

	Q->rear = (Q->rear + 1)%Max;
	Q->data[Q->rear] = num;
}

//出队
void DelQueue(CSqu *Q, int *num)
{
	if(IsFull(Q) == TRUE){
		printf("队已空，无法出队!\n");
		return ;
	}

	Q->front = (Q->front + 1)%Max;
	*num = Q->data[Q->front];
}


//深度遍历具体实现
void DFS(AdjList *g2, int vj)
{
	CSqu Q;
	ArcNode *p;
	int v;//接收出队的元素
	int count1 = 1;

	InitQueue(&Q);//初始化队列
	printf("%d.%s---->", g2->vertex[vj].num, g2->vertex[vj].name);
	visit[vj] = 1;

	EnQueue(&Q, vj);//将当前元素编号入队,保证下一次操作可正常进行
	while(IsEmpty(&Q) == 0){
		++count1;
		DelQueue(&Q, &v);
		p = g2->vertex[v].first;
		while(p){
			if(!visit[p->data]){
				if(count1 < g2->vernum)//控制输出格式
					printf("%d.%s---->", g2->vertex[p->data].num, g2->vertex[p->data].name);
				else
					printf("%d.%s", g2->vertex[p->data].num, g2->vertex[p->data].name);
				visit[p->data] = 1;//访问过的景点将其值赋为1
				EnQueue(&Q, p->data);
			}

			p = p->nextarc;
		}
	}
}

//深度优先遍历图中的景点
void DeepFS(AdjList *GD, int n)
{
	int j;
	for(j = 1; j <= GD->vernum; j++)
		visit[j] = 0;//初始化为0，表示均未被访问过
	for(j = 1; j <= GD->vernum; j++)
		if( !visit[j] && (n == GD->vertex[j].num) )//对未被访问过的结点进行深度遍历
			DFS(GD, j);
	getchar();
	printf("\n请按任意键继续....");
}


//采用Dijkstra算法求得从起点到终点景点的最短路线
void Dijkstra(AdjList *G, int start, int end, double dist[], int path[][VEXMAX])
{
	int mindist, i, j, k;
	int t = 1;
	ArcNode *p;

	for(i=0; i<VEXMAX; i++){
		dist[i] = INFINITY;//将dist初始化为最大
		for(j=0; j<VEXMAX; j++)
			path[i][j] = 0;//将path数组初始化为0
	}

	p = G->vertex[start].first;//初始化,将与start有直接路径的点初始化
	while(p){
		dist[p->data] = p->weight ;
		path[p->data][1] = start ;
		p = p->nextarc;
	}

	path[start][0] = 1;//判断结点是否被查找过

	for(i=2; i<=G->vernum; i++){//控制循环次数为n-1次
		mindist = INFINITY;
		for(j=1; j<=G->vernum; j++)//选择最小权值的路线
			if(!path[j][0] && dist[j] < mindist){//j未被访问过并且j对应的权值小于最小值，记录最小的信息
				k = j;
				mindist = dist[j];
			}

		if(mindist == INFINITY)
			break;

		path[k][0] = 1;//记录k对应的景点已被访问过
		p = G->vertex[k].first;//使p指向权值最小的结点的头指针
		while(p){
			if(!path[p->data][0] && dist[k] + p->weight < dist[p->data]){
				dist[p->data] = dist[k] + p->weight;
				for ( j = 1 ; path[k][j] != 0 ; j++ ) {
					path[p->data][j] = path[k][j] ;
				}
				path[p->data][j] = k ;
				path[p->data][++j] = 0 ;
			}

			p = p->nextarc;
		}
	}

	for(i=1; i<=G->vernum; i++){
		if(i == end){
			printf("%s---->%s的最短路线是: 从%s", G->vertex[start].name, G->vertex[end].name, G->vertex[start].name);

			for(j=2; path[i][j] != 0; j++)
				printf("-->%s", G->vertex[path[i][j]].name);
			printf("->%s, 距离为%.3lfm\n", G->vertex[end].name, dist[i]);
		}
	}

	getchar();
	printf("请按任意键继续....");
	getchar();
}


//寻找两景点之间最短路线
void ShortCut(AdjList *G)
{
	char name[Max];
	int start, end;
	double dist[VEXMAX];
	int path[VEXMAX][VEXMAX];

	printf("\n以上为学校景点,请输入起点景点名称:\n");
	scanf("%s", name);
	start = Locate(G, name);
	printf("请输入终点景点名称:\n");
	scanf("%s", name);
	end = Locate(G, name);
	//调用函数
	Dijkstra(G, start, end, dist, path);
}


//采用prim算法求得最短连通路线
void Prim(AdjList *G, int start)
{
	struct{
		int adjvex;//加入的结点的序号
		double lowcost;//两结点之间的权值
	}closedge[VEXMAX];
	int i, k, m, min;
	double sum = 0;//总距离
	int count;//控制循环次数
	ArcNode *p;

	for(i=0; i<=G->vernum; i++){//初始化closedge数组
		closedge[i].adjvex = start;
		closedge[i].lowcost = INFINITY;
	}
	closedge[start].lowcost= 0;//将起始点的lowcost值赋为一，表示以后不再访问该结点

	//对除了出发点以外所有顶点初始化对应的closedge数组
	p = G->vertex[start].first;//使p指向start的头指针

	while(p){
		closedge[p->data].adjvex = start;
		closedge[p->data].lowcost = p->weight;
		p = p->nextarc;
	}

	printf("浏览完全景需要的最短路径如下:\n");
	for(count = 1; count <= G->vernum - 1; ++count){//控制循环n-1次
		//选择权值最小的边
		min = INFINITY;
		for(k=1; k<=G->vernum; k++){
			if(closedge[k].lowcost != 0 && closedge[k].lowcost < min ){
				m = k;
				min = closedge[k].lowcost;//找到权值最小的结点
			}
		}


		printf("从%s---->%s:%10.3lfm\n", G->vertex[closedge[m].adjvex].name, G->vertex[m].name, closedge[m].lowcost);
		sum += closedge[m].lowcost;
		closedge[m].lowcost = 0;

		//当加入权值最小的点后修改closedge数组的值
		p = G->vertex[m].first;
		while(p){

			if(closedge[p->data].lowcost != 0 && p->weight < closedge[p->data].lowcost ){
				closedge[p->data].lowcost = p->weight;
				closedge[p->data].adjvex = m;
			}

			p = p->nextarc;
		}
	}


	printf("\n总路程为: %10.3lf\n", sum);
	getchar();
	printf("请按任意键继续....");
	getchar();
}

//浏览全部景点的最短路程
void MiniTree(AdjList *G)
{
	char name[Max];
	int start;

	PrintList(G);
	printf("\n以上为学校全部景点,请输入起点景点名称:\n");
	scanf("%s", name);
	start = Locate(G, name);

	Prim(G, start);
}

//游客选择函数模块
void PassChoice()
{
	int choice;
	AdjList *G;

	RGraph(&G);
	do{
		system("clear");
		Welcome3();
		printf("\t\t^v^  请输入您所需的操作对应的序号:\n");
		scanf("%d", &choice);
		switch(choice){
			int num1;
			case 1:
				Map();
				getchar();
				break;
			case 2:
				PrintList(G);
				break;
			case 3:
				SpotOut(G);
				printf("请输入起点景点编号:\n");
				scanf("%d", &num1);
				printf("广度游历景点路线如下:\n");
				BaordFS(G, num1);
				printf("\n\n深度游历景点路线如下:\n");
				DeepFS(G, num1);
				getchar();
				break;
			case 4:
				PrintList(G);
				ShortCut(G);
				break;
			case 5:
				Search(G);
				getchar();

				break;
			case 6:
				MiniTree(G);
				break;
			case 7:
				return ;
			case 0:
				exit(0);
		}
	}while(choice != 0);
}


//管理员选择函数
void AdminChoice()
{
	AdjList *G;
	int choose;
	RGraph(&G);

	do{
		system("clear");
		Welcome2();
		printf("请输入您所需操作对应的序号:");
		scanf("%d", &choose);
		switch(choose){
			case 1:
				Print(G);
				getchar();
				break;
			case 2:
				Map();
				getchar();
				break;
			case 3:
				AddPath(G);
				break ;
			case 4:
				DelPath(G);
				break ;
			case 5:
				return ;
			case 0:
				exit(1);
		}
	}while(choose != 0);
}


//选择函数
void OutChoice()
{

	int choice;

	do{
		system("clear");
		Welcome1();
		printf("\n请按照您所需的功能输入对应的序号值:\n");
		scanf("%d", &choice);

		switch(choice){
			case 1:
				AdminEnter();
				AdminChoice();//进入管理员选择模块
				break;
			case 2:
				PassEnter();
				PassChoice();//进入游客选择操作模块
				break;
			case 3:
				exit(0);
		}
	}while(choice != 0);
}

main()
{
	system("clear");//清屏
	welcome();//欢迎界面
	getchar();
	OutChoice();

}
