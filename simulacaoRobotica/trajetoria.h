#ifndef TRAJETORIA_H
#define TRAJETORIA_H
#include "../vetores.h"

// Representa uma reta parametrica: origem + parametro * direcao.
struct Trajetoria3D {
    Vetor3D origem;
    Vetor3D direcao;
};

// Resultado completo para validacao matematica do movimento.
// tipoIntersecao: 0 = unica; 1 = paralela; 2 = contida; 3 = direcao nula.
struct ResultadoRobotica {
    int tipoIntersecao;
    double parametro;
    Vetor3D impacto;
    double distanciaInicial;
    double anguloIncidenciaGraus;
    double distanciaPercorrida;
};

Vetor3D calcularPosicaoFerramenta(Trajetoria3D trajetoria, double parametro);
ResultadoRobotica analisarTrajetoria(Trajetoria3D trajetoria, Vetor3D normal, double termoIndependente);
#endif
