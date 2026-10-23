#include "planos.h"
#include <cmath>
#include <algorithm>

// Tolerancia relativa para comparar valores reais calculados.
// Valores muito pequenos podem surgir por arredondamento em ponto flutuante.
static constexpr double tolerancia = 1e-9;


// Determina a equacao do plano que passa por tres pontos.
// Primeiro construimos AB = B-A e AC = C-A. Em seguida, AB x AC
// fornece o vetor normal (a,b,c), perpendicular ao plano.
// Substituindo A na equacao, obtemos d = -(a*Ax+b*Ay+c*Az).
// Se o produto vetorial for praticamente nulo, os pontos sao
// colineares ou coincidentes e nao definem um plano unico.
bool gerarPlanoPorTresPontos(Vetor3D pontoA, Vetor3D pontoB, Vetor3D pontoC, Plano3D &plano) {
    Vetor3D vetorAB = subtrairVetores(pontoB, pontoA);
    Vetor3D vetorAC = subtrairVetores(pontoC, pontoA);
    Vetor3D vetorNormal = calcularProdutoVetorial(vetorAB, vetorAC);

    double comprimentoAB = calcularModulo(vetorAB);
    double comprimentoAC = calcularModulo(vetorAC);
    double comprimentoNormal = calcularModulo(vetorNormal);

    if (comprimentoAB == 0.0 || comprimentoAC == 0.0 ||
        comprimentoNormal <= tolerancia * comprimentoAB * comprimentoAC) {
        return false;
    }

    // Normalizamos os coeficientes para evitar numeros desnecessariamente grandes.
    // Multiplicar todos os coeficientes por uma constante nao nula
    // representa o mesmo plano geometrico.
    vetorNormal = multiplicarVetor(vetorNormal, 1.0 / comprimentoNormal);

    plano.a = vetorNormal.x;
    plano.b = vetorNormal.y;
    plano.c = vetorNormal.z;
    plano.d = -calcularProdutoEscalar(vetorNormal, pontoA);
    plano.vetorNormal = vetorNormal;

    return true;
}


// Calcula a distancia perpendicular de um ponto ate o plano.
// Formula: |ax+by+cz+d| / sqrt(a²+b²+c²).
// O denominador torna o resultado independente da escala dos coeficientes.
double calcularDistanciaPontoPlano(Vetor3D ponto, Plano3D plano) {
    double substituicao = plano.a * ponto.x + plano.b * ponto.y
                        + plano.c * ponto.z + plano.d;
    double moduloNormal = calcularModulo(plano.vetorNormal);

    return std::abs(substituicao) / moduloNormal;
}


// Verifica se o ponto pertence ao plano, observando sua distancia.
// Como numeros decimais possuem erros de arredondamento, aceitamos
// uma pequena tolerancia proporcional a escala das coordenadas.
bool verificarPertinencia(Vetor3D ponto, Plano3D plano) {
    double escala = std::max({1.0, calcularModulo(ponto), std::abs(plano.d)});
    return calcularDistanciaPontoPlano(ponto, plano) <= tolerancia * escala;
}
