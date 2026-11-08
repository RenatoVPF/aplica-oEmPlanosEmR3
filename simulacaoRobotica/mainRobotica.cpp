#include "trajetoria.h"
#include "raylib.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

// Converte coordenadas matematicas (x,y,z) para os eixos da raylib.
// O eixo Z matematico corresponde ao eixo Y vertical da raylib.
static Vector3 converter(Vetor3D ponto) {
    return {(float)ponto.x, (float)ponto.z, (float)ponto.y};
}



// Fonte compartilhada por todos os textos da interface. A fonte padrao
// permanece disponivel caso nenhuma fonte TrueType seja encontrada.
static Font fonteInterface = {};
static bool fonteExternaCarregada = false;

/*
 * Carrega uma fonte TrueType disponivel no Ubuntu ou no Windows.
 * O carregamento ocorre uma unica vez, depois de InitWindow.
 * Caso nenhum caminho exista, utiliza a fonte padrao da raylib.
 */
static void carregarFonteInterface() {
    const char *caminhos[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };

    fonteInterface = GetFontDefault();
    for (const char *caminho : caminhos) {
        if (!FileExists(caminho)) continue;
        Font fonteCarregada = LoadFontEx(caminho, 32, nullptr, 0);
        if (fonteCarregada.texture.id != 0) {
            fonteInterface = fonteCarregada;
            fonteExternaCarregada = true;
            SetTextureFilter(fonteInterface.texture, TEXTURE_FILTER_BILINEAR);
            break;
        }
    }
}

/*
 * Centraliza o desenho de textos da interface, usando a fonte carregada.
 * Os argumentos mantem a ordem da funcao DrawText para facilitar a
 * substituicao das chamadas ja existentes no programa.
 */
static void desenharTextoInterface(const char *texto, int posicaoX,
                                  int posicaoY, int tamanhoFonte, Color corTexto) {
    DrawTextEx(fonteInterface, texto,
               {(float)posicaoX, (float)posicaoY},
               (float)tamanhoFonte, 1.0f, corTexto);
}

/*
 * Desenha uma etiqueta legivel junto a um ponto tridimensional.
 * Converte a posicao 3D em coordenadas de tela, desloca a etiqueta
 * para fora da esfera e utiliza fundo claro para melhorar o contraste.
 * Evita desenhar etiquetas sobre o painel de campos e resultados.
 */
static void desenharIdentificacao3D(Camera3D camera, Vetor3D ponto,
                                    const char *texto, Color corTexto,
                                    int deslocamentoX, int deslocamentoY) {
    Vector3 posicao3D = {(float)ponto.x, (float)ponto.z, (float)ponto.y};
    Vector2 posicaoTela = GetWorldToScreen(posicao3D, camera);
    const int tamanhoFonte = 17;
    const int margem = 5;
    Vector2 medidas = MeasureTextEx(fonteInterface, texto, (float)tamanhoFonte, 1.0f);
    int posicaoX = (int)posicaoTela.x + deslocamentoX;
    int posicaoY = (int)posicaoTela.y + deslocamentoY;

    // Nao cobre o painel esquerdo nem a barra de opcoes superior.
    if (posicaoX < 440 || posicaoY < 65 ||
        posicaoX + (int)medidas.x + 2*margem > GetScreenWidth() - 10 ||
        posicaoY + (int)medidas.y + 2*margem > GetScreenHeight() - 25) return;

    DrawRectangle(posicaoX - margem, posicaoY - margem,
                  (int)std::ceil(medidas.x) + 2*margem,
                  (int)std::ceil(medidas.y) + 2*margem,
                  Fade(RAYWHITE, 0.93f));
    DrawRectangleLines(posicaoX - margem, posicaoY - margem,
                       (int)std::ceil(medidas.x) + 2*margem,
                       (int)std::ceil(medidas.y) + 2*margem, LIGHTGRAY);
    desenharTextoInterface(texto, posicaoX, posicaoY, tamanhoFonte, corTexto);
}


