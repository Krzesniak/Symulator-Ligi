#include <iostream>
#include <iomanip>
#include <time.h>
#include <windows.h>
#include <vector>
#include <unordered_set>
#include <list>
#include <limits>
#include <random>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <cassert>
using namespace std;

template <class RandomAccessIterator>
void sortuj (RandomAccessIterator first, RandomAccessIterator last)
{
    for(RandomAccessIterator i=last-1;i>first;i--)
    {
        for(RandomAccessIterator j=first;j<i;j++)
        {
            if(*(j+1)<*(j))
                swap(*(j),*(j+1));
        }
    }
}

template <class RandomAccessIterator, class Compare>
void sortuj (RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    for(RandomAccessIterator i=last-1;i>first;i--)
    {
        for(RandomAccessIterator j=first;j<i;j++)
        {
            if(comp(*(j+1),*(j)))
                swap(*(j),*(j+1));
        }
    }
}

int dni_lutego(int rok)
{
    if(rok%4)
        return 28;
    else
    {
        if(rok%100)
            return 29;
        else
        {
            if(rok%400)
                return 28;
            else
                return 29;
        }
    }
}

vector<int> generator_liczb_z_zakresu(int a, int b, int ile)//[a,b)
{
    vector <int> v;
    for(int i=0;i<ile;i++)//zalozmy ze [2,7) rand()%5 czyli od 0 do 4 wygeneruje +2 czyli od 2 do 6
        v.push_back(rand()%(b-a)+a);

    return v;
}

/*
template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    uniform_int_distribution<> dis(0, distance(start, end) - 1);
    advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static mt19937 gen(time(0));
    return select_randomly(start, end, gen);
}
*/
class Tabela;
class Mecz;
class Pilkarz
{
public:
    string nazwa, nazwa_klubu;
    int ilosc_bramek;
    public:
    Pilkarz(string name, string klub): nazwa{name}, nazwa_klubu{klub} {ilosc_bramek =0;}
    void strzelenie_bramki(int ilosc)
    {
        ilosc_bramek += ilosc;
    }
    int getter_ilosc_bramek()
    {
        return ilosc_bramek;
    }
    string getter_nazwa_pilkarza() {return nazwa;}
    string getter_nazwa_klubu() {return nazwa_klubu;}

};
class Druzyna
{
public:
    friend Tabela;
    friend Mecz;
    vector<string> Forma;
    string nazwa;
    int sila_zawodnikow;
    int miejsce_w_tabeli;
    vector<Pilkarz*> zawodnicy;
    int ilosc_pkt, ilosc_Zgoli, ilosc_Sgoli ; //Zgoli = zdobyte_gole Sgoli = stracone_gole
    int ilosc_w, ilosc_p,ilosc_r ;
    int ilosc_meczow;
    void dodowanie_zawodnikow(unordered_set<int>&used)
    {
        vector<string> lines;
        fstream plik;
        string linia_pliku;
        string nazwa_do_szukania = nazwa;
        plik.open("zawodnicy.txt", ios::in);
        if(!plik.good())
        {
            cout<<"Nie udalo sie otworzyc pliku! ";
            exit(0);
        }
        int licznik_linii = 0;
        int nr_linijki_zawierajaca_zawodnikow = -1;
         string zawodnik;
        transform(nazwa_do_szukania.begin(), nazwa_do_szukania.end(), nazwa_do_szukania.begin(), ::tolower);
        while(getline(plik, linia_pliku))
        {
            lines.push_back(linia_pliku);
            if(nazwa_do_szukania.find(linia_pliku) != string::npos) {nr_linijki_zawierajaca_zawodnikow = licznik_linii + 1; }
            licznik_linii++;
        }
        plik.clear(); plik.seekg(0);
        if(nr_linijki_zawierajaca_zawodnikow != -1)
        {
            licznik_linii = 0;
            while(getline(plik, linia_pliku))
            {
                if(nr_linijki_zawierajaca_zawodnikow == licznik_linii+1)
                {
                 for (int i=0; i<5; i++)
                 {
                     plik>>zawodnik;
                     Pilkarz* nowy = new Pilkarz(zawodnik, nazwa);
                     zawodnicy.push_back(nowy);
                 }

                 plik >> sila_zawodnikow;
                }
                licznik_linii++;
            }
        }
        else {

             ifstream plik2("zawodnicy_random.txt");

             if(!plik2.good())
             {
                 cout<<"Nie udalo sie otworzyc pliku2! ";
                 exit(0);
             }

             int random;

             do
             {
                random=rand()%4000;
             }
             while(used.count(random));

             used.insert(random);

             for(int i=0;i<random;i++)
             {
                 getline(plik2,linia_pliku);
             }

             for(int i=0;i<5;i++)
             {
                 plik2>>zawodnik;
                 Pilkarz* nowy = new Pilkarz(zawodnik, nazwa);
                 zawodnicy.push_back(nowy);
             }

             sila_zawodnikow=rand()%6;
        }

    }
    Druzyna(string name, unordered_set<int>&used) : nazwa{name}
    {
        dodowanie_zawodnikow(used);
        ilosc_pkt = 0; ilosc_Zgoli = 0; ilosc_Sgoli = 0; ilosc_w =0; ilosc_p =0; ilosc_r = 0; ilosc_meczow = 0;
    }
};
class Mecz
{
    friend Tabela;
    Druzyna* gospodarz;
    vector<Pilkarz*> strzelcy;
    Druzyna* gosc;
//    pair<string, string>czas_meczu;
    string data_meczu[5];

