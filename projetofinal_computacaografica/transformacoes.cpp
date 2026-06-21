#include <GL/freeglut.h>
#include <cstdio>
#include "VGlobais.h"
#include "transformacoes.h"

void desenharTransformacao(float xtranslate, float ytranslate, float xscale, float yscale){
    //variéveis de variacao de x,y para translação e escala
    //transformar ângulo y e z em variáveis globais
    //onde usar reset?
    glDisable(GL_LIGHTING); 
    //escolher objeto
    glTranslatef(0.0f, 0.0f, -5.0f);
    glutWireSphere(1.0, 32, 32);
    glutWireTeapot(1.0f);
    glEnable(GL_LIGHTING);
}