#ifndef VGLOBAIS_H
#define VGLOBAIS_H
#include <GL/freeglut.h>

//==============================================================
// OBJETOS
//==============================================================

typedef enum{

    // ----- Objetos 2D -----
    objeto_ponto = 0,
    objeto_linha,
    objeto_triangulo,
    objeto_quadrado,
    objeto_poligono,
    objeto_circulo,

    // ----- Objetos 3D -----
    objeto_cubo,
    objeto_esfera,
    objeto_cone,
    objeto_teapot,
    objeto_torus,
    objeto_cilindro,
    objeto_octaedro,
    objeto_icosaedro

}TipoObjeto;

#define NUM_OBJETOS 14

//==============================================================
// TRANSFORMAÇÕES
//==============================================================

typedef enum{

    transformacao_escala,
    transformacao_rotacao,
    transformacao_translacao

}TipoTransformacao;

//==============================================================
// PROJEÇÕES
//==============================================================

typedef enum{

    projecao_ortho,
    projecao_frustum,
    projecao_perspective

}TipoProjecao;

//==============================================================
// MÓDULOS
//==============================================================

typedef enum{

    mod_objetos = 0,
    mod_transformacoes,
    mod_projecoes,
    mod_iluminacao,
    mod_texturas,
    mod_curvas

}Modulo;

//==============================================================
// VALORES DAS TRANSFORMAÇÕES
//==============================================================

struct ValoresEixosTG
{
    float xscale = 1.0f;
    float yscale = 1.0f;

    float xtranslate = 0.0f;
    float ytranslate = 0.0f;

    float xrotate = 20.0f;
    float yrotate = -30.0f;
};

//==============================================================
// VALORES DAS PROJEÇÕES
//==============================================================

struct ValoresEixosP
{
    GLdouble xMinOrtho = -2;
    GLdouble yMinOrtho = -2;
    GLdouble nearOrtho = -50;

    GLdouble xMinFrustum = -1;
    GLdouble yMinFrustum = -1;
    GLdouble nearFrustum = 5;

    GLdouble fovyPerspective = 45;
    GLdouble aspectPerspective;

    GLdouble nearPerspective = 0.1;
    GLdouble farPerspective = 100;

    GLdouble zoom = 5;
};

typedef enum{

    xMinOrtho,
    yMinOrtho,
    nearOrtho,

    xMinFrustum,
    yMinFrustum,
    nearFrustum,

    fovyPerspective,
    nearPerspective,
    farPerspective

}ComandosProjecoes;

//==============================================================
// PARÂMETRO DESTACADO NO PAINEL
//==============================================================

enum ParametroSelecionado
{
    param_nenhum,
    param_x,
    param_y
};
struct CameraState {
    double modelview[16];
    double projecao[16];
    int    viewport[4];
};
extern CameraState cameraSalva;
//==============================================================
// CURVAS
//==============================================================

#define MAX_PONTOS_CONTROLE 4

struct ValoresCurva
{
    float pontosX[MAX_PONTOS_CONTROLE];
    float pontosY[MAX_PONTOS_CONTROLE];
    float pontosZ[MAX_PONTOS_CONTROLE];

    int pontoSelecionado;

    int numSegmentos;

    int mostrarPoligono;

    float espessuraLinha;

    int corAtual;

    bool modoRender;
};

//==============================================================
// PAINEL DIDÁTICO OPENGL
//==============================================================

#define MAX_LINHAS_CODIGO 6
#define MAX_LINHAS_INFO   6
#define MAX_LINHAS_ATALHO 20

struct PainelOpenGL {
    char titulo[64];

    char codigo[MAX_LINHAS_CODIGO][64];
    int  numLinhasCodigo;

    char info[MAX_LINHAS_INFO][64];
    int  numLinhasInfo;

    char atalhos[MAX_LINHAS_ATALHO][64];
    int  numLinhasAtalhos;
};

//==============================================================
// VARIÁVEIS GLOBAIS
//==============================================================

extern ValoresCurva curvaAtual;

extern ValoresEixosTG eixosTransformacoes;
extern ValoresEixosP  eixosProjecoes;

extern ParametroSelecionado parametroAtual;

extern ComandosProjecoes comandoAtual;

extern TipoObjeto objetoAtual;

extern TipoTransformacao transformacaoAtual;

extern TipoProjecao projecaoAtual;

extern Modulo moduloAtual;

extern PainelOpenGL painel;

extern int modoWire;

extern int larguraJanela;

extern int alturaJanela;

//==============================================================
// FUNÇÕES DO PAINEL
//==============================================================

void limparPainel();
// Define o título do painel
void definirTituloPainel(const char* titulo);

// Adiciona uma linha de código (até MAX_LINHAS_CODIGO)
void adicionarLinhaCodigo(const char* linha);

// Adiciona uma linha de info/parâmetro (até MAX_LINHAS_INFO)
void adicionarLinhaInfo(const char* linha);

// Adiciona uma linha de atalho de teclado (até MAX_LINHAS_ATALHO)
void adicionarLinhaAtalho(const char* linha);

#endif