    int licznik_meczow , ilosc_strzelonych, ilosc_straconych;
    bool czy_zostal_rozegrany;
    vector<string>::iterator it;
    void dopisywanie_pkt()
    {
        gospodarz->ilosc_Zgoli += ilosc_strzelonych;
        gospodarz->ilosc_Sgoli += ilosc_straconych;
        gospodarz->ilosc_meczow++;
        gosc->ilosc_Zgoli += ilosc_straconych; //tutaj na odwrot
        gosc->ilosc_Sgoli += ilosc_strzelonych;
        gosc->ilosc_meczow++;
        if (ilosc_strzelonych > ilosc_straconych)
        {
            gospodarz->ilosc_pkt += 3;
            gospodarz->ilosc_w++;
            gosc->ilosc_p++;
            zmiana_formy_druzyny("Z", gospodarz);
            zmiana_formy_druzyny("P", gosc);

        }
       else if (ilosc_straconych == ilosc_strzelonych)
        {
            gospodarz->ilosc_pkt+=1;
            gospodarz->ilosc_r++;
            gosc->ilosc_pkt+=1;
            gosc->ilosc_r++;
            zmiana_formy_druzyny("R", gospodarz);
            zmiana_formy_druzyny("R", gosc);
        }
        else
        {
         gospodarz->ilosc_p++;
         gosc->ilosc_pkt +=3;
         gosc->ilosc_w++;
         zmiana_formy_druzyny("P", gospodarz);
         zmiana_formy_druzyny("Z", gosc);
        }

    }
    void dodawanie_bramek_pilkarzom(int gole_gospodarzy, int gole_gosci, default_random_engine &engine)
    {
        discrete_distribution<int> discrete_dist({70, 50, 30, 20, 5, });

        for (size_t i=0; i<gole_gospodarzy; i++)
        {
           int nr_pilkarza = discrete_dist(engine);
           gospodarz->zawodnicy[nr_pilkarza]->strzelenie_bramki(1);
           strzelcy.push_back(gospodarz->zawodnicy[nr_pilkarza]);

        }

        for (size_t i=0; i<gole_gosci; i++)
        {
           int nr_pilkarza = discrete_dist(engine);
           gosc->zawodnicy[nr_pilkarza]->strzelenie_bramki(1);
           strzelcy.push_back(gosc->zawodnicy[nr_pilkarza]);
        }
    }
    public:
    Mecz(Druzyna* team, Druzyna* Team2, int licznik, int liczba_dni, int liczba_godzin, int rok_start, int miesiac_start, int dzien_start) : gospodarz{team}, gosc{Team2}, licznik_meczow{licznik}
    {
        int dni_miesiaca[]={31,dni_lutego(rok_start),31,30,31,30,31,31,30,31,30,31};

        while(liczba_dni>=dni_miesiaca[miesiac_start-1])
        {
            liczba_dni-=dni_miesiaca[miesiac_start-1];

            miesiac_start++;
            if(miesiac_start>12)
            {
                dni_miesiaca[1]=dni_lutego(++rok_start);
                miesiac_start=1;
            }

        }

        dzien_start+=liczba_dni;

        data_meczu[0]=to_string(dzien_start);
        data_meczu[1]=to_string(miesiac_start);
        data_meczu[2]=to_string(rok_start);

        string godziny[] = {"14","15","16","17","18","19","20"};
        string minuty[] = {"00","15","30","45"};

        data_meczu[3] = godziny[liczba_godzin/4];
        data_meczu[4] = minuty[liczba_godzin%4];

        ilosc_strzelonych=0;
        ilosc_straconych=0;
        czy_zostal_rozegrany = false;
    }
    void zmiana_formy_druzyny(string wynik, Druzyna* Team)
    {
        it = Team->Forma.begin();
        Team->Forma.insert(it, wynik);
        if (Team->Forma.size()>5) {Team->Forma.pop_back(); }
    }
    void rozegraj_mecz_losowo(default_random_engine &engine)
    {
        const int stala = 10;
        czy_zostal_rozegrany = true;
        double bonus_sily=(gospodarz->sila_zawodnikow-gosc->sila_zawodnikow)*stala;
        if(bonus_sily > 0)
        {
             const double bonus = bonus_sily;
            discrete_distribution<int> discrete_dist({36-bonus/1.5 , 56-bonus/2, 42+bonus/2, 25+bonus/2, 7+bonus/2, 5+bonus/3, 5+bonus/6, 2+bonus/16, 1+bonus/20, });
            discrete_distribution<int> discrete_dist2({36+bonus*2.5, 56+bonus*1.3, 42-bonus/8, 25-bonus/4, 7, 5, 5, 2, 1, });
            ilosc_strzelonych = discrete_dist(engine);
            ilosc_straconych = discrete_dist2(engine);
        }
        else
        {
              const double bonus = -bonus_sily;
              discrete_distribution<int> discrete_dist({36-bonus/2 , 56-bonus/3, 42+bonus/2, 25+bonus/2, 7+bonus/2, 5+bonus/3, 5+bonus/6, 2+bonus/16, 1+bonus/20, });
              discrete_distribution<int> discrete_dist2({36+bonus*2.5, 56+bonus*1.3, 42-bonus/8, 25-bonus/4, 7, 5, 5, 2, 1, });
              ilosc_strzelonych = discrete_dist2(engine);
              ilosc_straconych = discrete_dist(engine);
        }
        dodawanie_bramek_pilkarzom(ilosc_strzelonych, ilosc_straconych,engine);
        dopisywanie_pkt();

    }
    void rozgrywaj_mecz_manualnie(default_random_engine &engine, list <int> &wyniki)
    {
        czy_zostal_rozegrany = true;
   //     cout<<"Podaj ilosc bramek strzelonych przez "<<gospodarz->nazwa<<": "; cin>>ilosc_strzelonych;
   //     cout<<"Podaj ilosc bramek srzelonych przez "<<gosc->nazwa<<": "; cin>>ilosc_straconych;
        ilosc_strzelonych=wyniki.front();
        wyniki.pop_front();
        ilosc_straconych=wyniki.front();
        wyniki.pop_front();
        dodawanie_bramek_pilkarzom(ilosc_strzelonych, ilosc_straconych,engine);
        dopisywanie_pkt();
    }
    void wyswietl_mecz(string napis="bez_szczegolow")
    {
       if(czy_zostal_rozegrany)
       {
           cout<<licznik_meczow<<". "<<setw(14)<<left<<gospodarz->nazwa<<" "<<ilosc_strzelonych<<" - "<<ilosc_straconych<<" "<<setw(13)<<right<<
                 gosc->nazwa<<"         TERMIN: " <<data_meczu[0]<<"."<<data_meczu[1]<< "." << data_meczu[2] << " " << data_meczu[3] << ":" << data_meczu[4] << endl;
           if(napis == "szczegoly")
           {
               if(ilosc_strzelonych != 0)
               {
                   cout<<"Bramki dla "<<gospodarz->nazwa<<" strzelili : ";
                   for (int i=0; i<ilosc_strzelonych; i++) { cout<<strzelcy[i]->getter_nazwa_pilkarza()<<" ";}
                   cout << endl;
               }
               if(ilosc_straconych != 0)
               {
                   cout<<"Bramki dla "<<gosc->nazwa<<" strzelili : ";
                   for(int i=ilosc_strzelonych; i<strzelcy.size(); i++) {cout<<strzelcy[i]->getter_nazwa_pilkarza()<<" ";}
                   cout<<endl;
               }
           }
       }

       else
       {
           cout<<licznik_meczow<<". "<<setw(15)<<left<<gospodarz->nazwa<<" vs " <<setw(15)<<right<<gosc->nazwa<<"         TERMIN: " <<data_meczu[0]<<"."<<data_meczu[1]<< "." << data_meczu[2] << " " << data_meczu[3] << ":" << data_meczu[4] << endl;
       }
    }

};
class Tabela
{
public:
    unsigned int ilosc_druzyn;
    size_t aktualna_kolejka;
    vector<pair<Pilkarz*, int>> pilkarz_ilosc_bramek;
    vector<Druzyna*> Druzyny;
    vector<vector<Mecz*>> Terminarz;
    default_random_engine engine;
    unordered_set<int>used;//uzyci randomowi zawodnicy
    size_t licznik_kolejek = 1;

