//============================================================================
// Name        : casino.cpp
// Author      : 7socks
// Version     : 1.0.0
// Copyright   : --
// Description : It's a casino with multiple games to play. Have fun,
//				 I guess?
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <tuple>
#include <random>
#include <functional>
using namespace std;

std::random_device rd; // For randomization
std::mt19937 mt(rd());

/// normal distribution (should replace uniform_int_ with this at some pt)
int distribute(int min,int max){
	double mind = min; double maxd = max;
	double median = ((maxd-mind)/2) + mind;
	std::normal_distribution<double> dis(median,median/3); // this is probably BAAD
	double almost = dis(mt); int there;
	if (almost<mind){there=min;}else if (almost>maxd){there=max;}
	else{there=almost;}
	return there;
}

bool saveflag; // records whether the current game is previously saved
string savetitle; // records the title of the current save if applicable

int anum;	 // Player input storage

int tbl;	// current game table

/*!! VERY IMPORTANT! Update the sizes of these arrays whenever you add
 * more fish to the library.*/
const int pond = 77; // the total number of fishing items (KEEP UPDATED)
string commonfish[28]; // fish lists (names)
string uncommonfish[26];
string rarefish[13];
string ultrararefish[9];
string allfish[pond];
int commonfishvalue[28]; // fish lists (values)
int uncommonfishvalue[26];
int rarefishvalue[13];
int ultrararefishvalue[9];
int allfishvalue[pond];
int commonfishref[28];   // fish lists (reference to allfish index)
int uncommonfishref[26];
int rarefishref[13];
int ultrararefishref[9];

class Gamer{
private:
	string gender;
	double risk; // 10-100 percentage of money they are willing to risk
	int skill_1; // skill rating for coinflip (luck)
	int skill_2; // skill rating for gamble pot (strategy)
	int skill_3; // skill rating for zener cards (psychic)
	int skill_4; // skill rating for fishing hole (probably n/a or clout?)
				 // skill ratings should all be 1-5, 0 if they don't play it
	bool table_arena[5]; // tables they play at
	int fishes[pond]; //stores unclaimed fish items, qty slot for each item
public:
	string name;
	int funds;
	int table;  // current table
	string nom; // for pronouns
	string acc;
	string gena;
	string genp;
	string ref;
	string ptr;
	Gamer(string n, string g, int f){ // @suppress("Class members should be properly initialized")
		name = n;
		gender = g;
		funds = f;
		if (g=="Y"){
			nom="you";acc="you";gena="your";genp="yours";ref="yourself";}
		else if (g=="M"){
			nom="he";acc="him";gena="his";genp="his";ref="himself";}
		else if (g=="F"){
			nom="she";acc="her";gena="her";genp="hers";ref="herself";}
		for (int i=0;i<10;i++){fishes[i] = 0;}
	}
	void init_tables(bool cflip, bool pot, bool zener, bool fish){
		table = 0;
		table_arena[0]=true;table_arena[1]=cflip;
		table_arena[2]=pot;table_arena[3]=zener;
		table_arena[4]=fish;
	}
	void init_behavior(double r,int c, int g, int z, int f){
		risk = r;
		skill_1 = c;
		skill_2 = g;
		skill_3 = z;
		skill_4 = f;
		//add more attributes as necessary
	}
	void reset(){table = 0;}
	bool tables(int t){return table_arena[t];}
	int do_bet(int t){ // t = table no. (for contextual behavior/max bets)
		int b; int max_; double funds_d = funds;
		if (t==3){ // zener cards
			if (funds>1000){
				//provides higher chance of betting maximum amount (1000)
				double stake = ((funds_d*2)-1000)/funds_d;
				max_ = stake*(risk/100)*1000;
			}
			else{max_ = funds_d*risk/100;} // already < 1000, no adjustment
			std::uniform_int_distribution<int> pay(1,max_);
			b = pay(mt); if (b>1000){b=1000;}
		}
		else{
			max_ = funds_d*risk/100;
			std::uniform_int_distribution<int> pay(1,max_);
			b = pay(mt);
		}
		return b;
	}
	int do_coinflip(int real){
		int s; // ^^ real = the actual coin toss result
		std::uniform_int_distribution<int> guess(1,2);
		if (skill_1==1){
			for(int i=0;i<3;i++){ // repeats TWICE favoring a loss
				s = guess(mt);
				if (s!=real){break;}
			}
		}
		if (skill_1==2){
			for(int i=0;i<2;i++){ // repeats ONCE favoring a loss
				s = guess(mt);
				if (s!=real){break;}
			}
		}
		else if (skill_1==4){
			for(int i=0;i<2;i++){ // repeats ONCE favoring a win
				s = guess(mt);
				if (s==real){break;}
			}
		}
		else if (skill_1==5){
			for(int i=0;i<3;i++){ // repeats TWICE favoring a win
				s = guess(mt);
				if (s==real){break;}
			}
		}
		else {s = guess(mt);} // NO repetition -- normal chance
		return s;
	}
	int do_potstake(int pm){ // NPC places stake in pot
		double funds_d = funds; int maxpay = funds_d*(risk/100);
		int stake;
		if (maxpay>pm){
			std::uniform_int_distribution<int> mpay(1,maxpay);
			int max_stake = mpay(mt); if (max_stake>pm){max_stake=pm;}
			std::uniform_int_distribution<int> stk(1,max_stake);
			stake = stk(mt);
		}
		else {
			std::uniform_int_distribution<int> stk(1,maxpay);
			stake = stk(mt);
		}
		return stake;
	}
	int do_potguess(int pot, int maxstake, int qty){
		int min_pot = 1*qty;int max_pot = maxstake*qty; int guess;
		double stkd=maxstake; double adj=stkd/2; int adjust = adj;
		guess = distribute(min_pot,max_pot);
		if (skill_2==1){
			for (int i=0;i<3;i++){
				if (guess<=pot) {guess += adjust;}
			}
		}
		else if (skill_2==2){
			for (int i=0;i<2;i++){
				if (guess<=pot) {guess += adjust;}
			}
		}
		else if (skill_2==4){
			for (int i=0;i<2;i++){
				if (guess>pot) {guess -= adjust;}
			}
		}
		else if (skill_2==5){
			for (int i=0;i<3;i++){
				if (guess>pot) {guess -= adjust;}
			}
		}
		if (guess<min_pot){guess=min_pot;} // final safety catch
		else if (guess>max_pot){guess=max_pot;} // might be unnecessary
		return guess;
	}
	std::pair<int,int> do_zener(int c, int s){
		std::uniform_int_distribution<int> card(1,5);
		int a = card(mt); int b = card(mt); // must select 1st b4 repeating
		if (skill_3==1){
			for (int i=0;i<2;i++){
				int x = card(mt); int y = card(mt);
				if (a==c){a=x;}
				if (b==s){b=y;}
			}
		}
		else if (skill_3==2){
			for (int i=0;i<1;i++){
				int x = card(mt); int y = card(mt);
				if (a==c){a=x;}
				if (b==s){b=y;}
			}
		}
		else if (skill_3==4){
			for (int i=0;i<3;i++){ // More repetitions for skilled vs not
				int x = card(mt); int y = card(mt);
				if (a!=c){a=x;}
				if (b!=s){b=y;}
			}
		}
		else if (skill_3==5){
			for (int i=0;i<5;i++){
				int x = card(mt); int y = card(mt);
				if (a!=c){a=x;}
				if (b!=s){b=y;}
			}
		}
		std::tuple<int,int> select = {a,b};
		return select;
	}
	void do_zener_deal(bool color, bool shape){
		int answer; int predict;
		if (color and shape){answer=3;}
		else if (color or shape) {answer=2;}
		else {answer=1;}
		// Making predictions...
		std::array<double,4> options = {1,2,3,4};
		std::array<double,3> chances = {4,5,1};
		std::piecewise_constant_distribution<double> pick(
				options.begin(),options.end(),chances.begin());
		predict = pick(mt);
		if (skill_3==1){
			for (int i=0;i<2;i++){if (predict==answer){predict=pick(mt);}}
		}
		else if (skill_3==2){
			for (int i=0;i<1;i++){if (predict==answer){predict=pick(mt);}}
		}
		else if (skill_3==4){
			for (int i=0;i<1;i++){if (predict!=answer){predict=pick(mt);}}
		}
		else if (skill_3==5){
			for (int i=0;i<2;i++){if (predict!=answer){predict=pick(mt);}}
		}
		// Now verifying prediction and awarding bonus directly...
		if ((answer==1)and(predict==1)){funds+=15;}
		if ((answer==2)and(predict==2)){funds+=30;}
		//if ((answer==3)and(predict==2)){funds+=10;}
		if ((answer==3)and(predict==3)){funds+=100;}
	}
	int do_fish(){
		int f;
		std::uniform_int_distribution<int> hook(0,399); f = hook(mt);
		if (skill_4==1){
			std::uniform_int_distribution<int> miss(15,30);
			f -= miss(mt);
		}
		else if (skill_4==2){
			std::uniform_int_distribution<int> miss(5,20);
			f -= miss(mt);
		}
		else if (skill_4==4){
			std::uniform_int_distribution<int> miss(5,20);
			f += miss(mt);
		}
		else if (skill_4==5){
			std::uniform_int_distribution<int> miss(15,30);
			f += miss(mt);
		}
		if (f<0){f=0;}
		if (f>399){f=399;}
		return f;
	}
	bool do_reel(int r){
		bool didcatch = false;
		std::uniform_int_distribution<int> rod(1,100);
		int reel = rod(mt); int tug; int moxy = 0;
		// Adjusting reel pct based on player skill
		// (Any other bonuses go here too)
		if (skill_4==1){moxy = -8;}
		else if (skill_4==2){moxy = -3;}
		else if (skill_4==4){moxy = 3;}
		else if (skill_4==5){moxy = 8;}
		reel -= moxy; // -= because trying for UNDER tug
		if (reel<1){reel=1;} if (reel>100){reel=100;}
		// Defining chance of success by fish rarity
		if (r<360){tug=90;}
		else if (r<385){tug=80;}
		else if (r<395){tug=65;}
		else if (r<399){tug=45;}
		else {tug=100;}
		if (reel<=tug){didcatch=true;}
		return didcatch;
	}
	void add_fish(int f,int qty=1){fishes[f] += qty;}
	int check_fish(int f){return fishes[f];} // returns qty of given fish
	bool has_fish(){ // returns whether player has any fish in basket
		bool iszero = true;
		for (int i=0;i<pond;i++){if(fishes[i]!=0){iszero=false;break;}}
		return (not iszero);
	}
	void clear_fish(){for(int i=0;i<pond;i++){fishes[i]=0;}}
	void list_fish(){
		cout << "\nYour Basket:"; // fine bc only used for player
		int iszero = true;
		for (int i=0;i<pond;i++){
			if (fishes[i]!=0){
				iszero=false;
				cout << "\n" << fishes[i] << "x   " << allfish[i];
			}
		}
		if (iszero){cout<<"\nIt's empty.";}
		cout << "\n";
	}
	void sell_fish(int f, int qty){ // sells given qty of given fish
		funds += (allfishvalue[f]*qty); fishes[f] -= qty;
	}
	void sell_basket(){ // sells ALL fish
		for (int i=0;i<pond;i++){
			funds += (allfishvalue[i]*fishes[i]);
			fishes[i] = 0;
		}
	}
	int price_fish(int f, int qty){return allfishvalue[f]*qty;}
	int price_basket(){
		int value = 0; for (int i=0;i<pond;i++){
			value += allfishvalue[i]*fishes[i];
		}
		return value;
	}
};

