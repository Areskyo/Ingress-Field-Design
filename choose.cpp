#include <cstdio>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <queue>
#include "jsoncpp/json.h"

#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define pi acos(-1)
#define travel(i,j) for(edge *p=fir[i][j]; p; p=p->n)

using namespace std;

typedef pair<double,int> Pdi;
typedef long double ld;

#define maxn 1009
#define maxs 166167001

struct Portal
{
	double x0, y0, x, y, z; int lbx, lby;
	string guid, latlng, label;
} P[maxn], G[9];

bool cmpX(Portal a, Portal b){return a.x0<b.x0;}
bool cmpY(Portal a, Portal b){return a.y0<b.y0;}
bool cmpXY(Portal a, Portal b){return a.x0<b.x0 || (a.x0==b.x0 && a.y0<b.y0);}

struct F{int LB; double S;}; bool operator < (F a, F b){return a.S<b.S;}

struct edge{int v; edge *n;} e[maxn], *fir[50][50], *pt=e;




int n, tot, q[maxn], pid_cal[maxn][maxn];

inline void SWAP(int &a, int &b, int &c) {if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);}

inline int GetLB(int a, int b, int c) {SWAP(a,b,c); return pid_cal[a][b]+c;}

inline void GetPortal(int LB, int &a, int &b, int &c)
{
	a=1; while (a<n-2 && pid_cal[a+1][a+2]+a+3<=LB) a++;
	b=a+1; while (b<n-1 && pid_cal[a][b+1]+b+2<=LB) b++;
	c=LB-pid_cal[a][b];
}




inline void Spin(int lb, int a, int b)
{
	double x=(P[a].y*P[b].z-P[a].z*P[b].y);
	double y=(P[a].z*P[b].x-P[a].x*P[b].z);
	double z=(P[a].x*P[b].y-P[a].y*P[b].x);
	G[lb].x=(P[b].y*z-P[b].z*y);
	G[lb].y=(P[b].z*x-P[b].x*z);
	G[lb].z=(P[b].x*y-P[b].y*x);
}

inline double COS(){return G[1].x*G[2].x+G[1].y*G[2].y+G[1].z*G[2].z;}

inline double Angle(int a, int b, int c) {Spin(1,a,b); Spin(2,c,b); return COS();} // Spin ����������ʱ������COS ������ɾȥ

inline double Area(int a, int b, int c) {return Angle(a,b,c)+Angle(b,c,a)+Angle(c,a,b)-pi;}




inline void FXL(int a, int b)
{
	G[1].x=P[a].y*P[b].z-P[a].z*P[b].y;
	G[1].y=P[a].z*P[b].x-P[a].x*P[b].z;
	G[1].z=P[a].x*P[b].y-P[a].y*P[b].x;
}

inline double COS(int a){return P[a].x*G[1].x+P[a].y*G[1].y+P[a].z*G[1].z;}

inline bool Left(int a, int b, int c) {FXL(a,b); return COS(c)>0;}

inline bool inField(int a, int b, int c, int d)
{
	if (d==a || d==b || d==c) return false;
	if (!Left(a,b,c)) swap(b,c);
	return Left(a,b,d) && Left(b,c,d) && Left(c,a,d);
}







inline void ChangetoPosition(int a)
{
	istringstream iss(P[a].latlng);
	char tmp;
	iss >> P[a].x0 >> tmp >> P[a].y0;
	P[a].x=cos(P[a].x0/180.0*pi)*cos(P[a].y0/180.0*pi);
	P[a].y=sin(P[a].x0/180.0*pi)*cos(P[a].y0/180.0*pi);
	P[a].z=sin(P[a].y0/180.0*pi);
}

inline void ReadInput(const char *localFileName) // ����JSON
{
	string str, chunk;
	
	if (localFileName)
	{
		ifstream fin(localFileName);
		if (fin)
			while (getline(fin, chunk) && chunk != "")
				str += chunk;
		else
			while (getline(cin, chunk) && chunk != "")
				str += chunk;
	}
	else
		while (getline(cin, chunk) && chunk != "") str += chunk;
	
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);
	
	Json::Value::Members arrayMember = input["portals"]["idOthers"]["bkmrk"].getMemberNames();
	for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter)
	{
		n++;
		P[n].guid = input["portals"]["idOthers"]["bkmrk"][*iter]["guid"].asString();
		P[n].latlng = input["portals"]["idOthers"]["bkmrk"][*iter]["latlng"].asString();
		P[n].label = input["portals"]["idOthers"]["bkmrk"][*iter]["label"].asString();
		ChangetoPosition(n);
	}
}







