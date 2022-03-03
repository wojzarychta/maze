//2020-12-16 Zarychta Wojciech
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <stack>
#include <cmath>
#include <conio.h>

using namespace std;

const int max_size = 22;      //stała użyta do deklaracji rozmiaru tablicy - labiryntu

//---------------------------
struct cell  {                //struktura użyta do tworzenia labiryntu;
    bool visited;             //zmienna do tworzenia losowego labiryntu; czy pole zostało już odwiedzone
    bool trap;                //do oznaczania pól z pułapką
    bool top;                 //zmienna do tworzenia losowego labiryntu; czy można przejść przez górną ścianę komórki
    bool bot;                 //zmienna do tworzenia losowego labiryntu; czy można przejść przez dolną ścianę komórki
    bool left;                //zmienna do tworzenia losowego labiryntu; czy można przejść przez lewą ścianę komórki
    bool right;               //zmienna do tworzenia losowego labiryntu; czy można przejść przez prawą ścianę komórki
    bool route;               //rozwiazanie labyrintu, na ktorym nie moga stac pułapki, aby zawsze była droga do mety
    char display;             //wyświetlany znak labiryntu
};

struct coordinates{           //para współrzędnych
    int x;
    int y;
};

//----------------------------
void welcome();
void initialize(cell maze[][max_size], int vol);
void maze_generator(cell maze[][max_size], int vol, coordinates start, coordinates meta);
void trap_generator(cell maze[][max_size], int vol, coordinates start, coordinates meta);
bool solve(cell maze[][max_size], int x, int y, coordinates meta);
void draw_maze(cell maze[][max_size], int vol);
bool control(cell maze[][max_size], int vol, coordinates start, coordinates meta, double& t, int& steps);
void up(cell maze[][max_size], coordinates& P, int vol, int& counter);
void down(cell maze[][max_size], coordinates& P, int vol, int& counter);
void right(cell maze[][max_size], coordinates& P, int vol, int& counter);
void left(cell maze[][max_size], coordinates& P, int vol, int& counter);
void result(bool result, double time, int steps);
void draw_quarter(cell maze[][max_size], coordinates P, int vol);
void draw_quarter_1(cell maze[][max_size], coordinates P, int vol);
void draw_quarter_2(cell maze[][max_size], coordinates P, int vol);
void draw_quarter_3(cell maze[][max_size], coordinates P, int vol);
void draw_quarter_4(cell maze[][max_size], coordinates P, int vol);
bool create_up(cell maze[][max_size], coordinates& P, int& visited, stack <coordinates>& stack_P);
bool create_down(cell maze[][max_size], coordinates& P, int& visited, stack <coordinates>& stack_P);
bool create_right(cell maze[][max_size], coordinates& P, int& visited, stack <coordinates>& stack_P);
bool create_left(cell maze[][max_size], coordinates& P, int& visited, stack <coordinates>& stack_P);
//------------------------------------------

int main()
{
    int volume = max_size - 1;          //rozmiar labiryntu
    double timer;
    cell maze[max_size][max_size];
    int steps;
    coordinates start, meta;
    start.y = volume - 1;
    start.x = 1;
    meta.y = 1;
    meta.x = volume - 1;

    srand( time( NULL ) );

    welcome();
    initialize(maze, volume);
    maze_generator(maze, volume, start, meta);
    bool outcome = control(maze, volume, start, meta, timer, steps);
    result(outcome, timer, steps);

    return 0;
}


void welcome(){
    cout << setw(48) << "Witaj w grze 'labirynt'!" << endl << endl << endl;
    cout << setw(40) << "ZASADY GRY" << endl << endl;
    cout << "1. W grze bierze udzial jeden gracz. Celem gry jest przejscie z dolnego lewego rogu planszy do gornego prawego w jak najkrotszym czasie." << endl;
    cout << "2. Gracz porusza sie po planszy o wymiarach 20x20 z ukladem barier (scian) i slepych korytarzy. Do celu moze prowadzi wiecej niz jedna droga" << endl;
    cout << "3. Na planszy znajduja sie pulapki, oznaczone znakiem 'X', po wpadnieciu w ktore gracz konczy rozgrywke. Jezeli gracz znajdzie sie na polu sssiadujscym z pulapka zostanie o tym poinformowany." << endl;
    cout << "4. Pelna plansze zobaczysz tylko raz na poczatku gry. Nastepnie prezentowany jest tylko jeden z czterech fragmentow planszy o wymiarach 10x10, w ktorym aktualnie znajduje sie gracz." << endl;
    cout << "5. Poruszasz sie klawiszami 'w', 'a', 's', 'd'." << endl;
    cout << "6. Twoje polozenie oznaczone jest znakiem '#', a slady pozostawiane za toba oznaczane beda przez '.'." << endl;
    cout << endl << endl << endl;

    system("pause");
}


