#include <cstdio>
#include <cmath>
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
#define travel(i) for(PortalLevelList *p=PlistFirst[i]; p; p=p->next) // ������ i �� List

using namespace std;

#define maxn 1001
#define maxs 166167001

struct Portal // ���� Portal �ṹ�壬x0,y0 �Ǿ�γ�����꣬x,y,z ����ά���꣨�뾶Ϊ 1 �������ϣ���guid,latlng,label �� JSON ��Ϣ
{
	double x0, y0, x, y, z;
	string guid, latlng, label;
} P[maxn], tmpP[2]; // P[] ���� Portal ��Ϣ��tmpP[] �Ǽ�¼��ʱ��ά����

bool cmpXY(Portal a, Portal b){return a.x0<b.x0 || (a.x0==b.x0 && a.y0<b.y0);} // ��С��������x0 Ϊ��һ�ؼ��֣�y0 Ϊ�ڶ��ؼ���

struct Field{int x; double S;} qL[maxn], qR[maxn]; bool operator < (Field a, Field b){return a.S<b.S;} // ��¼ Field ���ͣ�x Ϊ�м�� Portal �ı�ţ�S Ϊ Field �����

struct PortalLevelList{int x; PortalLevelList *next;} Plist[maxn], *PlistFirst[9], *PlistEnd=Plist; // ����һ�� List ����






int n, Field_ID[maxn][maxn]; // n Ϊ Portal ����, Field_ID ��Ϊ�˷������ÿ�� Field ����Ӧ�ı�ţ���ϸ�� GetFL ������

inline void SWAP(int &a, int &b, int &c) {if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);} // ����������С��������

inline int GetFL(int a, int b, int c) {SWAP(a,b,c); return Field_ID[a][b]+c;} // ������ Portal �ı�������Ӧ�� Field �ı��

inline void GetPortal(int FieldLabel, int &a, int &b, int &c) // �� Field �ı�������Ӧ������ Portal �ı��
{
	a=1; while (a<n-2 && Field_ID[a+1][a+2]+a+3<=FieldLabel) a++;
	b=a+1; while (b<n-1 && Field_ID[a][b+1]+b+2<=FieldLabel) b++;
	c=FieldLabel-Field_ID[a][b];
}







inline void Spin(int lb, int a, int b)
{
	double x=(P[a].y*P[b].z-P[a].z*P[b].y);
	double y=(P[a].z*P[b].x-P[a].x*P[b].z);
	double z=(P[a].x*P[b].y-P[a].y*P[b].x);
	tmpP[lb].x=(P[b].y*z-P[b].z*y);
	tmpP[lb].y=(P[b].z*x-P[b].x*z);
	tmpP[lb].z=(P[b].x*y-P[b].y*x);
}

inline double COS(){return (tmpP[0].x*tmpP[1].x+tmpP[0].y*tmpP[1].y+tmpP[0].z*tmpP[1].z)/sqrt(tmpP[0].x*tmpP[0].x+tmpP[0].y*tmpP[0].y+tmpP[0].z*tmpP[0].z)/sqrt(tmpP[1].x*tmpP[1].x+tmpP[1].y*tmpP[1].y+tmpP[1].z*tmpP[1].z);}

inline double Angle(int a, int b, int c) {Spin(0,a,b); Spin(1,c,b); return acos(COS());} // ���� a-b-c �������ϵĽǶ�

inline double Area(int a, int b, int c) {return Angle(a,b,c)+Angle(b,c,a)+Angle(c,a,b)-pi;} // ���� a-b-c �γɵ� Field �Ĵ�С






inline void FXL(int a, int b)
{
	tmpP[0].x=P[a].y*P[b].z-P[a].z*P[b].y;
	tmpP[0].y=P[a].z*P[b].x-P[a].x*P[b].z;
	tmpP[0].z=P[a].x*P[b].y-P[a].y*P[b].x;
}

inline double COS(int a){return P[a].x*tmpP[0].x+P[a].y*tmpP[0].y+P[a].z*tmpP[0].z;}

inline bool Left(int a, int b, int c) {FXL(a,b); return COS(c)>0;} // �ж� c �Ƿ��� a��b ���

inline bool inField(int a, int b, int c, int d) // �ж� d �Ƿ��� a-b-c �γɵ� Field ��
{
	if (d==a || d==b || d==c) return false;
	if (!Left(a,b,c)) swap(b,c);
	return Left(a,b,d) && Left(b,c,d) && Left(c,a,d);
}



inline void ChangetoPosition(int a)  // �Ѿ�γ������ת����ά����
{
	istringstream iss(P[a].latlng);
	char tmp;
	iss >> P[a].x0 >> tmp >> P[a].y0;
	P[a].x=cos(P[a].x0/180.0*pi)*cos(P[a].y0/180.0*pi);
	P[a].y=sin(P[a].x0/180.0*pi)*cos(P[a].y0/180.0*pi);
	P[a].z=sin(P[a].y0/180.0*pi);
}

