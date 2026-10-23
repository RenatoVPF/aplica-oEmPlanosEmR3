#include "visualizacao3D.h"
#include "vetores.h"
#include "planos.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <array>
#include <limits>

// Converte as coordenadas de nossa estrutura matematica para raylib.
// Mantemos os mesmos eixos X, Y e Z em todos os calculos e desenhos.
static Vector3 converterParaRaylib(Vetor3D vetor) {
    return {(float)vetor.x, (float)vetor.y, (float)vetor.z};
}


// Desenha um marcador esferico e o identificador de um ponto.
// O nome do ponto aparece no painel de informacoes, evitando
// que textos 2D escondam objetos quando a camera for movimentada.
static void desenharPonto(Vetor3D ponto, Color cor, float tamanho) {
    DrawSphere(converterParaRaylib(ponto), tamanho, cor);
}


// Desenha uma seta representando o vetor normal do plano.
// O segmento parte do centro da superficie; a ponta e formada
// por uma pequena esfera, distinguindo o sentido positivo do vetor.
static void desenharVetorNormal(Vetor3D origem, Vetor3D normal, double comprimento) {
    Vetor3D extremidade = somarVetores(origem, multiplicarVetor(normal, comprimento));
    DrawLine3D(converterParaRaylib(origem), converterParaRaylib(extremidade), MAROON);
    DrawSphere(converterParaRaylib(extremidade), (float)(comprimento * 0.09), MAROON);
}


// Desenha uma parte finita do plano infinito definido pela equacao.
// Para representar inclusive planos verticais, construimos dois vetores
// unitarios perpendiculares ao vetor normal e entre si.
// Os quatro cantos sao obtidos por deslocamentos nesses dois vetores.
// Dois triangulos formam a superficie quadrada semitransparente.
static void desenharSuperficie(Vetor3D centro, Vetor3D normal, double metadeLado) {
    Vetor3D referencia = std::abs(normal.y) < 0.9 ? Vetor3D{0, 1, 0} : Vetor3D{1, 0, 0};
    Vetor3D eixoU = calcularProdutoVetorial(normal, referencia);
    eixoU = multiplicarVetor(eixoU, 1.0 / calcularModulo(eixoU));
    Vetor3D eixoV = calcularProdutoVetorial(normal, eixoU);

    Vetor3D deslocamentoU = multiplicarVetor(eixoU, metadeLado);
    Vetor3D deslocamentoV = multiplicarVetor(eixoV, metadeLado);

    Vetor3D canto1 = somarVetores(somarVetores(centro, deslocamentoU), deslocamentoV);
    Vetor3D canto2 = somarVetores(subtrairVetores(centro, deslocamentoU), deslocamentoV);
    Vetor3D canto3 = subtrairVetores(subtrairVetores(centro, deslocamentoU), deslocamentoV);
    Vetor3D canto4 = somarVetores(subtrairVetores(centro, deslocamentoV), deslocamentoU);

    Color corPlano = Fade(SKYBLUE, 0.36f);

    // Desenhamos as faces em ambos os sentidos para que a superficie
    // continue visivel quando a camera observar o lado oposto.
    DrawTriangle3D(converterParaRaylib(canto1), converterParaRaylib(canto2), converterParaRaylib(canto3), corPlano);
    DrawTriangle3D(converterParaRaylib(canto3), converterParaRaylib(canto2), converterParaRaylib(canto1), corPlano);
    DrawTriangle3D(converterParaRaylib(canto1), converterParaRaylib(canto3), converterParaRaylib(canto4), corPlano);
    DrawTriangle3D(converterParaRaylib(canto4), converterParaRaylib(canto3), converterParaRaylib(canto1), corPlano);

    DrawLine3D(converterParaRaylib(canto1), converterParaRaylib(canto2), BLUE);
    DrawLine3D(converterParaRaylib(canto2), converterParaRaylib(canto3), BLUE);
    DrawLine3D(converterParaRaylib(canto3), converterParaRaylib(canto4), BLUE);
    DrawLine3D(converterParaRaylib(canto4), converterParaRaylib(canto1), BLUE);
}


