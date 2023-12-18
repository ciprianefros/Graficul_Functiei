#include <math.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <winbgim.h>
#include <graphics.h>
#define infinit INT_MAX
#define epsi 0.0001
#define  MAX 100
#define MAX1 20
using namespace std;


int top1, top2;
const int max_stiva = 100;

double Opd[max_stiva]; // stiva operanzilor
char Op[max_stiva]; // stiva operatorilor

char OperatiiBinare[200]="+-*/^<>=#";
char OperatiiUnare[200]="scarel";
char Operatii[200]="+-*/^<>=#scarel";

const float pi=3.1415926; // se da sub forma literei q

enum Fereastra { MENIU, GRAFIC, REGULI, INTERESANT, SETARI};

int x, y;

Fereastra fereastraDeschisa = MENIU;

struct functie
{
    char expresie[300];
    char vect[MAX+1][MAX1];
    // vectorul cuprinzand “cuvintele”ce formeaza expresia}
    int lung; // lungimea efectiva a vectorului
    float a,b; // intervalul de evaluare a functiei
    int n; //numarul de puncte de evaluare
};

struct punct {
    double x;
    double y;
};


int Prioritate(char c)  // prioritate operatorilor
{
    if(c=='(' || c==')')
        return 0;
    if(c=='+' || c=='-')
        return 1;
    if(c=='*' || c=='/')
        return 2;
    if(c=='^')
        return 3;
    if(c=='=' || c=='#' || c=='<' || c=='>')
        return 4;
    if(c=='c' || c=='s' || c=='l' || c=='e' || c=='t' || c=='a' || c=='r')
        return 5;
}

bool DifInf(float x)
{
    return fabs(infinit-fabs(x)) > infinit / 2.0;
}

float Logaritm(float x)
{
    if (x>epsi && DifInf(x))
        return log(x);
    else
        return infinit;
}

float Exponential(float x)
{
    if (DifInf(x)) return exp(x);
    else return infinit;
}

float Inmultit(float x, float y)
{
    if (fabs(x < epsi) || fabs(y) < epsi) return 0;
        else if (DifInf(x) && DifInf(y)) return x*y;
            else return infinit;
}

float Putere(float x, float y)
{
    // float p;
    if (x==0) return 0;
    else if (y==0) return 1;
    else if (x==infinit || y==infinit) return infinit;
    else
        return pow(x,y);
}

float Egal(float x, float y)
{
    return x==y;
}

float Diferit(float x, float y)
{
    return x!=y;
}

float MaiMic(float x, float y)
{
    return x < y;
}

bool MaiMare(float x, float y)
{
    return x > y;
}

float Plus(float x, float y)
{
    if (DifInf(x) && DifInf(y))  return x+y;
    else return infinit;
}

float Minus(float x, float y)
{
    if (DifInf(x) && DifInf(y))  return x-y;
    else return infinit;
}

float Impartit(float x, float y)
{
    if (fabs(y)>epsi) return x/y;
    else return infinit;
}

float Sinus(float x)
{
    if (DifInf(x))  return sin(x);
    else return infinit;
}

float Cosinus(float x)
{
    if (DifInf(x))  return cos(x);
    else return infinit;
}

float Modul(float x)
{
    if (DifInf(x)) return fabs(x);
    else return infinit;
}

float Radical(float x)
{
    if (DifInf(x) && (x>epsi)) return sqrt(x);
    else return infinit;
}

float Tangenta(float x)
{
    return (sin(x)/cos(x));

}

float Cotangenta(float x)
{
    return (cos(x)/sin(x));
}

bool EsteNumar(char sir[MAX1])
{
    return (atof(sir)!=0.0 &&
            strchr("0123456789",sir[0]));
}