inline void ReadInput(const char *localFileName) // ���� JSON
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
		ChangetoPosition(n); // �Ѿ�γ������ת����ά����
	}
}






inline int min3(int a, int b, int c){return (a<=b&&a<=c)?a:(b<=c?b:c);}
inline int min(int a, int b){return a<b?a:b;}
inline int max3(int a, int b, int c){return (a>=b&&a>=c)?a:(b>=c?b:c);}

inline void Max(int &a, int b){if (b>a) a=b;}

char Level[maxs]; // ��¼ÿ�� Field ��������
short int NextPortal[maxs];  // ��¼ÿ�� Field �ڴﵽ������ʱ���ڵ�
int Count[9]; // Count[i] ��ʾ�ж��ٸ� Field ��������Ϊ i
int Lmx[maxn][maxn]; // Lmx[a][b] ��ʾ���� a��b ������γɵ� Field ��������






clock_t gap; // ��¼ʱ��

Json::Value bm, dt, ini_bm, null_dt; // JSON ��Ϣ����







int QLevel; // ѯ�ʲ���

struct xyz{int x,y,z;} pos[maxn]; // xyz ��һ������Ĳ���
bool operator < (xyz a, xyz b){return a.x<b.x || (a.x==b.x && a.y<b.y) || (a.x==b.x && a.y==b.y && a.z<b.z);}
map<string,xyz> M0; // ���ӳ�䵽 xyz ����
map<xyz,string> M1; // xyz ����ӳ�䵽���

inline xyz Ave(xyz a, xyz b, xyz c){return (xyz){(a.x+b.x+c.x)/3, (a.y+b.y+c.y)/3, (a.z+b.z+c.z)/3};} // �� Field ������� xyz �õ��ڵ�� xyz

inline void GetOpinion()
{
	M0["A1"]=(xyz){(int)5e8,0,0}, M0["A2"]=(xyz){0,(int)5e8,0}, M0["A3"]=(xyz){0,0,(int)5e8}; // ������������� xyz ����
	M1[(xyz){(int)5e8,0,0}]="A1", M1[(xyz){0,(int)5e8,0}]="A2", M1[(xyz){0,0,(int)5e8}]="A3";
	
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
	
	// label.txt ÿһ�����ĸ��ַ�������A B C D�� ��ʾ�Ա��Ϊ A,B,C �γɵ� Field ���ڵ�ı��Ϊ D
	
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








int OPtot; // JSON �� Line ������

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
	if (lv == 7) dt[OPtot]["color"] = "#ccccff";
	
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
		pos[a]=(xyz){(int)5e8,0,0}, AddPortal(a,lv); // �� xyz ������ѯ����ǰ������Ӧ�ı��
		pos[b]=(xyz){0,(int)5e8,0}, AddPortal(b,lv);
		pos[c]=(xyz){0,0,(int)5e8}, AddPortal(c,lv);
	}
	int x=GetFL(a,b,c), d=NextPortal[x]; pos[d]=Ave(pos[a],pos[b],pos[c]);
	AddPortal(d,++lv);
	AddLine(a,d,lv), AddLine(b,d,lv), AddLine(c,d,lv);
	OutputPlan(a,b,d,lv); // �ݹ��������
	OutputPlan(b,c,d,lv);
	OutputPlan(c,a,d,lv);
}

inline void OutputPlan(int FieldLabel)
{
	int a, b, c; GetPortal(FieldLabel,a,b,c);
	dt=null_dt; bm=ini_bm; OPtot=0; OutputPlan(a,b,c,1);
	
	Json::FastWriter writer;
	puts("Bookmarks JSON:"); cout << writer.write(bm) << endl;
	puts("DrawTools JSON:"); cout << writer.write(dt) << endl;
}











int OPRandom[1000000];

inline void OutputResult()
{
	freopen("result.txt", "w", stdout);
	
	priority_queue<Field>q; int sz=0, tot=0, FieldLabel;
	
	rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) if (Level[FieldLabel=GetFL(i,j,k)]>=QLevel)
	{
		if (sz==10) q.pop(), sz--;
		q.push((Field){FieldLabel,Area(i,j,k)}), sz++;
		if (tot<=1000000) OPRandom[tot++]=FieldLabel;
	}
	
	dow(i, sz, 1) printf("�����С��#%d\n\n", i), OutputPlan(q.top().x), q.pop();
	
	sz=0, tot=0;
	
	rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) if (Level[FieldLabel=GetFL(i,j,k)]>=QLevel)
	{
		if (sz==10) q.pop(), sz--;
		q.push((Field){FieldLabel,-Area(i,j,k)}), sz++;
	}
	
	dow(i, sz, 1) printf("������#%d\n\n", i), OutputPlan(q.top().x), q.pop();
	
	random_shuffle(OPRandom, OPRandom+tot);
	rep(i, 1, min(tot,30)) printf("Random #%d\n\n", i), OutputPlan(OPRandom[i-1]);
	
	printf("��Ҫע������� Po��\n");
	rep(i, 1, n) rep(j, i+1, n) if (P[i].label == P[j].label) {cout << P[i].label << endl; break;}
	
	fclose(stdout);
}












