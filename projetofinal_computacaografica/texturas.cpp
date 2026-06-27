#include <GL/freeglut.h>
#include <cstdio>
#include <cmath>
#include "texturas.h"
#include "VGlobais.h"
#include "objetos.h"

// stb_image incluido POR ULTIMO para nao conflitar com freeglut
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.14159265358979323846f
#define TEX_1D_SIZE  16
#define TEX_2D_SIZE  64

// ============================================================
//  Estado interno
// ============================================================
static TipoTextura   tipoTexturaAtual = tex_2d;
static PadraoTextura padraoAtual      = padrao_xadrez;
static ModoTextura   modoAtual        = modo_modulate;
static FiltroTextura filtroAtual      = filtro_linear;

static GLuint idTex1D      = 0;
static GLuint idTex2D      = 0;
static GLuint idTexImagem  = 0;
static int    texturaSuja  = 1;
static int    imagemCarregada = 0;

// ============================================================
//  Carregar imagem de arquivo
// ============================================================
static void carregarImagemArquivo(const char* caminho) {
    int largura, altura, canais;
    unsigned char* dados = stbi_load(caminho, &largura, &altura, &canais, 3);

    if (!dados) {
        printf("Erro ao carregar textura: %s\n", caminho);
        imagemCarregada = 0;
        return;
    }

    if (idTexImagem == 0) glGenTextures(1, &idTexImagem);
    glBindTexture(GL_TEXTURE_2D, idTexImagem);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 largura, altura, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, dados);

    stbi_image_free(dados);
    imagemCarregada = 1;
    printf("Textura carregada: %dx%d\n", largura, altura);
}

// ============================================================
//  Gerar textura 1D procedural
// ============================================================
static void gerarTextura1D() {
    GLubyte dados[TEX_1D_SIZE][3];

    for (int i = 0; i < TEX_1D_SIZE; i++) {
        float t = (float)i / (TEX_1D_SIZE - 1);

        switch (padraoAtual) {
            case padrao_gradiente:
                dados[i][0] = (GLubyte)((1.0f - t) * 255);
                dados[i][1] = (GLubyte)(sinf(t * PI) * 200);
                dados[i][2] = (GLubyte)(t * 255);
                break;
            case padrao_listras:
            default:
                if ((i % 4) < 2) {
                    dados[i][0] = 255; dados[i][1] = 220; dados[i][2] = 0;
                } else {
                    dados[i][0] = 20;  dados[i][1] = 60;  dados[i][2] = 180;
                }
                break;
        }
    }

    if (idTex1D == 0) glGenTextures(1, &idTex1D);
    glBindTexture(GL_TEXTURE_1D, idTex1D);

    GLenum filtroGL = (filtroAtual == filtro_linear) ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, filtroGL);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, filtroGL);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB,
                 TEX_1D_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);
}

// ============================================================
//  Gerar textura 2D procedural
// ============================================================
static void gerarTextura2D() {
    GLubyte dados[TEX_2D_SIZE][TEX_2D_SIZE][3];

    for (int y = 0; y < TEX_2D_SIZE; y++) {
        for (int x = 0; x < TEX_2D_SIZE; x++) {
            float s = (float)x / TEX_2D_SIZE;
            float t = (float)y / TEX_2D_SIZE;
            GLubyte r, g, b;

            switch (padraoAtual) {
                case padrao_xadrez: {
                    int qx = (int)(s * 8);
                    int qy = (int)(t * 8);
                    if ((qx + qy) % 2 == 0) { r=240; g=60;  b=60;  }
                    else                      { r=60;  g=60;  b=240; }
                    break;
                }
                case padrao_listras: {
                    int faixa = (int)(t * 8) % 4;
                    if      (faixa == 0) { r=255; g=80;  b=80;  }
                    else if (faixa == 1) { r=80;  g=255; b=80;  }
                    else if (faixa == 2) { r=80;  g=80;  b=255; }
                    else                 { r=255; g=255; b=80;  }
                    break;
                }
                case padrao_gradiente: {
                    r = (GLubyte)(s * 255);
                    g = (GLubyte)(t * 255);
                    b = (GLubyte)((1.0f - s * 0.5f - t * 0.5f) * 255);
                    break;
                }
                case padrao_circular: {
                    float dx   = s - 0.5f;
                    float dy   = t - 0.5f;
                    float dist = sqrtf(dx*dx + dy*dy) * 2.0f;
                    float val  = sinf(dist * PI * 6) * 0.5f + 0.5f;
                    r = (GLubyte)(val * 200 + 55);
                    g = (GLubyte)((1.0f - val) * 180 + 40);
                    b = (GLubyte)(sinf(dist * PI * 3) * 100 + 155);
                    break;
                }
                default: r = g = b = 200;
            }

            dados[y][x][0] = r;
            dados[y][x][1] = g;
            dados[y][x][2] = b;
        }
    }

    if (idTex2D == 0) glGenTextures(1, &idTex2D);
    glBindTexture(GL_TEXTURE_2D, idTex2D);

    GLenum filtroGL = (filtroAtual == filtro_linear) ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtroGL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtroGL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 TEX_2D_SIZE, TEX_2D_SIZE, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, dados);
}

// ============================================================
//  inicializarTexturas
// ============================================================
void inicializarTexturas() {
    gerarTextura1D();
    gerarTextura2D();
    texturaSuja = 0;
}