inline int min3(int a, int b, int c){return (a<=b&&a<=c)?a:(b<=c?b:c);}
inline int max3(int a, int b, int c){return (a>=b&&a>=c)?a:(b>=c?b:c);}

char Level[maxs]; short int nx[maxs]; int Count[9];

int LevelCal(int a, int b, int c)
{
	int LB=GetLB(a,b,c); SWAP(a,b,c);
	if (Level[LB]) return (int)Level[LB]; else Level[LB]=1;
	int x0=min3(P[a].lbx,P[b].lbx,P[c].lbx);
	int x1=max3(P[a].lbx,P[b].lbx,P[c].lbx);
	int y0=min3(P[a].lby,P[b].lby,P[c].lby);
	int y1=max3(P[a].lby,P[b].lby,P[c].lby);
	rep(i, x0, x1) rep(j, y0, y1) travel(i,j) if (p->v<c && inField(a,b,c,p->v))
	{
		int d=p->v, A=GetLB(a,b,d), B=GetLB(b,c,d), C=GetLB(c,a,d);
		int tmp=min3(Level[A]?Level[A]:LevelCal(a,b,d),Level[B]?Level[B]:LevelCal(b,c,d),Level[C]?Level[C]:LevelCal(c,a,d))+1;
		if (tmp>Level[LB]) Level[LB]=tmp, nx[LB]=d;
	}
	return (int)Level[LB];
}










clock_t gap;

Json::Value bm, dt, ini_bm, null_dt;






/* int n, tot, Total, QLevel, pid_cal[maxn][maxn], lv[maxs], nx[maxs], po[maxs], Count[9];
double pretty[maxs];

inline int min(int a, int b){return a<b?a:b;}

inline double min3(double a, double b, double c){return (a<=b&&a<=c)?a:(b<=c?b:c);}

inline double max3(double a, double b, double c){return (a>=b&&a>=c)?a:(b>=c?b:c);}

inline double dis(int a, int b){return sqrt(pow(P[a].x-P[b].x,2) + pow(P[a].y-P[b].y,2));}

inline double FieldS(int a, int b, int c)
{
	return fabs(P[a].x*P[b].y+P[b].x*P[c].y+P[c].x*P[a].y - P[a].y*P[b].x-P[b].y*P[c].x-P[c].y*P[a].x)/2;
}

inline double disL(int a, int b, int c){return FieldS(a,b,c)*2/dis(a,b);}

inline double FangCha(double a, double b, double c)
{
	double sum=a+b+c; a=a/sum*3, b=b/sum*3, c=c/sum*3;
	return (pow(a-1,2)+pow(b-1,2)+pow(c-1,2))/3;
}

inline double abc(double a, double b, double c)
{
	double sum=a+b+c; a=a/sum, b=b/sum, c=c/sum;
	return max3(a,b,c)-min3(a,b,c);
}

inline double CalPretty(int a, int b, int c, int d)
{
	int r=FieldS(a,b,c)*2/(dis(a,d)+dis(b,d)+dis(c,d));
	return (max3(dis(a,d),dis(b,d),dis(c,d))-min3(dis(a,d),dis(b,d),dis(c,d)))/r;
}

inline double Dir(int a, int b, int c)
{
	return (P[b].x-P[a].x)*(P[c].y-P[a].y)-(P[c].x-P[a].x)*(P[b].y-P[a].y);
}

inline bool inField(int a, int b, int c, int d)
{
	return Dir(a,b,d)*Dir(b,c,d)>0 && Dir(b,c,d)*Dir(c,a,d)>0;
}

inline int GetPID(int a, int b, int c){return pid_cal[a][b]+c-b;}

int FieldLevel(int a, int b, int c)
{
	if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);
	int x=GetPID(a,b,c), tmp, y; double mn=0, tmp2;
	if (lv[x]) return lv[x]; tot++;
	for(int ta=1, tb=a, tc=b, td=c; ta<tb; ta++) if (inField(a,b,c,ta))
	{
		po[x]++; tmp=9; tmp2=0;
		y=GetPID(ta,tb,tc); if ((lv[y]?lv[y]:FieldLevel(ta,tb,tc))<lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(ta,tb,td); if ((lv[y]?lv[y]:FieldLevel(ta,tb,td))<lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(ta,tc,td); if ((lv[y]?lv[y]:FieldLevel(ta,tc,td))<lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		tmp2*=0.6; tmp2+=CalPretty(a,b,c,ta); if (tmp>lv[x] || (tmp==lv[x] && tmp2<mn)) lv[x]=tmp, nx[x]=ta, mn=tmp2;
	}
	for(int ta=a, tb=a+1, tc=b, td=c; tb<tc; tb++) if (inField(a,b,c,tb))
	{
		po[x]++; tmp=9; tmp2=0;
		y=GetPID(ta,tb,tc); if ((lv[y]?lv[y]:FieldLevel(ta,tb,tc))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(ta,tb,td); if ((lv[y]?lv[y]:FieldLevel(ta,tb,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(tb,tc,td); if ((lv[y]?lv[y]:FieldLevel(tb,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		tmp2*=0.6; tmp2+=CalPretty(a,b,c,tb); if (tmp>lv[x] || (tmp==lv[x] && tmp2<mn)) lv[x]=tmp, nx[x]=tb, mn=tmp2;
	}
	for(int ta=a, tb=b, tc=b+1, td=c; tc<td; tc++) if (inField(a,b,c,tc))
	{
		po[x]++; tmp=9; tmp2=0;
		y=GetPID(ta,tb,tc); if ((lv[y]?lv[y]:FieldLevel(ta,tb,tc))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(ta,tc,td); if ((lv[y]?lv[y]:FieldLevel(ta,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(tb,tc,td); if ((lv[y]?lv[y]:FieldLevel(tb,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		tmp2*=0.6; tmp2+=CalPretty(a,b,c,tc); if (tmp>lv[x] || (tmp==lv[x] && tmp2<mn)) lv[x]=tmp, nx[x]=tc, mn=tmp2;
	}
	for(int ta=a, tb=b, tc=c, td=c+1; td<=n; td++) if (inField(a,b,c,td))
	{
		po[x]++; tmp=9; tmp2=0;
		y=GetPID(ta,tb,td); if ((lv[y]?lv[y]:FieldLevel(ta,tb,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(ta,tc,td); if ((lv[y]?lv[y]:FieldLevel(ta,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(tb,tc,td); if ((lv[y]?lv[y]:FieldLevel(tb,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		tmp2*=0.6; tmp2+=CalPretty(a,b,c,td); if (tmp>lv[x] || (tmp==lv[x] && tmp2<mn)) lv[x]=tmp, nx[x]=td, mn=tmp2;
	}
	
	pretty[x]=mn; return ++lv[x];
} */



