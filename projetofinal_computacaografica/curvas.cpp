#include <GL/freeglut.h>
#include <cstdio>
#include <cmath>
#include "VGlobais.h"
#include "curvas.h"


// Modo de exibição atual (curva 1D / superfície / revolução)
ModoCurva modoCurvaAtual = modo_curva_1d;

// Grau de resolução do sólido de revolução (graus por fatia)
int passoRevolucao = 10;   // 360/10 = 36 fatias

// Altura do ponto central da superfície (ajustável com UP/DOWN)
float alturaSuperficie = 1.0f;

// Malha de controle 4x4 para glMap2f  [linha][coluna][x,y,z]
static float malhaControle[PATCH_N][PATCH_N][3];


static float coresCurva[4][3] = {
    { 0.1f, 0.5f, 0.9f },   // 0: Azul padrão
    { 0.1f, 0.8f, 0.3f },   // 1: Verde
    { 0.9f, 0.2f, 0.6f },   // 2: Rosa/Magenta
    { 0.9f, 0.6f, 0.1f }    // 3: Amarelo/Laranja
};


void calcularDeCasteljau(float t, float &outX, float &outY, float &outZ) {
    float x[4], y[4], z[4];
    for (int i = 0; i < 4; i++) {
        x[i] = curvaAtual.pontosX[i];
        y[i] = curvaAtual.pontosY[i];
        z[i] = curvaAtual.pontosZ[i];
    }
    // Reduz de 4 pontos para 1 ponto final via interpolações lineares
    for (int passo = 1; passo < 4; passo++)
        for (int i = 0; i < 4 - passo; i++) {
            x[i] = (1.0f - t) * x[i] + t * x[i+1];
            y[i] = (1.0f - t) * y[i] + t * y[i+1];
            z[i] = (1.0f - t) * z[i] + t * z[i+1];
        }
    outX = x[0];  outY = y[0];  outZ = z[0];
}


void desenharCurvaBezier() {
    glDisable(GL_LIGHTING);
    glLineWidth(curvaAtual.espessuraLinha);

    int c = curvaAtual.corAtual;
    glColor3f(coresCurva[c][0], coresCurva[c][1], coresCurva[c][2]);

    if (!curvaAtual.modoRender) {
        // --- Modo Evaluator nativo: glMap1f ---
        float pts[MAX_PONTOS_CONTROLE * 3];
        for (int i = 0; i < MAX_PONTOS_CONTROLE; i++) {
            pts[i*3+0] = curvaAtual.pontosX[i];
            pts[i*3+1] = curvaAtual.pontosY[i];
            pts[i*3+2] = curvaAtual.pontosZ[i];
        }
        glMap1f(GL_MAP1_VERTEX_3, 0.0f, 1.0f, 3, MAX_PONTOS_CONTROLE, pts);
        glEnable(GL_MAP1_VERTEX_3);
        glMapGrid1f(curvaAtual.numSegmentos, 0.0f, 1.0f);
        glEvalMesh1(GL_LINE, 0, curvaAtual.numSegmentos);
        glDisable(GL_MAP1_VERTEX_3);
    } else {
        // --- Modo De Casteljau manual ---
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= curvaAtual.numSegmentos; i++) {
            float t = (float)i / (float)curvaAtual.numSegmentos;
            float cx, cy, cz;
            calcularDeCasteljau(t, cx, cy, cz);
            glVertex3f(cx, cy, cz);
        }
        glEnd();
    }

    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}


void desenharPoligonoControle() {
    if (!curvaAtual.mostrarPoligono) return;
    glDisable(GL_LIGHTING);
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.5f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xAAAA);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < MAX_PONTOS_CONTROLE; i++)
        glVertex3f(curvaAtual.pontosX[i], curvaAtual.pontosY[i], curvaAtual.pontosZ[i]);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}