Gamer player("You","Y",100);
Gamer gambler("Madison","F",800);
Gamer richboy("Alexander","M",20000);
Gamer lady("Gertrude","F",2500);
Gamer sandwich("Carl","M",20);
Gamer psychic("Valerie","F",1000);
Gamer wildcard("Wilby","M",8000);
Gamer amateur("Tobias","M",2000);
Gamer ace("Penny","F",5000);

std::vector<Gamer*> gamerlist = {&player, &gambler, &richboy, &lady,
&sandwich, &psychic, &wildcard, &amateur, &ace};

void app_init(){
	player.init_tables(true,true,true,true);
	gambler.init_tables(true,true,true,true);
	richboy.init_tables(true,true,false,false);
	lady.init_tables(false,true,true,true);
	sandwich.init_tables(true,true,true,true);
	psychic.init_tables(true,false,true,true);
	wildcard.init_tables(true,true,false,false);
	amateur.init_tables(true,true,false,true);
	ace.init_tables(false,true,false,true);
	player.init_behavior(100,3,3,3,3); // only really necessary for fishing
	gambler.init_behavior(100,3,4,2,3);
	richboy.init_behavior(20,3,3,3,0);
	lady.init_behavior(40,0,2,3,5);
	sandwich.init_behavior(50,3,2,1,2);
	psychic.init_behavior(60,4,0,5,1);
	wildcard.init_behavior(80,3,3,0,0);
	amateur.init_behavior(30,3,1,0,3);
	ace.init_behavior(70,0,5,0,4);
	int a = 0; // keep track of all fish index
	string fl; ifstream fishfile("assets/common_fish.txt");
	int w = 0; while (getline(fishfile,fl)){
		std::stringstream fl2 (fl); string seg; std::vector<string> seglist;
		while(std::getline(fl2,seg,'/')){seglist.push_back(seg);}
		commonfish[w] = seglist[0]; commonfishvalue[w] = stoi(seglist[1]);
		allfish[a]=seglist[0];allfishvalue[a]=stoi(seglist[1]);
		commonfishref[w]=a;
		w++; a++;
	}
	fishfile.close();
	ifstream fishfile2("assets/uncommon_fish.txt");
	w = 0; while (getline(fishfile2,fl)){
		std::stringstream fl2 (fl); string seg; std::vector<string> seglist;
		while(std::getline(fl2,seg,'/')){seglist.push_back(seg);}
		uncommonfish[w] = seglist[0];uncommonfishvalue[w] = stoi(seglist[1]);
		allfish[a]=seglist[0];allfishvalue[a]=stoi(seglist[1]);
		uncommonfishref[w]=a;
		w++; a++;
	}
	fishfile2.close();
	ifstream fishfile3("assets/rare_fish.txt");
	w = 0; while (getline(fishfile3,fl)){
		std::stringstream fl2 (fl); string seg; std::vector<string> seglist;
		while(std::getline(fl2,seg,'/')){seglist.push_back(seg);}
		rarefish[w] = seglist[0];rarefishvalue[w] = stoi(seglist[1]);
		allfish[a]=seglist[0];allfishvalue[a]=stoi(seglist[1]);
		rarefishref[w]=a;
		w++; a++;
	}
	fishfile3.close();
	ifstream fishfile4("assets/ultrarare_fish.txt");
	w = 0; while (getline(fishfile4,fl)){
		std::stringstream fl2 (fl); string seg; std::vector<string> seglist;
		while(std::getline(fl2,seg,'/')){seglist.push_back(seg);}
		ultrararefish[w]=seglist[0];ultrararefishvalue[w]=stoi(seglist[1]);
		allfish[a]=seglist[0];allfishvalue[a]=stoi(seglist[1]);
		ultrararefishref[w]=a;
		w++; a++;
	}
	fishfile4.close();
	allfish[a] = "glowing rainbow fish of fortune"; allfishvalue[a]=5000;
	for (auto& gmr : gamerlist){Gamer &g = *gmr;g.clear_fish();}
}

