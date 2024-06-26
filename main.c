#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void applyNegativeFilter(unsigned char *img, int width, int height, int channels);
void applyGrayScaleFilter(unsigned char *img, int width, int height, int channels);
void applyBlurFilter(unsigned char *img, int width, int height, int channels);
void applySobelFilter(unsigned char *img, int width, int height, int channels);
void saveImage(const unsigned char *img, int width, int height, int channels, const char *outputFileName);

int main() {
    int width, height, channels;
    char inputFile[100] = "Rick.jpg";
    char outputFileName[100];
    int choice = 0;

  while(choice != 5){
      // Carregar a imagem
    unsigned char *img = stbi_load(inputFile, &width, &height, &channels, 0);
    if (img == NULL) {
        fprintf(stderr, "Erro ao carregar a imagem.\n");
        return 1;
    }
    printf("Escolha o filtro a ser aplicado:\n");
    printf("1: Negativo\n");
    printf("2: Escala de Cinza\n");
    printf("3: Blur\n");
    printf("4: Sobel\n");
    printf("5: fechar programa\n");
    printf("Digite sua escolha: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            applyNegativeFilter(img, width, height, channels);
            strcpy(outputFileName, "negativo.jpg");
            break;
        case 2:
            applyGrayScaleFilter(img, width, height, channels);
            strcpy(outputFileName, "escala_de_cinza.jpg");
            break;
        case 3:
            applyBlurFilter(img, width, height, channels);
            strcpy(outputFileName, "blur.jpg");
            break;
        case 4:
            applyGrayScaleFilter(img, width, height, channels); // Para Sobel, primeiro converte para escala de cinza
            applySobelFilter(img, width, height, channels);
            strcpy(outputFileName, "sobel.jpg");
            break;
        case 5:
            printf("Programa finalizado\n");
            break;
        default:
            printf("Escolha inválida!\n");
            stbi_image_free(img);
            return 1;
    }
    if(choice != 5){
        // Salvar a imagem processada com o nome apropriado
        saveImage(img, width, height, channels, outputFileName);
        // Liberar memória
        stbi_image_free(img);
    }
  }
    return 0;
}

void applySobelFilter(unsigned char *img, int width, int height, int channels){
    int soma1;
    int soma2;
    //inicializando o kernel responsável por Sobelizar na horizontal
    float kernelx[3][3]={{ -1,  0,  1 },{ -2,  0,  2 },{ -1,  0,  1 }};
    //inicializando o kernel responsável por Sobelizar na vertical
    float kernely[3][3]={{ -1,  -2,  -1 },{ 0,  0,  0 },{ 1,  2,  1 }};
    //Mesma coisa do Blur, por enquanto
    unsigned char *temp = malloc(width * height * 3); //iniciando a imagem temporária, para ser aplicada no processo de soma.
    unsigned char *temp2 = malloc(width * height * 3); //iniciando a imagem temporária, para ser aplicada no processo de sobel

    for (int i = 0; i < width * height * 3; ++i) {
        temp[i] = img[i]; // torna temporário = Imagem original
    }
    for (int i = 0; i < width * height * 3; ++i) {
        temp2[i] = img[i]; // torna temporário 2 = Imagem original
    }
    for (int i = 1; i < height - 1; ++i){
        for (int j = 1; j < width - 1; ++j){
            for (int k = 0; k < 3; ++k) {
                    soma1 = 0;
                    soma2 = 0;
                    for (int di = -1; di <= 1; ++di) {
                        for (int dj = -1; dj <= 1; ++dj) {
                            // processo de obtenção da soma e  aplicação do Kernel horizontal na soma, aplicando a parte horizontal do Sobel
                            soma1 +=  kernelx[dj+1][di+1] * temp[((i + di)* width + (j + dj)) * 3 + k];
                            soma2 +=  kernely[dj+1][di+1] * temp[((i + di)* width + (j + dj)) * 3 + k];
                        }
                    }
                    // Calcular a magnitude da borda
                    int magnitude = (int)sqrt((soma1 * soma1) + (soma2 * soma2));

                    // limitar a magnitude a 255 ( valor máximo do byte)
                    if(magnitude>255){
                        magnitude = 255;
                    }
                    // Atribuir o valor na imagem temporaria
                    temp2[(i * width + j) * 3 + k] = (unsigned char)magnitude;
                    img[(i * width + j ) * 3 + k] = temp2[(i * width + j) * 3 + k];
            }
        }
    }
}

void saveImage(const unsigned char *img, int width, int height, int channels, const char *outputFileName) {
    char outputPath[200];
    sprintf(outputPath, "img_%s", outputFileName); // Constrói o caminho no diretório atual
    stbi_write_jpg(outputPath, width, height, channels, img, 100);
    printf("Salvo como: %s\n\n", outputPath);
}


void applyNegativeFilter(unsigned char *img, int width, int height, int channels) {
    //o laço percorre cada pixel da imagem e reatribui o valor do pixel de mínimo para máximo e vice versa
        for (int x = 0; x < width * height * channels; x++) {
            *(img + x) = 255 - *(img + x);
        }
}

void applyGrayScaleFilter(unsigned char *img, int width, int height, int channels) {
    //nesse laço, percorremos cada pixel da imagem e em cada pixel é acesso os três canais de cores
    for(int i = 0; i < width*height*channels; i+=channels){
        //a variavel intensidade armazena o tom de cinza calculado (que precisa variar entre 0 à 255, logo é unsigned char)
        unsigned char intensidade = (unsigned char)(0.3*(*(img + i)) + 0.59 * (*(img + i + 1)) + 0.11 * (*(img + i + 2)));
        *(img + i) = intensidade;
        *(img + i + 1) = intensidade;
        *(img + i + 2) = intensidade;
    }
}

void applyBlurFilter(unsigned char *img, int width, int height, int channels) {
    float kernel[3][3] = {{1.0/9,1.0/9,1.0/9}, {1.0/9,1.0/9,1.0/9}, {1.0/9,1.0/9,1.0/9}}; // Kernel básico de média
    float soma=0;//irá aramazenar o novo valor do pixel na imagem
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            for (int c = 0; c < channels; ++c) {
                soma = 0;
                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        //acessamos a vizinhança do pixel para calcular uma média de seus valores
                        soma += *(img + ((y + i) * width + (x + j)) * channels + c) * kernel[i + 1][j + 1];
                    }
                }
                //acessamos o pixel e atribuimos o novo valor a ele
                *(img + (y * width + x) * channels + c) = soma;
            }
        }
    }
}

