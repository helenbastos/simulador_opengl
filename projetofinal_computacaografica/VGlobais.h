#ifndef VGlobais_H
#define VGlobais_H


typedef enum{

   // ---- 2D ----
    objeto_ponto = 0,
    objeto_linha,
    objeto_triangulo,
    objeto_quadrado,
    objeto_poligono,
    objeto_circulo,

    // ---- 3D ----
    objeto_cubo,
    objeto_esfera,
    objeto_cone,
    objeto_teapot,
    objeto_torus,
    objeto_cilindro,
    objeto_octaedro,
    objeto_icosaedro

} TipoObjeto;

#define NUM_OBJETOS 14

typedef enum{
    transformacao_escala,
    transformacao_rotacao,
    transformacao_translacao
}TipoTransformacao;

// Módulos do painel lateral
typedef enum
{
    mod_objetos = 0,
    mod_transformacoes,
    mod_projecoes,
    mod_iluminacao,
    mod_texturas,
    mod_curvas
} Modulo;

//modificados na main com os botões e usados em transformacoes.h para mostrar código
struct ValoresEixosTG {
    float xscale = 1.0f;
    float yscale = 1.0f;
    float xtranslate = 0.0f;
    float ytranslate = 0.0f;
    float xrotate = 20.0f;
    float yrotate = -30.0f;
};

enum ParametroSelecionado {
    param_nenhum,
    param_x,
    param_y
};


extern ParametroSelecionado parametroAtual;
extern ValoresEixosTG eixosTransformacoes; 
extern TipoObjeto objetoAtual;
extern TipoTransformacao transformacaoAtual;
extern Modulo moduloAtual;
extern int modoWire;
extern int larguraJanela;
extern int alturaJanela;


#endif