void tokenize(functie& F) {
    int n = strlen(F.expresie);
    strcpy(F.vect[0], "(");
    int i = 0, j = 1;

    while (i < n) {
        if (isdigit(F.expresie[i])) {
            int k = 0;
            while (isdigit(F.expresie[i])) {
                F.vect[j][k] = F.expresie[i];
                i++;
                k++;
            }
            F.vect[j][k] = '\0';
        } else if (isalpha(F.expresie[i])) {
            int k = 0;
            while (isalpha(F.expresie[i])) {
                F.vect[j][k] = F.expresie[i];
                i++;
                k++;
            }

            // Verificăm pentru funcțiile specifice și reducem dacă este cazul
            if (strncmp(F.vect[j], "sin", 3) == 0) {
                strcpy(F.vect[j], "s");
                i -= 2;  // Sărim înapoi peste "in"
            } else if (strncmp(F.vect[j], "cos", 3) == 0) {
                strcpy(F.vect[j], "c");
                i -= 2;  // Sărim înapoi peste "os"
            } else if (strncmp(F.vect[j], "log", 3) == 0) {
                strcpy(F.vect[j], "l");
                i -= 2;  // Sărim înapoi peste "og"
            } else if (strncmp(F.vect[j], "abs", 3) == 0) {
                strcpy(F.vect[j], "a");
                i -= 2;  // Sărim înapoi peste "bs"
            } else if (strncmp(F.vect[j], "root", 4) == 0) {
                strcpy(F.vect[j], "r");
                i -= 3;  // Sărim înapoi peste "oot"
            } else if (strncmp(F.vect[j], "exp", 4) == 0) {
                strcpy(F.vect[j], "e");
                i -= 2;  // Sărim înapoi peste "xp"
            }

            F.vect[j][k] = '\0';
        } else {
            strncpy(F.vect[j], F.expresie + i, 1);
            F.vect[j][1] = '\0';
            i++;
        }

        j++;
    }

    strcpy(F.vect[j], ")");
}

double ValoareFunctie(functie E, double x)
{
    int i;
    // returneaza valoarea functiei E in punctul x
    double valo,x_1,x_2;
    //calculul expresiei
    for (i=1; i<=max_stiva; i++)
    {
        Opd[i]=0;
        Op[i]='@';
    }
    top1=0;
    top2=1;
    Op[top2]='(';
    i=0;
    while (i<=E.lung && top2>0)
    {
        i++;
        if (EsteNumar(E.vect[i]))
        {
            // printf("\nE.vect[%d]=%s\n",i,E.vect[i]);
            top1++;
            valo=atof(E.vect[i]);
            Opd[top1]=valo;
            // depanare();
        }
        else
            switch (E.vect[i][0]) {
            /* constanta pi=3.1415926 se da sub forma literei q */
            case 'q': top1++; Opd[top1]=pi; break;
            case 'X': /* variabila x */ top1++; Opd[top1]=x;
                /* printf("\n-->x=%f\n",x); */ break;
            case '(': /* inceput de bloc */ top2++; Op[top2]='('; break;
            default:
                /* operatii binare si unare */
               while ((top2>0) && !(strchr("()",Op[top2])) &&
                   Prioritate(Op[top2])>=Prioritate(E.vect[i][0]))
                {
                    if (top1>1) x_1=Opd[top1-1];
                    x_2=Opd[top1];
                    // depanare();
                    /* functii scrise in OPERATII */
                    switch (Op[top2]) {
                        case '=': valo=Egal(x_1,x_2); break;
                        case '#': valo=Diferit(x_1,x_2); break;
                        case '<': valo=MaiMic(x_1,x_2); break;
                        case '>': valo=MaiMare(x_1,x_2); break;
                        case '+': valo=Plus(x_1,x_2); break;
                        case '-': valo=Minus(x_1,x_2); break;
                        case '*': valo=Inmultit(x_1,x_2); break;
                        case '/': valo=Impartit(x_1,x_2); break;
                        case '^': valo=Putere(x_1,x_2); break;
                        case 's': valo=Sinus(x_2); break;
                        case 'c': valo=Cosinus(x_2); break;
                        case 'l': valo=Logaritm(x_2); break;
                        case 'e': valo=Exponential(x_2); break;
                        case 'a': valo=Modul(x_2); break;
                        case 'r': valo=Radical(x_2); break;
                        case 't': valo=Tangenta(x_2); break;
                        case 'C': valo=Cotangenta(x_2); break;
                    } //operator[top2]
                    if (strchr(OperatiiBinare,Op[top2])) top1--;
                    if (strchr(Operatii,Op[top2])) Opd[top1]=valo;
                    top2--;
                }
            // depanare();
            if (top2>0)
                if ((Op[top2]!='(') || (strcmp(E.vect[i],")")))
                    {
                        top2++; Op[top2] = E.vect[i][0];
                    }
                else top2--;
            }
    }
    if (top2==0) return Opd[1];
    else return infinit;
}