void initialize(cell maze[][max_size], int vol){
    //funckja przygotowywuje komórki labiryntu pod jego generowanie
    for(int i=0; i<=vol; i++){
        for(int j=0; j<=vol; j++){
            maze[i][j].display = 43;
            maze[i][j].visited = false;
            maze[i][j].trap = false;
            maze[i][j].top = true;
            maze[i][j].bot = true;
            maze[i][j].right = true;
            maze[i][j].left = true;
            maze[i][j].route = false;
        }
    }
    //ściany labiryntu ograniczają labirynt, tak, aby algorytm generujący nie wyszedł poza labirynt:
    for(int i=1; i<=vol-1; i++){
        for(int j=1; j<=vol-1; j++){
            maze[1][j].top = false;
            maze[vol-2][j].bot = false;
            maze[i][vol-2].right = false;
            maze[i][1].left = false;
        }
    }
}


void maze_generator(cell maze[][max_size], int vol, coordinates start, coordinates meta){   //generator losowego labiryntu
    int direction;
    int visited = 1;
    int cells = pow((vol-1)/2, 2);       //ilosc komorek, które algorytm musi "odwiedzić"
    coordinates P;
    P.x = start.x;
    P.y = start.y-1;
    stack <coordinates> stack_P;

    while (visited <= cells){    //dopóki algorytm nie odwiedzi wszystkich komórek
        // warunek mówi, że można rozbić daną ścianę, tylko wtedy, gdy dane pole nie zostało odwiedzone oraz gdy ściany w drodze na to pole jeszcze nie zostały rozbite
        if(((maze[P.y-2][P.x].visited == false)  &&  (maze[P.y][P.x].top == true)  &&  (maze[P.y-2][P.x].bot == true)) ||
           ((maze[P.y+2][P.x].visited == false)  &&  (maze[P.y][P.x].bot == true)  &&  (maze[P.y+2][P.x].top == true)) ||
           ((maze[P.y][P.x-2].visited == false)  &&  (maze[P.y][P.x].left == true)  &&  (maze[P.y][P.x-2].right == true)) ||
           ((maze[P.y][P.x+2].visited == false)  &&  (maze[P.y][P.x].right == true)  &&  (maze[P.y][P.x+2].left == true))){

            direction = rand()%4;

            if (direction == 0  &&  P.y > 1){                                       //góra
                if (create_up(maze, P, visited, stack_P) == false) continue;}
            else if (direction == 1  &&  P.y < vol-2){                              //dół
                if (create_down(maze, P, visited, stack_P) == false) continue;}
            else if (direction == 2  &&  P.x < vol-2){                              //prawo
                if (create_right(maze, P, visited, stack_P) == false) continue;}
            else if (direction == 3  &&  P.x > 1){                                  //lewo
                if (create_left(maze, P, visited, stack_P) == false) continue;}
        }
        else{
            //jeżeli nie spełnił się warunek, wracamy do ostatniego punktu, a następnie ściągamy go ze stosu
            P = stack_P.top();
            stack_P.pop();
        }
    }

    for(int i = 0; i<=vol; i++){                      //labirynt 19x19 przekształca w 20x20
        maze[i][vol-1].display = maze[i][vol-2].display;
        maze[vol-1][i].display = maze[vol-2][i].display;
    }

    for (int i = 2; i<vol; i++){                      //zabieg estetyczny :)
        if (maze[i][vol-2].display == ' '  &&  maze[i-1][vol-1].display == ' ') maze[i][vol-1].display = 43;
        if (maze[vol-2][i].display == ' '  &&  maze[vol-1][i-1].display == ' ') maze[vol-1][i].display = 43;
    }

    maze[start.y][start.x].display = 'S';
    maze[meta.y][meta.x].display = 'M';

    trap_generator(maze, vol, start, meta);
}


