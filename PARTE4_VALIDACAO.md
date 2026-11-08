# Parte 4 — Validacao e resultados computacionais

## 4.1 Codigo computacional

Foi desenvolvido um modulo independente em C++17 com a biblioteca raylib,
que compartilha as rotinas de vetores do programa original. A aplicacao
recebe P0, vetor diretor v, normal n, coeficiente D do plano e ponto S.

A intersecao e calculada por t = -(n.P0 + D)/(n.v), quando n.v != 0.
Quando n.v = 0, a reta e paralela ao plano ou esta contida nele.
O modulo tambem calcula a distancia perpendicular e o angulo agudo
entre a trajetoria e a superficie.

## 4.2 Resultados de referencia — Secao 3

P0=(1,3,2)
v=(2,-1,1)
plano: x + 2y - z - 4 = 0
S=(2,2,3)

Substituicao:
(1+2t)+2(3-t)-(2+t)-4 = 0
1 - t = 0
t = 1
I = (3,2,3)

Distancia inicial:
|1 + 6 - 2 - 4|/sqrt(1+4+1) = 1/sqrt(6) = 0.408248...

Distancia do ponto S:
|2 + 4 - 3 - 4|/sqrt(6) = 1/sqrt(6) = 0.408248...

Angulo agudo entre reta e plano:
arcsin(|v.n|/(|v||n|)) = arcsin(1/6) = 9.594068... graus.

Comprimento do percurso de P0 ate I:
|t| * |v| = sqrt(6) = 2.449490...

## Correcao necessaria na Secao 3

A equacao declarada e x + 2y - z - 4 = 0.
No passo de substituicao, o documento usa +4 no lugar de -4.
Por isso encontra 9-t=0, t=9, I=(19,-6,11) e distancia 9/sqrt(6).
Esses resultados pertencem ao plano x + 2y - z + 4 = 0,
e nao ao plano originalmente declarado. Corrigir o sinal na
substituicao e refazer os resultados afetados.

## Evidencias a produzir no Ubuntu

1. Captura da interface com os dados originais e t=1, I=(3,2,3).
2. Captura da ferramenta em movimento e da ferramenta no ponto de impacto.
3. Captura do terminal executando `ctest --test-dir build --output-on-failure`.
4. Captura do GeoGebra 3D com os mesmos dados para comparacao independente.

As capturas reais devem ser produzidas no computador do aluno. Nao declarar
que uma imagem ou teste grafico foi executado sem essa verificacao.

## Limitacoes

A raylib renderiza um recorte finito do plano infinito. A animacao e
cinematica ilustrativa: nao simula a dinamica, colisoes, juntas nem
limites fisicos de um manipulador industrial.

## Recursos graficos da V2

A interface permite habilitar e desabilitar as camadas da trajetoria, vetor
normal, segmento de distancia perpendicular e identificacoes. O segmento
perpendicular e obtido pela projecao ortogonal da posicao instantanea da
ferramenta sobre o plano. Para o relatorio, capture o instante inicial,
um instante intermediario e o instante final, demonstrando o contato com
I=(3,2,3). Verifique visualmente no Ubuntu antes de afirmar que a animacao
foi executada e validada.
