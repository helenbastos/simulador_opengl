#include <GL/freeglut.h>
#include <cstdio>
#include "VGlobais.h"
#include "transformacoes.h"

void desenharTransformacao(){
    glColor3f(0.8f, 0.3f, 0.3f);
    glTranslatef(eixosTransformacoes.xtranslate, eixosTransformacoes.ytranslate, 0.0f);
    glScalef(eixosTransformacoes.xscale, eixosTransformacoes.yscale, 1.0f);
    glutSolidTeapot(1.0f);
}

const char* nomeTransformacaoAtual(){
    const char* nomes[] = {
        "Escala", "Rotacao", "Translacao"
    };
    return nomes[transformacaoAtual];
}

void obterCodigoTransformacao(char* codigo, char* descricao){
    snprintf(codigo, 256,"glColor3f(0.8f, 0.3f, 0.3f); glTranslatef(%.1ff, %.1ff, 0.0f); glScalef(%.1ff, %.1ff, 1.0f); glRotatef(%.1ff, 1.0f, 0.0f, 0.0f); glRotatef(%.1ff, 0.0f, 1.0f, 0.0f); glutSolidTeapot(1.0f);", 
        eixosTransformacoes.xtranslate, eixosTransformacoes.ytranslate, 
        eixosTransformacoes.xscale, eixosTransformacoes.yscale,
        eixosTransformacoes.xrotate, eixosTransformacoes.yrotate    
    );
}

