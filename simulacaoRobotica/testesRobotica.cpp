#include "trajetoria.h"
#include <cassert>
#include <cmath>
#include <iostream>

// Compara valores reais admitindo o erro de arredondamento numerico.
bool aproximadamente(double a, double b) {
    return std::abs(a-b) < 1e-5;
}

int main() {
    Trajetoria3D trajetoria = {{1,3,2},{2,-1,1}};
    Vetor3D normal = {1,2,-1};

    // Caso oficial da Secao 3: plano x + 2y - z - 4 = 0.
    ResultadoRobotica resultado = analisarTrajetoria(trajetoria, normal, -4);
    assert(resultado.tipoIntersecao == 0);
    assert(aproximadamente(resultado.parametro, 1));
    assert(aproximadamente(resultado.impacto.x, 3));
    assert(aproximadamente(resultado.impacto.y, 2));
    assert(aproximadamente(resultado.impacto.z, 3));
    assert(aproximadamente(resultado.distanciaInicial, 1/std::sqrt(6.0)));
    assert(aproximadamente(resultado.anguloIncidenciaGraus, std::asin(1.0/6.0)*180/std::acos(-1.0)));

    // O sinal +4 reproduz os resultados incorretamente associados ao plano -4.
    ResultadoRobotica outroPlano = analisarTrajetoria(trajetoria, normal, 4);
    assert(aproximadamente(outroPlano.parametro, 9));
    assert(aproximadamente(outroPlano.impacto.x, 19));

    // Retas paralelas e contidas devem ser identificadas sem divisao por zero.
    assert(analisarTrajetoria({{0,0,0},{1,0,1}}, {1,0,-1}, -4).tipoIntersecao == 1);
    assert(analisarTrajetoria({{0,0,0},{1,0,1}}, {1,0,-1}, 0).tipoIntersecao == 2);

    std::cout << "Todos os testes de robotica passaram.\n";
}
