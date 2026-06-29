#ifndef TEXTURAS_H
#define TEXTURAS_H

typedef enum {
    tex_1d = 0,
    tex_2d,
    tex_imagem
} TipoTextura;

// Padrões de textura procedural
typedef enum {
    padrao_gradiente = 0,  // gradiente de cores (demonstra textura 1D)
    padrao_xadrez,         // tabuleiro de xadrez
    padrao_listras,        // listras coloridas
    padrao_circular        // padrão circular/radial
} PadraoTextura;

// Modos de aplicação da textura
typedef enum {
    modo_modulate = 0,  // GL_MODULATE: combina textura com iluminação
    modo_decal,         // GL_DECAL: só a textura, ignora iluminação
    modo_replace        // GL_REPLACE: substitui cor do material
} ModoTextura;

// Filtros de textura
typedef enum {
    filtro_nearest = 0,  // GL_NEAREST: blocos sólidos
    filtro_linear        // GL_LINEAR: interpolação suave
} FiltroTextura;

void inicializarTexturas();
void desenharTexturas();
void obterCodigoTextura(char* codigo, char* descricao);
const char* nomeTipoTexturaAtual();
void processarTecladoTexturas(unsigned char key);

#endif