// ============================================================
//  aplicarModoTextura
// ============================================================
static void aplicarModoTextura() {
    GLenum modoGL;
    switch (modoAtual) {
        case modo_modulate: modoGL = GL_MODULATE; break;
        case modo_decal:    modoGL = GL_DECAL;    break;
        case modo_replace:  modoGL = GL_REPLACE;  break;
        default:            modoGL = GL_MODULATE;
    }
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, modoGL);
}

// ============================================================
//  configurarTexGen
// ============================================================
static void configurarTexGen() {
    if (tipoTexturaAtual == tex_1d) {
        glEnable(GL_TEXTURE_GEN_S);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        GLfloat planoS[] = { 1.0f, 0.0f, 0.0f, 0.0f };
        glTexGenfv(GL_S, GL_OBJECT_PLANE, planoS);
    } else {
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        GLfloat planoS[] = { 1.0f, 0.0f, 1.0f, 0.0f };
        GLfloat planoT[] = { 0.0f, 1.0f, 1.0f, 0.0f };
        glTexGenfv(GL_S, GL_OBJECT_PLANE, planoS);
        glTexGenfv(GL_T, GL_OBJECT_PLANE, planoT);
    }
}

// ============================================================
//  desenharTexturas
// ============================================================
void desenharTexturas() {
    if (moduloAtual != mod_texturas) return;

    if (texturaSuja) {
        gerarTextura1D();
        gerarTextura2D();
        texturaSuja = 0;
    }

    // Iluminacao so no modo modulate
    if (modoAtual == modo_modulate) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    } else {
        glDisable(GL_LIGHTING);
    }

    glColor3f(1.0f, 1.0f, 1.0f);

    if (tipoTexturaAtual == tex_1d) {
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, idTex1D);
        aplicarModoTextura();
    } else if (tipoTexturaAtual == tex_2d) {
        glDisable(GL_TEXTURE_1D);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTex2D);
        aplicarModoTextura();
    } else if (tipoTexturaAtual == tex_imagem) {
        if (!imagemCarregada) {
            carregarImagemArquivo("textura.jpg");
        }
        glDisable(GL_TEXTURE_1D);
        glEnable(GL_TEXTURE_2D);
        if (imagemCarregada) glBindTexture(GL_TEXTURE_2D, idTexImagem);
        aplicarModoTextura();
    }

    configurarTexGen();
    desenharObjeto();

    // RESTAURAR ESTADO — essencial para nao buglar outros modulos
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

// ============================================================
//  nomeTipoTexturaAtual
// ============================================================
const char* nomeTipoTexturaAtual() {
    switch (tipoTexturaAtual) {
        case tex_1d:     return "1D";
        case tex_2d:     return "2D";
        case tex_imagem: return "Imagem";
    }
    return "?";
}

// ============================================================
//  obterCodigoTextura
// ============================================================
void obterCodigoTextura(char* codigo, char* descricao) {
    const char* filtroStr = (filtroAtual == filtro_linear) ? "GL_LINEAR" : "GL_NEAREST";
    const char* modoStr;
    switch (modoAtual) {
        case modo_modulate: modoStr = "GL_MODULATE"; break;
        case modo_decal:    modoStr = "GL_DECAL";    break;
        case modo_replace:  modoStr = "GL_REPLACE";  break;
        default:            modoStr = "GL_MODULATE";
    }

    const char* padraoStr;
    switch (padraoAtual) {
        case padrao_xadrez:    padraoStr = "Xadrez";    break;
        case padrao_listras:   padraoStr = "Listras";   break;
        case padrao_gradiente: padraoStr = "Gradiente"; break;
        case padrao_circular:  padraoStr = "Circular";  break;
        default:               padraoStr = "?";
    }

    if (tipoTexturaAtual == tex_1d) {
        sprintf(codigo,
            "glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, %d, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);",
            TEX_1D_SIZE);
        sprintf(descricao,
            "Tex1D | Padrao: %s | Filtro: %s | Modo: %s",
            padraoStr, filtroStr, modoStr);
    } else if (tipoTexturaAtual == tex_imagem) {
        sprintf(codigo, "stbi_load(\"textura.jpg\", &w, &h, &c, 3); glTexImage2D(...);");
        sprintf(descricao, "Imagem carregada de arquivo | Filtro: %s | Modo: %s",
            filtroStr, modoStr);
    } else {
        sprintf(codigo,
            "glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, %d, %d, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);",
            TEX_2D_SIZE, TEX_2D_SIZE);
        sprintf(descricao,
            "Tex2D | Padrao: %s | Filtro: %s | Modo: %s",
            padraoStr, filtroStr, modoStr);
    }
}

// ============================================================
//  processarTecladoTexturas
// ============================================================
void processarTecladoTexturas(unsigned char key) {
    if (key == 't' || key == 'T') {
        tipoTexturaAtual = (TipoTextura)((tipoTexturaAtual + 1) % 3);
        texturaSuja = 1;
    }
    if (key == 'p' || key == 'P') {
        if (tipoTexturaAtual == tex_1d) {
            if (padraoAtual == padrao_gradiente) padraoAtual = padrao_listras;
            else padraoAtual = padrao_gradiente;
        } else {
            padraoAtual = (PadraoTextura)((padraoAtual + 1) % 4);
        }
        texturaSuja = 1;
    }
    if (key == 'f' || key == 'F') {
        filtroAtual = (FiltroTextura)((filtroAtual + 1) % 2);
        texturaSuja = 1;
    }
    if (key == 'm' || key == 'M') {
        modoAtual = (ModoTextura)((modoAtual + 1) % 3);
        texturaSuja = 1;
    }
}