int QLevel;

struct xyz{double x,y,z;} pos[maxn];
bool operator < (xyz a, xyz b){return a.x<b.x || (a.x==b.x && a.y<b.y) || (a.x==b.x && a.y==b.y && a.z<b.z);}
map<string,xyz> M0;
map<xyz,string> M1;

inline xyz Ave(xyz a, xyz b, xyz c){return (xyz){(a.x+b.x+c.x)/3, (a.y+b.y+c.y)/3, (a.z+b.z+c.z)/3};}

inline void GetOpinion()
{
	M0["A1"]=(xyz){1,0,0}, M0["A2"]=(xyz){0,1,0}, M0["A3"]=(xyz){0,0,1};
	M1[(xyz){1,0,0}]="A1", M1[(xyz){0,1,0}]="A2", M1[(xyz){0,0,1}]="A3";
	
	string tmp1="A", tmp2="Lv 0";
	rep(i, 1, QLevel)
	{
		tmp1[0]='A'+i-1, tmp2[3]='0'+i;
		ini_bm["portals"][tmp1]["label"]=tmp2;
		ini_bm["portals"][tmp1]["state"]=0;
	}
	ini_bm["portals"]["A"]["bkmrk"]["A1"]["guid"]="";
	ini_bm["portals"]["A"]["bkmrk"]["A2"]["guid"]="";
	ini_bm["portals"]["A"]["bkmrk"]["A3"]["guid"]="";
	
	ifstream fin("label.txt"); 
	int tmp, num=0; fin >> tmp; for(int i=1, a=1; i<tmp; i++) num+=a, a*=3;
	
	string a, b, c, d;
	rep(i, 1, num) 
	{
		fin >> a >> b >> c >> d;
		M0[a]=Ave(M0[b],M0[c],M0[d]), M1[M0[a]]=a;
		if (a[0]-'A'+1<=QLevel) tmp1[0]=a[0], ini_bm["portals"][tmp1]["bkmrk"][a]["guid"]="";
	}
}








int OPtot;

inline string D_toString(double a)
{
	char buffer[20];
	sprintf(buffer, "%f", a);
	return buffer;
}

