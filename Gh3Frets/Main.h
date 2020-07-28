#include <Windows.h>
#include <gdiplus.h>
#include <string>

static HWND g_hWnd = NULL;

typedef struct SNota {
public:
    DWORD dwPosX;
    DWORD dwPosY;
    DWORD dwOpCode;
    const char* descricao;
};

typedef struct SNotaState {
public:
    volatile bool bState;
    volatile int count;
};

typedef struct SEnviarTeclaParams {
public:
    WORD wKey;
    UINT uSleep;
};

#define DEFINIR_NOTA(Name, PosX, PosY, OpCode) SNota { PosX, PosY, OpCode, #Name }

#define MAX_NOTAS 5

static int s_multiplicador = 0;

static SNota g_notas[MAX_NOTAS]
{
    DEFINIR_NOTA(Verde, 488, 640, 0x31),
    DEFINIR_NOTA(Vermelho, 586, 640, 0x32),
    DEFINIR_NOTA(Amarelo, 684, 640, 0x33),
    DEFINIR_NOTA(Azul, 781, 640, 0x34),
    DEFINIR_NOTA(Laranja, 879, 640, 0x35)
};

static SNotaState g_states[MAX_NOTAS] =
{
    { false, 0 },
    { false, 0 },
    { false, 0 },
    { false, 0 },
    { false, 0 }
};

void IniciarThreads();
DWORD DispararNotaThread(LPVOID);
DWORD DispararTeclaThread(LPVOID);
DWORD AtualizarMultiplicador(LPVOID);

BOOL ProcurarNota(DWORD index, HDC hDc, DWORD* key);
void EnviarTeclaPressionada(WORD key);
void EnviarTeclaSolta(WORD key);
int ObterMultiplicador();

#define IsColorAtPixel(X, Y, R, G, B) \
GetRValue(GetPixel(hDc, X, Y)) == R && \
GetGValue(GetPixel(hDc, X, Y)) == G && \
GetBValue(GetPixel(hDc, X, Y)) == B

#define IsColorAtPixelCmp(X, Y, R, G, B) \
_CMP(GetRValue(GetPixel(hDc, X, Y)), R) && \
_CMP(GetGValue(GetPixel(hDc, X, Y)), G) && \
_CMP(GetBValue(GetPixel(hDc, X, Y)), B)

#define IsColorAtPixelEq(X, Y, R, G, B) \
_EQ(GetRValue(GetPixel(hDc, X, Y)), R) && \
_EQ(GetGValue(GetPixel(hDc, X, Y)), G) && \
_EQ(GetBValue(GetPixel(hDc, X, Y)), B)

#define IsColorAtPixelLte(X, Y, R, G, B) \
_LTE(GetRValue(GetPixel(hDc, X, Y)), R) && \
_LTE(GetGValue(GetPixel(hDc, X, Y)), G) && \
_LTE(GetBValue(GetPixel(hDc, X, Y)), B)

#define IsColorAtPixelGte(X, Y, R, G, B) \
_GTE(GetRValue(GetPixel(hDc, X, Y)), R) && \
_GTE(GetGValue(GetPixel(hDc, X, Y)), G) && \
_GTE(GetBValue(GetPixel(hDc, X, Y)), B)

#define _EQ(X, Y) X == Y
#define _LTE(X, Y) X <= Y
#define _GTE(X, Y) X >= Y
#define _CMP(X, Y) (_EQ(X, Y)) || (_LTE(X, Y)) || (_GTE(X, Y))