bool create_up(cell maze[][max_size], coordinates& P, int& visited, stack <coordinates>& stack_P){
    if(maze[P.y-2][P.x].visited == false){
        maze[P.y-1][P.x].display = ' ';
        maze[P.y-1][P.x].visited = true;
        maze[P.y][P.x].top = false;

        stack_P.push(P);    //daje na stos współrzędne obecnego punktu

        P.y -= 2;

        maze[P.y][P.x].display = ' ';
        maze[P.y][P.x].visited = true;
        maze[P.y][P.x].bot = false;

        visited++;
        return true;
    }
    else return false;
}


bool create_down(cell maze[][max_size], coordinates& P, int& visited, stack <coordinates>& stack_P){
    if(maze[P.y+2][P.x].visited == false){
        maze[P.y+1][P.x].display = ' ';
        maze[P.y+1][P.x].visited = true;
        maze[P.y][P.x].top = false;

        stack_P.push(P);      //daje na stos współrzędne obecnego punktu

        P.y += 2;

        maze[P.y][P.x].display = ' ';
        maze[P.y][P.x].visited = true;
        maze[P.y][P.x].bot = false;

        visited++;
        return true;
    }
    else return false;
}


bool create_right(cell maze[][max_size], coordinates& P, int& visited, stack <coordinates>& stack_P){
    if(maze[P.y][P.x+2].visited == false){
        maze[P.y][P.x+1].display = ' ';
        maze[P.y][P.x+1].visited = true;
        maze[P.y][P.x].top = false;

        stack_P.push(P);         //daje na stos współrzędne obecnego punktu

        P.x += 2;

        maze[P.y][P.x].display = ' ';
        maze[P.y][P.x].visited = true;
        maze[P.y][P.x].bot = false;

        visited++;
        return true;
    }
    else return false;
}


bool create_left(cell maze[][max_size], coordinates& P, int& visited, stack <coordinates>& stack_P){
    if(maze[P.y][P.x-2].visited == false){
        maze[P.y][P.x-1].display = ' ';
        maze[P.y][P.x-1].visited = true;
        maze[P.y][P.x].top = false;

        stack_P.push(P);          //daje na stos współrzędne obecnego punktu

        P.x -= 2;

        maze[P.y][P.x].display = ' ';
        maze[P.y][P.x].visited = true;
        maze[P.y][P.x].bot = false;

        visited++;
        return true;
    }
    else return false;
}


void trap_generator(cell maze[][max_size], int vol, coordinates start, coordinates meta){

    solve(maze, start.x, start.y, meta);       //wyznacza trasę, na której nie może stać żadna pułapka

    coordinates P;
    P.x = start.x;
    P.y = start.y;

    int traps = rand()%(6-4+1) + 4;            //wyznacza ilość pułapek od 4 do 6

    for(int i=0; i<traps; i++){
        P.x = rand()%(vol-1) + 1;
        P.y = rand()%(vol-1) + 1;
        if (maze[P.y][P.x].route == false || maze[P.y][P.x].trap == true){    //jeśli pole jest częścią rozwiązania lub stoi już na nim pułapka
            maze[P.y][P.x].trap = true;
            maze[P.y][P.x].display = ' ';
            }
        else i--;          //dekrementuje iterator, aby nie zmienic ilości wylosowanych pułapek
    }
}


void draw_maze(cell maze[][max_size], int vol){            //rysowanie labiryntu w konsoli
    system("cls");
    for(int i=0; i<=vol; i++){
        for(int j=0; j<=vol; j++){
            if (maze[i][j].trap == true) cout << setw(2) << 'X';
            else cout << setw(2) << maze[i][j].display;
        }
        cout << endl;
    }
}


