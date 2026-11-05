# Geometria Computacional 3D — C++ e raylib

Dois programas independentes no mesmo projeto:
- `geometriaComputacional`: plano definido por tres pontos, distancia e pertinencia.
- `simulacaoRobotica`: trajetoria parametrica de ferramenta industrial, intersecao com plano, distancia e animacao.

## Ubuntu

Requisitos: g++, cmake, pkg-config e raylib instalada.

```bash
cmake -S . -B build
cmake --build build
./build/geometriaComputacional
./build/simulacaoRobotica
ctest --test-dir build --output-on-failure
```

Alternativa para compilar somente o novo modulo:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic simulacaoRobotica/mainRobotica.cpp simulacaoRobotica/trajetoria.cpp vetores.cpp -o simulacaoRoboticaApp $(pkg-config --cflags --libs raylib)
./simulacaoRoboticaApp
```

## Operacao do modulo robotico

Edite os campos de origem P0, direcao v, normal n, termo D e ponto de checagem S.
Pressione Enter ou RECALCULAR. INICIAR/PAUSAR controla a animacao; REINICIAR volta
ao inicio. Arraste na area 3D para girar a camera; roda do mouse para zoom.

Dados iniciais correspondem a Secao 3:
P0=(1,3,2), v=(2,-1,1), n=(1,2,-1), D=-4, S=(2,2,3).

A simulacao representa uma reta matematica, nao um modelo fisico com articulacoes,
colisoes, aceleracao ou limites reais de um robo industrial.
O GeoGebra 3D pode ser usado como verificacao independente, sem substituir o
codigo proprio nem as evidencias produzidas pela raylib.

Veja `PARTE4_VALIDACAO.md` para os resultados e orientacoes de evidencia.
