#include "vetores.h"
#include <cmath>

// Subtrai duas coordenadas tridimensionais componente a componente.
// Ao subtrair o ponto inicial do ponto final, obtemos o vetor
// diretor que liga esses dois pontos no espaco.
Vetor3D subtrairVetores(Vetor3D primeiroVetor, Vetor3D segundoVetor) {
    return {primeiroVetor.x - segundoVetor.x,
            primeiroVetor.y - segundoVetor.y,
            primeiroVetor.z - segundoVetor.z};
}


// Soma dois vetores componente a componente.
// Esta operacao permite deslocar pontos e construir a geometria do plano.
Vetor3D somarVetores(Vetor3D primeiroVetor, Vetor3D segundoVetor) {
    return {primeiroVetor.x + segundoVetor.x,
            primeiroVetor.y + segundoVetor.y,
            primeiroVetor.z + segundoVetor.z};
}


// Multiplica cada componente do vetor por um numero real.
// O resultado altera seu comprimento e, para escalares negativos,
// inverte seu sentido sem alterar a direcao da reta suporte.
Vetor3D multiplicarVetor(Vetor3D vetor, double escalar) {
    return {vetor.x * escalar, vetor.y * escalar, vetor.z * escalar};
}


// Calcula o produto vetorial de dois vetores do R3.
// O vetor resultante e perpendicular aos dois vetores originais.
// Suas componentes sao obtidas pela expansao do determinante 3x3:
// X = Ay*Bz - Az*By; Y = Az*Bx - Ax*Bz; Z = Ax*By - Ay*Bx.
Vetor3D calcularProdutoVetorial(Vetor3D primeiroVetor, Vetor3D segundoVetor) {
    return {
        primeiroVetor.y * segundoVetor.z - primeiroVetor.z * segundoVetor.y,
        primeiroVetor.z * segundoVetor.x - primeiroVetor.x * segundoVetor.z,
        primeiroVetor.x * segundoVetor.y - primeiroVetor.y * segundoVetor.x
    };
}


// Calcula o produto escalar, somando os produtos das componentes.
// Utilizamos esta operacao na equacao do plano e em distancias.
double calcularProdutoEscalar(Vetor3D primeiroVetor, Vetor3D segundoVetor) {
    return primeiroVetor.x * segundoVetor.x
         + primeiroVetor.y * segundoVetor.y
         + primeiroVetor.z * segundoVetor.z;
}


// Calcula o comprimento de um vetor usando Pitagoras em tres dimensoes.
// O modulo e a raiz quadrada da soma dos quadrados das componentes.
double calcularModulo(Vetor3D vetor) {
    return std::sqrt(calcularProdutoEscalar(vetor, vetor));
}
