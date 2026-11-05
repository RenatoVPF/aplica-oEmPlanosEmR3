#include "trajetoria.h"
#include <cmath>
#include <limits>

// Calcula a posicao do efetuador para qualquer parametro t.
// A reta representa a trajetoria geometrica, sem impor limites mecanicos.
Vetor3D calcularPosicaoFerramenta(Trajetoria3D trajetoria, double parametro) {
    return somarVetores(trajetoria.origem, multiplicarVetor(trajetoria.direcao, parametro));
}


// Resolve a intersecao entre a reta e o plano n.P + d = 0.
// O denominador n.v determina se a reta corta o plano ou e paralela.
// O angulo informado e o angulo agudo entre a reta e o plano,
// e nao o angulo entre a reta e o vetor normal.
ResultadoRobotica analisarTrajetoria(Trajetoria3D trajetoria, Vetor3D normal, double termoIndependente) {
    ResultadoRobotica resultado = {};
    double moduloNormal = calcularModulo(normal);
    double moduloDirecao = calcularModulo(trajetoria.direcao);

    if (moduloNormal < 1e-12 || moduloDirecao < 1e-12) {
        resultado.tipoIntersecao = 3;
        return resultado;
    }

    double numerador = calcularProdutoEscalar(normal, trajetoria.origem) + termoIndependente;
    double denominador = calcularProdutoEscalar(normal, trajetoria.direcao);
    resultado.distanciaInicial = std::abs(numerador) / moduloNormal;

    double seno = std::abs(denominador) / (moduloNormal * moduloDirecao);
    resultado.anguloIncidenciaGraus = std::asin(std::fmin(1.0, seno)) * 180.0 / std::acos(-1.0);

    if (std::abs(denominador) <= 1e-12 * moduloNormal * moduloDirecao) {
        resultado.tipoIntersecao = std::abs(numerador) <= 1e-10 * moduloNormal ? 2 : 1;
        return resultado;
    }

    resultado.tipoIntersecao = 0;
    resultado.parametro = -numerador / denominador;
    resultado.impacto = calcularPosicaoFerramenta(trajetoria, resultado.parametro);
    resultado.distanciaPercorrida = std::abs(resultado.parametro) * moduloDirecao;
    return resultado;
}
