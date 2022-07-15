# Título do gráfico e tamanho de fonte
set title "Tempo de CPU (ns) p/ filtrar e enviar pacotes ao user space" font ", 24"

# Como nosso arquivo é separado por vírgulas temos que dizer isso ao GNU Plot
set datafile separator ","

# Vamos colocar a legenda no lado esquerdo do gráfico
set key left

# Adiciona linhas guias (um grid) para ajudar esteticamente na visualização
set grid

# Qual será o formato de saída e sua resolução
set terminal 'png' size 1000,800

# Define o nome do arquivo de saída
set output 'cpu_blocks.png'



# Configura que os dados serão representados por histogramas
set style data histogram

# Define que para o histograma iremos usar agrupamentos espaçados por 3 unidades
set style histogram cluster gap 2

# Diz ao GNU Plot para preencher as barras com uma cor sólida
set style fill solid

# Define a largura das barras
set boxwidth 1.5

# Rótulo do eixo Y e seu espaçamento
set ylabel "Tempo de CPU (nanosegundos)" offset 1.5,0

# Configura o menor e maior valor a ser dimensionado o eixo Y
set yrange [0:200]

# Configura o rótulo do eixo X
set xlabel "Pacotes por segundo"


# Instrui a leitura do arquivo de entrada
plot 'cpu_blocks.log' using 2:xtic(1) title "ePerf"