double maxVal(functie F) {

    double maxValue = ValoareFunctie(F, F.a);

    for (int i = F.a; i <= F.b; i++) {
        double value = ValoareFunctie(F, i);

        if (value > maxValue) {
            maxValue = value;
        }
    }
    return maxValue;
}

double minVal(functie F) {

    double minValue = ValoareFunctie(F, F.a);


    for (int i = F.a; i <= F.b; i++) {
        double value = ValoareFunctie(F, i);
        if (value < minValue) {
            minValue = value;
        }
    }
    return minValue;
}

void deseneazaGrafic(functie F) {
    const int x1 = 267;
    const int x2 = 799;
    const int y1 = 210;
    const int y2 = 590;

    double minValue = minVal(F);
    double maxValue = maxVal(F);

    outtextxy(20, 150, F.expresie);

    setcolor(COLOR(51, 115, 176));
    setlinestyle(0, 0, 2);

    double scaleX = (x2 - x1) / (F.b - F.a);
    double scaleY = (y2 - y1) / (maxValue - minValue);

    int prevX = -1, prevY = -1;

    for (double i = F.a; i <= F.b; i += 0.01) {
        double xValue = i;
        double yValue = ValoareFunctie(F, xValue);

        if (std::isfinite(yValue)) {
            int x = static_cast<int>((xValue - F.a) * scaleX) + x1;
            int y = y2 - static_cast<int>((yValue - minValue) * scaleY);

            if (prevX != -1 && prevY != -1) {
                line(prevX, prevY, x, y);
            }

            prevX = x;
            prevY = y;
        }
        else {
            // Handle undefined values (e.g., log(0)) by skipping to the next valid x
            prevX = -1;
            prevY = -1;
        }
    }
}

/*void deseneazaGrafic(functie F, double xScale, double yScale) {

    const int xGraphWindow = 533;
    const int yGraphWindow = 400;
    const int x1 = 267;
    const int x2 = 799;
    const int y1 = 590;
    const int y2 = 210;
    double minValue = minVal(F);
    double maxValue = maxVal(F);

    outtextxy(20, 150, F.expresie);

    setcolor(COLOR(51, 115, 176));
    setlinestyle(0, 0, 2);

    double k = (F.b-F.a)/(x2-x1);

    for (double i = F.a; i < F.b; i += k) {

        int x1e = ((x2-x1)*i/(F.b-F.a)+(F.b*x1-F.a*x2)/(F.b-F.a));
        int y1e = ((y2-y1)*ValoareFunctie(F, i)/(maxValue-minValue)+(maxValue*y1-minValue*y2)/(maxValue-minValue));

        int x2e = ((x2-x1)*(i+1)/(F.b-F.a)+(F.b*x1-F.a*x2)/(F.b-F.a));
        int y2e = ((y2-y1)*ValoareFunctie(F, i+1)/(maxValue-minValue)+(maxValue*y1-minValue*y2)/(maxValue-minValue));

        line(x1e, y1e, x2e, y2e);

       /* float x1e = (i * xScale) + xGraphWindow;
        float y1e = yGraphWindow - ((ValoareFunctie(F, i)) * yScale);

        float x2e = ((i + 1) * xScale) + xGraphWindow;
        float y2e = yGraphWindow - ((ValoareFunctie(F, i + 1)) * yScale );


        if (x1e >= 267 && x1e <= 799 && y1e >= 200 && y1e <= 599 &&
            x2e >= 267 && x2e <= 799 && y2e >= 200 && y2e <= 599) {
            line(x1e, y1e, x2e, y2e);
       } else if (x1e < 267 && y1e < 200 && x2e < 267 && y2e < 200 ){
                break;
       } else{
                continue;
       }
    }
}*/

