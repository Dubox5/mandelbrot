Programa em C que gera uma imagem do conjunto de Mandelbrot usando quatro implementações distintas: serial, OpenMP e duas versões com Pthreads, cada uma com uma estratégia diferente de divisão de trabalho.

# Compilação:
```bash
make LOGIN=<login>
```

`LOGIN` define o sufixo usado nos nomes dos arquivos de saída (por exemplo, `LOGIN=mla` gera `mandelbrot_mla_serial.pgm`). Se `LOGIN` não for informado, o valor padrão `login` é usado.

```bash
make clean
```

Remove o executável, os arquivos objeto e os arquivos gerados por uma execução anterior (`.pgm` e `times.txt`).

# Execução:
```bash
./mandelbrot <largura> <altura> <max_iteracoes> <num_threads>
```

Exemplo:

```bash
./mandelbrot 800 600 1000 4
```

Uma execução roda as quatro implementações (serial, OpenMP, Pthreads 1 e Pthreads 2) sobre a região real `[-2,0; 1,0]` e imaginária `[-1,5; 1,5]` do plano complexo, gerando:
- `mandelbrot_<login>_serial.pgm`
- `mandelbrot_<login>_openmp.pgm`
- `mandelbrot_<login>_pthreads1.pgm`
- `mandelbrot_<login>_pthreads2.pgm`
- `times.txt` — tempo de execução (em segundos) de cada implementação

Cada arquivo `.pgm` tem só os valores de intensidade de cada pixel (sem cabeçalho de formato), um valor por pixel separado por espaço, com uma linha de texto por linha da imagem. A intensidade é proporcional ao número de iterações realizadas até o ponto escapar, normalizada entre 0 e 255. As quatro implementações produzem exatamente a mesma imagem.

O programa não imprime nada em stdout durante a execução normal; mensagens de erro são exibidas em stderr.

# Estrutura:
`mandelbrot.h` Structs e protótipos compartilhados
`common.c`     Parsing de argumentos, cálculo do ponto de Mandelbrot, escrita do arquivo de saída
`serial.c`   Implementação serial (baseline)
`openmp.c`   Implementação com OpenMP, `schedule(dynamic, 1)`
`pthreads1.c`Pthreads — divisão **estática em blocos contíguos** de linhas
`pthreads2.c`Pthreads — divisão **estática cíclica (round-robin)** de linhas
`main.c`     Orquestra as 4 execuções, valida argumentos e escreve as saídas

# Estratégias:

- **Pthreads 1 (blocos contíguos):** a imagem é dividida em até `num_threads`
  faixas contíguas de linhas. Cada thread processa sua faixa inteira, do
  início ao fim, sem nenhuma sincronização durante o cálculo.
- **Pthreads 2 (cíclica / round-robin):** a thread `t` processa as linhas
  `t, t + num_threads, t + 2*num_threads, ...`. O trabalho fica intercalado
  entre as threads, o que ajuda a balancear a carga quando o custo por linha
  varia (pontos dentro do conjunto sempre gastam `max_iter` iterações;
  pontos fora escapam mais cedo).
- **OpenMP:** usa `schedule(dynamic, 1)`, no qual o próprio runtime do
  OpenMP distribui as linhas dinamicamente entre as threads em tempo de
  execução — uma terceira estratégia, distinta das duas divisões estáticas
  usadas nas implementações Pthreads.

As três estratégias de paralelização produzem exatamente a mesma imagem que
a versão serial (mesmo mapeamento pixel → plano complexo e mesma fórmula de
normalização de intensidade), o que foi validado comparando o hash MD5 dos
quatro arquivos `.pgm` gerados em diversos testes (diferentes resoluções,
números de threads que dividem e que não dividem a altura exatamente, e
números de threads maiores que a altura da imagem).

# Tratamento de erros:

O programa valida, antes de aceitar:
O número de argumentos (`argc == 5`);
Se largura, altura, máximo de iterações e número de threads são inteiros positivos válidos (rejeita strings não numéricas, zero, negativos, valores com lixo após o número, e overflow);   Falhas de alocação de memória (imagens e estruturas de threads);
Falhas na criação de arquivos de saída;
Falhas na criação de threads (com *cleanup* das threads já criadas antes de encerrar).

Em qualquer uma dessas situações, uma mensagem de erro é exibida em stderr e
o programa encerra com código de saída diferente de zero, sem travar.