void desenharPontosControle() {
    float coresPontos[MAX_PONTOS_CONTROLE][3] = {
        { 0.85f, 0.15f, 0.15f },  // P0 - Vermelho
        { 0.15f, 0.55f, 0.85f },  // P1 - Azul
        { 0.85f, 0.55f, 0.10f },  // P2 - Laranja
        { 0.45f, 0.20f, 0.75f }   // P3 - Roxo
    };
    for (int i = 0; i < MAX_PONTOS_CONTROLE; i++) {
        glPushMatrix();
        glTranslatef(curvaAtual.pontosX[i], curvaAtual.pontosY[i], curvaAtual.pontosZ[i]);
        glDisable(GL_LIGHTING);
        if (i == curvaAtual.pontoSelecionado) {
            glColor3f(1.0f, 1.0f, 1.0f);
            glutSolidSphere(0.16, 16, 16);
        } else {
            glColor3f(coresPontos[i][0], coresPontos[i][1], coresPontos[i][2]);
            glutSolidSphere(0.12, 14, 14);
        }
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}


void selecionarPontoControle(int xTela, int yTela,
                              int larguraViewport, int alturaViewport) {
    GLint    viewport[4];
    GLdouble mv[16], proj[16];
    glGetIntegerv(GL_VIEWPORT,         viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, mv);
    glGetDoublev (GL_PROJECTION_MATRIX,proj);

    float yInv = (float)(viewport[3] - yTela);
    GLfloat prof;
    glReadPixels(xTela, (int)yInv, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &prof);

    GLdouble wx, wy, wz;
    gluUnProject(xTela, yInv, prof, mv, proj, viewport, &wx, &wy, &wz);

    const float tol = 0.35f;
    curvaAtual.pontoSelecionado = -1;
    for (int i = 0; i < MAX_PONTOS_CONTROLE; i++) {
        float dx = (float)wx - curvaAtual.pontosX[i];
        float dy = (float)wy - curvaAtual.pontosY[i];
        if (sqrtf(dx*dx + dy*dy) < tol) {
            curvaAtual.pontoSelecionado = i;
            break;
        }
    }
}

void arrastarPontoControle(int xTela, int yTela,
                            int larguraViewport, int alturaViewport) {
    if (curvaAtual.pontoSelecionado < 0) return;
    GLint    viewport[4];
    GLdouble mv[16], proj[16];
    glGetIntegerv(GL_VIEWPORT,         viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, mv);
    glGetDoublev (GL_PROJECTION_MATRIX,proj);

    float yInv = (float)(viewport[3] - yTela);
    GLdouble px, py, pz;
    // Projeta o ponto atual para obter a profundidade correta
    gluProject(curvaAtual.pontosX[curvaAtual.pontoSelecionado],
               curvaAtual.pontosY[curvaAtual.pontoSelecionado],
               curvaAtual.pontosZ[curvaAtual.pontoSelecionado],
               mv, proj, viewport, &px, &py, &pz);

    GLdouble wx, wy, wz;
    gluUnProject(xTela, yInv, pz, mv, proj, viewport, &wx, &wy, &wz);
    curvaAtual.pontosX[curvaAtual.pontoSelecionado] = (float)wx;
    curvaAtual.pontosY[curvaAtual.pontoSelecionado] = (float)wy;
    glutPostRedisplay();
}

void soltarPontoControle() {
    curvaAtual.pontoSelecionado = -1;
}


void resetarCurva() {
    aplicarPreset(preset_S);
    curvaAtual.pontoSelecionado = -1;
    curvaAtual.espessuraLinha   = 3.0f;
    curvaAtual.corAtual         = 0;
    curvaAtual.modoRender       = false;
}


void aplicarPreset(PresetCurva preset) {
    // Coordenadas X e Y para cada preset (Z = 0)
    static const float tabelaX[NUM_PRESETS][4] = {
        { -1.5f, -0.5f,  0.5f,  1.5f },  // S
        { -1.5f, -0.5f,  0.5f,  1.5f },  // Arco
        { -1.5f, -0.5f,  0.5f,  1.5f },  // Onda
        { -1.5f,  0.0f,  0.5f,  1.5f },  // Diagonal
    };
    static const float tabelaY[NUM_PRESETS][4] = {
        { -1.0f,  1.2f, -1.2f,  1.0f },  // S (curva em S clássica)
        { -0.5f,  1.8f,  1.8f, -0.5f },  // Arco (curva para cima)
        { -1.0f,  1.0f, -1.0f,  1.0f },  // Onda senoidal aproximada
        { -1.5f, -0.5f,  0.5f,  1.5f },  // Diagonal (quase linear)
    };
    for (int i = 0; i < MAX_PONTOS_CONTROLE; i++) {
        curvaAtual.pontosX[i] = tabelaX[preset][i];
        curvaAtual.pontosY[i] = tabelaY[preset][i];
        curvaAtual.pontosZ[i] = 0.0f;
    }
}


void resetarSuperficie() {
    for (int i = 0; i < PATCH_N; i++) {
        for (int j = 0; j < PATCH_N; j++) {
            // Distribui os pontos de -1.5 a +1.5 em X e Z
            malhaControle[i][j][0] = -1.5f + j * 1.0f;   // X
            malhaControle[i][j][2] = -1.5f + i * 1.0f;   // Z
            // Y padrão = 0, exceto a região central
            bool centroI = (i == 1 || i == 2);
            bool centroJ = (j == 1 || j == 2);
            malhaControle[i][j][1] = (centroI && centroJ) ? alturaSuperficie : 0.0f;
        }
    }
}


void ajustarAlturaSuperficie(float delta) {
    alturaSuperficie += delta;
    if (alturaSuperficie >  3.0f) alturaSuperficie =  3.0f;
    if (alturaSuperficie < -3.0f) alturaSuperficie = -3.0f;
    resetarSuperficie();  // recalcula a malha com o novo valor
}
void desenharSuperficieBezier() {
    resetarSuperficie();   // garante que a malha está atualizada

    // Flat array exigido pelo glMap2f: [v][u][xyz]
    float pts[PATCH_N * PATCH_N * 3];
    for (int i = 0; i < PATCH_N; i++)
        for (int j = 0; j < PATCH_N; j++) {
            int idx = (i * PATCH_N + j) * 3;
            pts[idx+0] = malhaControle[i][j][0];
            pts[idx+1] = malhaControle[i][j][1];
            pts[idx+2] = malhaControle[i][j][2];
        }
    vMin, vMax, passo_v (stride), ordem_v, dados
    glMap2f(GL_MAP2_VERTEX_3,
            0.0f, 1.0f, 3,        PATCH_N,  // u: 4 pontos, stride=3 floats
            0.0f, 1.0f, PATCH_N*3, PATCH_N, // v: 4 pontos, stride=4*3 floats
            pts);

    glEnable(GL_MAP2_VERTEX_3);
    glEnable(GL_AUTO_NORMAL);   // normais calculadas automaticamente

    // Grade de avaliação: resolução da malha renderizada
    int res = curvaAtual.numSegmentos / 5;
    if (res < 4) res = 4;
    glMapGrid2f(res, 0.0f, 1.0f, res, 0.0f, 1.0f);

    // Alterna entre wireframe e preenchido conforme modoWire global
    int c = curvaAtual.corAtual;
    glColor3f(coresCurva[c][0], coresCurva[c][1], coresCurva[c][2]);
    glEvalMesh2(GL_FILL, 0, res, 0, res);

    // Sobreposição wireframe para visualizar a malha
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(0.5f);
    glEvalMesh2(GL_LINE, 0, res, 0, res);
    glLineWidth(1.0f);

    glDisable(GL_AUTO_NORMAL);
    glDisable(GL_MAP2_VERTEX_3);
}


void desenharSolidoRevolucao() {
    int c = curvaAtual.corAtual;

    // Quantas fatias de revolução (depende do passoRevolucao)
    int numFatias = 360 / passoRevolucao;
    int numPts    = curvaAtual.numSegmentos + 1;

    // Pré-calcula todos os pontos da curva 1D (perfil)
    float* perfX = new float[numPts];
    float* perfY = new float[numPts];
    for (int i = 0; i < numPts; i++) {
        float t = (float)i / (float)curvaAtual.numSegmentos;
        float cx, cy, cz;
        calcularDeCasteljau(t, cx, cy, cz);
        perfX[i] = cx;   // raio em relação ao eixo Y
        perfY[i] = cy;   // altura
    }

    glEnable(GL_LIGHTING);

    // Itera pelas fatias angulares
    for (int f = 0; f < numFatias; f++) {
        float angA = (float)( f    * passoRevolucao) * M_PI / 180.0f;
        float angB = (float)((f+1) * passoRevolucao) * M_PI / 180.0f;
        float cosA = cosf(angA), sinA = sinf(angA);
        float cosB = cosf(angB), sinB = sinf(angB);

        // Cor com leve variação por fatia para realçar o volume
        float t_cor = (float)f / (float)numFatias;
        float r = coresCurva[c][0] * (0.7f + 0.3f * t_cor);
        float g = coresCurva[c][1] * (0.7f + 0.3f * t_cor);
        float b = coresCurva[c][2] * (0.7f + 0.3f * t_cor);
        glColor3f(r, g, b);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i < numPts; i++) {
            float rx = perfX[i];   // raio (distância ao eixo Y)
            float ry = perfY[i];   // altura

            // Ponto na fatia A
            glNormal3f(cosA, 0.0f, sinA);  // normal aproximada radial
            glVertex3f(rx * cosA, ry, rx * sinA);

            // Ponto na fatia B
            glNormal3f(cosB, 0.0f, sinB);
            glVertex3f(rx * cosB, ry, rx * sinB);
        }
        glEnd();
    }

    // Wireframe sobreposto (linhas de meridiano)
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(0.4f);
    for (int f = 0; f < numFatias; f++) {
        float ang = (float)(f * passoRevolucao) * M_PI / 180.0f;
        float c_  = cosf(ang), s_ = sinf(ang);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < numPts; i++) {
            glVertex3f(perfX[i]*c_, perfY[i], perfX[i]*s_);
        }
        glEnd();
    }
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);

    delete[] perfX;
    delete[] perfY;
}


