#include <iostream>
#include "Main.h"

static HANDLE g_pStdOut = NULL;
static bool bCancel = false;

using namespace std;

int main()
{
    g_pStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    while (!g_hWnd)
    {
        g_hWnd = FindWindow(NULL, L"Guitar Hero III: Legends Of Rock");
        Sleep(1);
    }

    IniciarThreads();

    while (1)
    {
        SetConsoleCursorPosition(g_pStdOut, COORD{ 0,0 });
        //system("cls");

        cout << "================================" << endl;

        int total = 0;

        for (int i = 0; i < MAX_NOTAS; i++)
        {
            SNota nota = g_notas[i];
            SNotaState state = g_states[i];

            cout << "\tnota: " << nota.descricao << endl;
            cout << "\t- pressionado: " << (state.bState ? "sim" : "nao") << endl;
            cout << "\t- quantidade: " << state.count << endl;
            cout << endl << endl;

            total += state.count;
        }

        cout << endl;
        cout << "================================" << endl;
        cout << "\tmultiplicador: x" << s_multiplicador << "           " << endl;
        cout << "\ttotal de notas: " << total << endl;
        cout << "================================" << endl;
        cout << "                                " << endl;
        cout << "                                " << endl;

        Sleep(500);
    }
}

void IniciarThreads()
{
    int index = 0;

    for (int i = 0; i < MAX_NOTAS; ++i)
    {
        int* p = new int(index++);
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DispararNotaThread, (LPVOID)p, 0, 0));
    }

    CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AtualizarMultiplicador, NULL, 0, 0));
}

DWORD AtualizarMultiplicador(LPVOID)
{
    while (!bCancel)
    {
        s_multiplicador = ObterMultiplicador();
        Sleep(1000);
    }

    return 0;
}

DWORD DispararNotaThread(LPVOID lpThreadParam)
{
    auto num = *(int*)reinterpret_cast<int*>(lpThreadParam);
    auto fp = g_notas[num];

    while (!bCancel)
    {
        DWORD key;

        auto hDc = GetDC(NULL);

        if (ProcurarNota(num, hDc, &key) == FALSE)
            g_states[num].bState = false;
        else
        {
            /*EnviarTeclaPressionada(key);
            EnviarTeclaPressionada(VK_SUBTRACT);*/

            /*CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DispararTeclaThread, (LPVOID)SEnviarTeclaParams { KeyCode, Sleep }, 0, 0)

            ENVIAR_TECLA_ASYNC(key, 100);*/

            if (!g_states[num].bState)
            {
                g_states[num].bState = true;
                g_states[num].count++;
            }
        }

        if (hDc != NULL)
            ReleaseDC(0, hDc);

        Sleep(1);
    }

    return 0;
}

BOOL ProcurarNota(DWORD index, HDC hDc, DWORD* key)
{
    SNota nota = g_notas[index];

    //COLORREF color = GetPixel(hDc, nota.dwPosX, nota.dwPosY);

    if(IsColorAtPixelGte(nota.dwPosX, nota.dwPosY, 222, 227, 222))

    /*if (GetRValue(color) >= 222 &&
        GetGValue(color) >= 227 &&
        GetBValue(color) >= 222)*/
    {
        *key = nota.dwOpCode;
        return TRUE;
    }

    return FALSE;
}

void EnviarTeclaPressionada(WORD key)
{
    /*INPUT i;
    i.type = INPUT_KEYBOARD;
    i.ki.dwFlags = 0;

    SendInput(1, (LPINPUT)&i, sizeof(INPUT));*/

    //SendMessage(g_hWnd, WM_KEYDOWN, (WPARAM)key, (LPARAM)1);

    INPUT i;
    i.type = INPUT_KEYBOARD;
    i.ki.wVk = key;
    i.ki.dwFlags = 0;

    SendInput(1, &i, sizeof(INPUT));
}

void EnviarTeclaSolta(WORD key)
{
    /*INPUT i;
    i.type = INPUT_KEYBOARD;
    i.ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(1, (LPINPUT)&i, sizeof(INPUT));*/

    //SendMessage(g_hWnd, WM_KEYUP, (WPARAM)key, (LPARAM)1);

    INPUT i;
    i.type = INPUT_KEYBOARD;
    i.ki.wVk = key;
    i.ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(1, &i, sizeof(INPUT));
}

int ObterMultiplicador()
{
    auto hDc = GetDC(0);

    if (IsColorAtPixel(346, 592, 219, 212, 33))
        return 2;

    if (IsColorAtPixel(342, 599, 181, 243, 82))
        return 3;

    if (IsColorAtPixel(332, 584, 252, 183, 229))
        return 4;

    return 1;

    if (hDc != NULL)
        ReleaseDC(0, hDc);
}