int get_input(string expect = "any",int min_ = 0,int max_ = 1000){
	// expect values: any, num
	int o; bool valid = false; string i;
	cout << "\n\n > ";
	while ((not valid)){
		int i; cin >> i;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
			continue;
		}
		if (expect=="any"){o=0;valid=true;}
		else if (expect=="num"){
			if ((i >= min_)and(i<=max_)){
				if (i==0){o=0;valid=true;} // fix this if you even can
				else if (i!=0){o=i;valid=true;}
			}
		}
	}
	return o;
}

int get_bet(int min_ = 1, int max_ = 10000){
	int o;
	while (true){
		cout << "\nEnter bet amount:  "; cin >> o;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
			cout << "\nPlease enter a valid number\n";
			continue;
		}
		if (o>player.funds){
			cout << "\nYou cannot bet more than you have!\n";
		}
		else if ((o>=min_)and(o<=max_)){break;}
		else if (o<min_){
			cout << "\nThe minimum bet is " << min_ << " gold.\n";
		}
		else if (o>max_){
			cout << "\nThe maximum bet is " << max_ << " gold.\n";
		}
	}
	return o;
}

int get_potbet(int min_, int max_){
	int o;
	while (true){
		cout << "\n\nGuess the pot size:  "; cin >> o;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
			cout << "\nPlease enter a valid number";
			continue;
		}
		if ((o>=min_)and(o<=max_)){break;}
		else if (o<min_){
			cout << "\nThe minimum pot is " << min_ << " gold.";
		}
		else if (o>max_){
			cout << "\nThe maximum pot is " << max_ << " gold.";
		}
	}
	return o;
}

int get_bribe(){
	int o;
	while (true){
		cout << "\nEnter bribe amount:  "; cin >> o;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
			cout << "\nPlease enter a valid number";
			continue;
		}
		if (o>player.funds){
			cout << "\nYou cannot give more than you have!\n";
		}
		// The minimum is 0 in case they want to cancel (or have no gold!)
		else if ((o>=0)and(o<=900)){break;}
		else if (o<0){cout << "\nPlease enter a valid number";}
		else if (o>900){cout << "\nThe maximum bribe is 900 gold.";}
	}
	return o;
}

void reset_objects(){
	for (auto& gmr : gamerlist){
		Gamer &g = *gmr;
		g.reset(); // reset current table of each player
	}
}

void check_players();
void print_players();

void go_table(int t);

void go_lounge();
void go_prizebooth();

void go_coinflip();
void play_coinflip();

void go_pot();
void play_pot();

void go_zener();
void play_zener();
void zener_turn0(std::vector<Gamer*> players,int qty); // dealer's turn
void zener_turn1(std::vector<Gamer*> players,int qty); // player's turn
void zener_turn2(std::vector<Gamer*> players,int qty, int d); // npc turns

void go_fish();
void play_fish();
void catch_fish();
int get_fish(int r);
int bribe_fish(int r);
void fish_reward();

void read_rules(int t){
	string ruleline; string page;
	if (t==1){page="rulebook/coinflip_rules.txt";}
	else if (t==2){page="rulebook/gamblepot_rules.txt";}
	else if (t==3){page="rulebook/zener_rules.txt";}
	else if (t==4){page="rulebook/fish_rules.txt";}
	ifstream rulepage(page);
	cout << "\n";
	while (getline(rulepage,ruleline)){
		cout << ruleline << "\n";
	}
	rulepage.close();
	cout << "\nSelect:\n1: Return to Table"; anum = get_input("num",1,1);
}

void save_init(){
	cout<<"\nSelect Player:";
	ifstream direct("savefiles/save_directory.txt");string savename;
	std::vector<string> savelist;
	int w = 0; while(getline(direct,savename)){
		cout<<"\n"<<++w<<": "<< savename;
		savelist.push_back(savename);
	}
	direct.close();
	anum = get_input("num",1,w);
	string savefile = "savefiles/"+savelist[anum-1]+".txt";
	fstream myfile(savefile); string dat; std::vector<string> datrec;
	while(getline(myfile,dat)){datrec.push_back(dat);}
	myfile.close();
	for (int i=0;i<pond;i++){         // fish basket data
		player.add_fish(i,stoi(datrec[i]));
	}
	int i = pond; for (auto& gmr : gamerlist){ // player & npc funds
		Gamer &g = *gmr;
		g.funds = stoi(datrec[i]);
		cout << g.name << " "<< g.funds << endl;
		i++;
	}
	saveflag=true;
	savetitle=savename;
}

void make_save(){
	/* This currently re-writes the file from the name provided, so if
	 * someone happened to choose the same name twice, the previous data
	 * would be overwritten.*/
	string savename;
	if (saveflag){savename=savetitle;}
	else{cout<<"\nPlayer Name:  "; cin >> savename;}
	string mytitle = "savefiles/" + savename + ".txt";
	ofstream savefile(mytitle); savefile.clear();
	for (int i=0;i<pond;i++){
		savefile << player.check_fish(i) << endl;
	}
	for	(auto& gmr : gamerlist){
		Gamer &g = *gmr;
		if (g.name!="You"){savefile << "\n";}
		savefile << g.funds;
	}
	savefile.close();
	if (not saveflag){
		std::vector<string> hover;
		fstream direct("savefiles/save_directory.txt",ios::app);
		direct << "\n" << savename;
		direct.close();
	}
	cout << "\n\nGame saved.\n\nSelect:\n1: Return to Menu";
	anum = get_input("num",1,1);
}

