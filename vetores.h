#ifndef VETORES_H
#define VETORES_H

// Representa um ponto ou vetor no espaco tridimensional.
// Cada componente corresponde a uma coordenada dos eixos X, Y e Z.
struct Vetor3D {
    double x;
    double y;
    double z;
};

Vetor3D subtrairVetores(Vetor3D primeiroVetor, Vetor3D segundoVetor);
Vetor3D somarVetores(Vetor3D primeiroVetor, Vetor3D segundoVetor);
Vetor3D multiplicarVetor(Vetor3D vetor, double escalar);
Vetor3D calcularProdutoVetorial(Vetor3D primeiroVetor, Vetor3D segundoVetor);
double calcularProdutoEscalar(Vetor3D primeiroVetor, Vetor3D segundoVetor);
double calcularModulo(Vetor3D vetor);

#endif