inline string I_toString(int a)
{
	char buffer[20];
	sprintf(buffer, "%d", a);
	return buffer;
}

inline void AddLine(int a, int b, int lv)
{
	if (lv == 1) dt[OPtot]["color"] = "#0000ff";
	if (lv == 2) dt[OPtot]["color"] = "#2222ff";
	if (lv == 3) dt[OPtot]["color"] = "#4444ff";
	if (lv == 4) dt[OPtot]["color"] = "#6666ff";
	if (lv == 5) dt[OPtot]["color"] = "#8888ff";
	if (lv == 6) dt[OPtot]["color"] = "#aaaaff";
	
	dt[OPtot]["latLngs"][0]["lat"] = D_toString(P[a].x0);
	dt[OPtot]["latLngs"][0]["lng"] = D_toString(P[a].y0);
	dt[OPtot]["latLngs"][1]["lat"] = D_toString(P[b].x0);
	dt[OPtot]["latLngs"][1]["lng"] = D_toString(P[b].y0);
	
	dt[OPtot]["type"] = "polyline";
	
	OPtot++;
}

inline void AddPortal(int a, int lv)
{
	string tmp="A"; tmp[0]+=lv-1;
	bm["portals"][tmp]["bkmrk"][M1[pos[a]]]["guid"] = P[a].guid;
	bm["portals"][tmp]["bkmrk"][M1[pos[a]]]["latlng"] = P[a].latlng;
	bm["portals"][tmp]["bkmrk"][M1[pos[a]]]["label"] = P[a].label;
}

void OutputPlan(int a, int b, int c, int lv)
{
	if (lv == QLevel) return; SWAP(a,b,c);
	if (lv == 1)
	{
		AddLine(a,b,lv), AddLine(b,c,lv), AddLine(c,a,lv);
		pos[a]=(xyz){1,0,0}, AddPortal(a,lv);
		pos[b]=(xyz){0,1,0}, AddPortal(b,lv);
		pos[c]=(xyz){0,0,1}, AddPortal(c,lv);
	}
	int x=GetLB(a,b,c), d=nx[x]; pos[d]=Ave(pos[a],pos[b],pos[c]);
	AddPortal(d,++lv);
	AddLine(a,d,lv), AddLine(b,d,lv), AddLine(c,d,lv);
	OutputPlan(a,b,d,lv);
	OutputPlan(b,c,d,lv);
	OutputPlan(c,a,d,lv);
}

inline void OutputPlan(int LB)
{
	int a, b, c; GetPortal(LB,a,b,c);
	dt=null_dt; bm=ini_bm; OPtot=0; OutputPlan(a,b,c,1);
	
	Json::FastWriter writer;
	puts("Bookmarks JSON:"); cout << writer.write(bm) << endl;
	puts("DrawTools JSON:"); cout << writer.write(dt) << endl;
}











int OPRandom[1000000];

inline void OutputResult()
{
	freopen("result.txt", "w", stdout);
	
	priority_queue<F>q; int sz=0, tot=0, LB;
	
	/* rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) if (Level[LB=GetLB(i,j,k)]>=QLevel)
	{
		if (sz==10) q.pop(), sz--;
		q.push((F){LB,Area(i,j,k)}), sz++;
		if (tot<=1000000) OPRandom[tot++]=LB;
	}
	
	dow(i, sz, 1) printf("�����С��#%d\n\n", i), OutputPlan(q.top().LB), q.pop(); */
	
	rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) if (Level[LB=GetLB(i,j,k)]>=QLevel)
	{
		if (sz==10) q.pop(), sz--;
		q.push((F){LB,-Area(i,j,k)}), sz++;
		if (tot<=1000000) OPRandom[tot++]=LB;
	}
	
	dow(i, sz, 1) printf("������#%d\n\n", i), OutputPlan(q.top().LB), q.pop();
	
	random_shuffle(OPRandom, OPRandom+tot);
	rep(i, 1, min(tot,30)) printf("Random #%d\n\n", i), OutputPlan(OPRandom[i-1]);
	
	fclose(stdout);
}












