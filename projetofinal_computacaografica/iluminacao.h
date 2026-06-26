#ifndef ILUMINACAO_H
#define ILUMINACAO_H

typedef enum {
    luz_ambiental = 0,
    luz_direcional,
    luz_pontual,
    luz_spot
} TipoLuz;

typedef enum {
    mat_plastico = 0,
    mat_metal,
    mat_borracha,
    mat_ceramica
} TipoMaterial;

void desenharIluminacao();
void obterCodigoIluminacao(char* codigo, char* descricao);
const char* nomeLuzAtual();
void processarTecladoIluminacao(unsigned char key);
void processarTeclaEspecialIluminacao(int key);

#endif