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


// Desenha identificacoes em coordenadas de tela para acompanhar a camera.
static void desenharNome(Camera3D camera, Vetor3D ponto, const char *nome, Color cor) {
    Vector2 posicao = GetWorldToScreen(converter(ponto),camera);
    if (posicao.x > 440 && posicao.x < GetScreenWidth()-30 &&
        posicao.y > 15 && posicao.y < GetScreenHeight()-35) {
        DrawText(nome,(int)posicao.x+8,(int)posicao.y-18,20,cor);
    }
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

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse,iniciar))
            reproduzindo=!reproduzindo;
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
            tempo+=GetFrameTime()*velocidade;
            if (tempo>=1) {tempo=1;reproduzindo=false;}
        }
        double tAtual=tempo*fim;
        Vetor3D ferramenta=calcularPosicaoFerramenta(trajetoria,tAtual);

        if (GetMouseX()>430) {
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
        DrawLine3D(converter(trajetoria.origem),
                   converter(calcularPosicaoFerramenta(trajetoria,fim)),ORANGE);
        DrawSphere(converter(trajetoria.origem),0.15f,ORANGE);
        DrawSphere(converter(ferramenta),0.23f,MAROON);
        DrawSphere(converter(pontoChecagem),0.17f,PURPLE);
        if (resultado.tipoIntersecao==0)
            DrawSphere(converter(resultado.impacto),0.19f,DARKGREEN);
        EndMode3D();

        desenharNome(camera,trajetoria.origem,"P0",ORANGE);
        desenharNome(camera,ferramenta,"Ferramenta",MAROON);
        desenharNome(camera,pontoChecagem,"S",PURPLE);
        if (resultado.tipoIntersecao==0)
            desenharNome(camera,resultado.impacto,"Impacto",DARKGREEN);

        DrawRectangle(0,0,430,840,Color{244,247,251,255});
        DrawLine(429,0,429,840,LIGHTGRAY);
        DrawText("SIMULACAO ROBOTICA",20,20,26,DARKBLUE);
        DrawText("Trajetoria ferramenta / plano",20,57,18,DARKGRAY);
        DrawText("Campos: X, Y, Z (exceto D)",20,94,16,GRAY);
        const char *rotulos[]={"Origem P0","Direcao v","Normal n","Termo D","Checagem S"};
        for (int linha=0;linha<5;linha++) {
            DrawText(rotulos[linha],20,114+linha*58,15,DARKGRAY);
        }
        for (int i=0;i<13;i++) {
            Rectangle area=campos[i].area;
            DrawRectangleRec(area,WHITE);
            DrawRectangleLinesEx(area,ativo==i?2:1,ativo==i?BLUE:LIGHTGRAY);
            DrawText(campos[i].texto.c_str(),(int)area.x+8,(int)area.y+10,17,BLACK);
        }

        DrawRectangleRec(calcular,DARKBLUE);
        DrawText("RECALCULAR  [ENTER]",89,459,20,WHITE);
        DrawRectangleRec(iniciar,BLUE);
        DrawText(reproduzindo?"PAUSAR":"INICIAR",43,513,17,WHITE);
        DrawRectangleRec(reiniciar,GRAY);
        DrawText("REINICIAR",167,513,17,WHITE);
        DrawRectangleRec(cameraBotao,DARKGRAY);
        DrawText("CAMERA",314,513,17,WHITE);

        char texto[180];
        DrawText("RESULTADOS MATEMATICOS",20,555,19,DARKBLUE);
        std::snprintf(texto,sizeof(texto),"Plano: %.2fx %+.2fy %+.2fz %+.2f = 0",
                      normal.x,normal.y,normal.z,termo);
        DrawText(texto,20,583,16,BLACK);
        const char *tipo[]={"Intersecao unica","Reta paralela","Reta contida","Entrada invalida"};
        DrawText(tipo[resultado.tipoIntersecao],20,611,18,DARKBLUE);

        if (resultado.tipoIntersecao==0) {
            std::snprintf(texto,sizeof(texto),"t impacto: %.5f",resultado.parametro);
            DrawText(texto,20,638,18,BLACK);
            std::snprintf(texto,sizeof(texto),"I = (%.3f, %.3f, %.3f)",
                          resultado.impacto.x,resultado.impacto.y,resultado.impacto.z);
            DrawText(texto,20,662,18,DARKGREEN);
            std::snprintf(texto,sizeof(texto),"Percurso ate impacto: %.5f",resultado.distanciaPercorrida);
            DrawText(texto,20,686,17,BLACK);
        }

        double distS=std::abs(calcularProdutoEscalar(normal,pontoChecagem)+termo)/
                     calcularModulo(normal);
        std::snprintf(texto,sizeof(texto),"Distancia de S: %.5f",distS);
        DrawText(texto,20,711,17,BLACK);
        std::snprintf(texto,sizeof(texto),"Dist. inicial: %.5f  |  Angulo: %.3f graus",
                      resultado.distanciaInicial,resultado.anguloIncidenciaGraus);
        DrawText(texto,20,734,16,BLACK);
        DrawText(mensagem.c_str(),20,762,15,MAROON);
        DrawText("Arrastar: girar | Roda: zoom | ESC: fechar",444,809,17,DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
