/***
 * Raja Aizin Nofrizivaldy    2301020027
 * Hermawan                   2301020025
 * Fauzan Aldi                2301020022
***/

#include <windows.h>
#include <stdio.h>
#include <string.h>

#define TINGGI 720
#define LEBAR 1100
#define GRID 20 // 60x20 pixel
#define TENGAH_BOX 50

// disini data
float data[4][53];

typedef struct BoxPlot{
    float *data;
    float Q1;
    float Q2;
    float Q3;
    float median;
    float maksimum;
    float minimum;
    float *outlier;
    int jmlh_outlier;
}BoxPlot;

void swap(float *a, float *b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

void quickSort(float *data, int kiri, int kanan) {
    if (kiri >= kanan) return;

    float pivot = data[kanan];
    int i = kiri - 1;

    for (int j = kiri; j < kanan; j++) {
        if (data[j] < pivot) { 
            i++;
            swap(&data[i], &data[j]);
        }
    }
    swap(&data[i + 1], &data[kanan]); 

    int pivot_idx = i + 1;

    quickSort(data, kiri, pivot_idx - 1);     
    quickSort(data, pivot_idx + 1, kanan); 
}

float getNilaiKolom(char *line, int kolom_idx){
    char *token = strtok(line, ";");
    for (int col = 1; token && col <= kolom_idx; col++, token = strtok(NULL, ";")){
        if (col == kolom_idx){ 
            return atof(token); // konversi ke float
        }
    }
    return 0.0;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void drawBoxPlot(HDC hdc, BoxPlot *data_plot, int geser, const wchar_t *label);
void prosesBoxPlot(BoxPlot *data_plot, int banyak);
float median(float data[], int start, int end);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR lpCmdLine, int nCmdShow) {
    
    MSG  msg;
    WNDCLASSW wc = {0};

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = L"Box Plot";
    wc.hInstance     = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc   = WndProc;
    wc.hCursor       = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"Box PLot",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100, LEBAR, TINGGI, NULL, NULL, hInstance, NULL);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam) {
    
    HDC hdc;
    PAINTSTRUCT ps;

    //atur teks font
    HFONT hFont, holdFont;

    static const wchar_t *label[4] = {
        L"Nilai TBP",
        L"Nilai Tugas",
        L"Nilai UTS",
        L"Nilai UAS"
    };

    switch(msg) {
    
        case WM_PAINT:

            hdc = BeginPaint(hwnd, &ps);

            hFont = CreateFontW(15, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0,
                0, 0, 0, 0, L"Arial");
            holdFont = SelectObject(hdc, hFont);

            MoveToEx(hdc, 100, 20, NULL);
            LineTo(hdc, 1000, 20);
            LineTo(hdc, 1000, 620);
            LineTo(hdc, 100, 620);
            LineTo(hdc, 100, 20);

            // proses import data dari csv
            const char *nama_file = "Data.csv";
            FILE *file = fopen(nama_file , "r");
            if (file == NULL){
                MessageBoxW(hwnd, L"Gagal buka file.", L"wih error!", MB_OK | MB_ICONERROR);
                return 0;
            }

            // baca kolom mana
            char data_kolom[500], line[400];
            int id_kolom[5] = {-1, -1, -1, -1, -1};
            int id = 1;
            fgets(data_kolom, sizeof(data_kolom), file);
            char *kolom = strtok(data_kolom, ";");
            while (kolom != NULL){
                if (strcmp(kolom, "Total_TBP") == 0){
                    id_kolom[0] = id;
                }else if (strcmp(kolom, "Total_Tugas") == 0){
                    id_kolom[1] = id;
                }else if (strcmp(kolom, "Total_UTS") == 0){
                    id_kolom[2] = id;
                }else if (strcmp(kolom, "Total_UAS") == 0){
                    id_kolom[3] = id;
                }
                kolom = strtok(NULL, ";");
                id++;
            }

            int j_data = 0;
            while(fgets(line, sizeof(line), file)){
                char temp_line[400];

                strcpy(temp_line, line);
                data[0][j_data] = getNilaiKolom(temp_line, id_kolom[0]);
                strcpy(temp_line, line);
                data[1][j_data] = getNilaiKolom(temp_line, id_kolom[1]); 
                strcpy(temp_line, line);
                data[2][j_data] = getNilaiKolom(temp_line, id_kolom[2]);
                strcpy(temp_line, line);
                data[3][j_data] = getNilaiKolom(temp_line, id_kolom[3]); 

                j_data++;
            }
            fclose(file);


            int x = 100;
            //   for (int i = 0; i < 45; i++){
            //       MoveToEx(hdc, x, 20, NULL);
            //       LineTo(hdc, x, 620);
            //        x += 3 * GRID;
            //    }

            int y = 20;
            //  for (int i = 0; i < 30; i++){
            //      MoveToEx(hdc, 100, y, NULL);
            //      LineTo(hdc, 1000, y);
            //      y += GRID;
            //  }

            // int y_axis = GRID;
            // for (int i = 0; i < 9; i++){
            // MoveToEx(hdc, 100, y_axis, NULL);
            //      LineTo(hdc, 1000, y_axis);

            //      y_axis += 3 * GRID;
            // }

            // angka pada garis y
            int angka_grafik = 0;
            // pake alamat posisi y sebelumnya
            y = 620;
            for (int i = 0; i < 11; i++){
                wchar_t buffer[10];
                wsprintfW(buffer, L"%d", angka_grafik);
                TextOutW(hdc, 100 - GRID, y - 10, buffer, wcslen(buffer));
                angka_grafik += 10;

                // angka garis y dari bawah ke atas
                y -= 3 * GRID;
            }
            
            BoxPlot data_box[4];
            int banyak = 53;
            int geser_grafik = 0;

            for (int i = 0; i < 4; i++){
                data_box[i].data = (float*) malloc(banyak * sizeof(float));
                if (!data_box[i].data){
                    MessageBoxW(hwnd, L"Alokasi memori gagal.", L"Ini Error", MB_OK | MB_ICONERROR);
                    return 0;
                }

                memcpy(data_box[i].data, data[i], banyak * sizeof(float));
                prosesBoxPlot(&data_box[i], banyak);
                drawBoxPlot(hdc, &data_box[i], geser_grafik, (wchar_t *)label[i]);

                geser_grafik += 160;

                // cek data
                printf("\nData ke-%d\n", i+1);
                printf("Q1: %.2f\n", data_box[i].Q1);
                printf("Q2: %.2f\n", data_box[i].Q2);
                printf("Q3: %.2f\n", data_box[i].Q3);
                printf("maksimum: %.2f\n", data_box[i].maksimum);
                printf("minimum: %.2f\n", data_box[i].minimum);
                printf("outlier: ");
                for (int j = 0; j < data_box[i].jmlh_outlier; j++){
                    printf("%.2f ", data_box[i].outlier[j]);
                }
                printf("\n");
            }

            //  printf("[");
            //  for (int i = 0; i < 53; i++){
            //      printf("%2.f , ", data[0][i]);
            //  }
            //  printf("]\n");

            // printf("[");
            // for (int i = 0; i < 53; i++){
            //     printf("%2.f , ", data[1][i]);
            // }
            // printf("]\n");

            // printf("[");
            // for (int i = 0; i < 53; i++){
            //     printf("%2.f , ", data[2][i]);
            // }
            // printf("]\n");

            // printf("[");
            // for (int i = 0; i < 53; i++){
            //     printf("%2.f , ", data[3][i]);
            // }
            // printf("]\n");

            SelectObject(hdc, holdFont);
            DeleteObject(hFont);
            EndPaint(hwnd, &ps);
            break;

        case WM_DESTROY:
            
            for (int i = 0; i < 4; i++){
                free(data_box[i].data);
                free(data_box[i].outlier);
            }
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

float median(float data[], int start, int end){
    int banyak = end - start + 1;
    int tengah = start + banyak / 2;
    // kalo ganjil maka langsung data ditengah mediannya
    if (banyak % 2 == 1){
        return data[tengah]; // tengah adalah index
    }else {
        return (data[tengah - 1] + data[tengah]) / 2.0;
    }
}

void prosesBoxPlot(BoxPlot *data_plot, int banyak){
    if (banyak <= 0 || data_plot->data == NULL){
        printf("Data tidak jelas. tidak valid.\n");
        return;
    }

    quickSort(data_plot->data, 0, banyak - 1);

    // mencari Q2 (median pertama)
    data_plot->Q2 = median(data_plot->data, 0,  banyak - 1);
    // mencari Q1 dan Q2 (banyak data Q1 pasti sama dengan Q2)
    data_plot->Q1 = median(data_plot->data, 0, (banyak/2) - 1);
    data_plot->Q3 = median(data_plot->data, (banyak + 1)/2, banyak - 1);

    data_plot->minimum = data_plot->data[0];
    data_plot->maksimum = data_plot->data[banyak - 1];

    // outlier
    float IQR = data_plot->Q3 - data_plot->Q1;
    float rentang_minimum = data_plot->Q1 - (1.50 * IQR);
    float rentang_maksimum = data_plot->Q3 + (1.50 * IQR);

    data_plot->jmlh_outlier = 0;

    data_plot->outlier = (float *) malloc(banyak * sizeof(float));
    if (!data_plot->outlier){
        printf("Gagal Alokasi memori.\n");
        return;
    }

    for (int i = 0; i < banyak; i++){
        if (data_plot->data[i] < rentang_minimum || data_plot->data[i] > rentang_maksimum){
            data_plot->outlier[data_plot->jmlh_outlier] = data_plot->data[i];
            data_plot->jmlh_outlier++;

        }

        // cek keberadaan outlier, (jangan jadikan maksimum/minimum)
        if (data_plot->minimum < rentang_minimum){
            data_plot->minimum = data_plot->data[i + 1];
        }else if (data_plot->maksimum > rentang_maksimum){
            data_plot->maksimum = data_plot->data[banyak - 1 - i];
        }

    }

    data_plot->outlier = (float*) realloc(data_plot->outlier, data_plot->jmlh_outlier * sizeof(float));
}

void drawBoxPlot(HDC hdc, BoxPlot *data_plot, int geser, const wchar_t *label){
        int pixel_dimensi = 6;
        int titik_0 = 620;

        float Q1 = data_plot->Q1;
        float Q2 = data_plot->Q2;
        float Q3 = data_plot->Q3;
        float min = data_plot->minimum;
        float maks = data_plot->maksimum;
        float *outlier = data_plot->outlier;

        // kotak (box)
        Rectangle(hdc, 160 + geser, titik_0 - (Q1 * pixel_dimensi), 260 + geser, titik_0 - (Q3 * pixel_dimensi));
        MoveToEx(hdc, 160 + geser, titik_0 - (Q2 * pixel_dimensi), NULL);
        LineTo(hdc, 260 + geser, titik_0 - (Q2 * pixel_dimensi));
            
         //180 = 160 + 20 untuk posisi nilai minimum
        MoveToEx(hdc, 180 + geser, titik_0 - (min * pixel_dimensi), NULL);
        LineTo(hdc, 240 + geser, titik_0 - (min * pixel_dimensi));

        // maksimum
        MoveToEx(hdc, 180 + geser, titik_0 - (maks * pixel_dimensi), NULL);
        LineTo(hdc, 240 + geser, titik_0 - (maks * pixel_dimensi));

        // garis tengah bawah
        MoveToEx(hdc, 160 + TENGAH_BOX + geser, titik_0 - (Q1 * pixel_dimensi), NULL);
        LineTo(hdc, 160 + TENGAH_BOX + geser, titik_0 - (min * pixel_dimensi));

        // garis tengah atas
        MoveToEx(hdc, 160 + TENGAH_BOX + geser, titik_0 - (Q3 * pixel_dimensi), NULL);
        LineTo(hdc, 160 + TENGAH_BOX + geser, titik_0 - (maks * pixel_dimensi));


        // buat outlier
        for (int i = 0; i < data_plot->jmlh_outlier; i++){
            Ellipse(hdc, 160 + TENGAH_BOX - 10 + geser, titik_0 - (outlier[i] * pixel_dimensi - 10), 
            160 + TENGAH_BOX + 10 + geser, titik_0 - (outlier[i] * pixel_dimensi + 10));
        }

        // buat keterangan
        TextOutW(hdc, 160 + TENGAH_BOX - GRID + geser, 640, label, lstrlen(label));
}