// Controla a camera orbital sem depender de recursos extras.
// Arrastar o mouse com o botao esquerdo muda os angulos de observacao.
// A roda do mouse aproxima ou afasta a camera do centro do plano.
// O limite do angulo vertical evita inversoes bruscas da camera.
static void atualizarCamera(Camera3D &camera, Vetor3D centro, float &anguloHorizontal,
                           float &anguloVertical, float &distancia) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && GetMouseX() > 385) {
        Vector2 movimento = GetMouseDelta();
        anguloHorizontal -= movimento.x * 0.008f;
        anguloVertical += movimento.y * 0.008f;
        anguloVertical = std::clamp(anguloVertical, -1.45f, 1.45f);
    }

    if (GetMouseX() > 385) distancia *= std::pow(0.88f, GetMouseWheelMove());
    distancia = std::clamp(distancia, 1.0f, 100000.0f);

    camera.target = converterParaRaylib(centro);
    camera.position = {
        camera.target.x + distancia * std::cos(anguloVertical) * std::cos(anguloHorizontal),
        camera.target.y + distancia * std::sin(anguloVertical),
        camera.target.z + distancia * std::cos(anguloVertical) * std::sin(anguloHorizontal)
    };
}



// Cada campo armazena o texto digitado e sua area clicavel.
// Os valores sao convertidos para double somente quando o usuario
// solicita um novo calculo, evitando resultados parciais.
struct CampoNumerico {
    std::string texto;
    Rectangle area;
};


// Converte o conteudo de um campo em numero real finito.
// A verificacao do fim do texto impede aceitar entradas incompletas
// como "2abc". Coordenadas infinitas ou NaN tambem sao rejeitadas.
static bool converterNumero(const std::string &texto, double &valor) {
    if (texto.empty()) return false;

    char *fim = nullptr;
    valor = std::strtod(texto.c_str(), &fim);

    return fim != texto.c_str() && *fim == '\0' && std::isfinite(valor);
}


// Atualiza a digitacao do campo selecionado.
// Aceita numeros, sinal negativo, ponto decimal e notacao cientifica.
// Tab e Shift+Tab permitem percorrer os doze campos rapidamente.
static void atualizarCampos(std::array<CampoNumerico, 12> &campos, int &campoAtivo) {
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        campoAtivo = -1;

        for (int indice = 0; indice < 12; indice++) {
            if (CheckCollisionPointRec(mouse, campos[indice].area)) {
                campoAtivo = indice;
                break;
            }
        }
    }

    if (IsKeyPressed(KEY_TAB)) {
        int deslocamento = (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) ? 11 : 1;
        campoAtivo = (campoAtivo + deslocamento + 12) % 12;
    }

    if (campoAtivo < 0) return;

    int caractere = GetCharPressed();

    while (caractere > 0) {
        bool permitido = (caractere >= '0' && caractere <= '9') ||
                         caractere == '-' || caractere == '+' ||
                         caractere == '.' || caractere == 'e' || caractere == 'E';

        if (permitido && campos[campoAtivo].texto.size() < 18) {
            campos[campoAtivo].texto.push_back((char)caractere);
        }

        caractere = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !campos[campoAtivo].texto.empty()) {
        campos[campoAtivo].texto.pop_back();
    }
}


// Desenha um campo numerico com sua borda e valor atual.
// O destaque azul identifica qual campo recebera o teclado.
static void desenharCampo(const CampoNumerico &campo, bool selecionado) {
    DrawRectangleRec(campo.area, WHITE);
    DrawRectangleLinesEx(campo.area, selecionado ? 2.0f : 1.0f,
                         selecionado ? BLUE : LIGHTGRAY);
    DrawText(campo.texto.c_str(), (int)campo.area.x + 8,
             (int)campo.area.y + 9, 17, DARKGRAY);
}


// Desenha um marcador de identificacao ao lado de cada ponto.
// A posicao na tela e calculada novamente a cada quadro, portanto
// os nomes acompanham os pontos durante a rotacao da camera.
static void desenharIdentificador(Camera3D camera, Vetor3D ponto,
                                 const char *nome, Color cor) {
    Vector2 posicao = GetWorldToScreen(converterParaRaylib(ponto), camera);

    if (posicao.x > 390 && posicao.x < GetScreenWidth() - 15 &&
        posicao.y > 10 && posicao.y < GetScreenHeight() - 35) {
        DrawCircle((int)posicao.x, (int)posicao.y, 4, cor);
        DrawText(nome, (int)posicao.x + 10, (int)posicao.y - 15, 21, BLACK);
    }
}


// Produz uma representacao mais legivel da equacao geral.
// Termos com coeficiente aproximadamente zero sao omitidos;
// coeficientes unitarios sao apresentados sem o numero 1.
static std::string formatarEquacao(Plano3D plano) {
    const double coeficientes[] = {plano.a, plano.b, plano.c, plano.d};
    const char *nomes[] = {"x", "y", "z", ""};
    std::string equacao;
    char numero[64];

    for (int indice = 0; indice < 4; indice++) {
        double valor = coeficientes[indice];

        if (std::abs(valor) < 1e-8) continue;

        bool primeiro = equacao.empty();
        equacao += primeiro ? (valor < 0 ? "-" : "") : (valor < 0 ? " - " : " + ");

        double magnitude = std::abs(valor);
        bool omitirUnidade = indice < 3 && std::abs(magnitude - 1.0) < 1e-8;

        if (!omitirUnidade) {
            std::snprintf(numero, sizeof(numero), "%.4g", magnitude);
            equacao += numero;
        }

        equacao += nomes[indice];
    }

    return (equacao.empty() ? "0" : equacao) + " = 0";
}


