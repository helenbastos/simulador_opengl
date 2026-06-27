# include "VGlobais.h"

TipoObjeto objetoAtual = objeto_cubo;
Modulo     moduloAtual   = mod_objetos;

ValoresEixosTG eixosTransformacoes;
ValoresEixosP eixosProjecoes;
ComandosProjecoes comandoAtual = xMinOrtho;
TipoTransformacao transformacaoAtual = transformacao_escala;
TipoProjecao projecaoAtual = projecao_ortho; 

ParametroSelecionado parametroAtual = param_x;

int modoWire = 0;
int larguraJanela = 1000;
int alturaJanela = 680;
