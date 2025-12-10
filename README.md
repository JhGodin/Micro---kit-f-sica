# Micro---kit-f-sica
aula de Microcontroladores I, grupo kit de física

Objetivo:
  O projeto busca montar um conjunto de laboratório de física que possa ser usado como material didático para crianças. Para isso, foi criado um total de três esperimentos, sendo eles, uma rampa que mede a velocidade e aceleração, um pequeno guindaste que mede a velocidade angular e a corrente do motor, e um imâ que mede a corrente, tensão e campo magnético.

Projeto 1:
Rampa

  [descrição]

  [imagem]

Projeto 2:
Guindaste

  A ideia é encaixar no guindaste um peso, e com a ajuda do potenciometro ajustar a potência do motor, assim medindo a sua velocidade, corrente e tempo gastos. Para isso, usamo um motor de brinquedo ligado a uma ponte H que responde diretamente ao potenciometro. Enquanto o motor estiver ligado e a roda estiver girando, um encoder fara a medição do tempo necessário para uma volta completa, com esse valor calculamos a velocidade. Em relação a corrente, usamos um sensor montado com um resistor e a leitura da fonte pelo arduino.
  Dentro do código temos 3 principais funções, lerEncoder, frotacao e lerPotenciometro. A primeira função, e mais inmportante, é a lerEncoder que captura o tempo entre cada Tick, para uma maior precisão declaramos ela junto de um attachInterrupt, de tal forma que a perda de dados seja mínima. As demais funções, respectivamente, calculam a velocidade e retornam para a Serial, e leem o valor do potenciometro e traduzem para o motor.

  <img width="664" height="679" alt="image" src="https://github.com/user-attachments/assets/970abeb4-81d8-4a0c-9dd5-de3910510250" />


    
Projeto 3:
Imã

  A ideia é gerar um campo magnético forte o suficiente para levantar pequenos objetos. Para isso, ligamos um potenciometro à uma ponte H já ligada à bobina. Dessa forma, o potenciometro configura a entrada de tensão na ponte H, alterando o campo gerado pela bobina. Enquanto na leitura da tensão e corrente fazemos o mesmo processo do Projeto 2.
  Dentro do código temos somente uma função, lerPotenciometro. Ela já possui dentro de si todos os calculos compactados, ou seja, realiza a leitura do potenciometro e automaticamente já repassa para o motor, assim como faz a leitura da tensão e o calculo da corrente, tudo isso a cada alteração de valor.

  <img width="999" height="748" alt="image" src="https://github.com/user-attachments/assets/c360e2bb-8647-4789-9248-692b2be2a66f" />


  Intergração:
  
   A integração é realizada via Serial, onde cada projeto Arduino envia os dados ordenadamente para um único aplicativo Python (Tkinter/Matplotlib). O software recebe os dados de três portas COM distintas e os atribui automaticamente aos gráficos de duas maneiras diferentes.

Rampa (Bloco de Dados): A função le_dados_rampa() implementa a leitura em bloco (usando os marcadores #LISTA_ULTRASSONICO e #FIM_ULTRASSONICO). O software limpa todo o histórico anterior (hist_tempo.clear()), coleta o novo conjunto completo de pontos, e redesenha o gráfico integralmente. Isso garante que o usuário visualize apenas a linha do último experimento finalizado.
Guindaste e Eletroímã (Ponto Contínuo): As funções de leitura utilizam a lógica de linha única, onde cada novo ponto lido é anexado (.append()) ao histórico. O gráfico é redesenhado com o novo ponto, fazendo com que a linha cresça progressivamente na tela, mostrando o registro contínuo ao longo do tempo.

O aplicativo Tkinter permite alternar a visualização entre os três experimentos, enquanto a coleta serial continua ativa em segundo plano para todas as portas.