    void uzupelnij_miejsce_w_tabeli()
    {
         for (unsigned int i=0; i<ilosc_druzyn; i++)
         {
             Druzyny[i]->miejsce_w_tabeli=i+1;
         }
    }

    int liczba_bramek_w_meczach_bezposrednich(string nazwa, string nazwa2)
    {
        int licznik=0;

        for (auto kolejka: Terminarz)
        {
            for (auto mecz: kolejka)
            {
                if(nazwa == mecz->gospodarz->nazwa && nazwa2 == mecz->gosc->nazwa)
                    licznik+=mecz->ilosc_strzelonych;

                if(nazwa == mecz->gosc->nazwa && nazwa2 == mecz->gospodarz->nazwa)
                    licznik+=mecz->ilosc_straconych;
            }
        }

        return licznik;
    }

    void sortowanie_druzyn_przed_wyswietlaniem(int kryterium)
    {
        switch(kryterium)
        {
        case 1:
            sortuj(Druzyny.begin(),Druzyny.end(),[this](const Druzyna* lhs, const Druzyna* rhs)
            {
//                return lhs->ilosc_pkt > rhs->ilosc_pkt;

                  //1* decyduje ogolna ilosc punktow
                  if(lhs->ilosc_pkt>rhs->ilosc_pkt)
                      return true;
                  if(lhs->ilosc_pkt<rhs->ilosc_pkt)
                      return false;

                  //2* decyduje liczba bramek zdobyta w meczach bezposrednich
                  if(liczba_bramek_w_meczach_bezposrednich(lhs->nazwa,rhs->nazwa) > liczba_bramek_w_meczach_bezposrednich(rhs->nazwa,lhs->nazwa))
                      return true;
                  if(liczba_bramek_w_meczach_bezposrednich(lhs->nazwa,rhs->nazwa) < liczba_bramek_w_meczach_bezposrednich(rhs->nazwa,lhs->nazwa))
                      return false;

                  //3* decyduje bilans bramek ogolny
                  if(lhs->ilosc_Zgoli-lhs->ilosc_Sgoli>rhs->ilosc_Zgoli-rhs->ilosc_Sgoli)
                      return true;
                  if(lhs->ilosc_Zgoli-lhs->ilosc_Sgoli<rhs->ilosc_Zgoli-rhs->ilosc_Sgoli)
                      return false;

                  //4* decyduje liczba zdobytych bramek ogolnie
                  if(lhs->ilosc_Zgoli>rhs->ilosc_Zgoli)
                      return true;
                  if(lhs->ilosc_Zgoli<rhs->ilosc_Zgoli)
                      return false;

                  //5* decyduje liczba zwyciestw ogolnie
                  return lhs->ilosc_w>rhs->ilosc_w;

            }
                 );
            uzupelnij_miejsce_w_tabeli();
            break;
        case 2:
            sortuj(Druzyny.begin(),Druzyny.end(),[](const Druzyna* lhs, const Druzyna* rhs)
            {
                return lhs->ilosc_w > rhs->ilosc_w;
            }
                 );
            break;
        case 3:
            sortuj(Druzyny.begin(),Druzyny.end(),[](const Druzyna* lhs, const Druzyna* rhs)
            {
                return lhs->ilosc_p > rhs->ilosc_p;
            }
                 );
            break;
        }


    }
public:
    Tabela(unsigned int ilosc): ilosc_druzyn{ilosc},  Terminarz { (ilosc_druzyn-1)*2, vector<Mecz*>(ilosc_druzyn/2)}
    {
      aktualna_kolejka = 0;
      //engine.seed(chrono::system_clock::now().time_since_epoch().count());
    }
    size_t getter_aktualna_kolejka() {return aktualna_kolejka;}

