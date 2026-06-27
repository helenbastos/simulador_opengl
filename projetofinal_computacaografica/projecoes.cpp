#include <GL/freeglut.h>
#include <cstdio>
#include "VGlobais.h"
#include "projecoes.h"

void desenhaImagemProjecao(){ //três cubos identicos em posições distintas
    glColor3f(0.2f, 0.7f, 1.0f);
    glPushMatrix();
        glTranslatef(0, 1, -2);
        glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-2, -2, -1);
        glutSolidCube(1);
    glPopMatrix();
    glPushMatrix();
        glTranslatef(1, -1, -3);
        glutSolidCube(1);
    glPopMatrix();
}

void desenharProjecao(){
    switch(projecaoAtual){
        case projecao_ortho:
            glMatrixMode( GL_PROJECTION ); 
            glLoadIdentity(); 
            glOrtho(eixosProjecoes.xMinOrtho*eixosProjecoes.zoom, (-eixosProjecoes.xMinOrtho)*eixosProjecoes.zoom, eixosProjecoes.yMinOrtho*eixosProjecoes.zoom, (-eixosProjecoes.yMinOrtho)*eixosProjecoes.zoom, eixosProjecoes.nearOrtho, 300.0); 
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0, 2, 5,  0, 0, 0,  0, 1, 0);  
            break;
        case projecao_frustum:
            glMatrixMode( GL_PROJECTION ); 
            glLoadIdentity(); 
            glFrustum(eixosProjecoes.xMinFrustum,-eixosProjecoes.xMinFrustum,eixosProjecoes.yMinFrustum,-eixosProjecoes.yMinFrustum,eixosProjecoes.nearFrustum,300); 
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0,20,1*eixosProjecoes.zoom, 0,0,0,0,1,0);
            break;
        case projecao_perspective:
            glMatrixMode( GL_PROJECTION ); 
            glLoadIdentity(); 
            gluPerspective(eixosProjecoes.fovyPerspective, eixosProjecoes.aspectPerspective, eixosProjecoes.nearPerspective, eixosProjecoes.farPerspective);                              //ver como integrar isso ao de projecoes
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0, 2, eixosProjecoes.zoom,  0, 0, 0,  0, 1, 0);
            break;
        default:
            break;
    }
    
}

const char* nomeProjecaoAtual(){
    const char* nomes[] = {
        "Ortorgonal", "Frustum", "Perspectiva"
    };
    return nomes[projecaoAtual];
}

void obterCodigoProjecao(char* codigo, char* descricao){
    switch(projecaoAtual){
        case projecao_ortho:
            snprintf(codigo, 256,"glOrtho(%.1f, %.1f, %.1f, %.1f, %.1f, 300); gluLookAt(0, 2, 5,  0, 0, 0,  0, 1, 0);", 
                eixosProjecoes.xMinOrtho*eixosProjecoes.zoom, (-eixosProjecoes.xMinOrtho)*eixosProjecoes.zoom,
                eixosProjecoes.yMinOrtho*eixosProjecoes.zoom, (-eixosProjecoes.yMinOrtho)*eixosProjecoes.zoom,
                eixosProjecoes.nearOrtho
            );
            //snprintf(descricao,256,"void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)");
            if(comandoAtual==xMinOrtho){
                snprintf(descricao, 256, "Valores de x definem os limites esquerdo/direito da janela de visualizacao.");
            }else if(comandoAtual == yMinOrtho){
                snprintf(descricao, 256, "Valores de y definem os limites inferior/superior da janela de visualizacao.");
            }else if(comandoAtual==nearOrtho){
                snprintf(descricao, 256, "Valores de near/far define a distancia minima/maxima do plano de recorte.");
            }
            break;
        case projecao_perspective:
            snprintf(codigo, 256,"gluPerspective(%.1f, %.1f, %.1f, %.1f); gluLookAt(0, 2, %.1f,  0, 0, 0,  0, 1, 0);", 
                eixosProjecoes.fovyPerspective, eixosProjecoes.aspectPerspective,
                eixosProjecoes.nearPerspective, eixosProjecoes.farPerspective,
                eixosProjecoes.zoom
            );
            if(comandoAtual==fovyPerspective){
                snprintf(descricao, 256, "O angulo de abertura controla o campo de visao da camera.");
            }else if(comandoAtual==nearPerspective){
                snprintf(descricao, 256, "Valores de near modificam a distancia minima visivel.");
            }else if(comandoAtual==farPerspective){
                snprintf(descricao, 256, "Valores de far modificam a distancia maxima visivel.");
            }
            break;
        case projecao_frustum:
            snprintf(codigo, 256,"glFrustum(%.1f, %.1f, %.1f, %.1f,1, %.1f, 300); gluLookAt(0, 2, %.1f,  0, 0, 0,  0, 1, 0);", 
                eixosProjecoes.xMinFrustum, (-eixosProjecoes.xMinFrustum),
                eixosProjecoes.yMinFrustum, (-eixosProjecoes.yMinFrustum),
                eixosProjecoes.nearFrustum, 1*eixosProjecoes.zoom
            );
            if(comandoAtual==yMinFrustum){
                snprintf(descricao, 256, "Valores de y definem o campo de visao vertical.");
            }else if(comandoAtual==xMinFrustum){
                snprintf(descricao, 256, "Valores de x definem a abertura horizontal da projecao.");
            }else if(comandoAtual==nearFrustum){
                snprintf(descricao, 256, "Valores de near/far define a distancia minima/maxima do volume de visualizacao.");
            }
            break;
        default:
            break;
    }

}

void obterComandosTeclas(char* buf){
    switch (projecaoAtual){
        case projecao_ortho:
            snprintf(buf, 100, "X - X minimo | Y - Y minimo | N - Near | R - Resetar");
            break;
        case projecao_frustum:
            snprintf(buf, 100, "X - X minimo | Y - Y minimo | N - Near | R - Resetar");
            break;
        case projecao_perspective:
            snprintf(buf, 100, "Y - Angulo de abertura  |  N - Near | F - Far | R - Resetar");
            break;
        default:
            break;
    }
}