int main() {
	app_init();
	while(true){
		cout << "\nWELCOME TO THE CASINO!\n";
		cout << "\nSELECT:";
		cout << "\n1: Start";
		cout << "\n2: Continue";
		cout << "\n3: Quit";
		anum = get_input("num",1,3);
		if (anum==1){saveflag=false;go_lounge();}
		else if (anum==2){
			save_init();
			go_lounge();
		}
		else{break;}
	}
	cout << "\n\n***PROGRAM TERMINATED\n";
	return 0;
}

void check_players(){
	reset_objects();
	player.table = tbl;
	int qty; int x; int y; int minfunds = 1;
	switch (tbl){ // tbl = current game table
	case 1:
		x = 0; y = 3;
		break;
	case 2:
		x = 2; y = 4; minfunds = 10;
		break;
	case 3:
		x = 1; y = 2;
		break;
	case 4:
		x = 0; y = 6; minfunds = 0;
		break;
	}
	std::uniform_int_distribution<int> gen(x,y); //Random number b/w x and y
	qty = gen(mt);
	int count = 0;
	std::uniform_int_distribution<int> gen2(0,1);
	while(count<qty){
		for (auto& gmr : gamerlist){
			int r = gen2(mt);
			Gamer &g = *gmr;
			if ((g.tables(tbl))and(g.table==0)and(r==1)
					and(g.funds>=minfunds)){
				g.table = tbl;
				count++;
			}
			if (count==qty){break;}
		}
	}
}

void update_players(){
	int tblmin; // min includes player
	if (tbl==1){tblmin=1;}
	else if (tbl==2){tblmin=3;}
	else if (tbl==3){tblmin=2;}
	else if (tbl==4){tblmin=1;}
	int qty = 0; for (auto& gmr : gamerlist){
		Gamer &g = *gmr;
		if (g.table==tbl){qty++;}
	}
	if (qty<tblmin){check_players();}
}

void print_players(){
	cout<<"\nCurrent players:";
	std::vector<Gamer*> players;
	int qty = 0; for (auto& gmr : gamerlist){
		Gamer &g = *gmr;
		if (g.table==tbl){players.push_back(gmr);qty++;}
	}
	if (qty==1){cout << " Just you!";}
	else {
		int i = 0; for (auto& gmr : players){
			Gamer &g = *gmr;
			cout << " " << g.name;
			if (i<qty-1){cout << ",";}
			i++;
		}
	}
	cout << "\n";
}

void go_table(int t){ // Sends player to chosen game table.
	switch(t){
	case 1:
		tbl = 1;
		go_coinflip();
		break;
	case 2:
		tbl = 2;
		go_pot();
		break;
	case 3:
		tbl = 3;
		go_zener();
		break;
	case 4:
		tbl = 4;
		go_fish();
		break;
	}
}

void go_lounge(){
	while (true){
		tbl = 0;
		reset_objects();
		cout << "\nYOU HAVE ENTERED THE LOUNGE\n";
		cout << "\nFunds: "<<player.funds<<"\n";
		cout << "\nSelect:";
		cout << "\n1: Coin Flip";
		cout << "\n2: Gamble Pot";
		cout << "\n3: Zener Cards";
		cout << "\n4: Fishing Hole";
		cout << "\n5: Prize Booth";
		cout << "\n\n6: Quit";
		int anum; anum = get_input("num",1,6);
		if (anum<5){go_table(anum);}
		else if (anum==5){go_prizebooth();}
		else{
			cout << "\nSELECT:\n1: Save and Quit\n2: Quit\n3: Cancel";
			anum = get_input("num",1,3);
			if (anum==1){make_save();break;}
			else if (anum==2){
				cout<<"\nAre you sure you want to quit without saving?";
				cout<<"\n1: Yes\n2: Cancel";
				anum = get_input("num",1,2);
				if (anum==1){break;} else{continue;}
			}
		}
	}
}

void go_prizebooth(){
	while(true){
		cout << "\n\nPRIZE BOOTH\n";
		cout << "\nFunds: " << player.funds;
		cout << "\n\nThere's no one here right now..."; // i'll do it later
		cout << "\n\nSelect:\n1: Return to Lounge";
		anum = get_input("num",1,1);if(anum==1){break;}
	}
}

void go_coinflip(){
	check_players();
	while (true){
		cout << "\n\nYOU HAVE JOINED THE COIN FLIP TABLE\n";
		update_players(); print_players();
		cout << "\nFunds: " << player.funds;
		cout << "\n\nSelect: \n";
		cout << "1: Play Coin Flip\n";
		cout << "2: Read rules\n";
		cout << "3: Return to Lounge";
		anum = get_input("num",1,3);
		if (anum==1){
			if (player.funds>0){play_coinflip();}
			else{cout<<"\nYou must have at least 1 gold to play.\n";}
		}
		else if (anum==2){read_rules(1);}
		else{break;};
	}
}

void play_coinflip(){
	std::vector<Gamer*> players; int qty = 0;
	for (auto& gmr : gamerlist){
		Gamer &g = *gmr;
		if (g.table==1){players.push_back(gmr);qty++;}
	}
	int bets[qty]; int guesses[qty];
	cout << "\nFunds: " << player.funds;
	cout << "\n\nThe dealer is preparing to flip the coin...\n";
	cout << "\nPlace your bet: ";
	cout << "\n1: Heads";
	cout << "\n2: Tails";
	guesses[0] = get_input("num",1,2);
	bets[0] = get_bet(1,player.funds);
	std::uniform_int_distribution<int> toss(1,2); int side = toss(mt);
	int i = 0; for (auto& gmr : players){
		Gamer &g = *gmr;
		if (g.name!="You"){
			int s = g.do_coinflip(side); guesses[i] = s;
			int b = g.do_bet(1); bets[i] = b;
		}
		i++;
	}
	int winners = 0;
	i = 0; for (auto& gmr : players){
		Gamer &g = *gmr;
		if (guesses[i]==side){winners++;}
		cout << "\n" << g.name << " bet " << bets[i] << " gold on ";
		if(guesses[i]==1){
			cout<<"Heads";}
		else{
			cout<<"Tails";}
		i++;
	}
	cout << "\n\nSelect: \n1: Continue"; anum = get_input("num",1,1);
	cout << "\nThe dealer has flipped the coin!"; string result;
	if (side==1){cout << "\n         It's HEADS!\n\n";}
	else {cout << "\n         It's TAILS!\n\n";}
	i = 0; for (auto& gmr : players){
		Gamer &g = *gmr;
		if (guesses[i]==side){
			double q = qty; double fact = q/2;//otherwise calculates wrong
			int w = (bets[i]*fact)/winners; if (w<1){w=1;}
			g.funds += w;
			if(i==0){cout << "You won " << w << " gold!\n";}
		}
		else {
			g.funds -= bets[i];
			if (i==0){cout << "You lost " << bets[0] << " gold.\n";}
		}
		if ((g.funds==0)and(i!=0)){g.table=0;}
		i++;
	}
	cout << "\nSelect: \n1: Return to Table"; get_input("num",1,1);
}