    void dodaj_druzyne(string nazwa)
    {
        Druzyna* nowa = new Druzyna(nazwa,used);
        for (size_t i=0; i<5; i++)
        {
            pilkarz_ilosc_bramek.push_back(make_pair(nowa->zawodnicy[i], nowa->zawodnicy[i]->getter_ilosc_bramek()));
        }
        Druzyny.push_back(nowa);
    }
    void wyswietl_tabele(int nr=1)
    {
        sortowanie_druzyn_przed_wyswietlaniem(nr);
        string forma;
        string linia('_', 10);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),2);
        cout<<"----------------------Tabela ligowa---------------------"<<endl;
        cout<<setw(4)<<"|Lp|"<<setw(14)<<"Nazwa druzyny|"<<setw(14)<<"Ilosc punktow|"<<"Me|Wy|Po|Re|BZ|BS|Forma|"<<endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
        for (unsigned int i=0; i<ilosc_druzyn; i++)
        {
            forma="";

            cout<<"|"<<setw(2)<<left<<Druzyny[i]->miejsce_w_tabeli<<"|"<<setw(13)<<left<<Druzyny[i]->nazwa<<"|"<<setw(13)<<left<<Druzyny[i]->ilosc_pkt
               <<"|"<<setw(2)<<left<<Druzyny[i]->ilosc_meczow<<"|"<<setw(2)<<left<<Druzyny[i]->ilosc_w<<"|"<<setw(2)<<left<<Druzyny[i]->ilosc_p<<"|"
              <<setw(2)<<left<<Druzyny[i]->ilosc_r<<"|"<<setw(2)<<left<<Druzyny[i]->ilosc_Zgoli<<"|"<<setw(2)<<left<<Druzyny[i]->ilosc_Sgoli<<"|";
            for (auto x: Druzyny[i]->Forma) {forma += x;}
            cout<<setw(5)<<forma<<"|"<<endl;

        }

    }
    void wyswietl_krola_strzelcow()
    {
        for_each(pilkarz_ilosc_bramek.begin(), pilkarz_ilosc_bramek.end(), [](pair<Pilkarz*, int> &a)
        {
           a.second = a.first->getter_ilosc_bramek();
        });
        sortuj(pilkarz_ilosc_bramek.begin(), pilkarz_ilosc_bramek.end(), [](const pair<Pilkarz*, int> &a, const pair<Pilkarz*, int> &b)
        {
            return (a.second > b.second);
        }
             );
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),2);
        cout<<"--------------------Krol Strzelcow-------------------"<<endl;
        cout<<setw(4)<<"|Lp|"<<setw(14)<<"Nazwa druzyny|"<<setw(14)<<"Nazwa Zawodnika|"<<"Ilosc bramek|"<<endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
        for (size_t i=0; i<10; i++)
        {
            cout<<"|"<<setw(2)<<left<<i+1<<"|"<<setw(13)<<pilkarz_ilosc_bramek[i].first->getter_nazwa_klubu()<<"|"<<setw(15)<<left<<
            pilkarz_ilosc_bramek[i].first->getter_nazwa_pilkarza()<<"|"<<pilkarz_ilosc_bramek[i].second<<endl;
        }
    }
    void utworz_kolejke(list<pair<Druzyna*, Druzyna*>> & mozliwe_mecze)
    {
        pair<Druzyna*, Druzyna*> mecz;

        int dl_sez=304;//ze wzgledu na przerwe miedzy sezonami niewiele to znaczy, dni od 1 sierpnia 2019 do 31 maja 2020
        int dl_kol=dl_sez/((Druzyny.size()-1)*2);//dlugosc kazdej kolejki
        int rok_start;
        int miesiac_start;
        int dzien_start;
        vector <int> dni_rozgrywek;

        if(licznik_kolejek<Druzyny.size())//sezon jesienny
        {
            rok_start=2019;
            miesiac_start=8;
            dzien_start=1;
            dni_rozgrywek = generator_liczb_z_zakresu((licznik_kolejek-1)*dl_kol,licznik_kolejek*dl_kol,Druzyny.size()/2);
        }
        else//sezon wiosenny
        {
            rok_start=2020;
            miesiac_start=2;
            dzien_start=1;
            dni_rozgrywek = generator_liczb_z_zakresu((licznik_kolejek-Druzyny.size())*dl_kol,(licznik_kolejek-Druzyny.size()+1)*dl_kol,Druzyny.size()/2);

        }

        vector <int> godziny_rozgrywek=generator_liczb_z_zakresu(0,28,Druzyny.size()/2);
        sortuj(dni_rozgrywek.begin(),dni_rozgrywek.end());
        sortuj(godziny_rozgrywek.begin(),godziny_rozgrywek.end());

        for (size_t i=0; i<Druzyny.size()/2; i++)
        {
  //          mecz = *select_randomly(mozliwe_mecze.begin(), mozliwe_mecze.end());
              mecz = mozliwe_mecze.front();
            Mecz* nowy = new Mecz(mecz.first, mecz.second, i+1,dni_rozgrywek[i],godziny_rozgrywek[i],rok_start,miesiac_start,dzien_start);
            Terminarz[licznik_kolejek-1][i] = nowy;
 //           mozliwe_mecze.erase(remove(mozliwe_mecze.begin(), mozliwe_mecze.end(), mecz));
            mozliwe_mecze.pop_front();
        }
        licznik_kolejek++;

    }
    void wyswielt_kolejke(size_t nr)
    {
        for (auto meczyk: Terminarz[nr-1])
        {
            meczyk->wyswietl_mecz();

        }
  //      Sleep(2000);
  //      system("cls");
    }
    void rozegraj_kolejke(int wybor, list <int> &wyniki, bool silent=false)
    {
        bool losowanie_do_konca = false;
 //       int wybor;
        int nr_od_ktorego_losowe_mecze = 0;
 //       cout<<"Rozpoczynamy kolejke : "<<aktualna_kolejka+1<<endl;
        for (size_t i=0; i<Druzyny.size()/2; i++)
        {

            if(!losowanie_do_konca)
            {
                 nr_od_ktorego_losowe_mecze++;
 //              cout<<"Aktualny mecz "<<endl;
 //               Terminarz[aktualna_kolejka][i]->wyswietl_mecz();
 //               cout<<"Jak chcesz rozegrac mecz? "<<endl;
 //               cout<<"1. Wpisywanie reczne"<<endl;
 //               cout<<"2. Rozegranie jednej gry losowo"<<endl;
 //               cout<<"3. Losowanie wynikow meczow do konca rundy"<<endl;
 //               cout<<"Wybor: "; cin>>wybor;
                switch(wybor)
                {
                case 1:
                    Terminarz[aktualna_kolejka][i]->rozgrywaj_mecz_manualnie(engine,wyniki);
                    break;
                case 2:
                    Terminarz[aktualna_kolejka][i]->rozegraj_mecz_losowo(engine);
                    break;
                case 3:
                    Terminarz[aktualna_kolejka][i]->rozegraj_mecz_losowo(engine);
                    losowanie_do_konca = true;
                    continue;
                default:
                    Terminarz[aktualna_kolejka][i]->rozegraj_mecz_losowo(engine);
                    losowanie_do_konca = true;
                    continue;
                }
        //        system("cls");
                if(!silent)
                {
                    cout<<"\nWynik meczu: ";
                    Terminarz[aktualna_kolejka][i]->wyswietl_mecz("szczegoly");
                }

        //        Sleep(2000);
        //        system("cls");
          }
            else {
                Terminarz[aktualna_kolejka][i]->rozegraj_mecz_losowo(engine);
            }

        }
        if(losowanie_do_konca)
        {
            system("cls");
            cout<<"WYNIKI MECZOW: "<<endl;
            for(int i=nr_od_ktorego_losowe_mecze-1; i<Druzyny.size()/2; i++)
            {
                Terminarz[aktualna_kolejka][i]->wyswietl_mecz();
            }
            Sleep(4000);
            system("cls");
        }
        aktualna_kolejka++;
    }
   void utworz_terminarz_helper(int k, char sezon, vector<vector<int>>&T, list<pair<Druzyna*, Druzyna*>> &dostepne_mecze)
    {
            for(int j=0;j<2*k-1;j++)
            {
            //tutaj beda sie powtorzenia zaczynac
            for(int i=0;i<k;i++)
            {
                if(T[1][i]==0 or T[0][i]==0)
                    continue;

                if(sezon=='0')
                    dostepne_mecze.push_back(make_pair(Druzyny[T[0][i]-1], Druzyny[T[1][i]-1]));
                else
                    dostepne_mecze.push_back(make_pair(Druzyny[T[1][i]-1], Druzyny[T[0][i]-1]));
            }

            if(k>1)
            {
                int tmp=T[0][k-1];//rotacja
                int tmp2=T[1][0];

                for(int i=k-1;i>1;i--)//rotacja
                {
                    T[0][i]=T[0][i-1];
                }

                for(int i=0;i<k-1;i++)//rotacja
                {
                    T[1][i]=T[1][i+1];
                }

                T[1][k-1]=tmp;
                T[0][1]=tmp2;
            }

            }


    }
    void utworz_terminarz()
    {
        list<pair<Druzyna*, Druzyna*>> dostepne_mecze;
        int n=Druzyny.size();

        int w=2;
        int k=n%2?n/2+1:n/2;
        int l=1;

        vector<vector<int>>T;
        T.resize(w, vector<int>(k));

        for(int j=0;j<k;j++)
        {
            T[0][j]=l++;
        }

        for(int j=k-1;j>=0;j--)
        {
            T[1][j]=l++;
        }

        if(n%2)
            T[1][0]=0;//tak oznaczam atrape przeciwnika

        utworz_terminarz_helper(k,'0',T,dostepne_mecze);
        utworz_terminarz_helper(k,'1',T,dostepne_mecze);



        int ilosc_kolejek=Druzyny.size()%2?Druzyny.size()*2:(Druzyny.size()-1)*2;//tutaj uwzgledniam nieparzysta ilosc druzyn, ale w reszcie programu nie

        for (int i=0; i<ilosc_kolejek; i++)
        {
            utworz_kolejke(dostepne_mecze);
        }

    }
    void wyswielt_terminarz()
    {
        cout<<"RUNDA JESIENNA"<<endl;
        unsigned int licznik_kolejek = 1;
        for (auto kolejka: Terminarz)
        {
            if(licznik_kolejek == Druzyny.size()) {  cout<<"RUNDA WIOSENNA"<<endl;}
            if(licznik_kolejek<=Druzyny.size()*2-2) { cout<<"Kolejka nr : "<<licznik_kolejek<<endl;}
            for (auto meczyk: kolejka)
            {
                if(meczyk!= nullptr) {meczyk->wyswietl_mecz();}
            }
            licznik_kolejek++;
        }
    }
    void wyswietl_wybrany_mecz(string nazwa, string nazwa2="dowolna", bool less=false)
    {

        int i=0;
        for (auto kolejka: Terminarz)
        {
            for (auto mecz: kolejka)
            {
                //if((nazwa == mecz->gospodarz->nazwa && nazwa2 ==mecz->gosc->nazwa) || nazwa == mecz->gosc->nazwa && nazwa2 == mecz->gospodarz->nazwa )
                if((nazwa == mecz->gospodarz->nazwa && (nazwa2=="dowolna" or nazwa2 == mecz->gosc->nazwa)) or (nazwa == mecz->gosc->nazwa && (nazwa2=="dowolna" or nazwa2 == mecz->gospodarz->nazwa)))
                {
                    i++;

                    if(!less)
                    mecz->wyswietl_mecz("szczegoly");
                    else
                    mecz->wyswietl_mecz();

                }
            }
        }
        if(i==0)
        {
            cout<<"Podano zespol, ktory nie istnieje! ";
        }
        else
        {
        //    Sleep(3000);
        //    system("cls");
        }

    }



};