void preencherPainelCurvas() {
    limparPainel();

    switch (modoCurvaAtual) {

    // ----------------------------------------------------------
    case modo_curva_1d:
        definirTituloPainel("CURVAS DE BEZIER 1D");

        if (!curvaAtual.modoRender) {
            adicionarLinhaCodigo("glMap1f(GL_MAP1_VERTEX_3,");
            adicionarLinhaCodigo("  0, 1, 3, 4, pontos);");
            adicionarLinhaCodigo("glEnable(GL_MAP1_VERTEX_3);");
            adicionarLinhaCodigo("glMapGrid1f(N, 0, 1);");
            adicionarLinhaCodigo("glEvalMesh1(GL_LINE,0,N);");
        } else {
            adicionarLinhaCodigo("// De Casteljau manual:");
            adicionarLinhaCodigo("for(int i=0;i<=N;i++){");
            adicionarLinhaCodigo("  t = i/(float)N;");
            adicionarLinhaCodigo("  deCasteljau(t,&x,&y,&z);");
            adicionarLinhaCodigo("  glVertex3f(x,y,z);");
            adicionarLinhaCodigo("}");
        }

        {
            char buf[64];
            sprintf(buf, "Modo: %s", curvaAtual.modoRender ? "De Casteljau" : "glMap1f");
            adicionarLinhaInfo(buf);
            sprintf(buf, "Segmentos: %d", curvaAtual.numSegmentos);
            adicionarLinhaInfo(buf);
            sprintf(buf, "Espessura: %.1f px", curvaAtual.espessuraLinha);
            adicionarLinhaInfo(buf);
            sprintf(buf, "P0(%.1f,%.1f) P1(%.1f,%.1f)",
                curvaAtual.pontosX[0], curvaAtual.pontosY[0],
                curvaAtual.pontosX[1], curvaAtual.pontosY[1]);
            adicionarLinhaInfo(buf);
            sprintf(buf, "P2(%.1f,%.1f) P3(%.1f,%.1f)",
                curvaAtual.pontosX[2], curvaAtual.pontosY[2],
                curvaAtual.pontosX[3], curvaAtual.pontosY[3]);
            adicionarLinhaInfo(buf);
        }

        adicionarLinhaAtalho("M - Alterna algoritmo");
        adicionarLinhaAtalho("G - Mostra/oculta poligono");
        adicionarLinhaAtalho("+/- Segmentos da curva");
        adicionarLinhaAtalho("[ ] Espessura da linha");
        adicionarLinhaAtalho("C - Cicla cor");
        adicionarLinhaAtalho("R - Reset pontos (S)");
        adicionarLinhaAtalho("1/2/3/4 - Presets de forma");
        adicionarLinhaAtalho("V - Modo Superficie");
        adicionarLinhaAtalho("B - Modo Revolucao");
        adicionarLinhaAtalho("Mouse: arrasta pontos");
        break;

    // ----------------------------------------------------------
    case modo_superficie:
        definirTituloPainel("SUPERFICIE DE BEZIER 2D");

        adicionarLinhaCodigo("glMap2f(GL_MAP2_VERTEX_3,");
        adicionarLinhaCodigo("  0,1, 3,4,");
        adicionarLinhaCodigo("  0,1, 12,4, pts);");
        adicionarLinhaCodigo("glEnable(GL_MAP2_VERTEX_3);");
        adicionarLinhaCodigo("glEnable(GL_AUTO_NORMAL);");
        adicionarLinhaCodigo("glMapGrid2f(N,0,1,N,0,1);");
        adicionarLinhaCodigo("glEvalMesh2(GL_FILL,0,N,0,N);");

        {
            char buf[64];
            sprintf(buf, "Altura central: %.2f", alturaSuperficie);
            adicionarLinhaInfo(buf);
            adicionarLinhaInfo("Patch 4x4 bicubico");
            adicionarLinhaInfo("GL_AUTO_NORMAL: ativo");
            int res = curvaAtual.numSegmentos / 5;
            if (res < 4) res = 4;
            sprintf(buf, "Resolucao grid: %dx%d", res, res);
            adicionarLinhaInfo(buf);
        }

        adicionarLinhaAtalho("UP/DOWN - Altura do bump");
        adicionarLinhaAtalho("+/-     - Resolucao grid");
        adicionarLinhaAtalho("C       - Cicla cor");
        adicionarLinhaAtalho("R       - Reset superficie");
        adicionarLinhaAtalho("V       - Volta curva 1D");
        adicionarLinhaAtalho("B       - Modo Revolucao");
        break;

    // ----------------------------------------------------------
    case modo_revolucao:
        definirTituloPainel("SOLIDO DE REVOLUCAO");

        adicionarLinhaCodigo("// Rotaciona perfil em Y:");
        adicionarLinhaCodigo("for(f=0;f<360;f+=passo){");
        adicionarLinhaCodigo("  angA=f*PI/180;");
        adicionarLinhaCodigo("  angB=(f+p)*PI/180;");
        adicionarLinhaCodigo("  glBegin(GL_QUAD_STRIP);");
        adicionarLinhaCodigo("  // x'=r*cos, z'=r*sin");
        adicionarLinhaCodigo("  glEnd();");
        adicionarLinhaCodigo("}");

        {
            char buf[64];
            sprintf(buf, "Passo angular: %d graus", passoRevolucao);
            adicionarLinhaInfo(buf);
            sprintf(buf, "Fatias: %d", 360 / passoRevolucao);
            adicionarLinhaInfo(buf);
            sprintf(buf, "Pontos por fatia: %d", curvaAtual.numSegmentos+1);
            adicionarLinhaInfo(buf);
            adicionarLinhaInfo("Perfil: curva Bezier 1D");
            adicionarLinhaInfo("Eixo de revolucao: Y");
        }

        adicionarLinhaAtalho("UP/DOWN - Passo angular");
        adicionarLinhaAtalho("+/-     - Segmentos perfil");
        adicionarLinhaAtalho("C       - Cicla cor");
        adicionarLinhaAtalho("R       - Reset perfil (S)");
        adicionarLinhaAtalho("1/2/3/4 - Presets de perfil");
        adicionarLinhaAtalho("Mouse   - Edita perfil");
        adicionarLinhaAtalho("V       - Volta curva 1D");
        adicionarLinhaAtalho("B       - Modo Superficie");
        break;
    }
}