int main()
{
	ReadInput("portal.txt"); srand(time(NULL));
	
	system("cls"); printf("�ܵ�����%d\n�����ڶ��� Portal��", n); cout << P[n-1].label << endl;
	getchar();
	
	if (n>maxn-9)
	{
		puts("�������࣬��ɾ����");
		getchar(); return 0;
	}
	
	int GAP=(int)sqrt(n), Btot=0;
	sort(P+1, P+1+n, cmpX); 
	for(int st=0; Btot++,st<n; st+=GAP) rep(i, 1, GAP) if (st+i>n) break; else P[st+i].lbx=Btot;
	Btot=0;
	sort(P+1, P+1+n, cmpY); 
	for(int st=0; Btot++,st<n; st+=GAP) rep(i, 1, GAP) if (st+i>n) break; else P[st+i].lby=Btot;
	sort(P+1, P+1+n, cmpXY); 
	rep(i, 1, n) pt->v=i, pt->n=fir[P[i].lbx][P[i].lby], fir[P[i].lbx][P[i].lby]=pt++;
	
	int tot=0; rep(i, 1, n-2) rep(j, i+1, n-1) tot+=n-j, pid_cal[i][j]=tot-n;
	
	int now=0;
	rep(c, 1, n) dow(a, c-1, 1) rep(b, a+1, c-1) 
	{
		Count[LevelCal(a,b,c)]++; now++;
		if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
			system("cls"), printf("%.6lf%%", 100.0*now/tot), gap=clock();
	}
	
	system("cls"); 
	rep(i, 3, 7) printf("%d �������: %d ��\n", i, Count[i]); puts(""); puts("");
	printf("����Ҫ���ݶ����ص�����(3-7)\n"); 
	
	QLevel=3; scanf("%d", &QLevel);
	puts("�й���Ϣ���������result.txt��ף�����>.<");
	GetOpinion();
	OutputResult();
	
	return 0;
}












/* inline void GetAvaliableArea(int LB, int a, int b, int c)
{
	Line A0=GetLine(a,b), A1=Adjust(A0,c);
	Line B0=GetLine(b,c), B1=Adjust(B0,a);
	Point tmp; double x_mn, x_mx, y_mn, y_mx;
	tmp=CROSS(A0,B0); x_mn=x_mx=tmp.x; y_mn=y_mx=tmp.y;
	tmp=CROSS(A0,B1); Min(x_mn,tmp.x); Max(x_mx,tmp.x); Min(y_mn,tmp.y); Max(y_mx,tmp.y);
	tmp=CROSS(A1,B0); Min(x_mn,tmp.x); Max(x_mx,tmp.x); Min(y_mn,tmp.y); Max(y_mx,tmp.y);
	tmp=CROSS(A1,B1); Min(x_mn,tmp.x); Max(x_mx,tmp.x); Min(y_mn,tmp.y); Max(y_mx,tmp.y);
	int lbx_mn=1, lbx_mx=PortalBtot, lby_mn=1, lby_mx=PortalBtot;
	while (Bx_mx[lbx_mn]<x_mn) lbx_mn++;
	while (x_mx<Bx_mn[lbx_mx]) lbx_mx++;
	while (By_mx[lby_mn]<y_mn) lby_mn++;
	while (y_mx<By_mn[lby_mx]) lby_mx++;
	rep(i, lbx_mn, lbx_mx) rep(j, lby_mn, lby_mx) travel(i,j)
	{
		int d=p->v;
		if (InField(a,c,d,b)) Update(GetLB(a,c,d),b,min3(F[GetLB(a,b,c)],F[GetLB(a,b,d)],F[GetLB(d,b,c)])+1);
	}
} */

	
	/* for(int st=0; st<tot; st+=GAP)
	{
		Bst[++Btot]=st+1;
		rep(i, 1, GAP) if (st+i>tot) break; else q0[i].LB=q[st+i], q0[i].S=Area(q[st+i]);
		sort(q0+1, q0+(st+GAP>tot?tot-st:GAP), cmpS);
		rep(i, 1, GAP) if (st+i>tot) break; else q[st+i]=q0[i].LB;
	}
	
	priority_queue<F>q1;
	
	rep(i, 1, Btot) q1.push((F){i,Area(q[Bst[i]])});
	
	gap=clock(); rep(i, 1, tot)
	{
		F a=q1.top(); q1.pop(); 
		int x=q[Bst[a.LB]++]; if (q[Bst[a.LB]] && Bst[a.LB]/GAP!=a.LB) q1.push((F){a.LB,Area(q[Bst[a.LB]])});
		
		int a,b,c; GetPortal(x,a,b,c); if (Left(a,b,c)) swap(b,c);
		
		if (!Level[x]) Level[x]=1; Count[Level[x]]++;
		
		GetAvaliableArea(x,a,b,c);
		GetAvaliableArea(x,b,c,a);
		GetAvaliableArea(x,c,a,b);
	} */