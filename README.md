# Renovador_de_livros
Este projeto tem como objetivo renovar os títulos emprestados nas bibliotecas dos campus da UTFPR-CT.

Para utilizá-lo, siga os passos abaixo:

    Crie um arquivo nomeado "userdata.txt" (sem aspas) e insira o seguinte conteúdo:

    RA:senha

    onde "RA" deve ser composto por 7 números e a "senha" deve conter 4 ou 6 dígitos.

Atualmente, o projeto utiliza a API LIBCURL para se comunicar com o servidor da biblioteca da UTFPR
e a biblioteca cJSON para processar os dados recebidos.
