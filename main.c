// ERMANTRAUT, JOEL

#include <windows.h>
#include "ids.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

// Librerias

#define ID_GROUP    400
#define ID_4x4      401
#define ID_3x3      402
#define ID_2x2      403
// RadioButtons
#define ID_LABELA   404
#define ID_LABELB   405
#define ID_LABELX   406
// Labels de arriba
#define ID_LABELC  407 // Hasta 410
#define ID_LABELF  411 // Hasta 414
// Labels de numeros de matriz
#define ID_COLB     415 // Hasta 418
#define ID_COLX     419 // Hasta 422
// Edits de arreglos
#define ID_AYUDA    423 // Hasta 425
// Texto
#define ID_BUTTON_BORRAR    426
#define ID_BUTTON_CALC      427
// Botones
#define ID_MAT      500
// 500 501 502 503
// 510 511 512 513
// 520 521 522 523
// 530 531 532 533

// DEFINEs

double **alocarMatriz(int n);
double *alocarArreglo(int n);
void liberarMatriz(double **ptr);
void liberarArreglo(void *ptr);
double determinante(double *matriz[4], int dim);

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
void InsertarMenu(HWND);
void InitForm(HWND, WPARAM, LPARAM);

// Prototipos

// Funciones -->

double **alocarMatriz(int n) {
    return (double **) malloc(n * sizeof(double));
}
double *alocarArreglo(int n) {
    return (double *) malloc(n * sizeof(double));
}
void liberarMatriz(double **ptr) {
    free(ptr);
    ptr = 0;
    return;
}
void liberarArreglo(void *ptr) {
    free(ptr);
    ptr = 0;
    return;
}
double determinante(double *matriz[4], int dim) {
    int i, j, k;
    int i2, j2;
    double suma = 0.0;

    if (dim == 1)
        return matriz[0][0];
    // El determinante de un elemento es ese elemento

    double **aux = alocarMatriz(dim - 1);
    for(i = 0; i < dim - 1; i++) {
        aux[i] = alocarArreglo(dim - 1);
    }
    // Aca genera una matriz auxiliar de una columna y una fila menos
    // de la original

    for(k = 0; k < dim; k++) {

        //i2 = 0;
        for(i = 1; i < dim; i++) {
            j2 = 0;
            for(j = 0; j < dim; j++) {
                if (j == k) continue;
                aux[i - 1][j2] = matriz[i][j];
                j2++;
            }
            i2++;
        }
        // Recorre toda la matriz, agregando elementos
        // de una matriz a otra, pero ignorando las
        // columnas y filas que corresponden

        if (k % 2 == 0)
            suma += matriz[0][k] * determinante(aux, dim - 1);
        else
            suma -= matriz[0][k] * determinante(aux, dim - 1);
        // + - + -
        // - + - +
        // + - + -
        // - + - +
        // Guia de signos al calcular determinante

    }

    return suma;
}

//------------------------------------------------
// WinMain: función de inicio del programa
//------------------------------------------------
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    /* Declaración: */
    HWND hwnd;
    MSG mensaje;
    WNDCLASSEX wincl;
    /* Inicialización: */
    /* Estructura de la ventana */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "NUESTRA_CLASE";
    wincl.lpfnWndProc = WinProc;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);
    /* Usar icono y puntero por defecto */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = "Menu";
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)COLOR_WINDOW;

    /* Registrar la clase de ventana, si falla, salir del programa */
    if(!RegisterClassEx(&wincl))
        return 0;

    hwnd = CreateWindowEx( 0,
                           "NUESTRA_CLASE",
                           "Resolución de sistemas lineales mediante la Regla de Cramer",
                           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                           820, 350,  /* tamaño en píxeles */
                           HWND_DESKTOP,
                           NULL,
                           hThisInstance,
                           NULL );

    ShowWindow(hwnd, SW_SHOWDEFAULT);

    /* Bucle de mensajes: */
    while(TRUE == GetMessage(&mensaje, 0, 0, 0))
    {
        if (!IsDialogMessage(hwnd, &mensaje)) //agregado
        {
            TranslateMessage(&mensaje);
            DispatchMessage(&mensaje);
        }
    }
    return mensaje.wParam;
}

