# include "VGlobais.h"

TipoObjeto objetoAtual = objeto_cubo;
Modulo     moduloAtual   = mod_objetos;

ValoresEixosTG eixosTransformacoes;
ValoresEixosP eixosProjecoes;
ComandosProjecoes comandoAtual = xMinOrtho;
TipoTransformacao transformacaoAtual = transformacao_escala;
TipoProjecao projecaoAtual = projecao_ortho; 

ParametroSelecionado parametroAtual = param_x;

CameraState cameraSalva;

int modoWire = 0;
int larguraJanela = 1000;
int alturaJanela = 680;

ValoresCurva curvaAtual = {
    { -1.5f, -0.5f,  0.5f,  1.5f },   // X dos 4 pontos
    { -1.0f,  1.2f, -1.2f,  1.0f },   // Y dos 4 pontos
    {  0.0f,  0.0f,  0.0f,  0.0f },   // Z dos 4 pontos (plano fixo)
    -1,    // nenhum ponto selecionado no início
    60,    // 60 segmentos de resolução
    1      // mostrar polígono de controle por padrão
};

PainelOpenGL painel;
void limparPainel() {
    painel.titulo[0] = '\0';
    painel.numLinhasCodigo  = 0;
    painel.numLinhasInfo    = 0;
    painel.numLinhasAtalhos = 0;
}

void definirTituloPainel(const char* titulo) {
    strncpy(painel.titulo, titulo, 63);
    painel.titulo[63] = '\0';
}

void adicionarLinhaCodigo(const char* linha) {
    if (painel.numLinhasCodigo >= MAX_LINHAS_CODIGO) return;
    strncpy(painel.codigo[painel.numLinhasCodigo], linha, 63);
    painel.codigo[painel.numLinhasCodigo][63] = '\0';
    painel.numLinhasCodigo++;
}

void adicionarLinhaInfo(const char* linha) {
    if (painel.numLinhasInfo >= MAX_LINHAS_INFO) return;
    strncpy(painel.info[painel.numLinhasInfo], linha, 63);
    painel.info[painel.numLinhasInfo][63] = '\0';
    painel.numLinhasInfo++;
}

void adicionarLinhaAtalho(const char* linha) {
    if (painel.numLinhasAtalhos >= MAX_LINHAS_ATALHO) return;
    strncpy(painel.atalhos[painel.numLinhasAtalhos], linha, 63);
    painel.atalhos[painel.numLinhasAtalhos][63] = '\0';
    painel.numLinhasAtalhos++;
}