bool control(cell maze[][max_size], int vol, coordinates start, coordinates meta, double& t, int& steps){
    char direction;
    coordinates P;
    P.x = start.x;
    P.y = start.y;
    steps = 0;
    bool result;

    draw_maze(maze, vol);
    cout << endl << "Sprobuj zapamietac uklad labiryntu oraz rozmieszczenie pulapek!" << endl << endl;
    system("pause");
    maze[start.y][start.x].display = '#';
    draw_quarter(maze, P, vol);

    clock_t t1, t2;
    t1 = clock();     //zaczyna mierzyć czas

    while (1){        //nieskończona pętla, która przerywa się w momencie wygranej lub przegranej
        cout << endl << "W jakim kierunku chcesz isc? " << setw(30) << "Licznik krokow: " << steps << endl << "> ";
        direction = _getch();        //aby nie zatwierdzać za każdym razem enterem, można użyć funkcji z biblioteki conio.h

        switch(direction){
            case 'w': {
               up(maze, P, vol, steps);
            }
            break;
            case 's': {
               down(maze, P, vol, steps);
            }
            break;
            case 'a': {
               left(maze, P, vol, steps);
            }
            break;
            case 'd': {
               right(maze, P, vol, steps);
            }
            break;
            default:{
                cout << "Niepoprawny klawisz, spróbuj ponownie" << endl;
                draw_quarter(maze, P, vol);
            }
        }
        if (P.x == meta.x && P.y == meta.y){             //wygrana
            result = true;
            break;
        }
        if (maze[P.y][P.x].trap){                        //przegrana
            result = false;
            break;
        }
        //gdy obok pułapki, wyświetla stosowny komunikat
        if (maze[P.y-1][P.x].trap || maze[P.y+1][P.x].trap || maze[P.y][P.x-1].trap || maze[P.y][P.x+1].trap)
                cout << "UWAGA! Znajdujesz sie obok pulapki. Jesli w nia wejdziesz graz sotanie zakonczona" << endl;

    }
    t2 = clock();                               //koniec pomiaru
    t = (double)(t2 - t1)/CLOCKS_PER_SEC;       //łączny czas
    draw_maze(maze, vol);
    system("pause");
    return result;
}


void up(cell maze[][max_size], coordinates& P, int vol, int& counter){
    if (!(maze[P.y-1][P.x].display == 43)){       //jeśli pole nie jest ścianą
        if (maze[P.y-1][P.x].display == '.'){     //jeśli na polu jest już ślad, to go usuń
            maze[P.y][P.x].display = ' ';
        }
        else
            maze[P.y][P.x].display = '.';         //zostaw za sobą ślad
        counter++;                                //krok więcej
        P.y--;                                    //zmiana odpowiedniej współrzędnej
        maze[P.y][P.x].display = '#';             //wyświetla położenia gracza na nowym polu
        draw_quarter(maze, P, vol);
    }
    else {
        draw_quarter(maze, P, vol);
        cout << "Nie mozesz tam isc, sprobuj ponownie" << endl;
    }
}


//patrz up()
void down(cell maze[][max_size], coordinates& P, int vol, int& counter){
    if (!(maze[P.y+1][P.x].display == 43)){
        if (maze[P.y+1][P.x].display == '.'){
            maze[P.y][P.x].display = ' ';
        }
        else
            maze[P.y][P.x].display = '.';
        counter++;
        P.y++;
        maze[P.y][P.x].display = '#';
        draw_quarter(maze, P, vol);
    }
    else {
        draw_quarter(maze, P, vol);
        cout << "Nie mozesz tam isc, sprobuj ponownie" << endl;
    }
}


//patrz up()
void right(cell maze[][max_size], coordinates& P, int vol, int& counter){
    if (!(maze[P.y][P.x+1].display == 43)){
        if (maze[P.y][P.x+1].display == '.'){
            maze[P.y][P.x].display = ' ';
        }
        else
            maze[P.y][P.x].display = '.';
        counter++;
        P.x++;
        maze[P.y][P.x].display = '#';
        draw_quarter(maze, P, vol);
    }
    else {
        draw_quarter(maze, P, vol);
        cout << "Nie mozesz tam isc, sprobuj ponownie" << endl;
    }
}


//patrz up()
void left(cell maze[][max_size], coordinates& P, int vol, int& counter){
    if (!(maze[P.y][P.x-1].display == 43)){
        if (maze[P.y][P.x-1].display == '.'){
            maze[P.y][P.x].display = ' ';
        }
        else
            maze[P.y][P.x].display = '.';
        counter++;
        P.x--;
        maze[P.y][P.x].display = '#';
        draw_quarter(maze, P, vol);
    }
    else {
        draw_quarter(maze, P, vol);
        cout << "Nie mozesz tam isc, sprobuj ponownie" << endl;
    }
}


void draw_quarter(cell maze[][max_size], coordinates P, int vol){
    system("cls");

    if(P.y <= (vol-1)/2  &&  P.x <= (vol-1)/2)    //2 cwiartka
        draw_quarter_2(maze, P, vol);

    if(P.y <= (vol-1)/2  &&  P.x > (vol-1)/2)     //1 cwiartka
        draw_quarter_1(maze, P, vol);

    if(P.y > (vol-1)/2  &&  P.x <= (vol-1)/2)     //3 cwiartka
        draw_quarter_3(maze, P, vol);

    if(P.y > (vol-1)/2  &&  P.x > (vol-1)/2)      //4 cwiartka
        draw_quarter_4(maze, P, vol);
}