// Le os quatro pontos, verifica as coordenadas e atualiza o plano.
// A geometria anterior permanece visivel quando houver erro,
// permitindo que o usuario corrija apenas o campo necessario.
static bool calcularComCampos(const std::array<CampoNumerico, 12> &campos,
                             Vetor3D &pontoA, Vetor3D &pontoB,
                             Vetor3D &pontoC, Vetor3D &pontoTeste,
                             Plano3D &plano, std::string &mensagem) {
    double valores[12];

    for (int indice = 0; indice < 12; indice++) {
        if (!converterNumero(campos[indice].texto, valores[indice])) {
            mensagem = "Entrada invalida no campo " + std::to_string(indice + 1) + ".";
            return false;
        }
    }

    Vetor3D novoA = {valores[0], valores[1], valores[2]};
    Vetor3D novoB = {valores[3], valores[4], valores[5]};
    Vetor3D novoC = {valores[6], valores[7], valores[8]};
    Vetor3D novoTeste = {valores[9], valores[10], valores[11]};
    Plano3D novoPlano;

    if (!gerarPlanoPorTresPontos(novoA, novoB, novoC, novoPlano)) {
        mensagem = "A, B e C sao coincidentes ou colineares.";
        return false;
    }

    pontoA = novoA;
    pontoB = novoB;
    pontoC = novoC;
    pontoTeste = novoTeste;
    plano = novoPlano;
    mensagem = "Plano atualizado com sucesso.";
    return true;
}