void go_pot(){
	check_players();
	while (true){
		cout << "\n\nYOU HAVE JOINED THE GAMBLING POT TABLE\n";
		update_players(); print_players();
		cout << "\nFunds: " << player.funds;
		cout << "\n\n";
		cout << "Select: \n";
		cout << "1: Play\n";
		cout << "2: Read rules\n";
		cout << "3: Return to Lounge";
		anum = get_input("num",1,3);
		if (anum==1){
			if (player.funds>10){play_pot();}
			else{cout<<"You must have at least 10 gold to play.\n";}
		}
		else if (anum==2){read_rules(2);continue;}
		else{break;}
	}
}

void play_pot(){
	int potmax = 10;
	int round = 1; while(true){
		std::vector<Gamer*> players; int qty = 0;
		for (auto& gmr : gamerlist){
			Gamer &g = *gmr;
			if (g.table==2){players.push_back(gmr);qty++;}
		}
		int stakes[qty]; int sizebets[qty]; int pot = 0;
		cout << "\nROUND "<< round;
		cout << "\nThe maximum stake is "<<potmax<<" gold.\n";
		cout << "\nFunds: "<<player.funds<<"\n";
		stakes[0] = get_bet(1,potmax); // edit this later to look right
		int i = 0; for (auto& gmr : players){
			if(i!=0){
				Gamer &g = *gmr;
				stakes[i] = g.do_potstake(potmax);
			}
			pot += stakes[i]; // adding the stake to the pot
			i++;
		}
		cout << "\nAll "<<qty<<" players have placed their bets.";
		sizebets[0] = get_potbet(qty,potmax*qty);
		int winners=0;
		std::vector<Gamer*> bigwlist; int bigw=0; int highbet = 1;
		i = 0; for (auto& gmr : players){
			Gamer &g = *gmr;
			if(i!=0){sizebets[i] = g.do_potguess(pot,potmax,qty);}
			cout << "\n" << g.name << " guessed a pot size of ";
			cout << sizebets[i] << " gold.";
			if (sizebets[i]==pot){bigwlist.push_back(gmr);bigw++;}
			if (sizebets[i]<=pot){
				winners++;
				if (sizebets[i]>highbet){highbet=sizebets[i];}
			}
			i++;
		}
		cout<<"\n\nSelect:\n1: Continue";get_input("num",1,1);
		cout<<"\nThe dealer opens the pot:\n        "<<pot<<" GOLD\n";
		// Exact match winners::
		if (bigw>0){
			double potd = pot; double wind = bigw;
			cout << "\n";
			i = 0; for (auto& gmr : bigwlist){
				Gamer &g = *gmr;
				int payout = potd/wind; g.funds += payout;
				cout << g.name << " ";
				if (i+1<bigw){cout << "and ";}
				else {cout << "won " << payout << " gold!";}
				i++;
			}
		}
		// Non-match winners and low guessers:
		else if (winners>0){ // used only when there is no exact match
			std::vector<Gamer*> winnerlist; int wincount = 0;//real winners
			i = 0; for (auto& gmr : players){
				if(sizebets[i]==highbet){
					winnerlist.push_back(gmr);
					wincount++;
				}
				i++;
			}
			double highd = highbet; double wind = wincount;
			int payout = highd/wind; // payout for highest bet winners
			cout << "\n";
			i = 0; for (auto& gmr: winnerlist){
				Gamer &g = *gmr;
				g.funds += payout;
				cout << g.name << " ";
				if (i+1<wincount){cout << "and ";}
				else {cout << "won " << payout << " gold!";}
				i++;
			}
			double potd = pot; double remain = potd - highd;
			double winnersd = winners;
			double payout2d = remain/(winnersd-wind);int payout2 = payout2d;
			if (payout2<1){payout2=1;} // no "won 0 gold" in this house
			int lc = 1; // tracking order of low guessers for cout stuff
			cout << "\n";
			i = 0; for (auto& gmr: players){
				Gamer &g = *gmr;
				if (sizebets[i]<highbet){ // only lower (non-win) guessers
					g.funds -= stakes[i];
					g.funds += payout2;
					cout << g.name << " ";
					if (lc<(winners-wincount)){cout << "and ";}
					else {cout << "won back " << payout2 << " gold.";}
					lc++;
				}
				i++;
			}
		}
		// Players who overestimated and lose their bets:
		///// used even after winners, but only if needed
		if (winners!=qty){
			i = 0; for (auto& gmr : players){
				Gamer &g = *gmr;
				if (sizebets[i]>pot){ // only players that overestimated
					g.funds -= stakes[i];
					cout << "\n" << g.name << " lost "<<g.gena;
					cout << " bet of " << stakes[i] << " gold.";
				}
				else if ((bigw>0)and(sizebets[i]<pot)){
					g.funds -= stakes[i];
					cout << "\n" << g.name << " lost "<<g.gena;
					cout << " bet of " << stakes[i] << " gold.";
				}
				i++;
			}
		}
		cout<<"\n\nFunds: "<<player.funds;
		cout << "\n";
		// Checking if anyone ran out of gold:
		int bye = 0; // counter of ppl who leave
		i = 0; for (auto& gmr : players){
			Gamer &g = *gmr;
			if (g.funds==0){
				cout << "\n" << g.name;
				cout << " ran out of funds and must leave the table.\n";
				g.table = 0;
				bye++;
			}
			i++;
		}
		if (player.funds==0){break;}
		if (qty-bye<3){
			cout<<"\nThere aren't enough players left to continue.";
			cout<<"\n\nSelect:\n1: Return to Table";
			get_input("num",1,1); break;
		}
		cout<<"\nThe maximum stake will be "<<pot<<" in the next round.";
		cout << "\n\nKeep playing?\n1: Continue\n2: Leave game";
		anum = get_input("num",1,2);
		if (anum==1){potmax = pot; round++;}
		else{break;}
	}
}

void go_zener(){
	check_players();
	while (true){
		cout << "\n\nYOU HAVE JOINED THE ZENER CARD TABLE\n";
		update_players(); print_players();
		cout << "\nFunds: " << player.funds;
		cout << "\n\nSelect: \n";
		cout << "1: Play Zener Cards\n";
		cout << "2: Read rules\n";
		cout << "3: Return to Lounge";
		anum = get_input("num",1,3);
		if (anum==1){
			if(player.funds>0){play_zener();}
			else{cout<<"\nYou must have at least 1 gold to play.\n";}
		}
		else if (anum==2){read_rules(3);}
		else{break;};
	}
}

