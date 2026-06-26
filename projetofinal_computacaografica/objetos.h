#ifndef OBJETOS_H
#define OBJETOS_H

// Desenha o objeto selecionado em objetoAtual
void desenharObjeto();

// Retorna o nome do objeto atual (para exibir no HUD)
const char* nomeObjetoAtual();

// Indica se o objeto atual é 2D (true) ou 3D (false)
// Usado para decidir se mostra a opção de wireframe
bool objetoAtualE2D();

// Monta a string do comando OpenGL e a descrição
// do objeto atual (para o painel de feedback)
void obterCodigoObjeto(char* codigo, char* descricao);  

#endif