//------------------------------------------------------
// Procesamiento de los mensajes de la ventana principal
// Obs.: podría usarse también para otras ventanas
//------------------------------------------------------
LRESULT CALLBACK WinProc(
    HWND hwnd,     // Manipulador de ventana
    UINT msg,      // Mensaje
    WPARAM wParam, // Parámetro palabra, varía
    LPARAM lParam  // Parámetro doble palabra, varía
)
{
    int i, j, k, tam;
    double detA;
    double x[4] = {0.0}, b[4] = {0.0};
    double **matriz, **aux;
    char txt[64];
    static HINSTANCE hInstance;

    HWND hEditMat[4][4];
    HWND hEditB[4];
    HWND hEditX[4];
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            hEditMat[i][j] = GetDlgItem(hwnd, ID_MAT + i * 10 + j);
    // Aca obtengo los manejadores de los elementos de la matriz
    for(i = 0; i < 4; i++) {
        hEditB[i] = GetDlgItem(hwnd, ID_COLB + i);
        hEditX[i] = GetDlgItem(hwnd, ID_COLX + i);
    }
    // Y de b y x

    switch (msg) /* manipulador del mensaje */
    {
    case WM_CREATE:
        // Evento de creación de la ventana
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        InitForm(hwnd, wParam, lParam);
        break;

    case WM_DESTROY:
        // Evento proveniente de haber cliqueado en el botón "cruz", para
        // dar la orden de cerrar la ventana. Si no se ejecuta la acción
        // siguiente, el programa quedará corriendo sin finalizar y habrá
        // que terminarlo desde el administrador de tareas.
        PostQuitMessage(0); /* envía un mensaje WM_QUIT a la cola de mensajes */
        break;

    case WM_COMMAND:
        // Este es el mensaje que se recibe cuando se ha generado algún evento
        // para la ventana, entre otras formas mediante algún control (botón, etc)

        // El LSB de wParam contiene el ID del objeto de origen del mensaje
        switch( LOWORD(wParam) )
        {
        case ID_BUTTON_CALC:
            if (IsDlgButtonChecked(hwnd, ID_4x4)) tam = 4;
            else if (IsDlgButtonChecked(hwnd, ID_3x3)) tam = 3;
            else if (IsDlgButtonChecked(hwnd, ID_2x2)) tam = 2;
            else return -1;
            // Obtengo el tamaño de la matriz

            matriz = alocarMatriz(tam);
            for(i = 0; i < tam; i++) {
                matriz[i] = alocarArreglo(tam);
            }
            // ---
            aux = alocarMatriz(tam);
            for(i = 0; i < tam; i++) {
                aux[i] = alocarArreglo(tam);
            }
            // Genero el espacio en memoria dinamica

            for(i = 0; i < tam; i++) {
                for(j = 0; j < tam; j++) {
                    GetDlgItemText(hwnd, ID_MAT + i * 10 + j, txt, 64);
                    matriz[i][j] = atof(txt);
                }
            }
            // Relleno la matriz con los valores de los hEdit

            detA = determinante(matriz, tam);
            if (detA == 0) {
                MessageBox(hwnd, "El determinante de la matriz es 0.", "No calculable", MB_OK | MB_ICONERROR);
                return -1;
            }
            // Si el determinante de la matriz principal es 0
            // No se puede resolver, vuelve sin calcular

            for(i = 0; i < tam; i++) {
                // La columna b va desde 415 a 418
                GetDlgItemText(hwnd, ID_COLB + i, txt, 64);
                b[i] = atof(txt);
            }
            // Relleno la columna b
            for(k = 0; k < tam; k++) {
                // Por cada columna

                for(i = 0; i < tam; i++) {
                    for(j = 0; j < tam; j++) {
                        aux[i][j] = matriz[i][j];
                    }
                }
                // Copio los valores de la matriz en la matriz auxiliar

                for(i = 0; i < tam; i++) {
                    aux[i][k] = b[i];
                }
                // Reemplazo la columna respectiva con b

                x[k] = determinante(aux, tam) / detA;

            }
            // Calculo con la Regla de Cramer los valores de x

            for(i = 0; i < tam; i++) {
                sprintf(txt, "%.3lf", x[i]);
                SetDlgItemText(hwnd, ID_COLX + i, txt);
            }
            // Coloco todos los valores en la columna b de hEdits
            break;

        case ID_BUTTON_BORRAR:
            for(i = 0; i < 4; i++) {
                for(j = 0; j < 4; j++) {
                    SetDlgItemText(hwnd, ID_MAT + i * 10 + j, "");
                }
            }
            // Clarea los valores de la matriz
            for(i = 0; i < 4; i++) {
                SetDlgItemText(hwnd, ID_COLB + i, "");
                SetDlgItemText(hwnd, ID_COLX + i, "");
            }
            // Clarea las columnas b y x
            break;

        case ID_4x4:
        case ID_3x3:
        case ID_2x2:
            if (IsDlgButtonChecked(hwnd, ID_4x4)) tam = 4;
            else if (IsDlgButtonChecked(hwnd, ID_3x3)) tam = 3;
            else if (IsDlgButtonChecked(hwnd, ID_2x2)) tam = 2;
            else return -1;

            for(i = 0; i < 4; i++) {
                for(j = 0; j < 4; j++) {
                    EnableWindow(hEditMat[i][j], TRUE);
                }
                EnableWindow(hEditB[i], TRUE);
                EnableWindow(hEditX[i], TRUE);
            }
            // Habilito todos
            for(i = tam; i < 4; i++) {
                for(j = 0; j <= i; j++) {
                    EnableWindow(hEditMat[j][i], FALSE);
                    EnableWindow(hEditMat[i][j], FALSE);
                }
                EnableWindow(hEditB[i], FALSE);
                EnableWindow(hEditX[i], FALSE);
            }
            // Inhabilito los correspondientes
            break;
        }
        break;

    default: /* procesamiento por defecto para los mensajes
                de los que no nos ocupamos explícitamente */
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

//----------------------------------------------------------------------
// Procesamiento de mensajes de un cuadro de diálogo
//----------------------------------------------------------------------
BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) /* manipulador del mensaje */
    {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hDlg, TRUE);
            break;
        case IDCANCEL:
            EndDialog(hDlg, FALSE);
            break;
        }
        return TRUE;
    }
    return FALSE;
}