void deseneazaMargine() {

    setcolor(COLOR(51, 115, 176));
    rectangle(267, 200, 798, 598);

}

void deseneazaAxe() {

    setcolor(COLOR(190, 212, 233));
    line(533, 202, 533, 599);
    line(269, 400, 799, 400);

}
void deseneazaGrid() {

    setcolor(WHITE);
    for(int y = 200; y <= 600; y += 30) {
        line(267, y, 800, y);
    }

    for(int x = 267; x <= 800; x += 30) {
        line(x, 200, x, 600);
    }
}

void backToMenu() {

    settextstyle(1, 0, 1);
    rectangle(30, 550, 230, 575);
    outtextxy(40, 555, "Inapoi la Menu");
}


void zoomInOutButtons() {


    outtextxy(600, 120, "ZOOM IN/OUT");
    setlinestyle(0, 0, 2);
    rectangle(680, 155, 705, 180);
    rectangle(710, 155, 735, 180);
    setcolor(COLOR(51, 115, 123));
    setlinestyle(0, 0, 3);
    line(692, 160, 692, 175);
    line(684, 168, 700, 168);
    line(714, 168, 730, 168);

}

void deseneazaChenar(){

    setcolor(COLOR(11, 56, 95));
    settextstyle(4, HORIZ_DIR, 2);
    outtextxy(20, 90, "Graficul functiei");
    outtextxy(20, 120, "tale:");
    deseneazaAxe();
    backToMenu();
    deseneazaMargine();
    zoomInOutButtons();

}