void play_zener(){
	while (true){
		int qty;
		std::vector<Gamer*> players; qty = 0;
		for (auto& gmr : gamerlist){
			Gamer &g = *gmr;
			if (g.table==3){players.push_back(gmr);qty++;}
		}
		cout << "\nFunds: " << player.funds;
		zener_turn0(players,qty); // DEALER TURN
		players.clear(); qty = 0; // in case anyone lost all funds
		for (auto& gmr : gamerlist){
			Gamer &g = *gmr;
			if (g.table==3){players.push_back(gmr);qty++;}
		}
		if (qty<2){
			cout<<"\nThere aren't enough players left to continue the game.";
			cout<<"\nSelect:\n1: Return"; anum = get_input("num",1,1);
			break;
		}
		else if (player.funds==0){break;}
		cout << "\nFunds: " << player.funds;
		cout << "\n\nSelect:\n1: Next turn"; anum = get_input("num",1,1);
		zener_turn1(players,qty); // PLAYER TURN
		players.clear(); qty = 0;
		for (auto& gmr : gamerlist){
			Gamer &g = *gmr;
			if (g.table==3){players.push_back(gmr);qty++;}
		}
		if (qty<2){
			cout<<"\nThere aren't enough players left to continue the game.";
			cout<<"\nSelect:\n1: Return"; anum = get_input("num",1,1);
			break;
		}
		else if (player.funds==0){break;}
		for (int i=1;i<qty;i++){ // NPC TURNS
			cout << "\nFunds: " << player.funds;
			cout << "\n\nSelect:\n1: Next turn"; anum = get_input("num",1,1);
			zener_turn2(players,qty,i);
			players.clear(); qty = 0;
			for (auto& gmr : gamerlist){
				Gamer &g = *gmr;
				if (g.table==3){players.push_back(gmr);qty++;}
			}
			if (qty<2){
				cout<<"\nThere aren't enough players left to continue the game.";
				cout<<"\nSelect:\n1: Return"; anum = get_input("num",1,1);
				break;
			}
			else if (player.funds==0){break;}
		}
		cout << "\nThe game has ended.\n";
		cout << "\n1: Return to Table"; get_input("num",1,1);
		break;
	}
}

void zener_turn0(std::vector<Gamer*> players,int qty){
	std::uniform_int_distribution<int> card(1,5); // Selecting card
	int color = card(mt); int shape = card(mt);
	int colorbets[qty]; int shapebets[qty]; int bets[qty];
	cout << "\n\nThe dealer has drawn a card...\n";
	cout << "\nPredict the color:";
	cout << "\n1: Black" << "\n2: Red" << "\n3: Green";
	cout << "\n4: Blue" << "\n5: Yellow";
	colorbets[0] = get_input("num",1,5);
	cout << "\nPredict the shape:";
	cout << "\n1: Circle" << "\n2: Square" << "\n3: Cross";
	cout << "\n4: Waves" << "\n5: Star";
	shapebets[0] = get_input("num",1,5);
	int i = 0;for (auto& gmr : players){
		Gamer &g = *gmr;
		if (i!=0){
			int c; int s;
			std::tuple <int,int> tie = g.do_zener(color,shape);
			colorbets[i] = c; shapebets[i] = s; bets[i] = g.do_bet(3);
		}
		i++;
	}
	string colorlist[5] = {"BLACK","RED","GREEN","BLUE","YELLOW"};
	string shapelist[5] = {"CIRCLE","SQUARE","CROSS","WAVES","STAR"};
	cout << "\nDo you want to peek?\n1: Yes\n2: No";
	int peek = get_input("num",1,2); // here peek is y/n whether you peek
	if (peek==1){
		cout << "\nSelect player:";
		int i = 0;for (auto& gmr : players){
			Gamer &g = *gmr;
			if (i!=0){cout << "\n" << i << ": " << g.name;}
			i++;
		}
		cout << "\n" << qty << ": Cancel";
		peek = get_input("num",1,qty); int boo; //here peek = npc peeked at
		if (peek<=qty-1){
			Gamer* gmr = players[peek]; Gamer &g = *gmr;
			std::uniform_int_distribution<int> look(1,4); boo = look(mt);
			if (boo==1){ // Peek at color
				cout << "\nYou peek at "<<g.name<<" and see "<<g.nom;
				cout << " has written "<<colorlist[colorbets[peek]-1];
			}
			else if (boo==2){ //Peek at shape
				cout << "\nYou peek at "<<g.name<<" and see "<<g.nom;
				cout << " has written "<<shapelist[shapebets[peek]-1];
			}
			else if (boo==3){ //Peek at color and shape!
				cout << "\nYou peek at "<<g.name<<" and see "<<g.nom;
				cout << " has written "<<colorlist[colorbets[peek]-1]<<
						" "<<shapelist[shapebets[peek]-1];
			}
			else{ // Caught peeking!
				cout << "\nYou were caught peeking!\n";
				cout << "\nYou lost ";
				if (player.funds<50){cout << player.funds;player.funds=0;}
				else {cout<<50; player.funds-=50;}
				cout << " gold.";
			}
			cout << "\n";
		}
	}
	if (peek!=4){ //if pc did not peek, this refers y/n = 1/2 (no conflict)
		cout<<"\nFunds: "<<player.funds << "\n";
		bets[0] = get_bet(1,1000); // no betting when caught
	}
	i = 0; for (auto& gmr : players){
		if ((i==0)and(peek==4)){i++;continue;} //skip player caught peeking
		Gamer &g = *gmr;
		string c = colorlist[colorbets[i]-1];
		string s = shapelist[shapebets[i]-1];
		int b = bets[i];
		cout << "\n" << g.name << " predicted " << c << " " << s;
		cout << " for " << b << " gold.";
		i++;
	}
	cout << "\n\nSelect:\n1: Continue"; anum = get_input("any");
	cout << "\nThe dealer has revealed the card:\n         ";
	cout << colorlist[color-1] << " " << shapelist[shape-1] << "\n\n";
	i = 0; for (auto& gmr : players){
		Gamer &g = *gmr;
		if ((i==0)and(peek==4)){} //skip player caught peeking
		else if ((colorbets[i]==color)and(shapebets[i]==shape)){
			int w = bets[i]*5;
			g.funds += w;
			cout << g.name << " won " << w << " gold!\n";
		}
		else if ((colorbets[i]==color)or(shapebets[i]==shape)){
			int w = bets[i]*2;
			g.funds += w;
			cout << g.name << " won " << w << " gold!\n";
		}
		else {
			g.funds -= bets[i];
			cout << g.name << " lost " << bets[i] << " gold.\n";
		}
		if (g.funds==0){
			cout << "\n" << g.name;
			cout << " ran out of funds and must leave the table.\n";
			g.table = 0;
			if (i==0){break;}
		}
		i++;
	}
}