int main()
{
	ReadInput("portal.txt"); srand(time(NULL));
	
	system("cls"); 
	printf("�ܵ�����%d\n�����ڶ��� Portal��", n); cout << P[n-1].label << endl; // ����ȷ���Ƿ�ȫ�� Portal ��ȷ����
	getchar();
	
	if (n>=maxn)
	{
		puts("�������࣬��ɾ����");
		getchar(); return 0;
	}
	
	sort(P+1, P+1+n, cmpXY); // ����γ��������
	
	int tot=0; rep(i, 1, n-2) rep(j, i+1, n-1) tot+=n-j, Field_ID[i][j]=tot-n; // ö����˵���м�˵㣬tot ��¼ Field ������Field_ID ��Ϊ�˷������ÿ�� Field ����Ӧ�ı�ţ���ϸ�� GetFL ������
	
	int now=0, totL, totR; // now ��ʾ��ǰ�Ѿ��������˶��ٸ� Field
	rep(c, 3, n) dow(a, c-2, 1) // ��������ö���Ҷ˵㣬��������ö����˵�
	{
		totL=totR=0; 
		rep(b, a+1, c-1) if (Left(a,c,b)) qL[++totL]=(Field){b,Area(a,b,c)}; else qR[++totR]=(Field){b,Area(a,c,b)}; // ö���м�˵㣬qL[] Ϊ���� a��c ��ߵ� Portal��qR[] Ϊ���� a��c �ұߵ� Portal
		if (totL)
		{
			sort(qL+1, qL+1+totL); // �������С��������
			clr(PlistFirst,0); PlistEnd=Plist; // List ��ʼ��
			rep(i, 1, totL)
			{
				int b=qL[i].x, FieldLabel=GetFL(a,b,c), mxLevel=max(Lmx[c][b],Lmx[b][a]); // mxLevel �������� c��b �� b��a ������γɵ� Field ���������������� Level ����
				char &Lv=Level[FieldLabel]; Lv=1; // Lv Ϊ��ǰ Field �� Level
				dow(o, mxLevel, 1) travel(o) if (Lv>o) break; else if (inField(a,b,c,p->x)) // �Ӹߵ���ö���ڲ� Field �ȼ������� List
				{
					int tmp=min3(o,Level[GetFL(a,b,p->x)],Level[GetFL(b,c,p->x)])+1; // ���ڵ�Ϊ p->x ʱ��ǰ Field ���ܴﵽ��������
					if (tmp>Lv) Lv=tmp, NextPortal[FieldLabel]=p->x;
				}
				
				int tmp=min(Lv,max(Lmx[a][b],Lmx[b][c])); // �ж��µ� b ��Ϊ�ڵ� a-c ���ߵ�ʱ������ܶ��ٲ�
				PlistEnd->x=b, PlistEnd->next=PlistFirst[tmp], PlistFirst[tmp]=PlistEnd++; // �� b ���뵽 List[tmp]
				
				Max(Lmx[a][c],Lv); Max(Lmx[c][b],Lv); Max(Lmx[b][a],Lv); // ���� Lmx ����
				
				Count[(int)Lv]++; now++;
				if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
					system("cls"), printf("%.6lf%%\n", 100.0*now/tot), gap=clock(); // ÿ�� 0.1s ��ʾһ�ΰٷֱȽ���
			}
		}
		if (totR) // ע��ͬ��
		{
			sort(qR+1, qR+1+totR); 
			clr(PlistFirst,0); PlistEnd=Plist;
			rep(i, 1, totR)
			{
				int b=qR[i].x, FieldLabel=GetFL(a,b,c), mxLevel=max(Lmx[a][b],Lmx[b][c]);
				char &Lv=Level[FieldLabel]; Lv=1;
				dow(o, mxLevel, 1) travel(o) if (Lv>o) break; else if (inField(a,b,c,p->x))
				{
					int tmp=min3(o,Level[GetFL(a,b,p->x)],Level[GetFL(b,c,p->x)])+1;
					if (tmp>Lv) Lv=tmp, NextPortal[FieldLabel]=p->x;
				}
				
				int tmp=min3(Lv,Lmx[c][b],Lmx[b][a]);
				PlistEnd->x=b, PlistEnd->next=PlistFirst[tmp], PlistFirst[tmp]=PlistEnd++;
				
				Max(Lmx[a][b],Lv); Max(Lmx[b][c],Lv); Max(Lmx[c][a],Lv);
				
				Count[(int)Lv]++; now++;
				if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
					system("cls"), printf("%.6lf%%\n", 100.0*now/tot), gap=clock();
			}
		}
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







