#ifndef VGlobais_H
#define VGlobais_H


typedef enum{

    objeto_ponto = 0,
    objeto_linha,
    objeto_triangulo,
    objeto_quadrado,
    objeto_cubo,
    objeto_esfera,
    objeto_teapot,
    objeto_cone

} TipoObjeto;

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


extern TipoObjeto objetoAtual;
extern TipoTransformacao transformacaoAtual;
extern Modulo moduloAtual;
extern int modoWire;
extern int larguraJanela;
extern int alturaJanela;


#endif