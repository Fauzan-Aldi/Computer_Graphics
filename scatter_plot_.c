#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#define OFFSET_X 200
#define OFFSET_Y 25
#define DIMENSI_PIXEL 25

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// deklarasi data scatter
float data_scatter[53][2];

// {x1, y2},  --> 0,0 dan 0,1
// {x2, y2},  --> 1,0 dan 1,1
// {x3, y3},  --> 2,0 dan 2,1
// dst

// Fungsi untuk menghitung koefisien regresi linier
void hitung_regresi_linier(float data[][2], int n, double *m, double *b) {
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
    for (int i = 0; i < n; i++) {
        sum_x += data[i][0];
        sum_y += data[i][1];
        sum_xy += data[i][0] * data[i][1];
        sum_x2 += data[i][0] * data[i][0];
    }
    *m = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    *b = (sum_y - (*m) * sum_x) / n;
}

float getNilaiKolom(char *line, int kolom_idx){
    char *token;
    int col = 1;

    token = strtok(line, ";");
    while (token != NULL){
        if (col == kolom_idx){
            return (float)atof(token);
        }
        token = strtok(NULL, ";");
        col++;
    }
    return 0.0f;
}

void DrawCircle(HDC hdc, int xc, int yc, int r, COLORREF color) {
    HBRUSH hBrush = CreateSolidBrush(color);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    HPEN hPen = CreatePen(PS_SOLID, 1, color);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Ellipse(hdc, xc - r, yc - r, xc + r, yc + r);

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow){
    MSG msg;
    WNDCLASSW wc = {0};

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = L"Scatter_plot";
    wc.hInstance = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"Scatter_Plot", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 1100, 725, NULL, NULL, hInstance, NULL);

    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    HDC hdc;
    PAINTSTRUCT ps;

    DWORD color;
    HFONT hFont, holdFont;

    switch(msg){
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            color = GetSysColor(COLOR_BTNFACE);
            SetBkColor(hdc, color);

            int angka_x = 5;
            int angka_y = 105;
        
            // ----------------    label x dan y  ------------------ //
            setlocale(LC_ALL, "");
        
            char* x_label = "Total_TBP";
            char* y_label = "Total_UAS";
        
            size_t len_x = mbstowcs(NULL, x_label, 0);
            size_t len_y = mbstowcs(NULL, y_label, 0);

            wchar_t* x_label_win = malloc((len_x + 1) * sizeof(wchar_t)); // +1 untuk null
            wchar_t* y_label_win = malloc((len_y + 1) * sizeof(wchar_t)); // +1 untuk null

            if  (x_label_win == NULL || y_label_win == NULL){
                MessageBoxW(hwnd, L"Gagal alokasi label", L"Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            // konversi dari char ke wchar_t
            mbstowcs(x_label_win, x_label, len_x + 1);
            mbstowcs(y_label_win, y_label, len_y + 1);
            // ----------------    label x dan y  ------------------ //

            // Judul Grafik
            TextOutW(hdc, OFFSET_X + 150, OFFSET_Y - 20, L"Scatter Plot - Data Visualisasi", wcslen(L"Scatter Plot - Data Visualisasi"));

            // baca data csv
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
                if (strcmp(kolom, x_label) == 0){
                    id_kolom[0] = id;
                }else if (strcmp(kolom, y_label) == 0){
                    id_kolom[1] = id;
                }
                kolom = strtok(NULL, ";");
                id++;
            }

            int j_data = 0;
            while(fgets(line, sizeof(line), file)){
                char temp_line[400];

                strcpy(temp_line, line);
                data_scatter[j_data][0] = getNilaiKolom(temp_line, id_kolom[0]);
                strcpy(temp_line, line);
                data_scatter[j_data][1] = getNilaiKolom(temp_line, id_kolom[1]); 

                j_data++;
            }
            fclose(file);

            printf("\n===== TBP =====\n");
            for (int i = 0; i < 53; i++){
                printf("%.1f", data_scatter[i][0]);
                printf("\n");
            }

            
            printf("\n===== UAS =====\n");
            for (int i = 0; i < 53; i++){
                printf("%.1f", data_scatter[i][1]);
                printf("\n");
            }

            // Garis grid horizontal
            for (int i = 0; i <= 21; i++){
                MoveToEx(hdc, OFFSET_X, OFFSET_Y + i * 25, NULL);
                LineTo(hdc, OFFSET_X + 525, OFFSET_Y + i * 25);
            }

            // Garis grid vertikal
            for (int i = 0; i <= 21; i++){
                MoveToEx(hdc, OFFSET_X + i * 25, OFFSET_Y, NULL);
                LineTo(hdc, OFFSET_X + i * 25, OFFSET_Y + 525);
            }

            // Sumbu X dan Y
            HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            HPEN oldPen = SelectObject(hdc, axisPen);
            MoveToEx(hdc, OFFSET_X, OFFSET_Y, NULL);
            LineTo(hdc, OFFSET_X, OFFSET_Y + 525); // Y
            MoveToEx(hdc, OFFSET_X, OFFSET_Y + 525, NULL);
            LineTo(hdc, OFFSET_X + 525, OFFSET_Y + 525); // X
            SelectObject(hdc, oldPen);
            DeleteObject(axisPen);

            // Hitung garis regresi
            double m, b;
            int data_length = sizeof(data_scatter) / sizeof(data_scatter[0]);
            hitung_regresi_linier(data_scatter, data_length, &m, &b);

            // Gambar garis regresi
            // ketika x = 0
            int x1 = 0;
            int y1 = (int)b;

            // ketika x = 100
            int x2 = 100;
            int y2 = (int)((m * x2) + b);

            HPEN linePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));  // Warna biru untuk garis regresi
            HPEN oldPenLine = SelectObject(hdc, linePen);
            MoveToEx(hdc, (x1 * DIMENSI_PIXEL / 5) + OFFSET_X, 525 - (y1 * DIMENSI_PIXEL / 5) + OFFSET_Y, NULL);
            LineTo(hdc, (x2 * DIMENSI_PIXEL / 5) + OFFSET_X, 525 - (y2 * DIMENSI_PIXEL / 5) + OFFSET_Y);
            SelectObject(hdc, oldPenLine);
            DeleteObject(linePen);

            // Scatter titik warna merah
            for (int i = 0; i < data_length; i++) {
                int px = (data_scatter[i][0] * DIMENSI_PIXEL / 5) + OFFSET_X;
                int py = 525 - (data_scatter[i][1] * DIMENSI_PIXEL / 5) + OFFSET_Y;
                COLORREF color = RGB(255, 0, 0); // Merah
                DrawCircle(hdc, px, py, 7, color);
            }

            // Label sumbu
            hFont = CreateFontW(15, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0,
                0, 0, 0, 0, L"Arial");
            holdFont = SelectObject(hdc, hFont);

            for (int i = 0; i <= 525; i += DIMENSI_PIXEL){
                wchar_t buffer[10];
                wsprintfW(buffer, L"%d", angka_y);
                TextOutW(hdc, OFFSET_X - DIMENSI_PIXEL, OFFSET_Y + i - 10, buffer, wcslen(buffer));
                angka_y -= 5;
            }

            for (int i = 0; i <= 500; i += DIMENSI_PIXEL){
                wchar_t buffer[10];
                wsprintfW(buffer, L"%d", angka_x);
                TextOutW(hdc, OFFSET_X + DIMENSI_PIXEL - 5 + i, 525 + OFFSET_Y + 10, buffer, wcslen(buffer));
                angka_x += 5;
            }

            hFont = CreateFontW(20, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0,
                0, 0, 0, 0, L"Arial");
            holdFont = SelectObject(hdc, hFont);

            // keterangan label x
            TextOutW(hdc, OFFSET_X + 200 + DIMENSI_PIXEL, OFFSET_Y + 560, x_label_win, wcslen(x_label_win));
        
            // keterangan label y
            TextOutW(hdc, 40, 250, y_label_win, wcslen(y_label_win));
            
            // wsprintfW(persamaan, L"y = %.2f x + %.2f", buff_m, buff_b);
            // TextOutW(hdc, 700, OFFSET_Y, persamaan, wcslen(persamaan));
            

            SelectObject(hdc, holdFont);
            DeleteObject(hFont);

            EndPaint(hwnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
