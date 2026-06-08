## Sistema-de-gerenciamento-de-pedidos-de-uma-lanchonete
Projeto da Disciplina de Algoritmo e estrutura de dados

## Sistema CLI de Pedidos — Lanchonete

Este é o projeto final desenvolvido em C puro (C11) para simular o gerenciamento de pedidos de uma lanchonete, utilizando estruturas de dados dinâmicas encadeadas e persistência em disco.


## Descrição do Problema que o Sistema Resolve

Em uma lanchonete com alto fluxo de clientes, a organização da ordem de preparo dos pratos é fundamental. O problema central é garantir que os clientes sejam atendidos de forma justa, priorizando a ordem de chegada, e lidar com imprevistos dinâmicos do dia a dia do comércio.

Este sistema resolve esses problemas automatizando o controle da cozinha e implementando as seguintes soluções para falhas e mudanças de fluxo:
Prevenção de Erros Humanos: Se um atendente marcar acidentalmente um pedido errado como "concluído/entregue", o sistema possui um mecanismo de "Desfazer" que devolve o pedido para o radar da cozinha.
Desistências: Se um cliente desistir de um pedido específico que está no meio da fila, o sistema permite o cancelamento pontual através do ID, sem afetar o restante da fila de produção.

## Estruturas de Dados Usadas e Por Quê

Para atingir a excelência na resolução do problema e evitar o uso de alocação estática (vetores/arrays) que poderiam causar desperdício de memória ou risco de *overflow*, o projeto utiliza **duas estruturas de dados dinâmicas encadeadas**:

1. Fila Encadeada (FIFO - First In, First Out): Uso: Gerencia os pedidos pendentes na cozinha. Além das operações padrão de fila (`enfileirar` e `desenfileirar`), ela é percorrida internamente como uma Lista Simples durante o cancelamento por ID.
Por quê: Justifica-se pois uma fila reflete perfeitamente a lógica do mundo real de uma lanchonete: o primeiro cliente a fazer o pedido deve ser o primeiro a ter sua comida preparada e entregue.

2. Pilha Encadeada (LIFO - Last In, First Out): Uso: Armazena temporariamente o histórico de pedidos concluídos para possibilitar a funcionalidade de "Desfazer".
Por quê: Justifica-se pois, ao cometer um erro, o atendente precisa anular a última ação realizada (acesso imediato ao último elemento inserido). Quando o desempilhamento ocorre, o pedido retorna para o início da fila (fura-fila), pois sua prioridade original não pode ser prejudicada por um erro do operador.


## Formato do Arquivo de Persistência

Os dados são persistidos localmente no formato **CSV (*Comma-Separated Values*)** no arquivo `pedidos.csv`. 

Justificativa: O formato CSV é texto puro, leve, altamente legível por humanos e facilmente importável para outros softwares (como planilhas Excel) para análise de vendas, caso o estabelecimento deseje. 
Estrutura: As colunas são separadas pelo delimitador ponto-e-vírgula (`;`).
Exemplo de gravação:
  ```csv
  1;João Silva;2 X-Burgers e 1 Refrigerante
  2;Maria Souza;1 Batata Frita Média
  3;Carlos Almeida;1 Suco de Laranja
  ```

## Limitações Conhecidas

1. Persistência Volátil do Histórico: Apenas o estado atual da Fila de pedidos pendentes é salvo em disco ao encerrar o sistema. A Pilha (histórico de ações para desfazer) reside apenas na memória RAM. Ao reiniciar a aplicação, a pilha inicia vazia, impossibilitando "desfazer" entregas de sessões anteriores.
2. Conflito de Caracteres Delimitadores: Como o sistema de leitura (parser) utiliza o caractere `;` para separar os campos no CSV, se o usuário inserir propositalmente um `;` no nome do cliente ou na descrição do pedido, ocorrerá uma quebra na leitura do arquivo na próxima execução.
3. Limite de Caracteres: Os campos de texto (Nome e Descrição) possuem um limite estático máximo de 100 caracteres por segurança na leitura dos buffers pelo C. Entradas maiores que isso serão truncadas ou podem deixar lixo no buffer do teclado.