// Inicia a aplicacao diretamente na interface grafica.
// O usuario pode alterar os doze valores e recalcular quantas vezes
// desejar, sem fechar a janela ou retornar ao terminal.
void visualizarPlano3D() {
    InitWindow(1320, 800, "Geometria Computacional 3D - C++ e raylib");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    std::array<CampoNumerico, 12> campos;
    const char *valoresIniciais[] = {
        "1", "0", "0", "0", "1", "0", "0", "0", "1",
        "1", "1", "1"
    };

    for (int indice = 0; indice < 12; indice++) {
        int linha = indice / 3;
        int coluna = indice % 3;
        campos[indice].texto = valoresIniciais[indice];
        campos[indice].area = {(float)(67 + coluna * 102),
                               (float)(125 + linha * 58), 91, 37};
    }

    Vetor3D pontoA = {}, pontoB = {}, pontoC = {}, pontoTeste = {};
    Plano3D plano = {};
    std::string mensagem;
    bool calculado = calcularComCampos(campos, pontoA, pontoB, pontoC,
                                       pontoTeste, plano, mensagem);
    int campoAtivo = -1;

    Camera3D camera = {};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float anguloHorizontal = 0.75f;
    float anguloVertical = 0.45f;
    float distanciaCamera = 12.0f;

    Rectangle botaoCalcular = {24, 382, 344, 44};
    Rectangle botaoCentralizar = {24, 437, 344, 36};

    while (!WindowShouldClose()) {
        atualizarCampos(campos, campoAtivo);

        bool apertouCalcular = IsKeyPressed(KEY_ENTER) ||
            (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
             CheckCollisionPointRec(GetMousePosition(), botaoCalcular));

        if (apertouCalcular) {
            calculado = calcularComCampos(campos, pontoA, pontoB, pontoC,
                                          pontoTeste, plano, mensagem) || calculado;
        }

        Vetor3D centro = calculado ?
            multiplicarVetor(somarVetores(somarVetores(pontoA, pontoB), pontoC), 1.0 / 3.0) :
            Vetor3D{0, 0, 0};

        double maiorDistancia = calculado ?
            std::max({calcularModulo(subtrairVetores(pontoA, centro)),
                      calcularModulo(subtrairVetores(pontoB, centro)),
                      calcularModulo(subtrairVetores(pontoC, centro)),
                      calcularModulo(subtrairVetores(pontoTeste, centro)), 1.0}) : 1.0;
        double metadeLado = maiorDistancia * 1.8;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            CheckCollisionPointRec(GetMousePosition(), botaoCentralizar)) {
            anguloHorizontal = 0.75f;
            anguloVertical = 0.45f;
            distanciaCamera = (float)(metadeLado * 3.5);
        }

        atualizarCamera(camera, centro, anguloHorizontal, anguloVertical, distanciaCamera);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (calculado) {
            BeginMode3D(camera);
            desenharSuperficie(centro, plano.vetorNormal, metadeLado);
            float tamanhoPonto = (float)(metadeLado * 0.055);
            desenharPonto(pontoA, RED, tamanhoPonto);
            desenharPonto(pontoB, ORANGE, tamanhoPonto);
            desenharPonto(pontoC, PURPLE, tamanhoPonto);
            bool pertence = verificarPertinencia(pontoTeste, plano);
            desenharPonto(pontoTeste, pertence ? DARKGREEN : MAROON, tamanhoPonto * 1.15f);
            desenharVetorNormal(centro, plano.vetorNormal, metadeLado * 0.7);

            float eixo = (float)(metadeLado * 1.5);
            DrawLine3D({-eixo, 0, 0}, {eixo, 0, 0}, RED);
            DrawLine3D({0, -eixo, 0}, {0, eixo, 0}, DARKGREEN);
            DrawLine3D({0, 0, -eixo}, {0, 0, eixo}, DARKBLUE);
            EndMode3D();

            desenharIdentificador(camera, pontoA, "A", RED);
            desenharIdentificador(camera, pontoB, "B", ORANGE);
            desenharIdentificador(camera, pontoC, "C", PURPLE);
            desenharIdentificador(camera, pontoTeste, "P", pertence ? DARKGREEN : MAROON);
            desenharIdentificador(camera,
                somarVetores(centro, multiplicarVetor(plano.vetorNormal, metadeLado * 0.7)),
                "Normal", MAROON);
        }

        // O painel lateral deixa a maior parte da janela livre para o grafico.
        DrawRectangle(0, 0, 385, GetScreenHeight(), Color{245, 247, 250, 255});
        DrawLine(384, 0, 384, GetScreenHeight(), LIGHTGRAY);
        DrawText("GEOMETRIA 3D", 24, 22, 27, DARKBLUE);
        DrawText("Plano por tres pontos", 24, 58, 19, DARKGRAY);
        DrawText("Digite as coordenadas:", 24, 92, 17, BLACK);

        const char *rotulos[] = {"A", "B", "C", "P"};
        const Color cores[] = {RED, ORANGE, PURPLE, DARKGREEN};

        for (int linha = 0; linha < 4; linha++) {
            DrawText(rotulos[linha], 25, 134 + linha * 58, 22, cores[linha]);

            for (int coluna = 0; coluna < 3; coluna++) {
                int indice = linha * 3 + coluna;
                desenharCampo(campos[indice], campoAtivo == indice);
            }
        }

        DrawText("X", 106, 105, 15, GRAY);
        DrawText("Y", 208, 105, 15, GRAY);
        DrawText("Z", 310, 105, 15, GRAY);

        DrawRectangleRec(botaoCalcular, DARKBLUE);
        DrawText("CALCULAR PLANO  [ENTER]", 57, 394, 19, WHITE);
        DrawRectangleRec(botaoCentralizar, LIGHTGRAY);
        DrawText("REDEFINIR CAMERA", 103, 446, 17, DARKGRAY);

        DrawText("RESULTADOS", 24, 502, 21, DARKBLUE);

        if (calculado) {
            std::string equacao = formatarEquacao(plano);
            DrawText("Equacao geral:", 24, 540, 17, DARKGRAY);
            DrawText(equacao.c_str(), 24, 564, 20, DARKBLUE);

            char linha[180];
            std::snprintf(linha, sizeof(linha), "Normal: (%.3f, %.3f, %.3f)",
                          plano.a, plano.b, plano.c);
            DrawText(linha, 24, 604, 17, MAROON);

            bool pertence = verificarPertinencia(pontoTeste, plano);
            DrawText(pertence ? "P pertence ao plano" : "P nao pertence ao plano",
                     24, 635, 18, pertence ? DARKGREEN : MAROON);
            std::snprintf(linha, sizeof(linha), "Distancia: %.6f",
                          calcularDistanciaPontoPlano(pontoTeste, plano));
            DrawText(linha, 24, 662, 18, DARKGRAY);
        }

        DrawText(mensagem.c_str(), 24, 703, 15,
                 mensagem == "Plano atualizado com sucesso." ? DARKGREEN : MAROON);
        DrawText("TAB: proximo campo  |  ESC: fechar", 24, 765, 14, DARKGRAY);
        DrawRectangle(398, 758, 530, 30, Fade(WHITE, 0.94f));
        DrawText("Arrastar: girar  |  Roda: zoom  |  ESC: fechar",
                 410, 766, 16, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
}