void color_cout(string text, int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color);
    cout << text << endl << endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
}

void testy_jednostkowe()
{
    vector<vector<string>>v={ { "moja1", "moja2" },
                              { "real", "bayern", "liverpool", "legia" },
                              { "moja3", "tottenham", "ajax", "psv", "arsenal", "moja4" },
                            };

    int wariant=0;

    for(auto &test:v)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),12);
        cout << "-----WARIANT " << ++wariant << ": ILOSC DRUZYN=" << test.size() << "-----" << endl << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);

        color_cout("TEST 1(tworzenie tabeli): ",14);

        Tabela MyLeagueA(test.size());
        cout << "ILOSC KOLEJEK=" << MyLeagueA.Terminarz.size() << endl;
        cout << "ILOSC MECZY W KAZDEJ KOLEJCE=" << MyLeagueA.Terminarz[0].size() << endl << endl;

        color_cout("TEST 2(dodawanie druzyn): ",14);

        for(int i=0;i<test.size();i++)
        {
            MyLeagueA.dodaj_druzyne(test[i]);
            cout << "DRUZYNA " << i+1 << "=" << MyLeagueA.Druzyny[i]->nazwa << endl;
            cout << "SILA=" << MyLeagueA.Druzyny[i]->sila_zawodnikow << endl;
            cout << "ZAWODNICY=";

            for(auto &k: MyLeagueA.Druzyny[i]->zawodnicy)
                cout << k->nazwa << " ";

            cout << endl << endl;
        }

        color_cout("TEST 3(tworzenie terminarza): ",14);
        MyLeagueA.utworz_terminarz();
        MyLeagueA.wyswielt_terminarz();

        cout << endl << endl;

        color_cout("TEST 4(wyswietlanie tabeli przed 1 kolejka): ",14);
        MyLeagueA.wyswietl_tabele();

        cout << endl << endl;

        color_cout("TEST 5(wyswietlanie krola strzelcow przed 1 kolejka): ",14);
        MyLeagueA.wyswietl_krola_strzelcow();

        cout << endl << endl;

        color_cout("TEST 6(rozegranie kolejki manualnie (zawsze X:X+1)): ",14);

        list <int> wyniki;

        for(int i=0;i<test.size();i++)
        {
            wyniki.push_back(i);
        }


        MyLeagueA.rozegraj_kolejke(1,wyniki);

        cout << endl << endl;
        color_cout("TEST 7(wyswietlanie tabeli po 1 kolejce): ",14);
        MyLeagueA.wyswietl_tabele();

        cout << endl << endl;
        color_cout("TEST 8(wyswietlanie krola strzelcow po 1 kolejce): ",14);
        MyLeagueA.wyswietl_krola_strzelcow();

        cout << endl << endl;

        color_cout("TEST 9(rozegranie kolejki manualnie (zawsze 2:2)): ",14);

        for(int i=0;i<test.size();i++)
        {
            wyniki.push_back(2);
        }


        MyLeagueA.rozegraj_kolejke(1,wyniki);

        cout << endl << endl;
        color_cout("TEST 10(wyswietlanie tabeli po 2 kolejce): ",14);
        MyLeagueA.wyswietl_tabele();

        cout << endl << endl;
        color_cout("TEST 11(wyswietlanie krola strzelcow po 2 kolejce): ",14);
        MyLeagueA.wyswietl_krola_strzelcow();

        cout << endl << endl;

        if(test.size()>2)
        {
            color_cout("TEST 12(rozegranie kolejki losowo): ",14);
            MyLeagueA.rozegraj_kolejke(2,wyniki);

            cout << endl << endl;
            color_cout("TEST 13(wyswietlanie tabeli po 3 kolejce): ",14);
            MyLeagueA.wyswietl_tabele();

            cout << endl << endl;
            color_cout("TEST 14(wyswietlanie krola strzelcow po 3 kolejce): ",14);
            MyLeagueA.wyswietl_krola_strzelcow();

            cout << endl << endl;

            //KONCZE SEZON:
            for(int i=3;i<MyLeagueA.Terminarz.size();i++)
               MyLeagueA.rozegraj_kolejke(2,wyniki,"silent");

            color_cout("TEST 15(wyswietlanie krola strzelcow po zakonczonym sezonie): ",14);

            MyLeagueA.wyswietl_krola_strzelcow();
            cout << endl << endl;

            color_cout("TEST 16(wyswietlanie tabeli po zakonczonym sezonie): ",14);
            MyLeagueA.wyswietl_tabele();

            cout << endl << endl;
        }


        color_cout("TEST 17(sortowanie tabeli wg wygranych): ",14);

        MyLeagueA.wyswietl_tabele(2);
        cout << endl << endl;
        color_cout("TEST 18(sortowanie tabeli wg przegranych): ",14);

        MyLeagueA.wyswietl_tabele(3);
        cout << endl << endl;

        color_cout("TEST 19(wyswietlanie wybranej kolejki (nr 1)): ",14);

        MyLeagueA.wyswielt_kolejke(1);
        cout << endl << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),14);
        cout << "TEST 20(wyswietlanie wybranej kolejki (nr " << test.size() << ")): " << endl << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
        MyLeagueA.wyswielt_kolejke(test.size());
        cout << endl << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),14);
        cout << "TEST 21(wyswietlanie wybranych meczy (miedzy " << test[0] << " i " << test[test.size()-1] << ")): " << endl << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
        MyLeagueA.wyswietl_wybrany_mecz(test[0], test[test.size()-1]);
        cout << endl << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),14);
        cout << "TEST 22(wyswietlanie wszystkich meczy zespolu (" << test[0] << "): " << endl << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
        MyLeagueA.wyswietl_wybrany_mecz(test[0],"dowolna");
        cout << endl << endl;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),14);
        cout << "TEST 23(wyswietlanie wszystkich meczy zespolu (" << test[test.size()-1] << ") (forma skrocona): " << endl << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),7);
        MyLeagueA.wyswietl_wybrany_mecz(test[test.size()-1],"dowolna",true);
        cout << endl << endl;

    }


}

int main()
{
    //srand(time(nullptr));

    //na czas testow generatory liczb losowych generuja przy kazdym uruchomieniu takie same liczby,
    //aby naprawic trzeba odkomentowac linie 955 i 476

    testy_jednostkowe();

    return 0;
}