//-------------------------------------------------
// Crea y dimensiona los controles del formulario
//-------------------------------------------------
void InitForm(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    HWND hButton4x4, hButton3x3, hButton2x2, hGroup,
         hEditMat[4][4],
         hEditB[4],
         hEditX[4],
         hButtonClear, hButtonCalc,
         hStaticA, hStaticB, hStaticX,
         hStaticCol[4], hStaticFila[4],
         hStaticHelp[3];

    HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

    // Texto estatico

    hStaticA = CreateWindow(
                 "STATIC", /* Nombre de la clase */
                 "A", /* Texto del título */
                 SS_SIMPLE | WS_CHILD | WS_VISIBLE, /* Estilo */
                 380, 10, /* Posición */
                 15, 15, /* Tamaño */
                 hwnd, /* Ventana padre */
                 (HMENU)ID_LABELA, /* Identificador del control */
                 hInstance, /* Instancia */
                 NULL);

    if(hStaticA == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    hStaticB = CreateWindow(
                 "STATIC",
                 "b",
                 SS_SIMPLE | WS_CHILD | WS_VISIBLE,
                 595, 10,
                 15, 15,
                 hwnd,
                 (HMENU)ID_LABELB,
                 hInstance,
                 NULL);

    if(hStaticB == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    hStaticX = CreateWindow(
                 "STATIC",
                 "x",
                 SS_SIMPLE | WS_CHILD | WS_VISIBLE,
                 705, 10,
                 15, 15,
                 hwnd,
                 (HMENU)ID_LABELX,
                 hInstance,
                 NULL);

    if(hStaticX == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    // MATRIZ

    int i, j;
    char aux[2];
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            hEditMat[i][j] = CreateWindow(
                             "EDIT",
                             "",
                             ES_CENTER | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                             250 + 70 * j, 60 + 40 * i,
                             60, 21,
                             hwnd,
                             (HMENU)(ID_MAT + i * 10 + j),
                             hInstance,
                             NULL);

            if(hEditMat[i][j] == NULL)
                MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);
        }
    }

    for(i = 0; i < 4; i++) {
        sprintf(aux, "%d", i);

        hStaticCol[i] = CreateWindow(
                     "STATIC",
                     aux,
                     SS_SIMPLE | WS_CHILD | WS_VISIBLE,
                     270 + 70 * i, 35,
                     15, 15,
                     hwnd,
                     (HMENU)(ID_LABELC + i),
                     hInstance,
                     NULL);

        if(hStaticCol[i] == NULL)
            MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

        hStaticFila[i] = CreateWindow(
                     "STATIC",
                     aux,
                     SS_SIMPLE | WS_CHILD | WS_VISIBLE,
                     230, 62 + 40 * i,
                     15, 15,
                     hwnd,
                     (HMENU)(ID_LABELF + i),
                     hInstance,
                     NULL);

        if(hStaticFila[i] == NULL)
            MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    }

    // Columna b y x

    for(i = 0; i < 4; i++) {

        hEditB[i] = CreateWindow(
                     "EDIT",
                     "",
                     ES_CENTER | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                     570, 60 + 40 * i,
                     60, 20,
                     hwnd,
                     (HMENU)(ID_COLB + i),
                     hInstance,
                     NULL);

        if(hEditB[i] == NULL)
            MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);
    }
    for(i = 0; i < 4; i++) {
        hEditX[i] = CreateWindow(
                     "EDIT",
                     "",
                     ES_CENTER | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                     680, 60 + 40 * i,
                     60, 20,
                     hwnd,
                     (HMENU)(ID_COLX + i),
                     hInstance,
                     NULL);

        if(hEditX[i] == NULL)
            MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);
    }
    // La columna b y x podrian ser hechos en el mismo bloque, pero de esta manera
    // al presionar TAB pasa al siguiente elemento de la misma columna, y no alterna
    // entre columnas

    // BOTONES

    hButtonClear = CreateWindow("BUTTON",
                           "Borrar valores",
                           BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                           500,230,
                           120,30,
                           hwnd,
                           (HMENU)ID_BUTTON_BORRAR,
                           GetModuleHandle(NULL),
                           NULL);

    if(hButtonClear == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    hButtonCalc = CreateWindow("BUTTON",
                           "Calcular",
                           BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
                           670,230,
                           80,30,
                           hwnd,
                           (HMENU)ID_BUTTON_CALC,
                           GetModuleHandle(NULL),
                           NULL);

    if(hButtonCalc == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    // TEXTO DE AYUDA

    hStaticHelp[0] = CreateWindow(
                 "STATIC",
                 "Ayuda: el sistema de ecuaciones permite calcular A.x = b",
                 SS_SIMPLE | WS_CHILD | WS_VISIBLE,
                 30, 230,
                 500, 15,
                 hwnd,
                 (HMENU)ID_AYUDA,
                 hInstance,
                 NULL);

    if(hStaticHelp[0] == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    hStaticHelp[1] = CreateWindow(
                 "STATIC",
                 "Se deben cargar los valores de A y b y luego,",
                 SS_SIMPLE | WS_CHILD | WS_VISIBLE,
                 80, 250,
                 500, 15,
                 hwnd,
                 (HMENU)(ID_AYUDA + 1),
                 hInstance,
                 NULL);

    if(hStaticHelp[1] == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    hStaticHelp[2] = CreateWindow(
                 "STATIC",
                 "al calcular, se obtienen los valores de x",
                 SS_SIMPLE | WS_CHILD | WS_VISIBLE,
                 80, 270,
                 500, 15,
                 hwnd,
                 (HMENU)(ID_AYUDA + 2),
                 hInstance,
                 NULL);

    if(hStaticHelp[2] == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    // RADIOBUTTONS

    hGroup = CreateWindow("BUTTON",
                            "Dimensión",
                            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                            80, 60,
                            90, 100,
                            hwnd,
                            (HMENU)ID_GROUP,
                            GetModuleHandle(NULL),
                            NULL);

    if(hGroup == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    hButton2x2 = CreateWindow("BUTTON",
                                    "2x2",
                                    BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                                    90,80,
                                    50,20,
                                    hwnd,
                                    (HMENU)ID_2x2,
                                    GetModuleHandle(NULL),
                                    NULL);

    if(hButton2x2 == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    hButton3x3 = CreateWindow("BUTTON",
                                    "3x3",
                                    BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                                    90,105,
                                    50,20,
                                    hwnd,
                                    (HMENU)ID_3x3,
                                    GetModuleHandle(NULL),
                                    NULL);

    if(hButton3x3 == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    hButton4x4 = CreateWindow("BUTTON",
                                    "4x4",
                                    BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                                    90,130,
                                    50,20,
                                    hwnd,
                                    (HMENU)ID_4x4,
                                    GetModuleHandle(NULL),
                                    NULL);

    if(hButton4x4 == NULL)
        MessageBox(hwnd, "No se pudo crear un botón", "Error", MB_OK | MB_ICONERROR);

    SendMessage(hButton4x4, BM_SETCHECK, BST_CHECKED,0);
    // Selecciona por defecto el radiobutton 4x4

    SetFocus(hEditMat[0][0]);
    // Pone el foco en el primer elemento de la matriz
}