// Desenha uma porcao finita do plano definido por n.P + d = 0.
// Os vetores de base sao obtidos por produtos vetoriais para permitir
// superficies inclinadas, verticais e horizontais.
static void desenharPlano(Vetor3D normal, double termo, float tamanho) {
    double norma = calcularModulo(normal);
    if (norma < 1e-12) return;

    Vetor3D n = multiplicarVetor(normal, 1.0/norma);
    Vetor3D referencia = std::abs(n.z) < 0.9 ? Vetor3D{0,0,1} : Vetor3D{0,1,0};
    Vetor3D u = calcularProdutoVetorial(n, referencia);
    u = multiplicarVetor(u, 1.0/calcularModulo(u));
    Vetor3D v = calcularProdutoVetorial(n, u);
    Vetor3D centro = multiplicarVetor(normal, -termo/(norma*norma));
    Vetor3D a = somarVetores(centro, somarVetores(multiplicarVetor(u,tamanho),multiplicarVetor(v,tamanho)));
    Vetor3D b = somarVetores(centro, somarVetores(multiplicarVetor(u,-tamanho),multiplicarVetor(v,tamanho)));
    Vetor3D c = somarVetores(centro, somarVetores(multiplicarVetor(u,-tamanho),multiplicarVetor(v,-tamanho)));
    Vetor3D d = somarVetores(centro, somarVetores(multiplicarVetor(u,tamanho),multiplicarVetor(v,-tamanho)));

    Color cor = {45,130,225,75};
    DrawTriangle3D(converter(a),converter(b),converter(c),cor);
    DrawTriangle3D(converter(a),converter(c),converter(d),cor);
    DrawTriangle3D(converter(c),converter(b),converter(a),cor);
    DrawTriangle3D(converter(d),converter(c),converter(a),cor);
    DrawLine3D(converter(a),converter(b),BLUE);
    DrawLine3D(converter(b),converter(c),BLUE);
    DrawLine3D(converter(c),converter(d),BLUE);
    DrawLine3D(converter(d),converter(a),BLUE);
}


// Desenha uma seta em coordenadas tridimensionais, incluindo a ponta.
// O segmento indica a direcao e a ponta distingue o vetor de uma reta.
static void desenharSeta(Vetor3D inicio, Vetor3D fim, Color cor) {
    Vetor3D deslocamento = subtrairVetores(fim, inicio);
    double comprimento = calcularModulo(deslocamento);
    if (comprimento < 1e-10) return;

    Vetor3D direcao = multiplicarVetor(deslocamento, 1.0/comprimento);
    Vetor3D referencia = std::abs(direcao.z) < 0.9 ? Vetor3D{0,0,1} : Vetor3D{0,1,0};
    Vetor3D lateral = calcularProdutoVetorial(direcao, referencia);
    lateral = multiplicarVetor(lateral, 1.0/calcularModulo(lateral));
    double ponta = std::min(0.35, comprimento*0.28);
    Vetor3D base = subtrairVetores(fim, multiplicarVetor(direcao,ponta));
    Vetor3D esquerda = somarVetores(base,multiplicarVetor(lateral,ponta*0.4));
    Vetor3D direita = subtrairVetores(base,multiplicarVetor(lateral,ponta*0.4));

    DrawLine3D(converter(inicio),converter(fim),cor);
    DrawLine3D(converter(esquerda),converter(fim),cor);
    DrawLine3D(converter(direita),converter(fim),cor);
}


// Determina a projecao ortogonal de um ponto sobre o plano.
// O deslocamento aplicado ao ponto e paralelo ao vetor normal e
// possui comprimento igual a distancia perpendicular ao plano.
static Vetor3D projetarPontoNoPlano(Vetor3D ponto, Vetor3D normal, double termo) {
    double normaQuadrada = calcularProdutoEscalar(normal,normal);
    double residuo = calcularProdutoEscalar(normal,ponto)+termo;
    return subtrairVetores(ponto,multiplicarVetor(normal,residuo/normaQuadrada));
}


// Desenha um segmento pontilhado para distinguir a distancia
// perpendicular da reta que representa a trajetoria robotica.
static void desenharSegmentoPontilhado(Vetor3D inicio, Vetor3D fim, Color cor) {
    Vetor3D deslocamento = subtrairVetores(fim,inicio);
    for (int indice=0;indice<20;indice+=2) {
        Vetor3D a = somarVetores(inicio,multiplicarVetor(deslocamento,indice/20.0));
        Vetor3D b = somarVetores(inicio,multiplicarVetor(deslocamento,(indice+1)/20.0));
        DrawLine3D(converter(a),converter(b),cor);
    }
}


// Cria uma caixa de selecao visual. O estado so e alterado quando
// o usuario clica sobre a caixa ou seu texto correspondente.
static void desenharOpcao(const char *rotulo, Rectangle area, bool selecionado) {
    DrawRectangleLinesEx(area,1,selecionado?DARKBLUE:GRAY);
    if (selecionado) DrawRectangle((int)area.x+4,(int)area.y+4,12,12,DARKBLUE);
    desenharTextoInterface(rotulo,(int)area.x+29,(int)area.y+2,16,DARKGRAY);
}


