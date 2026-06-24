#include <GL/freeglut.h>
#include <cstdio>
#include "VGlobais.h"
#include "projecoes.h"

void desenhaImagemProjecao(){
    
}

void desenharProjecao(GLdouble z){
    switch(projecaoAtual){
        case projecao_ortho:
            glMatrixMode( GL_PROJECTION ); 
            glLoadIdentity(); 
            glOrtho(-8.0, 8.0, -8.0, 8.0, -300.0, 300.0); //vai de 65 a ? testar near e far
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0, 2, 5,  0, 0, 0,  0, 1, 0); //para zoom, precisa duplicar todas as variáveis do glOrtho 
            break;
        case projecao_perspective:
            glMatrixMode( GL_PROJECTION ); //ver modos de ajustar rotação
            glLoadIdentity(); 
            glOrtho(-8.0, 8.0, -8.0, 8.0, -8.0, 300.0); //vai de 65 a ? testar near e far
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0, 2, z,  0, 0, 0,  0, 1, 0);
            break;
        default:
            break;
    }
    
}

const char* nomeProjecaoAtual(){
    const char* nomes[] = {
        "Ortho", "Frustum", "Perspectiva"
    };
    return nomes[projecaoAtual];
}


