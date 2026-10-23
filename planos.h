#ifndef PLANOS_H
#define PLANOS_H
#include "vetores.h"

// Armazena os coeficientes da equacao geral ax + by + cz + d = 0.
// O vetor (a,b,c) e normal ao plano, e d posiciona o plano no espaco.
struct Plano3D {
    double a;
    double b;
    double c;
    double d;
    Vetor3D vetorNormal;
};

bool gerarPlanoPorTresPontos(Vetor3D pontoA, Vetor3D pontoB, Vetor3D pontoC, Plano3D &plano);
double calcularDistanciaPontoPlano(Vetor3D ponto, Plano3D plano);
bool verificarPertinencia(Vetor3D ponto, Plano3D plano);

#endif
