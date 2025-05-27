#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define OFFSET_X 200
#define OFFSET_Y 25
#define DIMENSI_PIXEL 25

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int data_scatter[53][3];

// x      --> Total_UTS
// y      --> Total_UAS
// ukuran --> Total_Nilai_Akhir

void DrawCircle(HDC hdc, int xc, int yc, int r, COLORREF color) {
    // Simpan brush dan pen lama
    HBRUSH hBrush = CreateSolidBrush(color);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    HPEN hPen = CreatePen(PS_SOLID, 1, color);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Gambar elips seperti lingkaran
    Ellipse(hdc, xc - r, yc - r, xc + r, yc + r);

    // Kembalikan brush dan pen lama
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow){
    MSG msg;
    WNDCLASSW wc = {0};

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = L"Bubble Chart";
    wc.hInstance = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"Bubble Chart", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 1100, 725, NULL, NULL, hInstance, NULL);

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
    
    // dimensi 5
    int angka_x = 5; 
    int angka_y = 105;

    switch(msg){
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            color = GetSysColor(COLOR_BTNFACE);
            SetBkColor(hdc, color);

            TextOutW(hdc, OFFSET_X + 150, OFFSET_Y - 20, L"Bubble Chart", wcslen(L"Bubble Chart"));

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
                if (strcmp(kolom, "Total_UTS") == 0){
                    id_kolom[0] = id;
                }else if (strcmp(kolom, "Total_UAS") == 0){
                    id_kolom[1] = id;
                }else if (strcmp(kolom, "Total_Nilai_Akhir") == 0){
                    id_kolom[2] = id;
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
                strcpy(temp_line, line);
                data_scatter[j_data][2] = getNilaiKolom(temp_line, id_kolom[2]); 

                j_data++;
            }
            fclose(file);

            // garis x
            int y = 0;
            for (int i = 0; i <= 21; i++){
                MoveToEx(hdc, 0 + OFFSET_X, y + OFFSET_Y, NULL);
                LineTo(hdc, 525 + OFFSET_X, y + OFFSET_Y);
                y += 25;
            }

            // garis y
            int x = 0;
            for (int i = 0; i <= 21; i++){
                MoveToEx(hdc, x + OFFSET_X, 525 + OFFSET_Y, NULL);
                LineTo(hdc, x + OFFSET_X, 0 + OFFSET_Y);
                x += 25;
            }

            int jmlh = sizeof(data_scatter) / sizeof(data_scatter[0]);
            for (int i = 0; i < jmlh; i++) {
                int nilai_asli = data_scatter[i][2];
                int nilai_min = 0, nilai_max = 100;
            
                // Ukuran bubble non-linear
                int jari_jari = sqrt(nilai_asli) * 2;
                if (jari_jari < 5) jari_jari = 5;
                if (jari_jari > 40) jari_jari = 40;
            
                // Normalisasi nilai 0 - 1
                float t = (float)(nilai_asli - nilai_min) / (nilai_max - nilai_min);
                if (t < 0.0f) t = 0.0f;
                if (t > 1.0f) t = 1.0f;
            
                // Gradasi
                int r, g, b;
                if (t < 0.25f) {
                    r = 255;
                    g = (int)(t / 0.25f * 255);
                    b = 0;
                } else if (t < 0.5f) {
                    r = (int)((0.5f - t) / 0.25f * 255);
                    g = 255;
                    b = 0;
                } else if (t < 0.75f) {
                    r = 0;
                    g = (int)((0.75f - t) / 0.25f * 255);
                    b = (int)((t - 0.5f) / 0.25f * 255);
                } else {
                    r = (int)((t - 0.75f) / 0.25f * 255);
                    g = 0;
                    b = 255;
                }
            
                COLORREF warna_bubble = RGB(r, g, b);
                int x = ((data_scatter[i][0] * DIMENSI_PIXEL) / 5) + OFFSET_X;
                int y = 525 - ((data_scatter[i][1] * DIMENSI_PIXEL) / 5) + OFFSET_Y;
                DrawCircle(hdc, x, y, jari_jari, warna_bubble);
            }
            
            hFont = CreateFontW(15, 0, 0, 0, FW_MEDIUM, 0, 0, 0, 0,
                0, 0, 0, 0, L"Arial");
            holdFont = SelectObject(hdc, hFont);

            /** Penggambaran angka pada kartesian **/
            // 20 x DIMENSI_PIXEL = 500
            // angka y-axis
            for (int i = 0; i <= 525; i += DIMENSI_PIXEL){
                wchar_t buffer[10];
                wsprintfW(buffer, L"%d", angka_y);
    
                TextOutW(hdc, OFFSET_X - DIMENSI_PIXEL, OFFSET_Y + i - 10, buffer, wcslen(buffer));
                angka_y -= 5;
            }

            // angka x-axis
            for (int i = 0; i <= 500; i += DIMENSI_PIXEL){
                wchar_t buffer[10];
                wsprintfW(buffer, L"%d", angka_x);
    
                TextOutW(hdc, (OFFSET_X + DIMENSI_PIXEL - 5) + i, 525 + DIMENSI_PIXEL + 10, buffer, wcslen(buffer));
                angka_x += 5;
            }

            // keterangan
            TextOutW(hdc, OFFSET_X + 170 + DIMENSI_PIXEL, OFFSET_Y + 560, L"Total Nilai UTS", wcslen(L"Total Nilai UTS"));

            TextOutW(hdc, OFFSET_X  + 550 + DIMENSI_PIXEL, OFFSET_Y, L"Ukuran: Total Nilai Akhir", wcslen(L"Ukuran: Total Nilai Akhir"));
        
            TextOutW(hdc, 40, 250, L"Total Nilai UAS", wcslen(L"Total Nilai UAS"));


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