void zener_turn1(std::vector<Gamer*> players,int qty){
	cout << "\n\nIt is your turn!";
	std::uniform_int_distribution<int> card(1,5); // Selecting card
	int color = card(mt); int shape = card(mt);
	int colorbets[qty]; int shapebets[qty]; int bets[qty];
	string colorlist[5] = {"BLACK","RED","GREEN","BLUE","YELLOW"};
	string shapelist[5] = {"CIRCLE","SQUARE","CROSS","WAVES","STAR"};
	cout <<"\n\nSelect:\n1: Draw card"; anum = get_input("num",1,1);
	cout << "\nYou have drawn ";
	cout << colorlist[color-1] << " " << shapelist[shape-1] << "\n";
	int i = 0;for (auto& gmr : players){ // NPCs make predictions
		Gamer &g = *gmr;
		if (i!=0){
			int c; int s;
			std::pair <int,int> tie = g.do_zener(color,shape);
			colorbets[i] = c; shapebets[i] = s; bets[i] = g.do_bet(3);
		}
		i++;
	}
	i = 0; for (auto& gmr : players){ // Listing NPC bets
		if (i!=0){
			Gamer &g = *gmr;
			string c = colorlist[colorbets[i]-1];
			string s = shapelist[shapebets[i]-1];
			int b = bets[i];
			cout << "\n" << g.name << " made a prediction for ";
			cout << b << " gold.";
		}
		i++;
	}
	int playerbets[qty]; playerbets[0]=0;
	cout << "\n";
	i = 0; for (auto& gmr : players){ // Player predicts NPC accuracy
		if (i!=0){
			Gamer &g = *gmr;
			cout << "\nPredict " << g.name << "'s answer:";
			cout << "\n1: Incorrect color and shape";
			cout << "\n2: Correct color or shape";
			cout << "\n3: Correct color and shape";
			playerbets[i] = get_input("num",1,3);
		}
		i++;
	}
	int bonus = 0; // var for player bonus
	i = 0; for (auto& gmr : players){ // Calculating winnings
		if (i!=0){
			Gamer &g = *gmr;
			if ((colorbets[i]==color)and(shapebets[i]==shape)){
				int w = bets[i]*5;
				g.funds += w;
				cout << g.name << " predicted ";
				cout << colorlist[colorbets[i]-1] << " ";
				cout << shapelist[shapebets[i]-1];
				cout << " and won " << w << " gold!\n";
				if (playerbets[i]==3){bonus+=100;}
				//else if (playerbets[i]==2){bonus+=10;}
			}
			else if ((colorbets[i]==color)or(shapebets[i]==shape)){
				int w = bets[i]*2;
				g.funds += w;
				cout << g.name << " predicted ";
				cout << colorlist[colorbets[i]-1] << " ";
				cout << shapelist[shapebets[i]-1];
				cout << " and won " << w << " gold!\n";
				if (playerbets[i]==2){bonus+=30;}
			}
			else {
				g.funds -= bets[i];
				cout << g.name << " predicted ";
				cout << colorlist[colorbets[i]-1] << " ";
				cout << shapelist[shapebets[i]-1];
				cout << " and lost " << bets[i] << " gold.\n";
				if (playerbets[i]==1){bonus+=15;}
			}
			if (g.funds==0){
				cout << "\n" << g.name;
				cout << " ran out of funds and must leave the table.\n";
				g.table = 0;
			}
		}
		i++;
	}
	player.funds += bonus;
	if (bonus==0){cout<<"\nYou didn't win a bonus this round.\n";}
	else{cout<<"\nYou won a " << bonus << " gold bonus this round!\n";}
}

void zener_turn2(std::vector<Gamer*> players,int qty, int d){
	Gamer* gmr = players[d];
	Gamer &dealer = *gmr;  // current npc dealer
	std::uniform_int_distribution<int> card(1,5); // Selecting card
	int color = card(mt); int shape = card(mt);
	int colorbets[qty]; int shapebets[qty]; int bets[qty];
	cout << "\n\n" << dealer.name << " has drawn a card...\n";
	cout << "\nPredict the color:";
	cout << "\n1: Black" << "\n2: Red" << "\n3: Green";
	cout << "\n4: Blue" << "\n5: Yellow";
	colorbets[0] = get_input("num",1,5);
	cout << "\nPredict the shape:";
	cout << "\n1: Circle" << "\n2: Square" << "\n3: Cross";
	cout << "\n4: Waves" << "\n5: Star";
	shapebets[0] = get_input("num",1,5);
	cout<<"\nFunds: "<<player.funds << "\n";
	bets[0] = get_bet(1,1000);
	int i = 0;for (auto& gmr : players){
		Gamer &g = *gmr;
		if ((i!=0)and(i!=d)){ // skip dealer and player
			int c; int s;
			std::tuple <int,int> tie = g.do_zener(color,shape);
			colorbets[i] = c; shapebets[i] = s; bets[i] = g.do_bet(3);
		}
		if (i!=d){
			//Passes boolean whether the other player is correct
			dealer.do_zener_deal(
					(colorbets[i]==color),(shapebets[i]==shape));
		}
		i++;
	}
	string colorlist[5] = {"BLACK","RED","GREEN","BLUE","YELLOW"};
	string shapelist[5] = {"CIRCLE","SQUARE","CROSS","WAVES","STAR"};
	i = 0; for (auto& gmr : players){
		if (i!=d){ // skip the dealer
			Gamer &g = *gmr;
			string c = colorlist[colorbets[i]-1];
			string s = shapelist[shapebets[i]-1];
			int b = bets[i];
			cout << "\n" << g.name << " predicted " << c << " " << s;
			cout << " for " << b << " gold.";
		}
		i++;
	}
	cout << "\n\nSelect:\n1: Continue"; anum = get_input("num",1,1);
	cout << "\n" << dealer.name << " has revealed the card:\n         ";
	cout << colorlist[color-1] << " " << shapelist[shape-1] << "\n\n";
	i = 0; for (auto& gmr : players){
		if (i!=d){ // skip the dealer
			Gamer &g = *gmr;
			if ((colorbets[i]==color)and(shapebets[i]==shape)){
				int w = bets[i]*5;
				g.funds += w;
				cout << g.name << " won " << w << " gold!\n";
			}
			else if ((colorbets[i]==color)or(shapebets[i]==shape)){
				int w = bets[i]*2;
				g.funds += w;
				cout << g.name << " won " << w << " gold!\n";
			}
			else {
				g.funds -= bets[i];
				cout << g.name << " lost " << bets[i] << " gold.\n";
			}
			if (g.funds==0){
				cout << "\n" << g.name;
				cout << " ran out of funds and must leave the table.\n";
				g.table = 0;
			}
		}
		i++;
	}
}

