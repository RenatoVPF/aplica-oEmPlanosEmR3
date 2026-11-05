# Compilação, validação e entrega

## 1. Compilar no Ubuntu

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp vetores.cpp planos.cpp visualizacao3D.cpp -o geometriaComputacional $(pkg-config --cflags --libs raylib)
./geometriaComputacional
```

## 2. Executar testes matemáticos

```bash
g++ -std=c++17 -Wall -Wextra -pedantic testes.cpp vetores.cpp planos.cpp -o testesMatematicos
./testesMatematicos
```

Os testes automatizados atuais cobrem planos horizontal e vertical, pertinência, distância e rejeição de colinearidade. O plano inclinado e a entrada inválida devem ser testados manualmente na interface, conforme o relatório.

## 3. Capturas e relatório

Execute os quatro cenários descritos no relatório, capture a janela e salve as imagens na pasta `evidencias/` com os nomes sugeridos. Complete os nomes dos integrantes e suas contribuições. Registre os resultados realmente observados.

## 4. Arquivos de entrega

O enunciado exige que a parte de aplicações e integração computacional faça parte do manuscrito, enquanto código-fonte, scripts e gráficos sejam anexados separadamente em formato digital. Ajuste o relatório à seção 3 do manuscrito do grupo. As partes manuscritas são de próprio punho e digitalizadas em PDF único. Confirme com o professor a aceitação de C++/raylib.
