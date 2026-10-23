## 1. Objetivo

Desenvolver uma aplicação computacional em C++ que receba as coordenadas de três pontos do espaço tridimensional, verifique se eles determinam um plano único, calcule sua equação geral e apresente uma representação 3D interativa. A aplicação também recebe um quarto ponto e verifica sua pertinência ao plano, calculando sua distância perpendicular.

## 2. Fundamentação matemática

Dados A, B e C, calculam-se os vetores AB = B − A e AC = C − A. O produto vetorial n = AB × AC é perpendicular a ambos e, portanto, normal ao plano. Se n = (0,0,0), os pontos são coincidentes ou colineares e não definem um plano único. Para n = (a,b,c), a condição n · (P − A) = 0 conduz à equação geral ax + by + cz + d = 0, com d = −(a Ax + b Ay + c Az). A implementação normaliza o vetor normal antes de calcular os coeficientes.

A distância de Q = (x0,y0,z0) ao plano é |a x0 + b y0 + c z0 + d| / sqrt(a²+b²+c²). A pertinência é decidida comparando essa distância a uma tolerância numérica, necessária porque os cálculos utilizam números em ponto flutuante.

## 3. Ferramentas e arquitetura

O código foi escrito em C++17. As operações matemáticas estão em `vetores.cpp` e `planos.cpp`; a entrada interativa, a câmera e a renderização 3D ficam em `visualizacao3D.cpp`; `main.cpp` inicia o programa. A biblioteca raylib é utilizada para a janela e os gráficos 3D. O usuário altera as coordenadas de A, B, C e P na própria interface, solicita o cálculo e pode girar ou aproximar a câmera.

**Atenção:** o enunciado cita Python, GeoGebra 3D, wxMaxima e MATLAB/Octave/Scilab como plataformas permitidas, mas não menciona C++/raylib. Confirmar a aceitação com o professor antes da entrega.

## 4. Algoritmo

1. Ler as 12 coordenadas (A, B, C e ponto de teste P).
2. Validar se as entradas são números reais finitos.
3. Calcular AB e AC.
4. Calcular n = AB × AC e verificar se seu módulo é suficientemente diferente de zero.
5. Normalizar n e calcular d, obtendo ax + by + cz + d = 0.
6. Calcular a distância de P ao plano e verificar sua pertinência.
7. Desenhar um recorte finito do plano, os pontos e o vetor normal em uma janela 3D.
8. Permitir novos cálculos sem reiniciar o programa.

## 5. Casos de teste e resultados esperados

| Caso | A | B | C | P | Equação equivalente | Distância de P | Pertence? |
|---|---|---|---|---|---|---|---|
| Horizontal | (0,0,0) | (1,0,0) | (0,1,0) | (2,3,0) | z = 0 | 0 | Sim |
| Vertical | (2,0,0) | (2,1,0) | (2,0,1) | (5,0,0) | x − 2 = 0 | 3 | Não |
| Inclinado | (1,0,0) | (0,1,0) | (0,0,1) | (1,1,1) | x + y + z − 1 = 0 | 2/√3 ≈ 1,154701 | Não |
| Colinear | (0,0,0) | (1,1,1) | (2,2,2) | (0,0,1) | Indefinido | — | Rejeitar entrada |

**Nota:** a equação exibida pode ser um múltiplo não nulo da equação acima, pois o programa normaliza os coeficientes. Isso não altera o plano geométrico.

## 6. Evidências de execução

Inserir capturas reais obtidas no Ubuntu, sem fabricar resultados:

- **Figura 1:** janela com plano horizontal e teste de pertinência positivo — `evidencias/01-plano-horizontal.png`.
- **Figura 2:** janela com plano vertical e distância 3 — `evidencias/02-plano-vertical.png`.
- **Figura 3:** plano inclinado com equação e distância — `evidencias/03-plano-inclinado.png`.
- **Figura 4:** mensagem de rejeição de três pontos colineares — `evidencias/04-pontos-colineares.png`.
- **Figura 5 (opcional):** terminal com resultado dos testes automatizados — `evidencias/05-testes.png`.

Após executar os testes, registrar aqui a data, versão do compilador, versão da raylib e os resultados efetivamente observados. Não declarar como executados os testes ainda não realizados.

## 7. Discussão e limitações

A separação entre os módulos matemáticos e gráficos facilita a manutenção e a validação. O plano matemático é infinito, enquanto a janela exibe apenas um recorte finito da superfície. A precisão está sujeita a arredondamentos de ponto flutuante, e a decisão de coplanaridade/pertinência utiliza tolerância. Os cálculos geométricos são feitos com `double`, mas a biblioteca gráfica representa posições com `float`.

## 8. Conclusão

O projeto demonstra como operações vetoriais e equações de planos podem ser convertidas em um algoritmo computacional e representadas graficamente. O produto vetorial determina a direção normal ao plano, a equação geral permite testar pertinência, e a distância ponto–plano quantifica o afastamento. A visualização interativa facilita a interpretação espacial dos resultados e conecta Geometria Analítica à Computação Gráfica.

**Situação de validação:** os testes do núcleo matemático devem ser executados com o comando do guia. A versão gráfica foi relatada como funcional pelo responsável pelo projeto, mas as capturas e os resultados específicos acima ainda precisam ser anexados.
