#include "planos.h"
#include <cassert>
#include <cmath>
#include <iostream>

// Compara numeros reais usando uma margem de erro adequada aos testes.
bool valoresProximos(double primeiro, double segundo) {
    return std::abs(primeiro - segundo) < 1e-8;
}


// Testa o plano horizontal z=0, a pertinencia e a distancia.
void testarPlanoHorizontal() {
    Plano3D plano;
    assert(gerarPlanoPorTresPontos({0,0,0}, {1,0,0}, {0,1,0}, plano));
    assert(valoresProximos(plano.a, 0));
    assert(valoresProximos(plano.b, 0));
    assert(valoresProximos(plano.c, 1));
    assert(verificarPertinencia({2,3,0}, plano));
    assert(!verificarPertinencia({2,3,4}, plano));
    assert(valoresProximos(calcularDistanciaPontoPlano({2,3,4}, plano), 4));
}


// Testa um plano vertical x=2 e rejeita pontos colineares.
void testarPlanoVerticalEColinearidade() {
    Plano3D plano;
    assert(gerarPlanoPorTresPontos({2,0,0}, {2,1,0}, {2,0,1}, plano));
    assert(verificarPertinencia({2,7,-3}, plano));
    assert(valoresProximos(calcularDistanciaPontoPlano({5,0,0}, plano), 3));
    assert(!gerarPlanoPorTresPontos({0,0,0}, {1,1,1}, {2,2,2}, plano));
}


int main() {
    testarPlanoHorizontal();
    testarPlanoVerticalEColinearidade();
    std::cout << "Todos os testes matematicos passaram.\n";
    return 0;
}
