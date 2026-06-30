#ifndef CURVAS_H
#define CURVAS_H

// ---- Constantes ----
#define MAX_PONTOS_CONTROLE   4   // pontos da curva 1D
#define PATCH_N               4   // grid 4x4 para glMap2f
#define MAX_PASSOS_REVOLUCAO 36   // fatias de revolução (360 / 10 graus)

enum ModoCurva {
    modo_curva_1d = 0,   // Curva de Bézier 1D clássica
    modo_superficie,     // Patch de superfície Bézier (glMap2f)
    modo_revolucao       // Sólido de revolução gerado da curva
};


enum PresetCurva {
    preset_S = 0,
    preset_arco,
    preset_onda,
    preset_diagonal,
    NUM_PRESETS
};



// Desenha a curva de Bézier usando glMap1f (evaluator do OpenGL)
void desenharCurvaBezier();

// Desenha os pontos de controle como esferas coloridas
void desenharPontosControle();

// Desenha o polígono de controle com linha pontilhada
void desenharPoligonoControle();

// Detecta clique perto de um ponto de controle
void selecionarPontoControle(int xTela, int yTela,
                              int larguraViewport, int alturaViewport);

// Move o ponto selecionado para a posição do mouse
void arrastarPontoControle(int xTela, int yTela,
                            int larguraViewport, int alturaViewport);

// Solta o ponto selecionado
void soltarPontoControle();

// Reseta os pontos para o preset padrão (forma S)
void resetarCurva();

// Aplica um preset específico de pontos de controle
void aplicarPreset(PresetCurva preset);

// Desenha o patch de superfície Bézier 4x4 com glMap2f
void desenharSuperficieBezier();

// Inicializa a malha de controle 4x4 (grid plano com bump central)
void resetarSuperficie();

// Altera a altura (Y) do ponto de controle central do patch
// delta: valor a somar ao Y do ponto central (teclas UP/DOWN)
void ajustarAlturaSuperficie(float delta);

// Gera e desenha o sólido de revolução a partir da curva 1D
// Rotaciona a curva em torno do eixo Y, usando glRotatef
void desenharSolidoRevolucao();



// Preenche a struct global "painel" com título, código,
// infos e atalhos de acordo com o modo atual da curva
void preencherPainelCurvas();

// Retorna strings de código e descrição para o modo atual
// (mantido para compatibilidade com código existente)
void obterCodigoCurva(char* codigo, char* info);


extern ModoCurva modoCurvaAtual;

// Número de segmentos do sólido de revolução (controlado por +/-)
extern int passoRevolucao;   // graus por fatia (5..30)

// Altura atual do ponto central da superfície
extern float alturaSuperficie;

#endif