// ============================================================
//  obterCodigoCurva — mantido para compatibilidade
// ============================================================
void obterCodigoCurva(char* codigo, char* descricao)
{
    switch (modoCurvaAtual)
    {
    // ----------------------------------------------------------
    case modo_curva_1d:
 
        if (!curvaAtual.modoRender)
        {
            // Mostra os valores reais dos pontos de controle
            sprintf(codigo,
                "glMap1f(GL_MAP1_VERTEX_3,0,1,3,4,pts); "
                "glEvalMesh1(GL_LINE,0,%d);",
                curvaAtual.numSegmentos);
 
            sprintf(descricao,
                "P0(%.2f,%.2f) P1(%.2f,%.2f) P2(%.2f,%.2f) P3(%.2f,%.2f) | segs:%d esp:%.1f",
                curvaAtual.pontosX[0], curvaAtual.pontosY[0],
                curvaAtual.pontosX[1], curvaAtual.pontosY[1],
                curvaAtual.pontosX[2], curvaAtual.pontosY[2],
                curvaAtual.pontosX[3], curvaAtual.pontosY[3],
                curvaAtual.numSegmentos,
                curvaAtual.espessuraLinha);
        }
        else
        {
            sprintf(codigo,
                "deCasteljau(t,x,y,z); // t em [0,1], %d iteracoes",
                curvaAtual.numSegmentos);
 
            sprintf(descricao,
                "P0(%.2f,%.2f) P1(%.2f,%.2f) P2(%.2f,%.2f) P3(%.2f,%.2f)",
                curvaAtual.pontosX[0], curvaAtual.pontosY[0],
                curvaAtual.pontosX[1], curvaAtual.pontosY[1],
                curvaAtual.pontosX[2], curvaAtual.pontosY[2],
                curvaAtual.pontosX[3], curvaAtual.pontosY[3]);
        }
        break;
 
    // ----------------------------------------------------------
    case modo_superficie:
    {
        int res = curvaAtual.numSegmentos / 5;
        if (res < 4) res = 4;
 
        sprintf(codigo,
            "glMap2f(GL_MAP2_VERTEX_3,...); "
            "glEvalMesh2(GL_FILL,0,%d,0,%d);",
            res, res);
 
        sprintf(descricao,
            "Patch 4x4 | Altura bump central: %.2f | Grid: %dx%d | UP/DOWN muda altura",
            alturaSuperficie, res, res);
        break;
    }
 
    // ----------------------------------------------------------
    case modo_revolucao:
 
        sprintf(codigo,
            "GL_QUAD_STRIP | %d fatias x %d graus | perfil: %d pts",
            360 / passoRevolucao,
            passoRevolucao,
            curvaAtual.numSegmentos + 1);
 
        sprintf(descricao,
            "P0(%.2f,%.2f) P1(%.2f,%.2f) P2(%.2f,%.2f) P3(%.2f,%.2f) | UP/DOWN: fatias",
            curvaAtual.pontosX[0], curvaAtual.pontosY[0],
            curvaAtual.pontosX[1], curvaAtual.pontosY[1],
            curvaAtual.pontosX[2], curvaAtual.pontosY[2],
            curvaAtual.pontosX[3], curvaAtual.pontosY[3]);
        break;
    }
}