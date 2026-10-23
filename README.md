# Geometria Computacional 3D

Aplicação desenvolvida em **C++17**, utilizando a biblioteca **raylib**, para calcular equações de planos a partir de três pontos e apresentar sua representação gráfica tridimensional.

## Requisitos

Para compilar o projeto, é necessário ter:

- Compilador C++ com suporte a C++17.
- CMake 3.16 ou superior.
- Biblioteca raylib instalada.
- pkg-config (ou pkgconf) configurado para localizar a raylib.

## Compilação no Linux (Ubuntu)

### 1. Instalar ferramentas de compilação

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config
```

A biblioteca raylib também deve estar instalada e disponível para o `pkg-config`.

Para verificar:

```bash
pkg-config --modversion raylib
```

### 2. Compilar o projeto

Abra o terminal na pasta do projeto e execute:

```bash
cmake -S . -B build
cmake --build build
```

### 3. Executar o programa

```bash
./build/geometriaComputacional
```

### 4. Executar os testes matemáticos

```bash
ctest --test-dir build --output-on-failure
```

## Compilação no Windows

### 1. Instalar as ferramentas

Instale:

- Visual Studio 2022 com o componente **Desenvolvimento para desktop com C++**, ou outra versão compatível.
- CMake.
- vcpkg, para gerenciar a instalação da raylib.

### 2. Instalar a raylib

No terminal, dentro da pasta do vcpkg:

```powershell
.\vcpkg.exe install raylib:x64-windows
.\vcpkg.exe install pkgconf:x64-windows
```

### 3. Configurar e compilar

Abra o **Developer PowerShell for Visual Studio**, acesse a pasta do projeto e execute:

```powershell
cmake -S . -B build -A x64 -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```

Substitua `C:/vcpkg` pelo caminho em que o vcpkg foi instalado.

**Observação:** o `CMakeLists.txt` atual utiliza `pkg-config` para localizar a raylib. No Windows, essa configuração pode exigir ajustes adicionais de ambiente. A instalação pelo vcpkg, isoladamente, não garante que o `pkg-config` consiga localizar a biblioteca.

### 4. Executar o programa

```powershell
.\build\Release\geometriaComputacional.exe
```

### 5. Executar os testes matemáticos

```powershell
ctest --test-dir build -C Release --output-on-failure
```

## Como utilizar o programa

1. Execute a aplicação.
2. Informe as coordenadas X, Y e Z dos pontos A, B e C.
3. Informe as coordenadas do ponto P, utilizado para verificar a pertinência.
4. Clique em **Calcular Plano** ou pressione Enter.
5. Observe a equação geral, o vetor normal, a distância do ponto P ao plano e a representação gráfica 3D.

**Controles da interface:**

- **Mouse esquerdo + arrastar:** rotacionar a câmera.
- **Roda do mouse:** aproximar ou afastar a visualização.
- **Tab:** alternar entre os campos.
- **Shift + Tab:** retornar ao campo anterior.
- **Redefinir câmera:** restaurar o ângulo de visualização.
- **Esc:** encerrar o programa.

## Testes

O projeto inclui testes automatizados no arquivo `testes.cpp`.

Eles podem ser executados pelo CTest após a compilação ou diretamente pelo executável `testesMatematicos`.

Os testes verificam operações matemáticas utilizadas na determinação de planos, incluindo situações válidas e casos em que os pontos não definem um plano único.