// Recebe valores editaveis e mantem a simulacao no mesmo executavel.
// Sao 13 campos: origem (3), direcao (3), normal (3), D (1) e S (3).
struct Campo {
    std::string texto;
    Rectangle area;
};

static bool lerNumero(const std::string &texto, double &valor) {
    if (texto.empty()) return false;
    char *fim = nullptr;
    valor = std::strtod(texto.c_str(),&fim);
    return fim != texto.c_str() && *fim == '\0' && std::isfinite(valor);
}

int main() {
    InitWindow(1400,840,"Simulacao Robotica - Trajetoria Ferramenta/Plano");
    SetTargetFPS(60);
    carregarFonteInterface();
    std::array<Campo,13> campos;
    const char *padrao[] = {"1","3","2","2","-1","1","1","2","-1","-4","2","2","3"};

    for (int i=0;i<13;i++) {
        int linha = i<9 ? i/3 : (i==9 ? 3 : 4);
        int coluna = i<9 ? i%3 : (i==9 ? 0 : (i-10));
        campos[i] = {padrao[i],{(float)(75+coluna*113),(float)(126+linha*58),103,38}};
    }

    Trajetoria3D trajetoria = {{1,3,2},{2,-1,1}};
    Vetor3D normal = {1,2,-1}, pontoChecagem = {2,2,3};
    double termo = -4;
    ResultadoRobotica resultado = analisarTrajetoria(trajetoria,normal,termo);
    std::string mensagem = "Cenario oficial da Secao 3 carregado.";
    int ativo = -1;
    bool reproduzindo = false;
    double tempo = 0;
    float velocidade = 0.35f;
    bool mostrarTrajetoria = true;
    bool mostrarNormal = true;
    bool mostrarDistancia = true;
    bool mostrarIdentificacoes = true;
    Rectangle opcoes[] = {{450,22,20,20},{650,22,20,20},
                          {850,22,20,20},{1060,22,20,20}};
    float anguloH = 0.65f, anguloV = 0.45f, distanciaCamera = 17.0f;
    Camera3D camera = {};
    camera.up = {0,1,0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;
    Rectangle calcular = {20,447,399,42}, iniciar = {20,502,123,37};
    Rectangle reiniciar = {153,502,123,37}, cameraBotao = {286,502,133,37};

    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ativo = -1;
            for (int i=0;i<13;i++) {
                if (CheckCollisionPointRec(mouse,campos[i].area)) ativo = i;
            }
        }
        if (IsKeyPressed(KEY_TAB)) ativo = (ativo+1)%13;

        // Alterna individualmente as camadas da representacao 3D.
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            bool *estados[] = {&mostrarTrajetoria,&mostrarNormal,
                              &mostrarDistancia,&mostrarIdentificacoes};
            for (int i=0;i<4;i++) {
                Rectangle areaClique = {opcoes[i].x,opcoes[i].y,
                                        i==2?190.0f:185.0f,25};
                if (CheckCollisionPointRec(mouse,areaClique))
                    *estados[i] = !*estados[i];
            }
        }

        if (ativo>=0) {
            int letra = GetCharPressed();
            while (letra>0) {
                if (((letra>='0' && letra<='9') || letra=='-' || letra=='+' ||
                     letra=='.' || letra=='e' || letra=='E') &&
                    campos[ativo].texto.size()<16)
                    campos[ativo].texto.push_back((char)letra);
                letra=GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !campos[ativo].texto.empty())
                campos[ativo].texto.pop_back();
        }

        if (IsKeyPressed(KEY_ENTER) ||
            (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse,calcular))) {
            double valores[13];
            bool valido=true;
            for (int i=0;i<13;i++)
                if (!lerNumero(campos[i].texto,valores[i])) valido=false;

            if (!valido) {
                mensagem="Corrija os campos numericos invalidos.";
            } else {
                Trajetoria3D nova = {{valores[0],valores[1],valores[2]},
                                     {valores[3],valores[4],valores[5]}};
                Vetor3D novoNormal={valores[6],valores[7],valores[8]};
                if (calcularModulo(novoNormal)<1e-12 || calcularModulo(nova.direcao)<1e-12) {
                    mensagem="Normal e direcao devem ser nao nulas.";
                } else {
                    trajetoria=nova;
                    normal=novoNormal;
                    termo=valores[9];
                    pontoChecagem={valores[10],valores[11],valores[12]};
                    resultado=analisarTrajetoria(trajetoria,normal,termo);
                    tempo=0;
                    reproduzindo=false;
                    mensagem="Calculos atualizados.";
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse,iniciar)) {
            if (!reproduzindo && tempo>=1.0) tempo=0;
            reproduzindo=!reproduzindo;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse,reiniciar)) {
            tempo=0;
            reproduzindo=false;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse,cameraBotao)) {
            anguloH=0.65f; anguloV=0.45f; distanciaCamera=17;
        }

        // A animacao representa t no intervalo de 0 ate o impacto quando
        // o impacto ocorre no sentido positivo da trajetoria. Nos demais
        // casos, percorre um trecho ilustrativo de 0 a 6.
        double fim = resultado.tipoIntersecao==0 && resultado.parametro>0 ?
            resultado.parametro : 6.0;
        if (reproduzindo) {
            // Ao iniciar novamente apos o impacto, reinicia o percurso.
            tempo+=GetFrameTime()*velocidade;
            if (tempo>=1) {tempo=1;reproduzindo=false;}
        }
        double tAtual=tempo*fim;
        Vetor3D ferramenta=calcularPosicaoFerramenta(trajetoria,tAtual);

        if (GetMouseX()>430 && GetMouseY()>60) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Vector2 delta=GetMouseDelta();
                anguloH-=delta.x*0.008f;
                anguloV=std::clamp(anguloV+delta.y*0.008f,-1.45f,1.45f);
            }
            distanciaCamera=std::clamp(distanciaCamera-GetMouseWheelMove(),5.0f,65.0f);
        }

        Vetor3D centro = resultado.tipoIntersecao==0 ?
            multiplicarVetor(somarVetores(trajetoria.origem,resultado.impacto),0.5) :
            trajetoria.origem;
        Vector3 alvo=converter(centro);
        camera.target=alvo;
        camera.position={alvo.x+distanciaCamera*std::cos(anguloV)*std::cos(anguloH),
                         alvo.y+distanciaCamera*std::sin(anguloV),
                         alvo.z+distanciaCamera*std::cos(anguloV)*std::sin(anguloH)};

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        desenharPlano(normal,termo,5);
        DrawGrid(20,1);
        DrawLine3D({-8,0,0},{8,0,0},RED);
        DrawLine3D({0,-8,0},{0,8,0},DARKGREEN);
        DrawLine3D({0,0,-8},{0,0,8},DARKBLUE);
        if (mostrarTrajetoria) {
            // A seta acompanha o sentido crescente do parametro t.
            desenharSeta(trajetoria.origem,
                calcularPosicaoFerramenta(trajetoria,fim),ORANGE);
        }

        Vetor3D projecao = projetarPontoNoPlano(ferramenta,normal,termo);
        if (mostrarDistancia) {
            desenharSegmentoPontilhado(ferramenta,projecao,PURPLE);
            DrawSphere(converter(projecao),0.055f,PURPLE);
        }
        Vetor3D centroPlano = multiplicarVetor(normal,
            -termo/calcularProdutoEscalar(normal,normal));
        Vetor3D pontaNormal = somarVetores(centroPlano,
            multiplicarVetor(normal,2.3/calcularModulo(normal)));
        if (mostrarNormal) desenharSeta(centroPlano,pontaNormal,DARKGREEN);
        DrawSphere(converter(trajetoria.origem),0.085f,ORANGE);
        DrawSphere(converter(ferramenta),0.115f,MAROON);
        DrawSphere(converter(pontoChecagem),0.075f,PURPLE);
        if (resultado.tipoIntersecao==0)
            DrawSphere(converter(resultado.impacto),0.095f,DARKGREEN);
        EndMode3D();

        if (mostrarIdentificacoes) {
            // As etiquetas possuem deslocamentos diferentes para evitar
            // sobreposicao quando a ferramenta se aproxima do impacto.
            desenharIdentificacao3D(camera,trajetoria.origem,"P0",ORANGE,-34,12);
            desenharIdentificacao3D(camera,pontoChecagem,"S",PURPLE,-18,-35);

            bool ferramentaNoImpacto = resultado.tipoIntersecao==0 &&
                calcularModulo(subtrairVetores(ferramenta,resultado.impacto))<0.03;

            if (!ferramentaNoImpacto)
                desenharIdentificacao3D(camera,ferramenta,"Ferramenta",MAROON,15,-30);
            if (resultado.tipoIntersecao==0)
                desenharIdentificacao3D(camera,resultado.impacto,
                    ferramentaNoImpacto?"Impacto / ferramenta":"Impacto",
                    DARKGREEN,15,10);
            if (mostrarNormal)
                desenharIdentificacao3D(camera,pontaNormal,"Normal n",DARKGREEN,15,-25);
            if (mostrarDistancia)
                desenharIdentificacao3D(camera,projecao,"Projecao",PURPLE,15,15);
        }

        // A barra superior permite preparar capturas com diferentes
        // camadas, facilitando a apresentacao dos resultados no relatorio.
        DrawRectangle(430,0,970,60,Color{244,247,251,255});
        desenharOpcao("Trajetoria",opcoes[0],mostrarTrajetoria);
        desenharOpcao("Vetor normal",opcoes[1],mostrarNormal);
        desenharOpcao("Distancia",opcoes[2],mostrarDistancia);
        desenharOpcao("Identificacoes",opcoes[3],mostrarIdentificacoes);

        DrawRectangle(0,0,430,840,Color{244,247,251,255});
        DrawLine(429,0,429,840,LIGHTGRAY);
        desenharTextoInterface("SIMULACAO ROBOTICA",20,20,26,DARKBLUE);
        desenharTextoInterface("Trajetoria ferramenta / plano",20,57,18,DARKGRAY);
        desenharTextoInterface("Campos: X, Y, Z (exceto D)",20,94,16,GRAY);
        const char *rotulos[]={"Origem P0","Direcao v","Normal n","Termo D","Checagem S"};
        for (int linha=0;linha<5;linha++) {
            desenharTextoInterface(rotulos[linha],20,114+linha*58,15,DARKGRAY);
        }
        for (int i=0;i<13;i++) {
            Rectangle area=campos[i].area;
            DrawRectangleRec(area,WHITE);
            DrawRectangleLinesEx(area,ativo==i?2:1,ativo==i?BLUE:LIGHTGRAY);
            desenharTextoInterface(campos[i].texto.c_str(),(int)area.x+8,(int)area.y+10,17,BLACK);
        }

        DrawRectangleRec(calcular,DARKBLUE);
        desenharTextoInterface("RECALCULAR  [ENTER]",89,459,20,WHITE);
        DrawRectangleRec(iniciar,BLUE);
        desenharTextoInterface(reproduzindo?"PAUSAR":"INICIAR",43,513,17,WHITE);
        DrawRectangleRec(reiniciar,GRAY);
        desenharTextoInterface("REINICIAR",167,513,17,WHITE);
        DrawRectangleRec(cameraBotao,DARKGRAY);
        desenharTextoInterface("CAMERA",314,513,17,WHITE);

        char texto[180];
        desenharTextoInterface("RESULTADOS MATEMATICOS",20,555,19,DARKBLUE);
        std::snprintf(texto,sizeof(texto),"Plano: %.2fx %+.2fy %+.2fz %+.2f = 0",
                      normal.x,normal.y,normal.z,termo);
        desenharTextoInterface(texto,20,583,16,BLACK);
        const char *tipo[]={"Intersecao unica","Reta paralela","Reta contida","Entrada invalida"};
        desenharTextoInterface(tipo[resultado.tipoIntersecao],20,611,18,DARKBLUE);

        if (resultado.tipoIntersecao==0) {
            std::snprintf(texto,sizeof(texto),"t impacto: %.5f",resultado.parametro);
            desenharTextoInterface(texto,20,638,18,BLACK);
            std::snprintf(texto,sizeof(texto),"I = (%.3f, %.3f, %.3f)",
                          resultado.impacto.x,resultado.impacto.y,resultado.impacto.z);
            desenharTextoInterface(texto,20,662,18,DARKGREEN);
            std::snprintf(texto,sizeof(texto),"Percurso ate impacto: %.5f",resultado.distanciaPercorrida);
            desenharTextoInterface(texto,20,686,17,BLACK);
        }

        double distS=std::abs(calcularProdutoEscalar(normal,pontoChecagem)+termo)/
                     calcularModulo(normal);
        std::snprintf(texto,sizeof(texto),"Distancia de S: %.5f",distS);
        desenharTextoInterface(texto,20,711,17,BLACK);
        std::snprintf(texto,sizeof(texto),"Dist. inicial: %.5f  |  Angulo: %.3f graus",
                      resultado.distanciaInicial,resultado.anguloIncidenciaGraus);
        desenharTextoInterface(texto,20,734,16,BLACK);
        desenharTextoInterface(mensagem.c_str(),20,762,15,MAROON);
        std::snprintf(texto,sizeof(texto),"t atual: %.4f  |  progresso: %.0f%%",
                      tAtual,tempo*100.0);
        desenharTextoInterface(texto,20,786,16,DARKBLUE);
        desenharTextoInterface("Arrastar: girar | Roda: zoom | ESC: fechar",444,809,17,DARKGRAY);
        EndDrawing();
    }
    // Libera a fonte externa antes de encerrar a janela da raylib.
    if (fonteExternaCarregada) UnloadFont(fonteInterface);
    CloseWindow();
    return 0;
}