void go_fish(){
	check_players();
	while (true){
		cout << "\n\nYOU HAVE JOINED THE FISHING HOLE KIOSK\n";
		print_players();
		cout << "\nFunds: " << player.funds;
		cout << "\n\nSelect:";
		cout << "\n1: Start Fishing";
		cout << "\n2: Check Basket";
		cout << "\n3: Rewards Booth";
		cout << "\n4: Read rules";
		cout << "\n5: Return to Lounge";
		anum = get_input("num",1,5);
		if (anum==1){play_fish();}
		else if (anum==2){
			player.list_fish();
			cout<<"\nSelect:\n1: Back"; anum=get_input("any");
		}
		else if(anum==3){fish_reward();}
		else if (anum==4){read_rules(4);continue;}
		else{break;};
	}
}

void play_fish(){
	while (true){
		cout << "\nTHE FISHING HOLE\n";
		cout << "\nSelect:";
		cout << "\n1: Cast your line";
		cout << "\n2: Check Basket";
		cout << "\n3: Return to Kiosk";
		anum = get_input("num",1,3);
		if (anum==1) {catch_fish();}
		else if (anum==2){
			player.list_fish();
			cout<<"\nSelect:\n1: Back"; anum=get_input("any");
		}
		else{break;}
	}
}

void catch_fish(){
	std::vector<Gamer*> players; int qty = 0;
	for (auto& gmr : gamerlist){
		Gamer &g = *gmr;
		if (g.table==4){players.push_back(gmr);qty++;}
	}
	while (true){
		int nobite = true; for (auto& gmr : players){
			Gamer &g = *gmr;
			int rank = g.do_fish();
			int fish;
			if (rank>299){
				nobite = false; //tracks whether anything was caught
				cout << "\n"<< g.name << " caught something";
				fish = get_fish(rank);
				int bribe = bribe_fish(rank);
				if (g.nom=="you"){
					if (rank<360){cout<<"...";}
					else if (rank<385){cout<<"... You have a good feeling"
							" about this one.";}
					else if (rank<395){cout<<"... The line is shaking"
						" excitedly.";}
					else if (rank<399){cout<<"... It's tugging fiercely!";}
					else {cout<<"... There is an ominous silence...";}
					cout << "\n\nSelect:\n1: Reel in";
					cout << "\n2: Toss in a bribe";
					bool didcatch=false; anum = get_input("num",1,2);
					if (anum==2){
						int pb = get_bribe(); g.funds -= pb;
						cout<<"\nYou tossed in "<<pb<<" gold.\n";
						if (pb >= bribe){didcatch=true;}
					}
					if (not didcatch){didcatch = g.do_reel(rank);}
					if (didcatch){
						cout << "\nYou reeled in " << allfish[fish]<<"!";
						g.add_fish(fish);
					}
					else{cout << "\nIt got away...";}
				}
				else{
					cout << "!";
					// have to set up bribe here too (or not)
					if (g.do_reel(rank)){
						g.add_fish(fish);
						g.sell_fish(fish,1); //immediate cash-in for npcs
					}
					else{cout<<" But it got away.";}
				}
			}
			if (not nobite){break;}
		}
		if (nobite){cout<<"\nNobody caught anything...";}
		cout << "\n\nSelect:\n1: Keep fishing\n2: Take a break";
		anum = get_input("num",1,2);
		if (anum==2){break;}
	}
}

int get_fish(int r){
	int size; int fish;
	if (r<360){
		size = sizeof(commonfish)/sizeof(commonfish[0]);
		std::uniform_int_distribution<int> sel(0,size-1); int f = sel(mt);
		fish = commonfishref[f];}
	else if (r<385){
		size = sizeof(uncommonfish)/sizeof(uncommonfish[0]);
		std::uniform_int_distribution<int> sel(0,size-1); int f = sel(mt);
		fish = uncommonfishref[f];}
	else if (r<395){
		size = sizeof(rarefish)/sizeof(rarefish[0]);
		std::uniform_int_distribution<int> sel(0,size-1); int f = sel(mt);
		fish = rarefishref[f];}
	else if (r<399){
		size = sizeof(ultrararefish)/sizeof(ultrararefish[0]);
		std::uniform_int_distribution<int> sel(0,size-1); int f = sel(mt);
		fish = ultrararefishref[f];}
	else {fish = sizeof(allfish)/sizeof(allfish[0])-1;}
	return fish;
}

int bribe_fish(int r){ // checks the fish rarity and returns a bribe offer
	int mini; int maxi; double mean; double stddev;
	if (r<360){mini=15;maxi=35;mean=25.5;stddev=4;}
	else if (r<385){mini=100;maxi=150;mean=125.5;stddev=9;}
	else if (r<295){mini=300;maxi=400;mean=350.5;stddev=16;}
	else if (r<399){mini=650;maxi=850;mean=750.5;stddev=29;}
	else {mini=100;maxi=150;mean=125.5;stddev=9;}
	/* Legendary uses the same bribe pool as uncommon, but it's defined
	 * separately in case it changes later. */
	std::normal_distribution<double> ranger(mean,stddev);
	int b = ranger(mt);
	if (b<mini){b=mini;} if (b>maxi){b=maxi;}
	return b;
}

void fish_reward(){
	while (true){
		cout << "\nFISHING HOLE REWARDS BOOTH\n";
		cout << "\nFunds: " << player.funds << "\n";
		cout << "\n1: Select from basket";
		cout << "\n2: Sell all fish";
		cout << "\n3: Return to Kiosk";
		anum = get_input("num",1,3);
		if ((anum!=3)and(not player.has_fish())){
			cout << "\nYou have nothing to trade in.\n";
		}
		else if (anum==1){
			int basket[pond]; // provides ref for input-based selection
			cout << "\n\nSelect an item to sell:";
			int fishnum = 0; for (int i=0;i<pond;i++){
				if (player.check_fish(i)!=0){
					cout << "\n" << ++fishnum << ": " << allfish[i];
					cout << " ("<<player.check_fish(i)<<")";
					basket[fishnum] = i;
				}
			}
			anum = get_input("num",1,fishnum); int fishy = basket[anum];
			int maxqty = player.check_fish(fishy);
			int qty = 1; if (maxqty>1){
				cout << "\nYou have "<<maxqty<<" "<<allfish[fishy]<<".";
				cout << "\nHow many would you like to sell?";
				qty = get_input("num",0,maxqty);
			}
			if (qty>0){
				cout << "\nSell " << qty << " " << allfish[fishy];
				cout << " for " << allfishvalue[fishy]*qty << " gold?";
				cout << "\n1: Yes\n2: Cancel"; anum = get_input("num",1,2);
				if (anum==1){
					player.sell_fish(fishy,qty);
					//cout << "\nFunds: " << player.funds << "\n";
				}
			}
		}
		else if(anum==2){
			cout << "\n\nSell everything in your basket for ";
			cout << player.price_basket() << " gold?";
			cout << "\n1: Yes\n2: Cancel"; anum = get_input("num",1,2);
			if (anum==1){
				player.sell_basket();
				//cout << "\nFunds: " << player.funds << "\n";
			}
		}
		else{break;}
	}
}