void zoomInOut(functie F, double& xScale, double& yScale, double zoomFactor) {
    // Calculeaza minimul si maximul functiei
    double minY = ValoareFunctie(F, F.a);
    double maxY = minY;

    for (double x = F.a; x <= F.b; x += 0.1) {
        double y = ValoareFunctie(F, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
    }

    //
    double padding = 0.1;
    minY -= padding;
    maxY += padding;

    // Update the scales
    xScale *= zoomFactor;
    yScale *= zoomFactor;

    // Clear and redraw the graph with the updated scales
    cleardevice();
    deseneazaChenar();
    //deseneazaGrafic(F, xScale, yScale);
    deseneazaGrafic(F);
}
void reguli() {
    outtextxy(12, 12, "Ce reguli sa scriu aici???????");
    backToMenu();
}

void interesant() {


}

void drawSlider(int x, int y, int width, int height, int position) {
    setcolor(LIGHTGREEN);  // Change the color
    setfillstyle(SOLID_FILL, LIGHTGREEN);  // Change the fill color

    // Draw a rounded rectangle
    rectangle(x, y, x + width, y + height);
    floodfill(x + width / 2, y + height / 2, LIGHTGREEN);

    // Draw the filled part of the slider
    bar(x, y, x + position, y + height);
}

void setari() {

    int volumeSliderPosition = 0;
    bool musicToggleButtonState = true;
    bool soundToggleButtonState = true;

    settextstyle(6, HORIZ_DIR, 2);
    outtextxy(320, 50, "Setari");

    settextstyle(1, HORIZ_DIR, 1);

    outtextxy(100, 150, "Muzica:");

    outtextxy(100, 180, "Sunet:");

    outtextxy(100, 300, "Volum:");

    // Draw music toggle button
     musicToggleButtonState ?
            outtextxy(480, 150,  "ON"):
            outtextxy(480, 150,  "OFF");

    // Draw sound toggle button
    soundToggleButtonState ?
            outtextxy(480, 180,  "ON"):
            outtextxy(480, 180,  "OFF");
    backToMenu();

    rectangle(500, 300, 700, 320);
    drawSlider(501, 301, 20, 18, volumeSliderPosition);

    // Handle user interactions
    while (true) {
        char ch = getch();

        switch (ch) {
        case 'q':
            return;  // Quit the program
        case 'l':
            // Move the volume slider to the right
            if (volumeSliderPosition < 200)
                volumeSliderPosition += 20;
            break;
        case 'h':
            // Move the volume slider to the left
            if (volumeSliderPosition > 0)
                volumeSliderPosition -= 20;
            break;
        case 'm':
            // Toggle music button
            musicToggleButtonState = !musicToggleButtonState;
            break;
        case 's':
            // Toggle sound button
            soundToggleButtonState = !soundToggleButtonState;
            break;
        }

        // Redraw the settings tab after each interaction
        cleardevice();  // Assuming you have a function to clear the screen
        setcolor(WHITE);
        settextstyle(6, HORIZ_DIR, 2);
        outtextxy(320, 50, "Setari");
        settextstyle(1, HORIZ_DIR, 1);

        // Redraw music toggle button
        outtextxy(100, 150, "Muzica:");
        musicToggleButtonState ?
            outtextxy(480, 150,  "ON"):
            outtextxy(480, 150,  "OFF");

        // Redraw sound toggle button
        outtextxy(100, 180, "Sunet:");
        soundToggleButtonState ?
            outtextxy(480, 180,  "ON"):
            outtextxy(480, 180,  "OFF");

        outtextxy(100, 300, "Volum:");

        backToMenu();

        rectangle(500, 300, 700, 320);
        drawSlider(501, 301, 20, 19, volumeSliderPosition);
    }
}


void deseneazaMenu() {

    setbkcolor(COLOR(28,33,39));

    setfillstyle(SOLID_FILL, COLOR(28,33,39));
    bar(0, 0, getmaxx(), getmaxy());

    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    outtextxy(330, 30, "Grafic");
    settextstyle(1, 0, 1);
    rectangle(290, 195, 510, 220);
    outtextxy(300, 200, "Graficul Functiei");
    rectangle(290, 235, 388, 260);
    outtextxy(300, 240, "Reguli");
    rectangle(290, 275, 490, 300);
    outtextxy(300, 280, "Ceva interesant");
    rectangle(290, 315, 380, 340);
    outtextxy(300, 320, "Setari");
    rectangle(290, 355, 375, 380);
    outtextxy(300, 360, "Iesire");
}
void afisareAsimptote(functie F)
{
    int i;
    double valoare1,valoare2;

    valoare1=ValoareFunctie(F,i);
    valoare2=ValoareFunctie(F,-i);

    if(fabs(valoare1)>infinit/2.0)
        std::cout << "Asimptota orizontala la infinit: y = " << valoare1 << "\n";
    if(fabs(valoare2)>infinit/2.0)
        std::cout << "Asimptota orizontala la infinit: y = " << valoare2 << "\n";

}

void determinaPuncteMinime(functie F)
{
    double h = 0.0001;
    double x, df, d2f;

    for (int i = 0; i < F.n; ++i)
    {
        x = F.a + i * (F.b - F.a) / (F.n - 1);
        df = (ValoareFunctie(F, x + h) - ValoareFunctie(F, x - h)) / (2 * h);
        d2f = (ValoareFunctie(F, x + h) - 2 * ValoareFunctie(F, x) + ValoareFunctie(F, x - h)) / (h * h);
        if (fabs(df) < epsi && d2f > 0) {
            std::cout << "Punct de minim la x = " << x << ", y = " << ValoareFunctie(F, x) << "\n";
        }
    }
}

void determinaPuncteMaxime(functie F)
{
    double h = 0.0001;
    double x, df, d2f;
    F.n = F.a - F.b;

    for (int i = 0; i < F.n; ++i)
    {
        x = F.a + i * (F.b - F.a) / (F.n - 1);
        df = (ValoareFunctie(F, x + h) - ValoareFunctie(F, x - h)) / (2 * h);
        d2f = (ValoareFunctie(F, x + h) - 2 * ValoareFunctie(F, x) + ValoareFunctie(F, x - h)) / (h * h);
        if (fabs(df) < epsi && d2f < 0) {
            std::cout << "Punct de maxim la x = " << x << ", y = " << ValoareFunctie(F, x) << "\n";
        }
    }
}


int main() {

    initwindow(800, 600, "Function Graph");
    const int numPoints = 800;
    const double initialXScale = 800.0;
    const double initialYScale = 600.0;

    double xScale = 1;
    double yScale = 1;

    functie F;

    std::ifstream file("function.txt");

    if (file.is_open())
    {
        file.getline(F.expresie, sizeof(F.expresie));

        file >> F.a;

        file >> F.b;

        file.close();

        std::cout << "Functia din fisier: " << F.expresie << std::endl;

    } else
    {
        outtextxy(350, 290, "Fisierul nu contine nici o functie");
    }

    strcat(F.expresie, ")");
    tokenize(F);
    F.lung = strlen(F.expresie);
    deseneazaMenu();
    while (!kbhit()) {
        if (ismouseclick(WM_LBUTTONDOWN)) {
            getmouseclick(WM_LBUTTONDOWN, x, y);

            switch (fereastraDeschisa) {
                case MENIU:
                    // Handle menu clicks
                    if (x >= 290 && x <= 510 && y >= 195 && y <= 220) {

                        fereastraDeschisa = GRAFIC;
                        cleardevice();
                        deseneazaChenar();
                        //deseneazaGrafic(F, 1, 1);
                        deseneazaGrafic(F);
                        delay(200); // Reduce delay for better responsiveness

                    } else if (x >= 290 && x <= 388 && y >= 235 && y <= 260) {

                        fereastraDeschisa = REGULI;
                        cleardevice();
                        reguli();

                    } else if (x >= 290 && x <= 490 && y >= 275 && y <= 300) {

                        fereastraDeschisa = INTERESANT;
                        cleardevice();
                        backToMenu();

                    } else if (x >= 290 && x <= 410 && y >= 315 && y <= 340) {

                        fereastraDeschisa = SETARI;
                        cleardevice();
                        setari();
                    } else if (x >= 290 && x <= 375 && y >= 355 && y <= 380) {

                        closegraph();
                        std::cout << "Iesire button clicked! Exiting..." << std::endl;
                        return 0;
                    }
                    break;

                case GRAFIC:
                    // Handle clicks in the graphic window
                    if (x >= 30 && x <= 210 && y >= 550 && y <= 570) {
                        fereastraDeschisa = MENIU;
                        cleardevice();
                        deseneazaMenu();
                    }
                    break;
                case REGULI:
                    if (x >= 30 && x <= 210 && y >= 550 && y <= 570) {
                        fereastraDeschisa = MENIU;
                        cleardevice();
                        deseneazaMenu();
                    }
                    break;
                    case INTERESANT:
                    if (x >= 30 && x <= 210 && y >= 550 && y <= 570) {
                        fereastraDeschisa = MENIU;
                        cleardevice();
                        deseneazaMenu();
                    }
                    break;
                // Add cases for other windows
                case SETARI:
                    if (x >= 30 && x <= 210 && y >= 550 && y <= 570) {
                        fereastraDeschisa = MENIU;
                        cleardevice();
                        deseneazaMenu();
                    }
                    break;


                default:
                    break;
            }

            clearmouseclick(WM_LBUTTONDOWN);
        }
}

    delay(20000);
    closegraph();
    return 0;
}