void draw_quarter_1(cell maze[][max_size], coordinates P, int vol){
    for(int i=0; i<=vol; i++){
        for(int j=0; j<=vol; j++){
            if (i==0 || j==0 || i==vol || j==vol)   //ściany labiryntu
                cout << setw(2) << maze[i][j].display;
            else {
                if(i <= (vol-1)/2  &&  j > (vol-1)/2)
                    cout << setw(2) << maze[i][j].display;
                else
                    cout << setw(2) << " ";      //dla reszty labiryntu wyświetla spacje
            }
        }
        cout << endl;
    }
}


void draw_quarter_2(cell maze[][max_size], coordinates P, int vol){
    for(int i=0; i<=vol; i++){
        for(int j=0; j<=vol; j++){
            if (i==0 || j==0 || i==vol || j==vol)   //ściany labiryntu
                cout << setw(2) << maze[i][j].display;
            else {
                if(i <= (vol-1)/2  &&  j <= (vol-1)/2)
                    cout << setw(2) << maze[i][j].display;
                else
                    cout << setw(2) << " ";      //dla reszty labiryntu wyświetla spacje
            }
        }
        cout << endl;
    }
}


void draw_quarter_3(cell maze[][max_size], coordinates P, int vol){
    for(int i=0; i<=vol; i++){
        for(int j=0; j<=vol; j++){
            if (i==0 || j==0 || i==vol || j==vol)   //ściany labiryntu
                cout << setw(2) << maze[i][j].display;
            else {
                if(i > (vol-1)/2  &&  j <= (vol-1)/2)
                    cout << setw(2) << maze[i][j].display;
                else
                    cout << setw(2) << " ";      //dla reszty labiryntu wyświetla spacje
            }
        }
        cout << endl;
    }
}


void draw_quarter_4(cell maze[][max_size], coordinates P, int vol){
    for(int i=0; i<=vol; i++){
        for(int j=0; j<=vol; j++){
            if (i==0 || j==0 || i==vol || j==vol)   //ściany labiryntu
                cout << setw(2) << maze[i][j].display;
            else {
                if(i > (vol-1)/2  &&  j > (vol-1)/2)
                    cout << setw(2) << maze[i][j].display;
                else
                    cout << setw(2) << " ";      //dla reszty labiryntu wyświetla spacje
            }
        }
        cout << endl;
    }
}


void result(bool result, double time, int steps){
    system("cls");
    if (result){
        cout << setw(25) << "WYGRALES!" << endl << endl;
        cout << "Zrobiles " << steps << " kroki/ow w czasie " << fixed << setprecision(1) << time << " sekund." << endl << endl << endl;
    }
    else {
        cout << setw(25) << "PRZEGRALES :(" << endl << endl;
        cout << "Zrobiles " << steps << " kroki/ow w czasie " << fixed << setprecision(1) << time << " sekund." << endl << endl << endl;
    }

    system("pause");
    exit(0);
}

bool solve(cell maze[][max_size], int x, int y, coordinates meta){        //wyznacza rozwiazanie labyrintu, na ktorym nie moga stac pułapki, aby zawsze była droga do mety
    if (x == meta.x  &&  y == meta.y){               //jeśli dojdzie do mety, zwraca prawdę i algorytm zakończy działanie
        maze[y][x].route = true;
        return true;
    }

    if (maze[y][x].display != 43  &&  maze[y][x].route == false){          //sprawdza czy można wejść na dane pole
        maze[y][x].route = true;                                         //dodaje pole do rozwiązania
        //rekurencyjnie wywołuje poniższe funckje, które będą zwaracać prawdę dopóki znajdować będą drogę
        if (solve(maze, x+1, y, meta) == true) return true;
        if (solve(maze, x, y-1, meta) == true) return true;
        if (solve(maze, x, y+1, meta) == true) return true;
        if (solve(maze, x-1, y, meta) == true) return true;

        //jeśli nie znalazł drogi, usuwa pole z rozwiązania oraz zwraca fałsz
        maze[y][x].route = false;
        return false;
    }
    return false;

}
